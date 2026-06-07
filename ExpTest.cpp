#include <QCoreApplication>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QDir>
#include <QElapsedTimer>
#include "ExpLexer.h"
#include "ExpParser.h"
#include "ExpParser2.h"

static QStringList collectFiles( const QDir& dir )
{
    QStringList res;
    QStringList files = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );

    foreach( const QString& f, files )
        res += collectFiles( QDir( dir.absoluteFilePath(f) ) );

    files = dir.entryList( QStringList() << QString("*.exp"), QDir::Files, QDir::Name );
    foreach( const QString& f, files )
    {
        res.append( dir.absoluteFilePath(f) );
    }
    return res;
}


static void testLexer( const QStringList& files )
{
    int ok = 0, fail = 0;
    foreach( const QString& path, files )
    {
        Exp::Lexer lex;
        lex.setStream(path);
        Exp::Token t = lex.nextToken();
        while( t.isValid() )
        {
            t = lex.nextToken();
        }
        if( !t.isEof() )
        {
            fail++;
            qCritical() << "!!!! Error lexing" << path << ":" << t.d_val << "at" << t.d_lineNr << t.d_colNr;
        }else
        {
            ok++;
            qDebug() << "**** Success lexing" << path;
        }
    }
    qDebug() << "Processed" << ok + fail << "files," << ok << "ok," << fail << "fail";
}

class Lex : public Exp::Scanner {
public:
    Exp::Lexer lex;
    Exp::Token next() { return lex.nextToken(); }
    Exp::Token peek(int offset) { return lex.peekToken(offset); }
};

static void testParser( const QStringList& files )
{
    int ok = 0, fail = 0;
    foreach( const QString& path, files )
    {
        Lex lex;
        lex.lex.setStream(path);
        Exp::Parser p(&lex);
        p.RunParser();
        if( !p.errors.isEmpty() )
        {
            fail++;
            qCritical() << "!!!! Error parsing" << path;
            foreach( const Exp::Parser::Error& e, p.errors )
                qCritical() << "    " << e.row << ":" << e.col << ":" << e.msg.toUtf8().constData();
        }else
        {
            ok++;
            qDebug() << "**** Success parsing" << path;
        }
    }
    qDebug() << "Processed" << ok + fail << "files," << ok << "ok," << fail << "fail";
}

class Lex2 : public Exp::Scanner2 {
public:
    Exp::Lexer lex;
    Exp::Token next() { return lex.nextToken(); }
    Exp::Token peek(int offset) { return lex.peekToken(offset); }
    QString source() const { return lex.getSourcePath(); }
};

static void testParser2( const QStringList& files )
{
    int ok = 0, fail = 0;
    foreach( const QString& path, files )
    {
        Lex2 lex;
        lex.lex.setStream(path);
        Exp::Ast::AstModel mdl;
        Exp::Parser2 p(&mdl, &lex);
        p.RunParser();
        if( !p.errors.isEmpty() )
        {
            fail++;
            qCritical() << "!!!! Error parsing" << path;
            foreach( const Exp::Parser2::Error& e, p.errors )
                qCritical() << "    " << e.pos.d_row << ":" << e.pos.d_col << ":" << e.msg.toUtf8().constData();
        }else
        {
            ok++;
            qDebug() << "**** Success parsing" << path;
        }
    }
    qDebug() << "Processed" << ok + fail << "files," << ok << "ok," << fail << "fail";
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("me@rochus-keller.ch");
    a.setOrganizationDomain("https://github.com/rochus-keller/Simula");
    a.setApplicationName("ExpTest");
    a.setApplicationVersion("2026-06-06");

    QTextStream out(stdout);
    out << a.applicationName() << " version: " << a.applicationVersion() <<
                 " author: me@rochus-keller.ch  license: GPL" << endl;

    QStringList dirOrFilePaths;
    QString outPath;
    bool dump = false;
    const QStringList args = QCoreApplication::arguments();
    for( int i = 1; i < args.size(); i++ ) // arg 0 enthaelt Anwendungspfad
    {
        if(  args[i] == "-h" || args.size() == 1 )
        {
            out << "usage: " << a.applicationName() << " [options] sources" << endl;
            out << "  reads EXPRESS sources (files or directories)." << endl;
            out << "options:" << endl;
            out << "  -dst      dump syntax trees to files" << endl;
            out << "  -h        display this information" << endl;
            return 0;
        }else if( args[i] == "-dst" )
            dump = true;
        else if( args[i].startsWith("-o=") )
            outPath = args[i].mid(3);
        else if( !args[ i ].startsWith( '-' ) )
        {
            dirOrFilePaths += args[ i ];
        }else
        {
            qCritical() << "error: invalid command line option " << args[i] << endl;
            return -1;
        }
    }
    if( dirOrFilePaths.isEmpty() )
    {
        qWarning() << "no file or directory to process; quitting (use -h option for help)" << endl;
        return -1;
    }

    QStringList files;
    foreach( const QString& path, dirOrFilePaths )
    {
        QFileInfo info(path);
        if( outPath.isEmpty() )
            outPath = info.isDir() ? info.absoluteFilePath() : info.absolutePath();
        if( info.isDir() )
            files += collectFiles( info.absoluteFilePath() );
        else
            files << path;
    }

    QElapsedTimer t;
    t.start();

    testParser2(files);

    qDebug() << "run for" << t.elapsed() << "[ms]";

    return 0;
}
