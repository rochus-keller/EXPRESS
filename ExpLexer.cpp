/*
* Copyright 2023 Rochus Keller <mailto:me@rochus-keller.ch>
*
* This file is part of the ActiveOberon parser/navigator project.
*
* The following is the license that applies to this copy of the
* file. For a license to use the file under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU General Public License Usage
* This file may be used under the terms of the GNU General Public
* License (GPL) versions 2.0 or 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE.GPL included in
* the packaging of this file. Please review the following information
* to ensure GNU General Public Licensing requirements will be met:
* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
* http://www.gnu.org/copyleft/gpl.html.
*/

#include "ExpLexer.h"
#include <QBuffer>
#include <QFile>
#include <QIODevice>
#include <QTextCodec>
#include <QtDebug>
using namespace Exp;

Lexer::Lexer(QObject *parent) : QObject(parent),
    d_lastToken(Tok_Invalid),d_lineNr(0),d_colNr(0),
    d_ignoreComments(true), d_packComments(true),d_sloc(0),d_lineCounted(false)
{

}

void Lexer::setStream(QIODevice* in, const QString& sourcePath)
{
    if( in == 0 )
        setStream( sourcePath );
    else
        setStream(in->readAll(), sourcePath);
}

void Lexer::setStream(const QByteArray & source, const QString &sourcePath)
{
    d_lineNr = 0;
    d_colNr = 0;
    d_sourcePath = sourcePath;
    d_lastToken = Tok_Invalid;
    d_sloc = 0;
    d_lineCounted = false;

    if( d_in.isOpen() )
        d_in.close();
    d_in.buffer() = source;
    d_in.open(QIODevice::ReadOnly);
}

bool Lexer::setStream(const QString& sourcePath)
{
    QFile file(sourcePath);
    if( !file.open(QIODevice::ReadOnly) )
        return false;

     // else
    setStream( &file, sourcePath );
    return true;
}

Token Lexer::nextToken()
{
    Token t;
    if( !d_buffer.isEmpty() )
    {
        t = d_buffer.first();
        d_buffer.pop_front();
    }else
        t = nextTokenImp();
    if( t.d_type == Tok_Comment && d_ignoreComments )
        t = nextToken();
    return t;
}

Token Lexer::peekToken(quint8 lookAhead)
{
    Q_ASSERT( lookAhead > 0 );
    while( d_buffer.size() < lookAhead )
        d_buffer.push_back( nextTokenImp() );
    return d_buffer[ lookAhead - 1 ];
}

QList<Token> Lexer::tokens(const QString& code)
{
    return tokens( code.toLatin1() );
}

QList<Token> Lexer::tokens(const QByteArray& code, const QString& path)
{
    QBuffer in;
    in.setData( code );
    in.open(QIODevice::ReadOnly);
    setStream( &in, path );

    QList<Token> res;
    Token t = nextToken();
    while( t.isValid() )
    {
        res << t;
        t = nextToken();
    }
    return res;
}

Token Lexer::nextTokenImp()
{
    skipWhiteSpace();

    while( d_colNr >= d_line.size() )
    {
        if( d_in.atEnd() )
            return token( Tok_Eof, 0 );
        nextLine();
        skipWhiteSpace();
    }
    Q_ASSERT( d_colNr < d_line.size() );
    while( d_colNr < d_line.size() )
    {
        const char ch = quint8(d_line[d_colNr]);

        if( ch == '\'' )
            return simple_string();
        else if( ch == '%' )
            return binlit();
        else if( ::isalpha(ch) )
            return ident();
        else if( ::isdigit(ch) )
            return number();
        // else
        int pos = d_colNr;
        TokenType tt = tokenTypeFromString(d_line,&pos);

        if( tt == Tok_Latt )
            return comment();
        else if( tt == Tok_2Minus )
        {
            const int len = d_line.size() - d_colNr;
            return token( Tok_Comment, len, d_line.mid(d_colNr,len) );
        }else if( tt == Tok_Invalid || pos == d_colNr )
            return token( Tok_Invalid, 1, QString("unexpected character '%1' %2").arg(char(ch)).arg(int(ch)).toUtf8() );
        else {
            const int len = pos - d_colNr;
            return token( tt, len, d_line.mid(d_colNr,len) );
        }
    }
    Q_ASSERT(false);
    return token(Tok_Invalid);
}

int Lexer::skipWhiteSpace()
{
    const int colNr = d_colNr;
    while( d_colNr < d_line.size() && (::isspace( d_line[d_colNr] ) || d_line[d_colNr] == 0) )
        d_colNr++;
    return d_colNr - colNr;
}

void Lexer::nextLine()
{
    d_colNr = 0;
    d_lineNr++;
    d_line = d_in.readLine();
    d_lineCounted = false;

    if( d_line.endsWith("\r\n") )
        d_line.chop(2);
    else if( d_line.endsWith('\n') || d_line.endsWith('\r') || d_line.endsWith('\025') )
        d_line.chop(1);
}

int Lexer::lookAhead(int off) const
{
    if( int( d_colNr + off ) < d_line.size() )
    {
        return d_line[ d_colNr + off ];
    }else
        return 0;
}

Token Lexer::token(TokenType tt, int len, const QByteArray& val)
{
    if( tt != Tok_Invalid && tt != Tok_Comment && tt != Tok_Eof )
        countLine();
    Token t( tt, d_lineNr, d_colNr + 1, len, val );
    d_lastToken = t;
    d_colNr += len;
    t.d_sourcePath = d_sourcePath;
    return t;
}

Token Lexer::ident()
{
    int off = 1;
    while( true )
    {
        const char c = lookAhead(off);
        if( !::isalnum(c) && c != '_' )
            break;
        else
            off++;
    }
    const QByteArray str = d_line.mid(d_colNr, off );
    Q_ASSERT( !str.isEmpty() );
    int pos = 0;
    const QByteArray upper = str.toUpper();
    TokenType t = tokenTypeFromString( upper, &pos );
    if( t != Tok_Invalid && pos != str.size() )
        t = Tok_Invalid;
    if( t != Tok_Invalid )
        return token( t, off );
    else
        return token( Tok_ident, off, str );
}

Token Lexer::number()
{
    // real_literal ::= digits ’.’ [ digits ] [ ’e’ [ sign ] digits ]
    // integer_literal ::= digits
    // digits ::= digit { digit }
    int lhsPlaces = 0, rhsPlaces = 0, expPlaces = 0;
    int off = 1;
    while( true )
    {
        const char c = lookAhead(off);
        if( !::isdigit(c) )
            break;
        else
            off++;
    }
    lhsPlaces = off;
    bool isReal = false;
    int commaPos = -1, ePos = -1;
    const char o1 = lookAhead(off);
    if( o1 == '.'  )
    {
        commaPos = off;
        off++;
        isReal = true;
        while( true )
        {
            const char c = lookAhead(off);
            if( !::isdigit(c) )
                break;
            else
                off++;
            rhsPlaces++;
        }
        const char e = lookAhead(off);
        if( e == 'e' || e == 'E' )
        {
            ePos = off;
            off++;
            char o = lookAhead(off);
            if( o == '+' || o == '-' )
            {
                off++;
                o = lookAhead(off);
            }
            if( !::isdigit(o) )
                return token( Tok_Invalid, off, "invalid real" );
            while( true )
            {
                const char c = lookAhead(off);
                if( !::isdigit(c) )
                    break;
                else
                    off++;
                expPlaces++;
            }
        }
    }
    QByteArray str = d_line.mid(d_colNr, off );
    Q_ASSERT( !str.isEmpty() );

    if( isReal)
        return token( Tok_real_literal, off, str );
    else
        return token( Tok_integer_literal, off, str );
}

Token Lexer::binlit()
{
    int off = 1;
    while( true )
    {
        const char c = lookAhead(off);
        if( c != '0' || c != '1' )
            break;
        else
            off++;
    }
    const QByteArray str = d_line.mid(d_colNr, off );
    Q_ASSERT( !str.isEmpty() );
    return token( Tok_binary_literal, off, str );
}

Token Lexer::simple_string()
{
    const char quote = '\'';
    int off = 1;
    while( true )
    {
        const char c = lookAhead(off);
        off++;
        if( c == quote && lookAhead(off) == quote )
            ; // '' -> '
        else if( c == quote )
            break;
        if( c == 0 )
            return token( Tok_Invalid, off, "non-terminated string" );
    }
    QByteArray str = d_line.mid(d_colNr, off );
    str.replace("''", "'");
    return token( Tok_simple_string_literal, off, str );
}

Token Lexer::encoded_string()
{
    const char quote = '"';
    int off = 1;
    while( true )
    {
        const char c = lookAhead(off);
        off++;
        if( c == quote )
            break;
        if( c == 0 )
            return token( Tok_Invalid, off, "non-terminated string" );
    }
    QByteArray str = d_line.mid(d_colNr, off );
    for( int i = 1; i < str.size() - 1; i++ )
    {
        const char ch = str[i];
        if( !::isdigit(ch) && !( ch >= 'a' && ch <= 'f' || ch >= 'A' && ch <= 'F' ) )
            return token( Tok_Invalid, off, "invalid encoded string format (wrong character)" );
    }
    if( (str.size() - 2) % 8 != 0 )
        return token( Tok_Invalid, off, "invalid encoded string format (wrong length)" );
    return token( Tok_simple_string_literal, off, str );
}

Token Lexer::comment()
{
    const int startLine = d_lineNr;
    const int startCol = d_colNr;

    int level = 0;
    int pos = d_colNr;
    parseComment( d_line, pos, level );
    QByteArray str = d_line.mid(d_colNr,pos-d_colNr);
    while( level > 0 && !d_in.atEnd() )
    {
        nextLine();
        pos = 0;
        parseComment( d_line, pos, level );
        if( !str.isEmpty() )
            str += '\n';
        str += d_line.mid(d_colNr,pos-d_colNr);
    }
    if( d_packComments && level > 0 && d_in.atEnd() )
    {
        d_colNr = d_line.size();
        Token t( Tok_Invalid, startLine, startCol + 1, str.size(), tr("non-terminated comment").toLatin1() );
        return t;
    }
    // Col + 1 weil wir immer bei Spalte 1 beginnen, nicht bei Spalte 0
    Token t( ( d_packComments ? Tok_Comment : Tok_Latt ), startLine, startCol + 1, str.size(), str );
    t.d_sourcePath = d_sourcePath;
    d_lastToken = t;
    d_colNr = pos;
    if( !d_packComments && level == 0 )
    {
        Token t(Tok_Ratt,d_lineNr, pos - 2 + 1, 2 );
        t.d_sourcePath = d_sourcePath;
        d_lastToken = t;
        d_buffer.append( t );
    }
    return t;
}

void Lexer::countLine()
{
    if( !d_lineCounted )
        d_sloc++;
    d_lineCounted = true;
}

void Lexer::parseComment(const QByteArray& str, int& pos, int& level)
{
    enum State { Idle, Lb, Star } state = Idle;
    while( pos < str.size() )
    {
        const char* tmp = str.constData() + pos;
        const char c = str[pos++];
        switch( state )
        {
        case Idle:
            if( c == '(')
                state = Lb;
            else if( c == '*' )
                state = Star;
            break;
        case Lb:
            if( c == '*' )
            {
                level++;
                state = Idle;
            }else if( c != '(')
                state = Idle;
            break;
        case Star:
            if( c == ')')
            {
                level--;
                state = Idle;
            }else if( c != '*' )
                state = Idle;
            if( level <= 0 )
                return;
            break;
        }
    }
}
