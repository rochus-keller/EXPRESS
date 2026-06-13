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

// this parser was first generated with EbnfStudio and then manually extended to generate the AST

#include "ExpParser2.h"
using namespace Exp;
using namespace Ast;

static inline bool FIRST_schema_decl(int tt) {
    return tt == Tok_SCHEMA;
}

static inline bool FIRST_interface_specification(int tt) {
    return tt == Tok_REFERENCE || tt == Tok_USE;
}

static inline bool FIRST_declaration(int tt) {
    return tt == Tok_ENTITY || tt == Tok_FUNCTION || tt == Tok_PROCEDURE || tt == Tok_SUBTYPE_CONSTRAINT || tt == Tok_TYPE;
}

static inline bool FIRST_constant_decl(int tt) {
    return tt == Tok_CONSTANT;
}

static inline bool FIRST_constant_body(int tt) {
    return tt == Tok_ident;
}

static inline bool FIRST_entity_decl(int tt) {
    return tt == Tok_ENTITY;
}

static inline bool FIRST_supertype_constraint(int tt) {
    return tt == Tok_ABSTRACT || tt == Tok_SUPERTYPE;
}

static inline bool FIRST_subtype_constraint_fc(int tt) {
    return tt == Tok_OF;
}

static inline bool FIRST_subtype_declaration(int tt) {
    return tt == Tok_SUBTYPE;
}

static inline bool FIRST_supertype_expression(int tt) {
    return tt == Tok_Lpar || tt == Tok_ONEOF || tt == Tok_ident;
}

static inline bool FIRST_explicit_attr(int tt) {
    return tt == Tok_SELF || tt == Tok_ident;
}

static inline bool FIRST_redeclared_attribute(int tt) {
    return tt == Tok_SELF;
}

static inline bool FIRST_derive_clause(int tt) {
    return tt == Tok_DERIVE;
}

static inline bool FIRST_derived_attr(int tt) {
    return tt == Tok_SELF || tt == Tok_ident;
}

static inline bool FIRST_inverse_clause(int tt) {
    return tt == Tok_INVERSE;
}

static inline bool FIRST_inverse_attr(int tt) {
    return tt == Tok_SELF || tt == Tok_ident;
}

static inline bool FIRST_unique_clause(int tt) {
    return tt == Tok_UNIQUE;
}

static inline bool FIRST_unique_rule(int tt) {
    return tt == Tok_SELF || tt == Tok_ident;
}

static inline bool FIRST_type_decl(int tt) {
    return tt == Tok_TYPE;
}

static inline bool FIRST_concrete_types(int tt) {
    switch(tt){
    case Tok_ARRAY: case Tok_BAG: case Tok_BINARY: case Tok_BOOLEAN:
    case Tok_INTEGER: case Tok_LIST: case Tok_LOGICAL: case Tok_NUMBER:
    case Tok_REAL: case Tok_SET: case Tok_STRING: case Tok_ident:
        return true;
    default: return false;
    }
}

static inline bool FIRST_constructed_types(int tt) {
    return tt == Tok_ENUMERATION || tt == Tok_EXTENSIBLE || tt == Tok_SELECT;
}

static inline bool FIRST_enumeration_extension(int tt) {
    return tt == Tok_BASED_ON;
}

static inline bool FIRST_select_list(int tt) {
    return tt == Tok_Lpar;
}

static inline bool FIRST_select_extension(int tt) {
    return tt == Tok_BASED_ON;
}

static inline bool FIRST_generalized_types(int tt) {
    switch(tt){
    case Tok_AGGREGATE: case Tok_ARRAY: case Tok_BAG:
    case Tok_GENERIC: case Tok_GENERIC_ENTITY:
    case Tok_LIST: case Tok_SET:
        return true;
    default: return false;
    }
}

static inline bool FIRST_simple_types(int tt) {
    switch(tt){
    case Tok_BINARY: case Tok_BOOLEAN: case Tok_INTEGER: case Tok_LOGICAL:
    case Tok_NUMBER: case Tok_REAL: case Tok_STRING:
        return true;
    default: return false;
    }
}

static inline bool FIRST_aggregation_types(int tt) {
    return tt == Tok_ARRAY || tt == Tok_BAG || tt == Tok_LIST || tt == Tok_SET;
}

static inline bool FIRST_bound_spec(int tt) {
    return tt == Tok_Lbrack;
}

static inline bool FIRST_general_aggregation_types(int tt) {
    return tt == Tok_ARRAY || tt == Tok_BAG || tt == Tok_LIST || tt == Tok_SET;
}

static inline bool FIRST_width_spec(int tt) {
    return tt == Tok_Lpar;
}

static inline bool FIRST_subtype_constraint_decl(int tt) {
    return tt == Tok_SUBTYPE_CONSTRAINT;
}

static inline bool FIRST_abstract_supertype(int tt) {
    return tt == Tok_ABSTRACT;
}

static inline bool FIRST_total_over(int tt) {
    return tt == Tok_TOTAL_OVER;
}

static inline bool FIRST_function_decl(int tt) {
    return tt == Tok_FUNCTION;
}

static inline bool FIRST_procedure_decl(int tt) {
    return tt == Tok_PROCEDURE;
}

static inline bool FIRST_local_decl(int tt) {
    return tt == Tok_LOCAL;
}

static inline bool FIRST_local_variable(int tt) {
    return tt == Tok_ident;
}

static inline bool FIRST_rule_decl(int tt) {
    return tt == Tok_RULE;
}

static inline bool FIRST_stmt(int tt) {
    switch(tt){
    case Tok_ALIAS: case Tok_BEGIN: case Tok_CASE:
    case Tok_ESCAPE: case Tok_IF: case Tok_INSERT:
    case Tok_REMOVE: case Tok_REPEAT: case Tok_RETURN:
    case Tok_Semi: case Tok_SKIP: case Tok_ident:
        return true;
    default: return false;
    }
}

static inline bool FIRST_alias_stmt(int tt) {
    return tt == Tok_ALIAS;
}

static inline bool FIRST_assignOrCall_(int tt) {
    switch(tt){
    case Tok_INSERT: case Tok_REMOVE: case Tok_ident:
        return true;
    default: return false;
    }
}

static inline bool FIRST_built_in_procedure(int tt) {
    return tt == Tok_INSERT || tt == Tok_REMOVE;
}

static inline bool FIRST_case_stmt(int tt) {
    return tt == Tok_CASE;
}

static inline bool FIRST_compound_stmt(int tt) {
    return tt == Tok_BEGIN;
}

static inline bool FIRST_escape_stmt(int tt) {
    return tt == Tok_ESCAPE;
}

static inline bool FIRST_if_stmt(int tt) {
    return tt == Tok_IF;
}

static inline bool FIRST_null_stmt(int tt) {
    return tt == Tok_Semi;
}

static inline bool FIRST_repeat_stmt(int tt) {
    return tt == Tok_REPEAT;
}

static inline bool FIRST_return_stmt(int tt) {
    return tt == Tok_RETURN;
}

static inline bool FIRST_skip_stmt(int tt) {
    return tt == Tok_SKIP;
}

static inline bool FIRST_increment_control(int tt) {
    return tt == Tok_ident;
}

static inline bool FIRST_while_control(int tt) {
    return tt == Tok_WHILE;
}

static inline bool FIRST_until_control(int tt) {
    return tt == Tok_UNTIL;
}

static inline bool FIRST_expression(int tt) {
    switch(tt){
    case Tok_ABS: case Tok_ACOS: case Tok_ASIN: case Tok_ATAN:
    case Tok_BLENGTH: case Tok_CONST_E: case Tok_COS:
    case Tok_EXISTS: case Tok_EXP: case Tok_FALSE: case Tok_FORMAT:
    case Tok_HIBOUND: case Tok_HIINDEX:
    case Tok_LENGTH: case Tok_LOBOUND: case Tok_LOG: case Tok_LOG10: case Tok_LOG2: case Tok_LOINDEX:
    case Tok_Lbrace: case Tok_Lbrack: case Tok_Lpar:
    case Tok_Minus: case Tok_NOT: case Tok_NVL: case Tok_ODD:
    case Tok_PI: case Tok_Plus:
    case Tok_QUERY: case Tok_Qmark:
    case Tok_ROLESOF: case Tok_SELF: case Tok_SIN: case Tok_SIZEOF: case Tok_SQRT:
    case Tok_TAN: case Tok_TRUE: case Tok_TYPEOF: case Tok_UNKNOWN:
    case Tok_USEDIN: case Tok_VALUE: case Tok_VALUE_IN: case Tok_VALUE_UNIQUE:
    case Tok_binary_literal: case Tok_encoded_string_literal: case Tok_ident:
    case Tok_integer_literal: case Tok_real_literal: case Tok_simple_string_literal:
        return true;
    default: return false;
    }
}

static inline bool FIRST_aggregate_initializer(int tt) {
    return tt == Tok_Lbrack;
}

static inline bool FIRST_interval(int tt) {
    return tt == Tok_Lbrace;
}

static inline bool FIRST_query_expression(int tt) {
    return tt == Tok_QUERY;
}

static inline bool FIRST_unary_op(int tt) {
    return tt == Tok_Plus || tt == Tok_Minus || tt == Tok_NOT;
}

static inline bool FIRST_primary(int tt) {
    switch(tt){
    case Tok_ABS: case Tok_ACOS: case Tok_ASIN: case Tok_ATAN:
    case Tok_BLENGTH: case Tok_CONST_E: case Tok_COS:
    case Tok_EXISTS: case Tok_EXP: case Tok_FALSE: case Tok_FORMAT:
    case Tok_HIBOUND: case Tok_HIINDEX:
    case Tok_LENGTH: case Tok_LOBOUND: case Tok_LOG: case Tok_LOG10: case Tok_LOG2: case Tok_LOINDEX:
    case Tok_NVL: case Tok_ODD:
    case Tok_PI:
    case Tok_Qmark:
    case Tok_ROLESOF: case Tok_SELF: case Tok_SIN: case Tok_SIZEOF: case Tok_SQRT:
    case Tok_TAN: case Tok_TRUE: case Tok_TYPEOF: case Tok_UNKNOWN:
    case Tok_USEDIN: case Tok_VALUE: case Tok_VALUE_IN: case Tok_VALUE_UNIQUE:
    case Tok_binary_literal: case Tok_encoded_string_literal: case Tok_ident:
    case Tok_integer_literal: case Tok_real_literal: case Tok_simple_string_literal:
        return true;
    default: return false;
    }
}

static inline bool FIRST_literal(int tt) {
    switch(tt){
    case Tok_FALSE: case Tok_TRUE: case Tok_UNKNOWN:
    case Tok_binary_literal: case Tok_encoded_string_literal:
    case Tok_integer_literal: case Tok_real_literal: case Tok_simple_string_literal:
        return true;
    default: return false;
    }
}

static inline bool FIRST_built_in_constant(int tt) {
    return tt == Tok_CONST_E || tt == Tok_PI || tt == Tok_SELF || tt == Tok_Qmark;
}

static inline bool FIRST_built_in_function(int tt) {
    switch(tt){
    case Tok_ABS: case Tok_ACOS:
    case Tok_ASIN: case Tok_ATAN:
    case Tok_BLENGTH: case Tok_COS:
    case Tok_EXISTS: case Tok_EXP:
    case Tok_FORMAT:
    case Tok_HIBOUND: case Tok_HIINDEX:
    case Tok_LENGTH: case Tok_LOBOUND:
    case Tok_LOG: case Tok_LOG10:
    case Tok_LOG2: case Tok_LOINDEX:
    case Tok_NVL: case Tok_ODD:
    case Tok_ROLESOF: case Tok_SIN:
    case Tok_SIZEOF: case Tok_SQRT:
    case Tok_TAN: case Tok_TYPEOF:
    case Tok_USEDIN: case Tok_VALUE:
    case Tok_VALUE_IN: case Tok_VALUE_UNIQUE:
        return true;
    default: return false;
    }
}

static inline bool FIRST_qualifier(int tt) {
    return tt == Tok_Dot || tt == Tok_Bslash || tt == Tok_Lbrack || tt == Tok_Lpar;
}

static inline bool FIRST_rel_op_extended(int tt) {
    switch(tt){
    case Tok_Lt: case Tok_Gt:
    case Tok_Leq: case Tok_Geq:
    case Tok_LtGt: case Tok_Eq:
    case Tok_ColonLtGtColon: case Tok_ColonEqColon:
    case Tok_IN: case Tok_LIKE:
        return true;
    default: return false;
    }
}

static inline bool FIRST_add_like_op(int tt) {
    return tt == Tok_Plus || tt == Tok_Minus || tt == Tok_XOR;
}

static inline bool FIRST_multiplication_like_op(int tt) {
    return tt == Tok_Star || tt == Tok_Slash || tt == Tok_DIV || tt == Tok_MOD || tt == Tok_2Bar;
}

static inline bool FIRST_where_clause(int tt) {
    return tt == Tok_WHERE;
}

static inline bool FIRST_domain_rule(int tt) {
    return FIRST_expression(tt);
}

static inline bool FIRST_case_action(int tt) {
    return FIRST_expression(tt);
}

static inline bool FIRST_element(int tt) {
    return FIRST_expression(tt);
}

Parser2::Parser2(AstModel* m, Scanner2* s):scanner(s),mdl(m)
{
}

Parser2::~Parser2()
{
    foreach(Declaration* s, schemas)
        Declaration::deleteAll(s);
}

void Parser2::RunParser()
{
    errors.clear();
    next();
    syntax();
}

QList<Declaration*> Parser2::takeSchemas()
{
    QList<Declaration*> res = schemas;
    schemas.clear();
    return res;
}

void Parser2::next()
{
    cur = la;
    la = scanner->next();
    while( la.d_type == Tok_Invalid ) {
        errors << Error(la.d_val, la.toRowCol(), la.d_sourcePath);
        la = scanner->next();
    }
}

Token Parser2::peek(int off)
{
    if( off == 1 )
        return la;
    else if( off == 0 )
        return cur;
    else
        return scanner->peek(off-1);
}

void Parser2::invalid(const char* what)
{
    errors << Error(QString("invalid %1").arg(what), la.toRowCol(), la.d_sourcePath);
}

bool Parser2::expect(int tt, bool pkw, const char* where)
{
    if( la.d_type == tt ) { next(); return true; }
    else {
        errors << Error(QString("'%1' expected in %2").arg(tokenTypeString(tt)).arg(where),
                        la.toRowCol(), la.d_sourcePath);
        return false;
    }
}

Declaration* Parser2::addDecl(const Token& id, Declaration::Kind kind, bool checkDuplicate)
{
    if( checkDuplicate )
    {
        Declaration* d = mdl->findDecl(id.d_val,false);
        if( d != 0 )
        {
            error(id, QString("duplicate declaration '%1'").arg(id.d_val.constData()));
            return 0;
        }
    }
    Declaration* d = mdl->addDecl(id.d_val);
    d->kind = kind;
    d->pos = id.toRowCol();
    return d;
}

Declaration *Parser2::addPseudoMemberTo(const Token &id, Ast::Declaration::Kind kind)
{
    Declaration* d = addDecl(id, kind, false);
    Q_ASSERT(d);
    d->n = 0; // name is a reference, not for lookup
    return d;
}

void Parser2::error(const Token& t, const QString& msg)
{
    errors << Error(msg, t.toRowCol(), t.d_sourcePath);
}

void Parser2::error(const RowCol& rc, const QString& msg)
{
    errors << Error(msg, rc, scanner->source());
}

void Parser2::syntax()
{
    schema_decl();
    while( FIRST_schema_decl(la.d_type) ) {
        schema_decl();
    }
}

Declaration* Parser2::schema_decl()
{
    expect(Tok_SCHEMA, false, "schema_decl");
    expect(Tok_ident, false, "schema_decl");

    Declaration* schema = new Declaration();
    schema->kind = Declaration::Schema;
    schema->setName(cur.d_val);
    schema->pos = cur.toRowCol();

    SchemaData sd;
    sd.sourcePath = scanner->source();
    sd.schemaName = cur.d_val;

    schemas.append(schema);
    mdl->openScope(schema);

    // optional schema version string
    if( la.d_type == Tok_simple_string_literal || la.d_type == Tok_encoded_string_literal ) {
        next();
        sd.versionId = cur.d_val;
    }
    expect(Tok_Semi, false, "schema_decl");

    schema_body();

    expect(Tok_END_SCHEMA, false, "schema_decl");
    sd.end = la.toRowCol();
    schema->data = QVariant::fromValue(sd);
    expect(Tok_Semi, false, "schema_decl");

    mdl->closeScope();
    return schema;
}

void Parser2::schema_body()
{
    while( FIRST_interface_specification(la.d_type) ) {
        interface_specification();
    }
    if( FIRST_constant_decl(la.d_type) ) {
        constant_decl();
    }
    while( FIRST_declaration(la.d_type) || FIRST_rule_decl(la.d_type) ) {
        if( FIRST_declaration(la.d_type) ) {
            declaration();
        } else if( FIRST_rule_decl(la.d_type) ) {
            rule_decl();
        } else
            invalid("schema_body");
    }
}

void Parser2::interface_specification()
{
    if( la.d_type == Tok_REFERENCE ) {
        reference_clause();
    } else if( la.d_type == Tok_USE ) {
        use_clause();
    } else
        invalid("interface_specification");
}

void Parser2::reference_clause()
{
    expect(Tok_REFERENCE, false, "reference_clause");
    expect(Tok_FROM, false, "reference_clause");
    expect(Tok_ident, false, "reference_clause");
    Token schemaName = cur;

    Declaration* imp = addDecl(schemaName, Declaration::Reference, false);
    imp->n = 0; // this is a pseudo decl; name is still valid for reference

    if( la.d_type == Tok_Lpar ) {
        expect(Tok_Lpar, false, "reference_clause");
        // resource_or_rename
        from_as(imp);
        while( la.d_type == Tok_Comma ) {
            expect(Tok_Comma, false, "reference_clause");
            from_as(imp);
        }
        expect(Tok_Rpar, false, "reference_clause");
    }
    expect(Tok_Semi, false, "reference_clause");
}

void Parser2::use_clause()
{
    expect(Tok_USE, false, "use_clause");
    expect(Tok_FROM, false, "use_clause");
    expect(Tok_ident, false, "use_clause");
    Token schemaName = cur;

    Declaration* imp = addDecl(schemaName, Declaration::Use, false);
    imp->n = 0; // this is a pseudo decl; name is still valid for reference

    if( la.d_type == Tok_Lpar ) {
        expect(Tok_Lpar, false, "use_clause");
        // named_type_or_rename
        from_as(imp);

        while( la.d_type == Tok_Comma ) {
            expect(Tok_Comma, false, "use_clause");
            from_as(imp);
        }
        expect(Tok_Rpar, false, "use_clause");
    }
    expect(Tok_Semi, false, "use_clause");
}

void Parser2::from_as(Ast::Declaration * imp)
{
    expect(Tok_ident, false, "from_as");
    Token id = cur, as = cur;
    if( la.d_type == Tok_AS ) {
        expect(Tok_AS, false, "from_as");
        expect(Tok_ident, false, "from_as");
        as = cur;
    }
    Declaration* what = new Declaration();
    what->kind = Declaration::Imported;
    what->setName(as.d_val);
    what->pos = as.toRowCol();
    what->data = id.d_val; // can be different from as.d_val
    imp->appendMember(what);
}

void Parser2::declaration()
{
    if( FIRST_entity_decl(la.d_type) ) {
        entity_decl();
    } else if( FIRST_function_decl(la.d_type) ) {
        function_decl();
    } else if( FIRST_procedure_decl(la.d_type) ) {
        procedure_decl();
    } else if( FIRST_subtype_constraint_decl(la.d_type) ) {
        subtype_constraint_decl();
    } else if( FIRST_type_decl(la.d_type) ) {
        type_decl();
    } else
        invalid("declaration");
}

void Parser2::constant_decl()
{
    expect(Tok_CONSTANT, false, "constant_decl");
    constant_body();
    while( FIRST_constant_body(la.d_type) ) {
        constant_body();
    }
    expect(Tok_END_CONSTANT, false, "constant_decl");
    expect(Tok_Semi, false, "constant_decl");
}

void Parser2::constant_body()
{
    expect(Tok_ident, false, "constant_body");
    Token name = cur;
    expect(Tok_Colon, false, "constant_body");

    Declaration* d = addDecl(name, Declaration::ConstDecl);
    Type* t = instantiable_type();
    if( d )
        d->setType(t);
    else
        delete t;

    expect(Tok_ColonEq, false, "constant_body");
    Expression* e = expression();
    if( d )
        d->expr = e;
    else
        delete e;

    expect(Tok_Semi, false, "constant_body");
}

Declaration* Parser2::entity_decl()
{
    expect(Tok_ENTITY, false, "entity_head");
    expect(Tok_ident, false, "entity_head");
    Token name = cur;

    Declaration* d = addDecl(name, Declaration::TypeDecl);
    if( d == 0 )
        return 0;

    Type* entity = new Type();
    entity->pos = name.toRowCol();
    entity->kind = Type::ENTITY;
    d->setType(entity);
    entity->decl = d;

    mdl->openScope(0);
    subsuper(entity);

    expect(Tok_Semi, false, "entity_head");

    entity_body();
    expect(Tok_END_ENTITY, false, "entity_decl");
    expect(Tok_Semi, false, "entity_decl");

    entity->subs = AstModel::toList(mdl->closeScope(true));

    return d;
}


void Parser2::subsuper(Type* t)
{
    if( FIRST_supertype_constraint(la.d_type) ) {
        supertype_constraint(t);
    }
    if( FIRST_subtype_declaration(la.d_type) ) {
        subtype_declaration();
    }
}

void Parser2::supertype_constraint(Type* entity)
{
    if( la.d_type == Tok_ABSTRACT ) {
        expect(Tok_ABSTRACT, false, "supertype_constraint");
        entity->abstract_ = true;
        if( la.d_type == Tok_SUPERTYPE ) {
            expect(Tok_SUPERTYPE, false, "supertype_constraint");
            entity->supertype_ = true;
            if( FIRST_subtype_constraint_fc(la.d_type) ) {
                subtype_constraint(entity);
            }
        }
    } else if( la.d_type == Tok_SUPERTYPE ) {
        expect(Tok_SUPERTYPE, false, "supertype_constraint");
        entity->supertype_ = true;
        subtype_constraint(entity);
    } else
        invalid("supertype_constraint");
}

void Parser2::subtype_constraint(Type* entity)
{
    expect(Tok_OF, false, "subtype_constraint");
    expect(Tok_Lpar, false, "subtype_constraint");
    entity->expr = supertype_expression();
    expect(Tok_Rpar, false, "subtype_constraint");
}

void Parser2::subtype_declaration()
{
    expect(Tok_SUBTYPE, false, "subtype_declaration");
    expect(Tok_OF, false, "subtype_declaration");
    expect(Tok_Lpar, false, "subtype_declaration");
    expect(Tok_ident, false, "subtype_declaration");
    addPseudoMemberTo(cur, Declaration::Supertype);
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "subtype_declaration");
        expect(Tok_ident, false, "subtype_declaration");
        addPseudoMemberTo(cur, Declaration::Supertype);
    }
    expect(Tok_Rpar, false, "subtype_declaration");
}

Expression* Parser2::supertype_expression()
{
    Expression* e = supertype_factor();
    while( la.d_type == Tok_ANDOR ) {
        Expression* bin = new Expression(Expression::AndOr, la.toRowCol());
        expect(Tok_ANDOR, false, "supertype_expression");
        bin->lhs = e;
        bin->rhs = supertype_factor();
        e = bin;
    }
    return e;
}

Expression* Parser2::supertype_factor()
{
    Expression* e = supertype_term();
    while( la.d_type == Tok_AND ) {
        Expression* bin = new Expression(Expression::And, la.toRowCol());
        expect(Tok_AND, false, "supertype_factor");
        bin->lhs = e;
        bin->rhs = supertype_term();
        e = bin;
    }
    return e;
}

Expression* Parser2::supertype_term()
{
    if( la.d_type == Tok_ident ) {
        expect(Tok_ident, false, "supertype_term");
        Expression* e = new Expression(Expression::NameRef, cur.toRowCol());
        e->val = cur.d_val;
        return e;
    } else if( la.d_type == Tok_ONEOF ) {
        return one_of();
    } else if( la.d_type == Tok_Lpar ) {
        expect(Tok_Lpar, false, "supertype_term");
        Expression* e = supertype_expression();
        expect(Tok_Rpar, false, "supertype_term");
        return e;
    } else {
        invalid("supertype_term");
        return new Expression(Expression::Invalid, la.toRowCol());
    }
}

Expression* Parser2::one_of()
{
    Expression* e = new Expression(Expression::OneOf, la.toRowCol());
    expect(Tok_ONEOF, false, "one_of");
    expect(Tok_Lpar, false, "one_of");
    Expression* first = supertype_expression();
    e->lhs = first;
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "one_of");
        Expression* arg = supertype_expression();
        Expression::append(first, arg);
    }
    expect(Tok_Rpar, false, "one_of");
    return e;
}

void Parser2::entity_body()
{
    // explicit attributes
    while( FIRST_explicit_attr(la.d_type) ) {
        explicit_attr();
    }
    if( FIRST_derive_clause(la.d_type) ) {
        derive_clause();
    }
    if( FIRST_inverse_clause(la.d_type) ) {
        inverse_clause();
    }
    if( FIRST_unique_clause(la.d_type) ) {
        unique_clause();
    }
    if( FIRST_where_clause(la.d_type) ) {
        where_clause();
    }
}

void Parser2::explicit_attr()
{
    QList<Token> names;
    bool isSelf = false;
    attribute_decl(names, isSelf);
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "explicit_attr");
        attribute_decl(names, isSelf);
    }
    expect(Tok_Colon, false, "explicit_attr");

    bool opt = false;
    if( la.d_type == Tok_OPTIONAL ) {
        expect(Tok_OPTIONAL, false, "explicit_attr");
        opt = true;
    }
    Type* t = parameter_type();

    // create an Attribute declaration for each name
    foreach( const Token& n, names ) {
        Declaration* attr = addDecl(n, Declaration::Attribute);
        if( attr ) {
            attr->setType(t);
            attr->optional_ = opt;
        }
    }
    if( names.isEmpty() && t )
        delete t;

    expect(Tok_Semi, false, "explicit_attr");
}

void Parser2::attribute_decl(QList<Token>& names, bool& isSelf)
{
    if( la.d_type == Tok_ident ) {
        expect(Tok_ident, false, "attribute_decl");
        names.append(cur);
    } else if( FIRST_redeclared_attribute(la.d_type) ) {
        redeclared_attribute(names);
    } else
        invalid("attribute_decl");
}

void Parser2::redeclared_attribute(QList<Token>& names)
{
    Expression* selfExpr = 0;
    qualified_attribute(selfExpr);
    delete selfExpr; // TODO: store the self-path in the simple AST?

    if( la.d_type == Tok_RENAMED ) {
        expect(Tok_RENAMED, false, "redeclared_attribute");
        expect(Tok_ident, false, "redeclared_attribute");
        Token rn = cur;
        names.append(rn);
        // mark as redeclared; the actual attribute will have redeclared_ flag set later
    }
}

void Parser2::qualified_attribute(Expression*& selfExpr)
{
    expect(Tok_SELF, false, "qualified_attribute");
    selfExpr = new Expression(Expression::Self, cur.toRowCol());

    // group_qualifier
    expect(Tok_Bslash, false, "qualified_attribute");
    expect(Tok_ident, false, "qualified_attribute");
    Expression* grp = new Expression(Expression::GroupRef, cur.toRowCol());
    grp->val = cur.d_val;
    grp->lhs = selfExpr;
    selfExpr = grp;

    // attribute_qualifier
    expect(Tok_Dot, false, "qualified_attribute");
    expect(Tok_ident, false, "qualified_attribute");
    Expression* sel = new Expression(Expression::Select, cur.toRowCol());
    sel->val = cur.d_val;
    sel->lhs = selfExpr;
    selfExpr = sel;
}

void Parser2::derive_clause()
{
    expect(Tok_DERIVE, false, "derive_clause");
    derived_attr();
    while( FIRST_derived_attr(la.d_type) ) {
        derived_attr();
    }
}

void Parser2::derived_attr()
{
    QList<Token> names;
    bool isSelf = false;
    attribute_decl(names, isSelf);
    expect(Tok_Colon, false, "derived_attr");
    Type* t = parameter_type();
    expect(Tok_ColonEq, false, "derived_attr");
    Expression* e = expression();

    foreach( const Token& n, names ) {
        Declaration* attr = addDecl(n, Declaration::Attribute);
        if( attr ) {
            attr->derived_ = true;
            attr->setType(t);
            attr->expr = e;
            e = 0; // only first attr owns the expression
        }
    }
    if( e )
        delete e;
    if( names.isEmpty() )
        delete t;

    expect(Tok_Semi, false, "derived_attr");
}

void Parser2::inverse_clause()
{
    expect(Tok_INVERSE, false, "inverse_clause");
    inverse_attr();
    while( FIRST_inverse_attr(la.d_type) ) {
        inverse_attr();
    }
}

void Parser2::inverse_attr()
{
    QList<Token> names;
    bool isSelf = false;
    attribute_decl(names, isSelf);
    expect(Tok_Colon, false, "inverse_attr");

    // optional SET/BAG with bounds
    Type* aggType = 0;
    if( la.d_type == Tok_SET || la.d_type == Tok_BAG ) {
        aggType = new Type();
        if( la.d_type == Tok_SET ) {
            expect(Tok_SET, false, "inverse_attr");
            aggType->kind = Type::SET;
        } else {
            expect(Tok_BAG, false, "inverse_attr");
            aggType->kind = Type::BAG;
        }
        if( FIRST_bound_spec(la.d_type) ) {
            bound_spec(aggType);
        }
        expect(Tok_OF, false, "inverse_attr");
    }

    // entity type name
    expect(Tok_ident, false, "inverse_attr");
    Token entityName = cur;

    // FOR clause
    expect(Tok_FOR, false, "inverse_attr");
    Token forAttr;
    Token forEntity;
    if( la.d_type == Tok_SELF ) {
        // SELF { qualifier }
        expect(Tok_SELF, false, "inverse_attr");
        while( FIRST_qualifier(la.d_type) ) {
            // consume qualifiers (for AST simplicity, we don't build full path here)
            if( la.d_type == Tok_Dot ) {
                expect(Tok_Dot, false, "inverse_attr");
                expect(Tok_ident, false, "inverse_attr");
                forAttr = cur;
            } else if( la.d_type == Tok_Bslash ) {
                expect(Tok_Bslash, false, "inverse_attr");
                expect(Tok_ident, false, "inverse_attr");
            } else if( la.d_type == Tok_Lbrack ) {
                // index qualifier
                expect(Tok_Lbrack, false, "inverse_attr");
                Expression* idx = simple_expression();
                delete idx;
                if( la.d_type == Tok_Colon ) {
                    expect(Tok_Colon, false, "inverse_attr");
                    Expression* idx2 = simple_expression();
                    delete idx2;
                }
                expect(Tok_Rbrack, false, "inverse_attr");
            } else if( la.d_type == Tok_Lpar ) {
                Expression* args = actual_parameter_list();
                delete args;
            }
        }
    } else if( ( peek(1).d_type == Tok_ident && peek(2).d_type == Tok_Dot ) || la.d_type == Tok_ident ) {
        if( peek(1).d_type == Tok_ident && peek(2).d_type == Tok_Dot ) {
            expect(Tok_ident, false, "inverse_attr");
            forEntity = cur;
            expect(Tok_Dot, false, "inverse_attr");
        }
        expect(Tok_ident, false, "inverse_attr");
        forAttr = cur;
    } else
        invalid("inverse_attr");

    // create attribute declaration(s)
    foreach( const Token& n, names ) {
        Declaration* attr = addDecl(n, Declaration::Attribute);
        if( attr ) {
            attr->inverse_ = true;
            if( aggType ) {
                // create a NameRef type for the entity, set as base type of aggregation
                Type* entRef = new Type();
                entRef->kind = Type::NameRef;
                entRef->quali = new Quali(QByteArray(), entityName.d_val);
                entRef->pos = entityName.toRowCol();
                aggType->setType(entRef);
                attr->setType(aggType);
            } else {
                Type* entRef = new Type();
                entRef->kind = Type::NameRef;
                entRef->quali = new Quali(QByteArray(), entityName.d_val);
                entRef->pos = entityName.toRowCol();
                attr->setType(entRef);
            }
        }
    }
    if( names.isEmpty() )
        delete aggType;

    expect(Tok_Semi, false, "inverse_attr");
}

void Parser2::unique_clause()
{
    expect(Tok_UNIQUE, false, "unique_clause");
    unique_rule();
    expect(Tok_Semi, false, "unique_clause");
    while( FIRST_unique_rule(la.d_type) ) {
        unique_rule();
        expect(Tok_Semi, false, "unique_clause");
    }
}

void Parser2::unique_rule()
{
    // optional label
    if( peek(1).d_type == Tok_ident && peek(2).d_type == Tok_Colon ) {
        expect(Tok_ident, false, "unique_rule");
        // label, not stored in AST for now
        expect(Tok_Colon, false, "unique_rule");
    }
    // referenced attributes
    Expression* refExpr = 0;
    Token refName;
    referenced_attribute(refExpr, refName);
    delete refExpr;
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "unique_clause");
        referenced_attribute(refExpr, refName);
        delete refExpr;
        refExpr = 0;
    }
}

void Parser2::referenced_attribute(Expression*& refExpr, Token& refName)
{
    refExpr = 0;
    if( la.d_type == Tok_ident ) {
        expect(Tok_ident, false, "referenced_attribute");
        refName = cur;
    } else if( la.d_type == Tok_SELF ) {
        qualified_attribute(refExpr);
    } else
        invalid("referenced_attribute");
}

Declaration* Parser2::type_decl()
{
    expect(Tok_TYPE, false, "type_decl");
    expect(Tok_ident, false, "type_decl");
    Token name = cur;
    expect(Tok_Eq, false, "type_decl");

    Declaration* d = addDecl(name, Declaration::TypeDecl);
    Type* t = underlying_type();

    if( d ) {
        d->setType(t);
        mdl->openScope(d);
    } else
        delete t;

    expect(Tok_Semi, false, "type_decl");

    if( FIRST_where_clause(la.d_type) ) {
        where_clause();
    }

    expect(Tok_END_TYPE, false, "type_decl");
    expect(Tok_Semi, false, "type_decl");

    if( d )
        mdl->closeScope();

    return d;
}

Type* Parser2::underlying_type()
{
    if( FIRST_concrete_types(la.d_type) ) {
        return concrete_types();
    } else if( FIRST_constructed_types(la.d_type) ) {
        return constructed_types();
    } else {
        invalid("underlying_type");
        return new Type();
    }
}

Type* Parser2::concrete_types()
{
    if( FIRST_aggregation_types(la.d_type) ) {
        return aggregation_types();
    } else if( FIRST_simple_types(la.d_type) ) {
        return simple_types();
    } else if( la.d_type == Tok_ident ) {
        expect(Tok_ident, false, "concrete_types");
        Type* t = new Type();
        t->kind = Type::NameRef;
        t->quali = new Quali(QByteArray(), cur.d_val);
        t->pos = cur.toRowCol();
        return t;
    } else {
        invalid("concrete_types");
        return new Type();
    }
}

Type* Parser2::constructed_types()
{
    Type* t = new Type();

    if( la.d_type == Tok_EXTENSIBLE ) {
        expect(Tok_EXTENSIBLE, false, "constructed_types");
        t->extensible_ = true;

        if( la.d_type == Tok_ENUMERATION ) {
            expect(Tok_ENUMERATION, false, "constructed_types");
            t->kind = Type::ENUMERATION;
            if( la.d_type == Tok_OF || FIRST_enumeration_extension(la.d_type) ) {
                if( la.d_type == Tok_OF ) {
                    expect(Tok_OF, false, "constructed_types");
                    enumeration_items(t);
                } else {
                    enumeration_extension(t);
                }
            }
        } else if( la.d_type == Tok_GENERIC_ENTITY || la.d_type == Tok_SELECT ) {
            if( la.d_type == Tok_GENERIC_ENTITY ) {
                expect(Tok_GENERIC_ENTITY, false, "constructed_types");
                t->genericEntity_ = true;
            }
            expect(Tok_SELECT, false, "constructed_types");
            t->kind = Type::SELECT;
            if( FIRST_select_list(la.d_type) || FIRST_select_extension(la.d_type) ) {
                if( FIRST_select_list(la.d_type) ) {
                    select_list(t);
                } else {
                    select_extension(t);
                }
            }
        } else
            invalid("constructed_types");
    } else if( la.d_type == Tok_ENUMERATION ) {
        expect(Tok_ENUMERATION, false, "constructed_types");
        t->kind = Type::ENUMERATION;
        if( la.d_type == Tok_OF || FIRST_enumeration_extension(la.d_type) ) {
            if( la.d_type == Tok_OF ) {
                expect(Tok_OF, false, "constructed_types");
                enumeration_items(t);
            } else {
                enumeration_extension(t);
            }
        }
    } else if( la.d_type == Tok_SELECT ) {
        expect(Tok_SELECT, false, "constructed_types");
        t->kind = Type::SELECT;
        if( FIRST_select_list(la.d_type) || FIRST_select_extension(la.d_type) ) {
            if( FIRST_select_list(la.d_type) ) {
                select_list(t);
            } else {
                select_extension(t);
            }
        }
    } else {
        invalid("constructed_types");
    }

    return t;
}

void Parser2::enumeration_items(Type* t)
{
    expect(Tok_Lpar, false, "enumeration_items");
    expect(Tok_ident, false, "enumeration_items");
    {
        // add to global scope and allow duplicates
        Declaration* e = addDecl(cur,Declaration::Enumerator, false);
        t->subs.append(e);
    }
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "enumeration_items");
        expect(Tok_ident, false, "enumeration_items");
        Declaration* e = addDecl(cur,Declaration::Enumerator, false);
        t->subs.append(e);
    }
    expect(Tok_Rpar, false, "enumeration_items");
}

void Parser2::enumeration_extension(Type* t)
{
    expect(Tok_BASED_ON, false, "enumeration_extension");
    expect(Tok_ident, false, "enumeration_extension");
    t->text = new QByteArray(cur.d_val);
    if( la.d_type == Tok_WITH ) {
        expect(Tok_WITH, false, "enumeration_extension");
        enumeration_items(t);
    }
}

void Parser2::select_list(Type* t)
{
    expect(Tok_Lpar, false, "select_list");
    expect(Tok_ident, false, "select_list");
    {
        Declaration* s = new Declaration();
        s->kind = Declaration::SelectItem;
        s->setName(cur.d_val);
        s->pos = cur.toRowCol();
        t->subs.append(s);
    }
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "select_list");
        expect(Tok_ident, false, "select_list");
        Declaration* s = new Declaration();
        s->kind = Declaration::SelectItem;
        s->setName(cur.d_val);
        s->pos = cur.toRowCol();
        t->subs.append(s);
    }
    expect(Tok_Rpar, false, "select_list");
}

void Parser2::select_extension(Type* t)
{
    expect(Tok_BASED_ON, false, "select_extension");
    expect(Tok_ident, false, "select_extension");
    t->text = new QByteArray(cur.d_val);
    if( la.d_type == Tok_WITH ) {
        expect(Tok_WITH, false, "select_extension");
        select_list(t);
    }
}

Type* Parser2::parameter_type()
{
    if( FIRST_generalized_types(la.d_type) ) {
        return generalized_types();
    } else if( la.d_type == Tok_ident ) {
        return named_types();
    } else if( FIRST_simple_types(la.d_type) ) {
        return simple_types();
    } else {
        invalid("parameter_type");
        return new Type();
    }
}

Type* Parser2::generalized_types()
{
    if( la.d_type == Tok_AGGREGATE ) {
        return aggregate_type();
    } else if( FIRST_general_aggregation_types(la.d_type) ) {
        return general_aggregation_types();
    } else if( la.d_type == Tok_GENERIC_ENTITY ) {
        return generic_entity_type();
    } else if( la.d_type == Tok_GENERIC ) {
        return generic_type();
    } else {
        invalid("generalized_types");
        return new Type();
    }
}

Type* Parser2::simple_types()
{
    Type* t = new Type();
    if( la.d_type == Tok_BINARY ) {
        expect(Tok_BINARY, false, "binary_type");
        t->kind = Type::BINARY;
        if( FIRST_width_spec(la.d_type) ) {
            expect(Tok_Lpar, false, "width_spec");
            t->expr = simple_expression();
            expect(Tok_Rpar, false, "width_spec");
            if( la.d_type == Tok_FIXED ) {
                expect(Tok_FIXED, false, "width_spec");
                t->fixed_ = true;
            }
        }
    } else if( la.d_type == Tok_BOOLEAN ) {
        expect(Tok_BOOLEAN, false, "boolean_type");
        t->kind = Type::BOOLEAN;
    } else if( la.d_type == Tok_INTEGER ) {
        expect(Tok_INTEGER, false, "integer_type");
        t->kind = Type::INTEGER;
    } else if( la.d_type == Tok_LOGICAL ) {
        expect(Tok_LOGICAL, false, "logical_type");
        t->kind = Type::LOGICAL;
    } else if( la.d_type == Tok_NUMBER ) {
        expect(Tok_NUMBER, false, "number_type");
        t->kind = Type::NUMBER;
    } else if( la.d_type == Tok_REAL ) {
        expect(Tok_REAL, false, "real_type");
        t->kind = Type::REAL;
        if( la.d_type == Tok_Lpar ) {
            expect(Tok_Lpar, false, "real_type");
            t->expr = simple_expression();
            expect(Tok_Rpar, false, "real_type");
        }
    } else if( la.d_type == Tok_STRING ) {
        expect(Tok_STRING, false, "string_type");
        t->kind = Type::STRING;
        if( FIRST_width_spec(la.d_type) ) {
            expect(Tok_Lpar, false, "width_spec");
            t->expr = simple_expression();
            expect(Tok_Rpar, false, "width_spec");
            if( la.d_type == Tok_FIXED ) {
                expect(Tok_FIXED, false, "width_spec");
                t->fixed_ = true;
            }
        }
    } else {
        invalid("simple_types");
    }
    return t;
}

Type* Parser2::aggregation_types()
{
    if( la.d_type == Tok_ARRAY ) {
        return array_type();
    } else if( la.d_type == Tok_BAG ) {
        return bag_type();
    } else if( la.d_type == Tok_LIST ) {
        return list_type();
    } else if( la.d_type == Tok_SET ) {
        return set_type();
    } else {
        invalid("aggregation_types");
        return new Type();
    }
}

Type* Parser2::array_type()
{
    Type* t = new Type();
    t->kind = Type::ARRAY;
    expect(Tok_ARRAY, false, "array_type");
    t->pos = cur.toRowCol();
    bound_spec(t);
    expect(Tok_OF, false, "array_type");
    if( la.d_type == Tok_OPTIONAL ) {
        expect(Tok_OPTIONAL, false, "array_type");
        t->optional_ = true;
    }
    if( la.d_type == Tok_UNIQUE ) {
        expect(Tok_UNIQUE, false, "array_type");
        t->unique_ = true;
    }
    Type* base = instantiable_type();
    t->setType(base);
    return t;
}

Type* Parser2::bag_type()
{
    Type* t = new Type();
    t->kind = Type::BAG;
    expect(Tok_BAG, false, "bag_type");
    t->pos = cur.toRowCol();
    if( FIRST_bound_spec(la.d_type) ) {
        bound_spec(t);
    }
    expect(Tok_OF, false, "bag_type");
    Type* base = instantiable_type();
    t->setType(base);
    return t;
}

Type* Parser2::list_type()
{
    Type* t = new Type();
    t->kind = Type::LIST;
    expect(Tok_LIST, false, "list_type");
    t->pos = cur.toRowCol();
    if( FIRST_bound_spec(la.d_type) ) {
        bound_spec(t);
    }
    expect(Tok_OF, false, "list_type");
    if( la.d_type == Tok_UNIQUE ) {
        expect(Tok_UNIQUE, false, "list_type");
        t->unique_ = true;
    }
    Type* base = instantiable_type();
    t->setType(base);
    return t;
}

Type* Parser2::set_type()
{
    Type* t = new Type();
    t->kind = Type::SET;
    expect(Tok_SET, false, "set_type");
    t->pos = cur.toRowCol();
    if( FIRST_bound_spec(la.d_type) ) {
        bound_spec(t);
    }
    expect(Tok_OF, false, "set_type");
    Type* base = instantiable_type();
    t->setType(base);
    return t;
}

void Parser2::bound_spec(Type* t)
{
    expect(Tok_Lbrack, false, "bound_spec");
    t->expr = simple_expression();
    expect(Tok_Colon, false, "bound_spec");
    t->expr->next = simple_expression();
    expect(Tok_Rbrack, false, "bound_spec");
}

Type* Parser2::instantiable_type()
{
    return concrete_types();
}

Type* Parser2::general_aggregation_types()
{
    if( la.d_type == Tok_ARRAY ) {
        return general_array_type();
    } else if( la.d_type == Tok_BAG ) {
        return general_bag_type();
    } else if( la.d_type == Tok_LIST ) {
        return general_list_type();
    } else if( la.d_type == Tok_SET ) {
        return general_set_type();
    } else {
        invalid("general_aggregation_types");
        return new Type();
    }
}

Type* Parser2::general_array_type()
{
    Type* t = new Type();
    t->kind = Type::ARRAY;
    expect(Tok_ARRAY, false, "general_array_type");
    t->pos = cur.toRowCol();
    if( FIRST_bound_spec(la.d_type) ) {
        bound_spec(t);
    }
    expect(Tok_OF, false, "general_array_type");
    if( la.d_type == Tok_OPTIONAL ) {
        expect(Tok_OPTIONAL, false, "general_array_type");
        t->optional_ = true;
    }
    if( la.d_type == Tok_UNIQUE ) {
        expect(Tok_UNIQUE, false, "general_array_type");
        t->unique_ = true;
    }
    Type* base = parameter_type();
    t->setType(base);
    return t;
}

Type* Parser2::general_bag_type()
{
    Type* t = new Type();
    t->kind = Type::BAG;
    expect(Tok_BAG, false, "general_bag_type");
    t->pos = cur.toRowCol();
    if( FIRST_bound_spec(la.d_type) ) {
        bound_spec(t);
    }
    expect(Tok_OF, false, "general_bag_type");
    Type* base = parameter_type();
    t->setType(base);
    return t;
}

Type* Parser2::general_list_type()
{
    Type* t = new Type();
    t->kind = Type::LIST;
    expect(Tok_LIST, false, "general_list_type");
    t->pos = cur.toRowCol();
    if( FIRST_bound_spec(la.d_type) ) {
        bound_spec(t);
    }
    expect(Tok_OF, false, "general_list_type");
    if( la.d_type == Tok_UNIQUE ) {
        expect(Tok_UNIQUE, false, "general_list_type");
        t->unique_ = true;
    }
    Type* base = parameter_type();
    t->setType(base);
    return t;
}

Type* Parser2::general_set_type()
{
    Type* t = new Type();
    t->kind = Type::SET;
    expect(Tok_SET, false, "general_set_type");
    t->pos = cur.toRowCol();
    if( FIRST_bound_spec(la.d_type) ) {
        bound_spec(t);
    }
    expect(Tok_OF, false, "general_set_type");
    Type* base = parameter_type();
    t->setType(base);
    return t;
}

Type* Parser2::aggregate_type()
{
    Type* t = new Type();
    t->kind = Type::AGGREGATE;
    expect(Tok_AGGREGATE, false, "aggregate_type");
    t->pos = cur.toRowCol();
    if( la.d_type == Tok_Colon ) {
        expect(Tok_Colon, false, "aggregate_type");
        expect(Tok_ident, false, "aggregate_type");
        t->text = new QByteArray(cur.d_val);
    }
    expect(Tok_OF, false, "aggregate_type");
    Type* base = parameter_type();
    t->setType(base);
    return t;
}

Type* Parser2::generic_entity_type()
{
    Type* t = new Type();
    t->kind = Type::GENERIC_ENTITY;
    expect(Tok_GENERIC_ENTITY, false, "generic_entity_type");
    t->pos = cur.toRowCol();
    if( la.d_type == Tok_Colon ) {
        expect(Tok_Colon, false, "generic_entity_type");
        expect(Tok_ident, false, "generic_entity_type");
        t->text = new QByteArray(cur.d_val);
    }
    return t;
}

Type* Parser2::generic_type()
{
    Type* t = new Type();
    t->kind = Type::GENERIC;
    expect(Tok_GENERIC, false, "generic_type");
    t->pos = cur.toRowCol();
    if( la.d_type == Tok_Colon ) {
        expect(Tok_Colon, false, "generic_type");
        expect(Tok_ident, false, "generic_type");
        t->text = new QByteArray(cur.d_val);
    }
    return t;
}

Type* Parser2::named_types()
{
    expect(Tok_ident, false, "named_types");
    Type* t = new Type();
    t->kind = Type::NameRef;
    t->quali = new Quali(QByteArray(), cur.d_val);
    t->pos = cur.toRowCol();
    return t;
}

Declaration* Parser2::subtype_constraint_decl()
{
    expect(Tok_SUBTYPE_CONSTRAINT, false, "subtype_constraint_head");
    expect(Tok_ident, false, "subtype_constraint_head");
    Token name = cur;
    expect(Tok_FOR, false, "subtype_constraint_head");
    expect(Tok_ident, false, "subtype_constraint_head");
    Token forEntity = cur;
    expect(Tok_Semi, false, "subtype_constraint_head");

    Declaration* d = addDecl(name, Declaration::SubtypeConstraint);
    if( d )
    {
        d->data = forEntity.d_val;
        mdl->openScope(d);
        subtype_constraint_body(d);
        mdl->closeScope();
    }
    expect(Tok_END_SUBTYPE_CONSTRAINT, false, "subtype_constraint_decl");
    expect(Tok_Semi, false, "subtype_constraint_decl");
    return d;
}

void Parser2::subtype_constraint_body(Declaration* d)
{
    if( FIRST_abstract_supertype(la.d_type) ) {
        expect(Tok_ABSTRACT, false, "abstract_supertype");
        expect(Tok_SUPERTYPE, false, "abstract_supertype");
        expect(Tok_Semi, false, "abstract_supertype");
        d->abstract_ = true;
    }
    if( FIRST_total_over(la.d_type) ) {
        expect(Tok_TOTAL_OVER, false, "total_over");
        expect(Tok_Lpar, false, "total_over");
        expect(Tok_ident, false, "total_over");
        addPseudoMemberTo(cur, Declaration::TotalOver);
        while( la.d_type == Tok_Comma ) {
            expect(Tok_Comma, false, "total_over");
            expect(Tok_ident, false, "total_over");
            addPseudoMemberTo(cur, Declaration::TotalOver);
        }
        expect(Tok_Rpar, false, "total_over");
        expect(Tok_Semi, false, "total_over");
    }
    if( FIRST_supertype_expression(la.d_type) ) {
        d->expr = supertype_expression();
        expect(Tok_Semi, false, "subtype_constraint_body");
    }
}

Declaration* Parser2::function_decl()
{
    Declaration* func = 0;
    function_head(func);
    if( func ) {
        algorithm_head();
        Statement* first = stmt();
        Statement* block = new Statement(Statement::StatBlock, first ? first->pos : RowCol());
        block->body = first;
        while( FIRST_stmt(la.d_type) ) {
            Statement* s = stmt();
            if( s && first )
                first->append(s);
            else if( s )
                block->body = (first = s);
        }
        func->body = block;
        mdl->closeScope();
    } else {
        // error recovery: skip to END_FUNCTION
        // not ideal, but avoids cascading errors
    }
    expect(Tok_END_FUNCTION, false, "function_decl");
    expect(Tok_Semi, false, "function_decl");
    return func;
}

void Parser2::function_head(Declaration*& func)
{
    expect(Tok_FUNCTION, false, "function_head");
    expect(Tok_ident, false, "function_head");
    Token name = cur;

    func = addDecl(name, Declaration::Function);
    if( func )
        mdl->openScope(func);

    if( la.d_type == Tok_Lpar ) {
        expect(Tok_Lpar, false, "function_head");
        formal_parameter();
        while( la.d_type == Tok_Semi ) {
            expect(Tok_Semi, false, "function_head");
            formal_parameter();
        }
        expect(Tok_Rpar, false, "function_head");
    }
    expect(Tok_Colon, false, "function_head");
    Type* retType = parameter_type();
    if( func )
        func->setType(retType);
    else
        delete retType;
    expect(Tok_Semi, false, "function_head");
}

Declaration* Parser2::procedure_decl()
{
    Declaration* proc = 0;
    procedure_head(proc);
    if( proc ) {
        algorithm_head();
        Statement* first = 0;
        Statement* block = 0;
        while( FIRST_stmt(la.d_type) ) {
            Statement* s = stmt();
            if( s && first )
                first->append(s);
            else if( s ) {
                first = s;
            }
        }
        block = new Statement(Statement::StatBlock, first ? first->pos : RowCol());
        block->body = first;
        proc->body = block;
        mdl->closeScope();
    }
    expect(Tok_END_PROCEDURE, false, "procedure_decl");
    expect(Tok_Semi, false, "procedure_decl");
    return proc;
}

void Parser2::procedure_head(Declaration*& proc)
{
    expect(Tok_PROCEDURE, false, "procedure_head");
    expect(Tok_ident, false, "procedure_head");
    Token name = cur;

    proc = addDecl(name, Declaration::Procedure);
    if( proc )
        mdl->openScope(proc);

    if( la.d_type == Tok_Lpar ) {
        expect(Tok_Lpar, false, "procedure_head");
        bool isVar = false;
        if( la.d_type == Tok_VAR ) {
            expect(Tok_VAR, false, "procedure_head");
            isVar = true;
        }
        formal_parameter(isVar);
        while( la.d_type == Tok_Semi ) {
            expect(Tok_Semi, false, "procedure_head");
            isVar = false;
            if( la.d_type == Tok_VAR ) {
                expect(Tok_VAR, false, "procedure_head");
                isVar = true;
            }
            formal_parameter(isVar);
        }
        expect(Tok_Rpar, false, "procedure_head");
    }
    expect(Tok_Semi, false, "procedure_head");
}

void Parser2::formal_parameter(bool isVar)
{
    QList<Token> names;
    expect(Tok_ident, false, "formal_parameter");
    names.append(cur);
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "formal_parameter");
        expect(Tok_ident, false, "formal_parameter");
        names.append(cur);
    }
    expect(Tok_Colon, false, "formal_parameter");
    Type* t = parameter_type();

    foreach( const Token& n, names ) {
        Declaration* p = addDecl(n, Declaration::ParamDecl);
        if( p ) {
            p->setType(t);
            p->varParam = isVar;
        }
    }
    if( names.isEmpty() )
        delete t;
}

void Parser2::algorithm_head()
{
    while( FIRST_declaration(la.d_type) ) {
        declaration();
    }
    if( FIRST_constant_decl(la.d_type) ) {
        constant_decl();
    }
    if( FIRST_local_decl(la.d_type) ) {
        local_decl();
    }
}

void Parser2::local_decl()
{
    expect(Tok_LOCAL, false, "local_decl");
    local_variable();
    while( FIRST_local_variable(la.d_type) ) {
        local_variable();
    }
    expect(Tok_END_LOCAL, false, "local_decl");
    expect(Tok_Semi, false, "local_decl");
}

void Parser2::local_variable()
{
    QList<Token> names;
    expect(Tok_ident, false, "local_variable");
    names.append(cur);
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "local_variable");
        expect(Tok_ident, false, "local_variable");
        names.append(cur);
    }
    expect(Tok_Colon, false, "local_variable");
    Type* t = parameter_type();

    Expression* init = 0;
    if( la.d_type == Tok_ColonEq ) {
        expect(Tok_ColonEq, false, "local_variable");
        init = expression();
    }

    foreach( const Token& n, names ) {
        Declaration* lv = addDecl(n, Declaration::LocalDecl);
        if( lv ) {
            lv->setType(t);
            lv->expr = init;
            init = 0; // only first variable owns the init expression
        }
    }
    if( init )
        delete init;
    if( names.isEmpty() ) {
        delete t;
        delete init;
    }

    expect(Tok_Semi, false, "local_variable");
}

Declaration* Parser2::rule_decl()
{
    Declaration* rule = rule_head();
    if( rule == 0 )
        return 0;

    algorithm_head();
    Statement* first = 0;
    while( FIRST_stmt(la.d_type) ) {
        Statement* s = stmt();
        if( s && first )
            first->append(s);
        else if( s )
            first = s;
    }
    Statement* block = new Statement(Statement::StatBlock, first ? first->pos : RowCol());
    block->body = first;
    rule->body = block;

    where_clause();
    mdl->closeScope();

    expect(Tok_END_RULE, false, "rule_decl");
    expect(Tok_Semi, false, "rule_decl");
    return rule;
}

Declaration* Parser2::rule_head()
{
    expect(Tok_RULE, false, "rule_head");
    expect(Tok_ident, false, "rule_head");
    Token name = cur;
    expect(Tok_FOR, false, "rule_head");
    expect(Tok_Lpar, false, "rule_head");

    Declaration* rule = addDecl(name, Declaration::Rule);
    if( rule == 0 )
        return 0;
    mdl->openScope(rule);

    expect(Tok_ident, false, "rule_head");
    addPseudoMemberTo(cur, Declaration::RuleFor);
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "rule_head");
        expect(Tok_ident, false, "rule_head");
        if( rule )
            addPseudoMemberTo(cur, Declaration::RuleFor);
    }
    expect(Tok_Rpar, false, "rule_head");
    expect(Tok_Semi, false, "rule_head");
    return rule;
}

Statement* Parser2::stmt()
{
    if( FIRST_alias_stmt(la.d_type) ) {
        return alias_stmt();
    } else if( FIRST_assignOrCall_(la.d_type) ) {
        Statement* s = assignOrCall_();
        expect(Tok_Semi, false, "stmt");
        return s;
    } else if( FIRST_case_stmt(la.d_type) ) {
        return case_stmt();
    } else if( FIRST_compound_stmt(la.d_type) ) {
        return compound_stmt();
    } else if( FIRST_escape_stmt(la.d_type) ) {
        Statement* s = new Statement(Statement::Escape, la.toRowCol());
        expect(Tok_ESCAPE, false, "escape_stmt");
        expect(Tok_Semi, false, "escape_stmt");
        return s;
    } else if( FIRST_if_stmt(la.d_type) ) {
        return if_stmt();
    } else if( FIRST_null_stmt(la.d_type) ) {
        Statement* s = new Statement(Statement::Null, la.toRowCol());
        expect(Tok_Semi, false, "null_stmt");
        return s;
    } else if( FIRST_repeat_stmt(la.d_type) ) {
        return repeat_stmt();
    } else if( FIRST_return_stmt(la.d_type) ) {
        return return_stmt();
    } else if( FIRST_skip_stmt(la.d_type) ) {
        Statement* s = new Statement(Statement::Skip, la.toRowCol());
        expect(Tok_SKIP, false, "skip_stmt");
        expect(Tok_Semi, false, "skip_stmt");
        return s;
    } else {
        invalid("stmt");
        return 0;
    }
}

Statement* Parser2::assignOrCall_()
{
    if( la.d_type == Tok_ident ) {
        expect(Tok_ident, false, "assignOrCall_");
        Expression* e = new Expression(Expression::NameRef, cur.toRowCol());
        e->val = cur.d_val;

        while( FIRST_qualifier(la.d_type) ) {
            qualifier(e);
        }

        if( la.d_type == Tok_ColonEq ) {
            // assignment
            expect(Tok_ColonEq, false, "assignOrCall_");
            Statement* s = new Statement(Statement::Assig, e->pos);
            s->lhs = e;
            s->rhs = expression();
            return s;
        } else {
            // procedure call (or entity constructor call)
            Statement* s = new Statement(Statement::Call, e->pos);
            s->lhs = e;
            return s;
        }
    } else if( FIRST_built_in_procedure(la.d_type) ) {
        // INSERT or REMOVE
        Expression* e = new Expression(Expression::NameRef, la.toRowCol());
        if( la.d_type == Tok_INSERT ) {
            expect(Tok_INSERT, false, "built_in_procedure");
            e->val = QByteArray("INSERT");
        } else {
            expect(Tok_REMOVE, false, "built_in_procedure");
            e->val = QByteArray("REMOVE");
        }

        if( la.d_type == Tok_Lpar ) {
            Expression* args = actual_parameter_list();
            Expression* call = new Expression(Expression::Call, e->pos);
            call->lhs = e;
            call->rhs = args;
            e = call;
        }

        Statement* s = new Statement(Statement::Call, e->pos);
        s->lhs = e;
        return s;
    } else {
        invalid("assignOrCall_");
        return 0;
    }
}

Statement* Parser2::alias_stmt()
{
    Statement* s = new Statement(Statement::Alias, la.toRowCol());
    expect(Tok_ALIAS, false, "alias_stmt");
    expect(Tok_ident, false, "alias_stmt");
    Token aliasName = cur;
    expect(Tok_FOR, false, "alias_stmt");
    expect(Tok_ident, false, "alias_stmt");

    // create alias declaration in scope
    Declaration* ad = addDecl(aliasName, Declaration::AliasDecl);

    Expression* target = new Expression(Expression::NameRef, cur.toRowCol());
    target->val = cur.d_val;
    while( FIRST_qualifier(la.d_type) ) {
        qualifier(target);
    }
    s->rhs = target;

    expect(Tok_Semi, false, "alias_stmt");

    if( ad )
        mdl->openScope(ad);

    Statement* first = stmt();
    s->body = first;
    while( FIRST_stmt(la.d_type) ) {
        Statement* ns = stmt();
        if( ns && first )
            first->append(ns);
        else if( ns )
            s->body = (first = ns);
    }

    if( ad )
        mdl->closeScope();

    expect(Tok_END_ALIAS, false, "alias_stmt");
    expect(Tok_Semi, false, "alias_stmt");
    return s;
}

Statement* Parser2::case_stmt()
{
    Statement* s = new Statement(Statement::Case, la.toRowCol());
    expect(Tok_CASE, false, "case_stmt");
    s->lhs = expression();
    expect(Tok_OF, false, "case_stmt");

    Statement* firstAction = 0;
    while( FIRST_case_action(la.d_type) ) {
        Statement* ca = case_action();
        if( ca && firstAction )
            firstAction->append(ca);
        else if( ca )
            firstAction = ca;
    }
    s->body = firstAction;

    if( la.d_type == Tok_OTHERWISE ) {
        expect(Tok_OTHERWISE, false, "case_stmt");
        expect(Tok_Colon, false, "case_stmt");
        Statement* otherwise = new Statement(Statement::CaseAction, cur.toRowCol());
        otherwise->body = stmt();
        if( firstAction )
            firstAction->append(otherwise);
        else
            s->body = otherwise;
    }

    expect(Tok_END_CASE, false, "case_stmt");
    expect(Tok_Semi, false, "case_stmt");
    return s;
}

Statement* Parser2::case_action()
{
    Statement* ca = new Statement(Statement::CaseAction, la.toRowCol());
    // case labels (comma-separated expressions)
    Expression* first = expression();
    ca->rhs = first;
    while( la.d_type == Tok_Comma ) {
        expect(Tok_Comma, false, "case_action");
        Expression* label = expression();
        Expression::append(first, label);
    }
    expect(Tok_Colon, false, "case_action");
    ca->body = stmt();
    return ca;
}

Statement* Parser2::compound_stmt()
{
    Statement* s = new Statement(Statement::Compound, la.toRowCol());
    expect(Tok_BEGIN, false, "compound_stmt");
    Statement* first = stmt();
    s->body = first;
    while( FIRST_stmt(la.d_type) ) {
        Statement* ns = stmt();
        if( ns && first )
            first->append(ns);
        else if( ns )
            s->body = (first = ns);
    }
    expect(Tok_END, false, "compound_stmt");
    expect(Tok_Semi, false, "compound_stmt");
    return s;
}

Statement* Parser2::if_stmt()
{
    Statement* s = new Statement(Statement::If, la.toRowCol());
    expect(Tok_IF, false, "if_stmt");
    s->rhs = expression();
    expect(Tok_THEN, false, "if_stmt");

    Statement* first = stmt();
    s->body = first;
    while( FIRST_stmt(la.d_type) ) {
        Statement* ns = stmt();
        if( ns && first )
            first->append(ns);
        else if( ns )
            s->body = (first = ns);
    }

    if( la.d_type == Tok_ELSE ) {
        expect(Tok_ELSE, false, "if_stmt");
        Statement* elseStmt = new Statement(Statement::Else, cur.toRowCol());
        Statement* ef = stmt();
        elseStmt->body = ef;
        while( FIRST_stmt(la.d_type) ) {
            Statement* ns = stmt();
            if( ns && ef )
                ef->append(ns);
            else if( ns )
                elseStmt->body = (ef = ns);
        }
        // chain else as sibling of the if node (via next), not mixed into then-body
        s->append(elseStmt);
    }

    expect(Tok_END_IF, false, "if_stmt");
    expect(Tok_Semi, false, "if_stmt");
    return s;
}

Statement* Parser2::repeat_stmt()
{
    Statement* s = new Statement(Statement::Repeat, la.toRowCol());
    expect(Tok_REPEAT, false, "repeat_stmt");

    // repeat_control: [increment_control] [while_control] [until_control]
    if( FIRST_increment_control(la.d_type) ) {
        // increment_control: ident ':=' bound_1 TO bound_2 [ BY numeric_expression ]
        expect(Tok_ident, false, "increment_control");
        Expression* var = new Expression(Expression::NameRef, cur.toRowCol());
        var->val = cur.d_val;
        s->lhs = var;
        expect(Tok_ColonEq, false, "increment_control");
        Expression* from = simple_expression();
        expect(Tok_TO, false, "increment_control");
        Expression* to = simple_expression();
        // store from/to/by as linked list in rhs
        var->lhs = from;
        var->rhs = to;
        if( la.d_type == Tok_BY ) {
            expect(Tok_BY, false, "increment_control");
            to->next = simple_expression();
        }
    }
    if( FIRST_while_control(la.d_type) ) {
        expect(Tok_WHILE, false, "while_control");
        Expression* cond = expression();
        if( s->rhs == 0 )
            s->rhs = cond;
        else {
            // chain after existing
            Expression* r = s->rhs;
            while( r->next ) r = r->next;
            r->next = cond;
        }
    }
    if( FIRST_until_control(la.d_type) ) {
        expect(Tok_UNTIL, false, "until_control");
        Expression* cond = expression();
        cond->byVal = true; // mark as UNTIL (vs WHILE)
        if( s->rhs == 0 )
            s->rhs = cond;
        else {
            Expression* r = s->rhs;
            while( r->next ) r = r->next;
            r->next = cond;
        }
    }

    expect(Tok_Semi, false, "repeat_stmt");

    Statement* first = stmt();
    s->body = first;
    while( FIRST_stmt(la.d_type) ) {
        Statement* ns = stmt();
        if( ns && first )
            first->append(ns);
        else if( ns )
            s->body = (first = ns);
    }

    expect(Tok_END_REPEAT, false, "repeat_stmt");
    expect(Tok_Semi, false, "repeat_stmt");
    return s;
}

Statement* Parser2::return_stmt()
{
    Statement* s = new Statement(Statement::Return, la.toRowCol());
    expect(Tok_RETURN, false, "return_stmt");
    if( la.d_type == Tok_Lpar ) {
        expect(Tok_Lpar, false, "return_stmt");
        s->rhs = expression();
        expect(Tok_Rpar, false, "return_stmt");
    }
    expect(Tok_Semi, false, "return_stmt");
    return s;
}

Expression* Parser2::expression()
{
    Expression* e = and_expression();
    while( la.d_type == Tok_OR ) {
        Expression* bin = new Expression(Expression::Or, la.toRowCol());
        expect(Tok_OR, false, "expression");
        bin->lhs = e;
        bin->rhs = and_expression();
        e = bin;
    }
    return e;
}

Expression* Parser2::and_expression()
{
    Expression* e = rel_expression();
    while( la.d_type == Tok_AND ) {
        Expression* bin = new Expression(Expression::And, la.toRowCol());
        expect(Tok_AND, false, "and_expression");
        bin->lhs = e;
        bin->rhs = rel_expression();
        e = bin;
    }
    return e;
}

Expression* Parser2::rel_expression()
{
    Expression* e = simple_expression();
    if( FIRST_rel_op_extended(la.d_type) ) {
        Expression::Kind k = Expression::Invalid;
        switch( la.d_type ) {
        case Tok_Lt:
            k = Expression::Lt;
            break;
        case Tok_Gt:
            k = Expression::Gt;
            break;
        case Tok_Leq:
            k = Expression::Leq;
            break;
        case Tok_Geq:
            k = Expression::Geq;
            break;
        case Tok_LtGt:
            k = Expression::Neq;
            break;
        case Tok_Eq:
            k = Expression::Eq;
            break;
        case Tok_ColonLtGtColon:
            k = Expression::InstNeq;
            break;
        case Tok_ColonEqColon:
            k = Expression::InstEq;
            break;
        case Tok_IN:
            k = Expression::In;
            break;
        case Tok_LIKE:
            k = Expression::Like;
            break;
        default:
            break;
        }
        Expression* bin = new Expression(k, la.toRowCol());
        next(); // consume the relational operator
        bin->lhs = e;
        bin->rhs = simple_expression();
        e = bin;
    }
    return e;
}

Expression* Parser2::simple_expression()
{
    Expression* e = term();
    while( FIRST_add_like_op(la.d_type) ) {
        Expression::Kind k = Expression::Invalid;
        switch( la.d_type ) {
        case Tok_Plus:
            k = Expression::Add; break;
        case Tok_Minus:
            k = Expression::Sub; break;
        case Tok_XOR:
            k = Expression::Xor; break;
        default:
            break;
        }
        Expression* bin = new Expression(k, la.toRowCol());
        next();
        bin->lhs = e;
        bin->rhs = term();
        e = bin;
    }
    return e;
}

Expression* Parser2::term()
{
    Expression* e = factor();
    while( FIRST_multiplication_like_op(la.d_type) ) {
        Expression::Kind k = Expression::Invalid;
        switch( la.d_type ) {
        case Tok_Star:
            k = Expression::Mul; break;
        case Tok_Slash:
            k = Expression::Fdiv; break;
        case Tok_DIV:
            k = Expression::Div; break;
        case Tok_MOD:
            k = Expression::Mod; break;
        case Tok_2Bar:
            k = Expression::Combine; break;
        default:
            break;
        }
        Expression* bin = new Expression(k, la.toRowCol());
        next();
        bin->lhs = e;
        bin->rhs = factor();
        e = bin;
    }
    return e;
}

Expression* Parser2::factor()
{
    Expression* e = simple_factor();
    if( la.d_type == Tok_2Star ) {
        Expression* bin = new Expression(Expression::Power, la.toRowCol());
        expect(Tok_2Star, false, "factor");
        bin->lhs = e;
        bin->rhs = simple_factor();
        e = bin;
    }
    return e;
}

Expression* Parser2::simple_factor()
{
    if( FIRST_aggregate_initializer(la.d_type) ) {
        return aggregate_initializer();
    } else if( FIRST_interval(la.d_type) ) {
        return interval();
    } else if( FIRST_query_expression(la.d_type) ) {
        return query_expression();
    } else if( FIRST_unary_op(la.d_type) ) {
        Expression::Kind k = Expression::Invalid;
        switch( la.d_type ) {
        case Tok_Plus:
            k = Expression::Plus;
            break;
        case Tok_Minus:
            k = Expression::Minus;
            break;
        case Tok_NOT:
            k = Expression::Not;
            break;
        default:
            break;
        }
        Expression* unary = new Expression(k, la.toRowCol());
        next(); // consume unary op

        if( la.d_type == Tok_Lpar ) {
            expect(Tok_Lpar, false, "simple_factor");
            unary->lhs = expression();
            expect(Tok_Rpar, false, "simple_factor");
        } else if( FIRST_primary(la.d_type) ) {
            unary->lhs = primary();
        } else {
            invalid("simple_factor");
        }
        return unary;
    } else if( la.d_type == Tok_Lpar ) {
        expect(Tok_Lpar, false, "simple_factor");
        Expression* e = expression();
        expect(Tok_Rpar, false, "simple_factor");
        return e;
    } else if( FIRST_primary(la.d_type) ) {
        return primary();
    } else {
        invalid("simple_factor");
        return new Expression(Expression::Invalid, la.toRowCol());
    }
}

Expression* Parser2::primary()
{
    if( FIRST_literal(la.d_type) ) {
        // literal
        Expression* e = new Expression(Expression::Literal, la.toRowCol());
        switch( la.d_type ) {
        case Tok_integer_literal:
            next();
            e->val = cur.d_val.toLongLong();
            break;
        case Tok_real_literal:
            next();
            e->val = cur.d_val.toDouble();
            break;
        case Tok_simple_string_literal:
        case Tok_encoded_string_literal:
            next();
            e->val = cur.d_val;
            break;
        case Tok_binary_literal:
            next();
            e->val = cur.d_val;
            break;
        case Tok_TRUE:
            next();
            e->val = true;
            break;
        case Tok_FALSE:
            next();
            e->val = false;
            break;
        case Tok_UNKNOWN:
            next();
            e->val = QVariant(); // indeterminate
            break;
        default:
            next();
            break;
        }
        return e;
    } else if( FIRST_built_in_constant(la.d_type) ) {
        Expression* e = new Expression(Expression::NameRef, la.toRowCol());
        switch( la.d_type ) {
        case Tok_CONST_E:
            next(); e->val = QByteArray("CONST_E");
            break;
        case Tok_PI:
            next(); e->val = QByteArray("PI");
            break;
        case Tok_SELF:
            next(); e->kind = Expression::Self;
            break;
        case Tok_Qmark:
            next(); e->val = QByteArray("?");
            break;
        default:
            next();
            break;
        }
        while( FIRST_qualifier(la.d_type) )
            qualifier(e);
        return e;
    } else if( FIRST_built_in_function(la.d_type) ) {
        Expression* e = new Expression(Expression::NameRef, la.toRowCol());
        e->val = QByteArray(tokenTypeString(la.d_type));
        next();
        while( FIRST_qualifier(la.d_type) )
            qualifier(e);
        return e;
    } else if( la.d_type == Tok_ident ) {
        expect(Tok_ident, false, "primary");
        Expression* e = new Expression(Expression::NameRef, cur.toRowCol());
        e->val = cur.d_val;
        while( FIRST_qualifier(la.d_type) )
            qualifier(e);
        return e;
    } else {
        invalid("primary");
        return new Expression(Expression::Invalid, la.toRowCol());
    }
}

void Parser2::qualifier(Expression*& e)
{
    if( la.d_type == Tok_Dot ) {
        // attribute_qualifier
        expect(Tok_Dot, false, "attribute_qualifier");
        expect(Tok_ident, false, "attribute_qualifier");
        Expression* sel = new Expression(Expression::Select, cur.toRowCol());
        sel->val = cur.d_val;
        sel->lhs = e;
        e = sel;
    } else if( la.d_type == Tok_Bslash ) {
        // group_qualifier
        expect(Tok_Bslash, false, "group_qualifier");
        expect(Tok_ident, false, "group_qualifier");
        Expression* grp = new Expression(Expression::GroupRef, cur.toRowCol());
        grp->val = cur.d_val;
        grp->lhs = e;
        e = grp;
    } else if( la.d_type == Tok_Lbrack ) {
        // index_qualifier
        expect(Tok_Lbrack, false, "index_qualifier");
        Expression* idx = new Expression(Expression::Index, la.toRowCol());
        idx->lhs = e;
        idx->rhs = simple_expression();
        if( la.d_type == Tok_Colon ) {
            expect(Tok_Colon, false, "index_qualifier");
            idx->rhs->next = simple_expression();
        }
        expect(Tok_Rbrack, false, "index_qualifier");
        e = idx;
    } else if( la.d_type == Tok_Lpar ) {
        // actual_parameter_list (function call / entity constructor)
        Expression* args = actual_parameter_list();
        Expression* call = new Expression(Expression::Call, e->pos);
        call->lhs = e;
        call->rhs = args;
        e = call;
    } else
        invalid("qualifier");
}

Expression* Parser2::actual_parameter_list()
{
    expect(Tok_Lpar, false, "actual_parameter_list");
    Expression* first = 0;
    if( FIRST_expression(la.d_type) ) {
        first = expression();
        Expression* prev = first;
        while( la.d_type == Tok_Comma ) {
            expect(Tok_Comma, false, "actual_parameter_list");
            Expression* arg = expression();
            prev->next = arg;
            prev = arg;
        }
    }
    expect(Tok_Rpar, false, "actual_parameter_list");
    return first;
}

Expression* Parser2::aggregate_initializer()
{
    Expression* e = new Expression(Expression::AggInit, la.toRowCol());
    expect(Tok_Lbrack, false, "aggregate_initializer");
    if( FIRST_element(la.d_type) ) {
        // element: expression [ ':' repetition ]
        Expression* elem = expression();
        if( la.d_type == Tok_Colon ) {
            expect(Tok_Colon, false, "element");
            Expression* rep = new Expression(Expression::Range, elem->pos);
            rep->lhs = elem;
            rep->rhs = simple_expression();
            elem = rep;
        }
        e->lhs = elem;
        while( la.d_type == Tok_Comma ) {
            expect(Tok_Comma, false, "aggregate_initializer");
            Expression* elem2 = expression();
            if( la.d_type == Tok_Colon ) {
                expect(Tok_Colon, false, "element");
                Expression* rep = new Expression(Expression::Range, elem2->pos);
                rep->lhs = elem2;
                rep->rhs = simple_expression();
                elem2 = rep;
            }
            Expression::append(elem, elem2);
        }
    }
    expect(Tok_Rbrack, false, "aggregate_initializer");
    return e;
}

Expression* Parser2::interval()
{
    Expression* e = new Expression(Expression::Interval, la.toRowCol());
    expect(Tok_Lbrace, false, "interval");
    Expression* low = simple_expression();

    // interval_op 1
    Expression::Kind op1 = Expression::Lt;
    if( la.d_type == Tok_Lt ) {
        expect(Tok_Lt, false, "interval_op");
        op1 = Expression::Lt;
    } else if( la.d_type == Tok_Leq ) {
        expect(Tok_Leq, false, "interval_op");
        op1 = Expression::Leq;
    } else
        invalid("interval_op");

    Expression* mid = simple_expression();

    // interval_op 2
    Expression::Kind op2 = Expression::Lt;
    if( la.d_type == Tok_Lt ) {
        expect(Tok_Lt, false, "interval_op");
        op2 = Expression::Lt;
    } else if( la.d_type == Tok_Leq ) {
        expect(Tok_Leq, false, "interval_op");
        op2 = Expression::Leq;
    } else
        invalid("interval_op");

    Expression* high = simple_expression();

    // encode: lhs = low, rhs = mid (with mid->next = high)
    // use val to store operators
    e->lhs = low;
    e->rhs = mid;
    mid->next = high;
    // store op kinds in val as pair
    QVariantList ops;
    ops << (int)op1 << (int)op2;
    e->val = ops;

    expect(Tok_Rbrace, false, "interval");
    return e;
}

Expression* Parser2::query_expression()
{
    Expression* e = new Expression(Expression::Query, la.toRowCol());
    expect(Tok_QUERY, false, "query_expression");
    expect(Tok_Lpar, false, "query_expression");
    expect(Tok_ident, false, "query_expression");

    // create a NameRef for the query variable
    Expression* var = new Expression(Expression::NameRef, cur.toRowCol());
    var->val = cur.d_val;
    e->lhs = var;

    expect(Tok_LtStar, false, "query_expression");
    var->rhs = simple_expression(); // the aggregate source

    expect(Tok_Bar, false, "query_expression");
    e->rhs = expression(); // the condition

    expect(Tok_Rpar, false, "query_expression");
    return e;
}

void Parser2::where_clause()
{
    expect(Tok_WHERE, false, "where_clause");
    domain_rule();
    expect(Tok_Semi, false, "where_clause");
    while( FIRST_domain_rule(la.d_type) ) {
        domain_rule();
        expect(Tok_Semi, false, "where_clause");
    }
}

void Parser2::domain_rule()
{
    // optional label
    if( peek(1).d_type == Tok_ident && peek(2).d_type == Tok_Colon ) {
        expect(Tok_ident, false, "domain_rule");
        // label name, not stored explicitly
        expect(Tok_Colon, false, "domain_rule");
    }
    Expression* e = expression();
    delete e; // TODO where clause expressions are not stored in the simple AST for now
}
