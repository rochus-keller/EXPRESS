#ifndef __EXP_PARSER2__
#define __EXP_PARSER2__

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

#include "ExpToken.h"
#include "ExpAst.h"
#include <QList>

namespace Exp {

    class Scanner2 {
    public:
        virtual Token next() = 0;
        virtual Token peek(int offset) = 0;
        virtual QString source() const = 0;
    };

    class Parser2 {
    public:
        Parser2(Ast::AstModel* m, Scanner2* s);
        ~Parser2();

        void RunParser();
        QList<Ast::Declaration*> takeSchemas();

        struct Error {
            QString msg;
            RowCol pos;
            QString path;
            Error( const QString& m, const RowCol& pos, const QString& p):msg(m),pos(pos),path(p){}
        };
        QList<Error> errors;
    protected:
        // top-level
        void syntax();
        Ast::Declaration* schema_decl();
        void schema_body();
        void interface_specification();
        void reference_clause();
        void use_clause();
        void from_as(Ast::Declaration*);
        void declaration();
        void constant_decl();
        void constant_body();

        // entity
        Ast::Declaration* entity_decl();
        void subsuper(Ast::Type *t);
        void supertype_constraint(Ast::Type *);
        void subtype_constraint(Ast::Type *entity);
        void subtype_declaration();
        Ast::Expression* supertype_expression();
        Ast::Expression* supertype_factor();
        Ast::Expression* supertype_term();
        Ast::Expression* one_of();
        void entity_body();
        void explicit_attr();
        void attribute_decl(QList<Token>& names, bool& isSelf);
        void redeclared_attribute(QList<Token>& names);
        void qualified_attribute(Ast::Expression*& selfExpr);
        void derive_clause();
        void derived_attr();
        void inverse_clause();
        void inverse_attr();
        void unique_clause();
        void unique_rule();
        void referenced_attribute(Ast::Expression*& refExpr, Token& refName);

        // type
        Ast::Declaration* type_decl();
        Ast::Type* underlying_type();
        Ast::Type* concrete_types();
        Ast::Type* constructed_types();
        void enumeration_items(Ast::Type* t);
        void enumeration_extension(Ast::Type* t);
        void select_list(Ast::Type* t);
        void select_extension(Ast::Type* t);
        Ast::Type* parameter_type();
        Ast::Type* generalized_types();
        Ast::Type* simple_types();
        Ast::Type* aggregation_types();
        Ast::Type* array_type();
        Ast::Type* bag_type();
        Ast::Type* list_type();
        Ast::Type* set_type();
        void bound_spec(Ast::Type *t);
        Ast::Type* general_aggregation_types();
        Ast::Type* general_array_type();
        Ast::Type* general_bag_type();
        Ast::Type* general_list_type();
        Ast::Type* general_set_type();
        Ast::Type* aggregate_type();
        Ast::Type* generic_entity_type();
        Ast::Type* generic_type();
        Ast::Type* instantiable_type();
        Ast::Type* named_types();

        // subtype constraint decl
        Ast::Declaration* subtype_constraint_decl();
        void subtype_constraint_body(Ast::Declaration* d);

        // function/procedure/rule
        Ast::Declaration* function_decl();
        void function_head(Ast::Declaration*& func);
        Ast::Declaration* procedure_decl();
        void procedure_head(Ast::Declaration*& proc);
        void formal_parameter(bool isVar = false);
        void algorithm_head();
        void local_decl();
        void local_variable();
        Ast::Declaration* rule_decl();
        Ast::Declaration *rule_head();

        // statements
        Ast::Statement* stmt();
        Ast::Statement* assignOrCall_();
        Ast::Statement* alias_stmt();
        Ast::Statement* case_stmt();
        Ast::Statement* case_action();
        Ast::Statement* compound_stmt();
        Ast::Statement* if_stmt();
        Ast::Statement* repeat_stmt();
        Ast::Statement* return_stmt();

        // expressions
        Ast::Expression* expression();
        Ast::Expression* and_expression();
        Ast::Expression* rel_expression();
        Ast::Expression* simple_expression();
        Ast::Expression* term();
        Ast::Expression* factor();
        Ast::Expression* simple_factor();
        Ast::Expression* primary();
        Ast::Expression* aggregate_initializer();
        Ast::Expression* interval();
        Ast::Expression* query_expression();
        void qualifier(Ast::Expression*& e);
        Ast::Expression* actual_parameter_list();

        // where
        void where_clause();
        void domain_rule();

    protected:
        Ast::Declaration* addDecl(const Token& id, Ast::Declaration::Kind kind, bool checkDuplicate = true);
        Ast::Declaration* addPseudoMemberTo(const Token& id, Ast::Declaration::Kind kind);
        void error(const Token& t, const QString& msg);
        void error(const RowCol& rc, const QString& msg);

    protected:
        QList<Ast::Declaration*> schemas;
        Ast::AstModel* mdl;
        Token cur;
        Token la;
        Scanner2* scanner;
        void next();
        Token peek(int off);
        void invalid(const char* what);
        bool expect(int tt, bool pkw, const char* where);
    };
}
#endif // include
