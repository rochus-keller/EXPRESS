#include <QCoreApplication>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QDir>
#include <QElapsedTimer>
#include "ExpLexer.h"
#include "ExpParser.h"
#include "ExpParser2.h"
#include "ExpValidator2.h"
#include "ExpToken.h"

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

class Loader : public Exp::Ast::Importer
{
public:
    ~Loader() {
        foreach( Exp::Ast::Declaration* schema, schemas )
            delete schema;
    }
    Exp::Ast::AstModel mdl;
    QList<Exp::Ast::Declaration*> schemas;

    Exp::Ast::Declaration* findSchema(const QByteArray& name)
    {
        const char* n = Exp::Token::getSymbol(name.toUpper()).constData();
        foreach( Exp::Ast::Declaration* d, schemas )
        {
            if( d->n == n )
                return d;
        }
        return 0;
    }

    Exp::Ast::Declaration* loadSchema( const Exp::Ast::Import& imp )
    {
        // schemas can reference each other, so there is no point of depth-first traversal.
        // instead just deliver for name resolution and let validateAll give everyone a chance
        return findSchema(imp.name);
    }

    bool validate( Exp::Ast::Declaration* schema )
    {
        if( schema->validated )
            return true;

        Exp::Validator2 v(&mdl, this);

        v.validate(schema);
        Exp::Ast::SchemaData sd = schema->data.value<Exp::Ast::SchemaData>();
        if( !v.errors.isEmpty() )
        {
            qCritical() << "!!!! Error validating schema" << schema->name << sd.sourcePath;
            foreach( const Exp::Validator2::Error& e, v.errors )
                qCritical() << "    " << e.pos.d_row << ":" << e.pos.d_col << ":" << e.msg.toUtf8().constData();

            return false;
        }else
        {
            qDebug() << "**** Validated schema" << schema->name << sd.sourcePath;
            return true;
        }
    }

    void validateAll()
    {
        int all = 0;
        int ok = 0;
        foreach( Exp::Ast::Declaration* schema, schemas )
        {
            if( validate(schema) )
                ok++;
            all++;
            if( schema->validated )
                continue;
            if( schema->hasErrors )
                return;
        }
        qDebug() << "Validated" << all << "files," << ok << "ok," << (all-ok) << "fail";
    }
};

static void testParserValidator( const QStringList& files )
{
    Loader loader;
    int ok = 0, fail = 0;
    foreach( const QString& path, files )
    {
        Lex2 lex;
        lex.lex.setStream(path);
        Exp::Parser2 p(&loader.mdl, &lex);
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
            loader.schemas << p.takeSchemas();
        }
    }
    qDebug() << "Parsed" << ok + fail << "files," << ok << "ok," << fail << "fail";
    if( fail )
        return;

    loader.validateAll();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("me@rochus-keller.ch");
    a.setOrganizationDomain("https://github.com/rochus-keller/Simula");
    a.setApplicationName("ExpTest");
    a.setApplicationVersion("2026-06-11");

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

    //testParser2(files);
    testParserValidator(files);

    qDebug() << "run for" << t.elapsed() << "[ms]";

    return 0;
}
