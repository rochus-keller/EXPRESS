#ifndef AOLEXER_H
#define AOLEXER_H

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

#include <QObject>
#include <QBuffer>
#include <ExpToken.h>

class QIODevice;

namespace Exp
{
    class Lexer : public QObject
    {
    public:
        explicit Lexer(QObject *parent = 0);

        // all setStream expect Latin-1, Ascii or Oberon files
        void setStream( QIODevice*, const QString& sourcePath );
        void setStream( const QByteArray&, const QString& sourcePath );
        bool setStream(const QString& sourcePath);

        void setIgnoreComments( bool b ) { d_ignoreComments = b; }
        void setPackComments( bool b ) { d_packComments = b; }

        Token nextToken();
        Token peekToken(quint8 lookAhead = 1);
        QList<Token> tokens( const QString& code );
        QList<Token> tokens( const QByteArray& code, const QString& path = QString() );
        quint32 getSloc() const { return d_sloc; }
        const QString& getSourcePath() const { return d_sourcePath; }

        static void parseComment( const QByteArray& str, int& pos, int& level );

    protected:
        Token nextTokenImp();
        int skipWhiteSpace();
        void nextLine();
        int lookAhead(int off = 1) const;
        Token token(TokenType tt, int len = 1, const QByteArray &val = QByteArray());
        Token ident();
        Token number();
        Token binlit();
        Token simple_string();
        Token encoded_string();
        Token comment();
        void countLine();
    private:
        QBuffer d_in;
        quint32 d_sloc;
        quint32 d_lineNr;
        quint16 d_colNr;
        QString d_sourcePath;
        QByteArray d_line;
        QList<Token> d_buffer;
        Token d_lastToken;
        bool d_ignoreComments;  // don't deliver comment tokens
        bool d_packComments;    // Only deliver one Tok_Comment for /**/ instead of Tok_Lcmt and Tok_Rcmt
        bool d_lineCounted;
    };
}

#endif // AOLEXER_H
