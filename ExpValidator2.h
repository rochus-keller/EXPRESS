#ifndef __EXP_VALIDATOR2__
#define __EXP_VALIDATOR2__
/*
* Copyright (C) 2026 Rochus Keller (me@rochus-keller.ch)
*
* This file is part of the EXPRESS language project.
*
* The following is the license that applies to this copy of the
* file. For a license to use the file under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU Lesser General Public License Usage
* This file may be used under the terms of the GNU Lesser
* General Public License version 2.1 or version 3 as published by the Free
* Software Foundation and appearing in the file LICENSE.LGPLv21 and
* LICENSE.LGPLv3 included in the packaging of this file. Please review the
* following information to ensure the GNU Lesser General Public License
* requirements will be met: https://www.gnu.org/licenses/lgpl.html and
* http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#include <Express/ExpAst.h>
#include <QList>

namespace Exp {

    class Validator2 {
    public:
        Validator2(Ast::AstModel* mdl, Ast::Importer* imp = 0, bool haveXref = false);
        ~Validator2();

        bool validate(Ast::Declaration* schema);
        Ast::Xref takeXref();

        struct Error {
            QString msg;
            RowCol pos;
            QString path;
            Error(const QString& m, const RowCol& rc, const QString& p):msg(m),pos(rc),path(p){}
        };
        mutable QList<Error> errors;

    protected:
        // schema-level
        void Schema(Ast::Declaration* schema);
        void ImportDecl(Ast::Declaration* useOrRef);
        void DeclSeq(Ast::Declaration* d);

        // declarations
        bool decl(Ast::Declaration* d);
        void ConstDecl(Ast::Declaration* d);
        void TypeDecl(Ast::Declaration* d);
        void EntityDecl(Ast::Declaration* d);
        void AttributeDecl(Ast::Declaration* d);
        void FunctionDecl(Ast::Declaration* d);
        void ProcedureDecl(Ast::Declaration* d);
        void RuleDecl(Ast::Declaration* d);
        void SubtypeConstraintDecl(Ast::Declaration* d);

        // types
        bool Type_(Ast::Type* t);
        bool NameRefType(Ast::Type* t);
        bool AggregationType(Ast::Type* t);

        // statements
        void Body(Ast::Statement* s);
        void StatBlock(Ast::Statement* s);
        void StatSeq(Ast::Statement* s);
        Ast::Statement* Statement_(Ast::Statement* s);
        void Assig(Ast::Statement* s);
        void CallStmt(Ast::Statement* s);
        Ast::Statement* IfStat(Ast::Statement* s);
        Ast::Statement* CaseStat(Ast::Statement* s);
        void AliasStat(Ast::Statement* s);
        void RepeatStat(Ast::Statement* s);
        void ReturnStat(Ast::Statement* s);
        void CompoundStat(Ast::Statement* s);

        // expressions
        bool Expr(Ast::Expression* e);
        bool ConstExpr(Ast::Expression* e);

    protected:
        bool relation(Ast::Expression* e);
        bool unaryOp(Ast::Expression* e);
        bool arithOp(Ast::Expression* e);
        bool mulOp(Ast::Expression* e);
        bool combineOp(Ast::Expression* e);
        bool powerOp(Ast::Expression* e);
        bool declRef(Ast::Expression* e);
        bool nameRef(Ast::Expression* e);
        bool select(Ast::Expression* e);
        bool index(Ast::Expression* e);
        bool groupRef(Ast::Expression* e);
        bool call(Ast::Expression* e);
        bool literal(Ast::Expression* e);
        bool aggInit(Ast::Expression* e);
        bool range(Ast::Expression* e);
        bool query(Ast::Expression* e);
        bool interval(Ast::Expression* e);
        bool selfExpr(Ast::Expression* e);
        bool supertypeExpr(Ast::Expression* e);

        // type compatibility
        bool assigCompat(Ast::Type* lhs, Ast::Type* rhs);
        bool assigCompat(Ast::Type* lhs, Ast::Expression* rhs);
        bool paramCompat(Ast::Declaration* formal, Ast::Expression* actual);

        // helpers
        void resolveNameRefType(Ast::Type* nameRef, const RowCol& pos);
        Ast::Declaration* find(const char* n, const RowCol& pos, bool searchInEntity = true);
        Ast::Declaration* findInEntity(Ast::Type* entity, const char* n);
        Ast::Type* deref(Ast::Type* t);
        bool checkBuiltinArgs(quint16 builtin, const QList<Ast::Expression*>& args, Ast::Type** ret, const RowCol& pos);
        void resolveSupertypes(Ast::Type *entity);
        bool checkCircularSupertypes(Ast::Type* entity, QList<Ast::Type*>& seen);

        void invalid(const char* what, const RowCol& pos);
        bool error(const RowCol& pos, const QString& msg) const;
        void markDecl(Ast::Declaration* d);
        Ast::Symbol* markRef(Ast::Declaration* d, const RowCol& pos);
        Ast::Symbol* markUnref(int len, const RowCol& pos);

        static const char* sym(const QByteArray&);

    private:
        Ast::Declaration* curSchema;
        QString sourcePath;
        Ast::AstModel* mdl;
        Ast::Importer* imp;
        QList<Ast::Declaration*> scopeStack;
        QList<Ast::Statement*> repeatStack;
        Ast::Declaration* curFunction;
        Ast::Symbol* first;
        Ast::Symbol* last;
        QHash<Ast::Declaration*,Ast::SymList> xref;
        QHash<Ast::Declaration*,Ast::DeclList> subs;
    };
}
#endif // __EXP_VALIDATOR2__
