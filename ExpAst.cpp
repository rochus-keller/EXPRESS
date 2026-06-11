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

#include "ExpAst.h"
#include "ExpToken.h"
#include <QtDebug>
using namespace Exp;
using namespace Ast;

Declaration* AstModel::globalScope = 0;
Type* AstModel::types[Type::MaxKind] = {0};

const char* Type::name[] = {
    "Undefined",
    "NoType",
    "BINARY",
    "BOOLEAN",
    "INTEGER",
    "LOGICAL",
    "NUMBER",
    "REAL",
    "STRING",
    "ARRAY",
    "BAG",
    "LIST",
    "SET",
    "ENUMERATION",
    "SELECT",
    "AGGREGATE",
    "GENERIC",
    "GENERIC_ENTITY",
    "NameRef",
};

const char* Declaration::s_name[] = {
    "Invalid",
    "Scope",
    "Schema",
    "Entity",
    "TypeDecl",
    "ConstDecl",
    "Attribute",
    "Function",
    "Procedure",
    "Rule",
    "ParamDecl",
    "LocalDecl",
    "AliasDecl",
    "Enumerator",
    "SelectItem",
    "Import",
    "SubtypeConstraint",
};

// Node

void Node::setType(Type* t)
{
    _ty = t;
}

Node::~Node()
{
    // type ownership is managed elsewhere (ownstype flag on Declaration, or by AstModel for basic types)
}

// Type

Type::~Type()
{
    switch(kind) {
    case NameRef:
        delete quali;
        break;
    case BINARY:
    case STRING:
    case REAL:
        delete expr;
        break;  // single expression
    case ARRAY:
    case BAG:
    case LIST:
    case SET:
        if(expr)
            delete expr;
        break;
    case ENUMERATION:
    case SELECT:
    case AGGREGATE:
    case GENERIC:
    case GENERIC_ENTITY:
        delete text;
        break;
    default:
        break;
    }
    // TODO: subs are not owned by Type unless they are anonymous
    // decl is not owned
}

Declaration::~Declaration()
{
    if( ownstype && type() )
        delete type();
    if( link )
    {
        Declaration* cur = link;
        while( cur )
        {
            Declaration* next = cur->next;
            delete cur;
            cur = next;
        }
    }
    if( body )
        Statement::deleteAll(body);
    if( (kind == ConstDecl || kind == Entity || kind == SubtypeConstraint) && expr )
        delete expr;
}

void Declaration::deleteAll(Declaration* d)
{
    if( d == 0 )
        return;
    // delete members
    Declaration* cur = d->link;
    d->link = 0;
    while( cur )
    {
        Declaration* next = cur->next;
        cur->next = 0;
        cur->link = 0; // prevent recursive member deletion twice
        delete cur;
        cur = next;
    }
    delete d;
}

void Declaration::appendMember(Declaration* d)
{
    Q_ASSERT(d);
    if( link == 0 )
    {
        link = d;
    }
    else
    {
        Declaration* last = link;
        while( last->next )
            last = last->next;
        last->next = d;
        d->inList = true;
    }
    d->outer = this;
}

Declaration* Declaration::getLast() const
{
    Declaration* cur = link;
    if( cur == 0 )
        return 0;
    while( cur->next )
        cur = cur->next;
    return cur;
}

Declaration* Declaration::find(const QByteArray& name, bool recursive) const
{
    const char* n = Token::getSymbol(name.toUpper()).constData();
    Declaration* cur = link;
    while( cur )
    {
        if( cur->n == n )
            return cur;
        cur = cur->next;
    }
    if( recursive && outer )
        return outer->find(name, true);
    return 0;
}

Declaration *Declaration::findInImports(const QByteArray &name) const
{
    Q_ASSERT(kind == Schema);

    const char* n = Token::getSymbol(name.toUpper()).constData();

    // Look through all USE/REFERENCE imports until found
    Declaration* useOrRef = link;
    while( useOrRef )
    {
        if( useOrRef->kind == Declaration::Use || useOrRef->kind == Declaration::Reference )
        {
            if( useOrRef->link )
            {
                Declaration* import = useOrRef->link;
                while( import )
                {
                    Q_ASSERT( import->kind == Declaration::Imported );
                    if( import->n == n ) // canonical name comparison
                    {
                        if( import->validated && !import->hasErrors )
                            return import->data.value<Declaration*>();
                        else
                            return 0;
                    }
                    import = import->next;
                }
            }else if( useOrRef->validated && !useOrRef->hasErrors )
            {
                // If USE/REFERENCE without explicit items, check the foreign schema directly
                Declaration* foreignSchema = useOrRef->data.value<Declaration*>();
                Q_ASSERT(foreignSchema);
                Declaration* resolved = foreignSchema->findInImports(name);
                // TODO: the result could be a symbol imported in the foreignSchema but not exported by it
                return resolved;
            }else
                return 0;
        }
        useOrRef = useOrRef->next;
    }
    return 0;
}

QList<Declaration*> Declaration::getParams() const
{
    QList<Declaration*> res;
    Declaration* cur = link;
    while( cur )
    {
        if( cur->kind == ParamDecl )
            res.append(cur);
        cur = cur->next;
    }
    return res;
}

QByteArray Declaration::scopedName() const
{
    if( outer && outer->kind != Scope )
        return outer->scopedName() + "." + name;
    return name;
}

void Declaration::setName(const QByteArray &name_)
{
    name = name_;
    if( !name.isEmpty() )
        n = Token::getSymbol(name.toUpper()).constData();
    else
        n = 0;
}

Expression::~Expression()
{
    if( lhs )
        delete lhs;
    if( rhs )
        delete rhs;
    if( next )
        delete next;
}

void Expression::appendRhs(Expression* e)
{
    if( rhs == 0 )
        rhs = e;
    else
    {
        Expression* cur = rhs;
        while( cur->next )
            cur = cur->next;
        cur->next = e;
    }
}

int Expression::getCount(const Expression* list)
{
    int n = 0;
    while( list )
    {
        n++;
        list = list->next;
    }
    return n;
}

void Expression::append(Expression* list, Expression* elem)
{
    Q_ASSERT(list && elem);
    while( list->next )
        list = list->next;
    list->next = elem;
}

Statement::~Statement()
{
    if( lhs )
        delete lhs;
    if( rhs )
        delete rhs;
    if( body )
        deleteAll(body);
    if( next )
        deleteAll(next);
}

void Statement::deleteAll(Statement* s)
{
    while( s )
    {
        Statement* n = s->next;
        s->next = 0;
        delete s;
        s = n;
    }
}

Statement* Statement::getLast() const
{
    const Statement* cur = this;
    while( cur->next )
        cur = cur->next;
    return const_cast<Statement*>(cur);
}

void Statement::append(Statement* s)
{
    Q_ASSERT(s);
    Statement* last = getLast();
    last->next = s;
}

void Symbol::deleteAll(Symbol* s)
{
    while( s )
    {
        Symbol* n = s->next;
        delete s;
        s = n;
    }
}

AstModel::AstModel()
{
    if( globalScope == 0 )
    {
        globalScope = new Declaration();
        globalScope->kind = Declaration::Scope;
        openScope(globalScope);

        // predefined simple types
        types[Type::Undefined] = newType(Type::Undefined);
        types[Type::NoType] = newType(Type::NoType);
        types[Type::BINARY] = addType("BINARY", Type::BINARY);
        types[Type::BOOLEAN] = addType("BOOLEAN", Type::BOOLEAN);
        types[Type::INTEGER] = addType("INTEGER", Type::INTEGER);
        types[Type::LOGICAL] = addType("LOGICAL", Type::LOGICAL);
        types[Type::NUMBER] = addType("NUMBER", Type::NUMBER);
        types[Type::REAL] = addType("REAL", Type::REAL);
        types[Type::STRING] = addType("STRING", Type::STRING);

        // predefined constants
        addBuiltin("CONST_E", CONST_E);
        addBuiltin("PI", Ast::PI);
        addBuiltin("SELF", Ast::SELF);
        addBuiltin("?", INDETERMINATE);

        // predefined functions
        addBuiltin("ABS", Ast::ABS);
        addBuiltin("ACOS", Ast::ACOS);
        addBuiltin("ASIN", Ast::ASIN);
        addBuiltin("ATAN", Ast::ATAN);
        addBuiltin("BLENGTH", Ast::BLENGTH);
        addBuiltin("COS", Ast::COS);
        addBuiltin("EXISTS", Ast::EXISTS);
        addBuiltin("EXP", Ast::EXP);
        addBuiltin("FORMAT", Ast::FORMAT);
        addBuiltin("HIBOUND", Ast::HIBOUND);
        addBuiltin("HIINDEX", Ast::HIINDEX);
        addBuiltin("LENGTH", Ast::LENGTH);
        addBuiltin("LOBOUND", Ast::LOBOUND);
        addBuiltin("LOINDEX", Ast::LOINDEX);
        addBuiltin("LOG", Ast::LOG);
        addBuiltin("LOG2", Ast::LOG2);
        addBuiltin("LOG10", Ast::LOG10);
        addBuiltin("NVL", Ast::NVL);
        addBuiltin("ODD", Ast::ODD);
        addBuiltin("ROLESOF", Ast::ROLESOF);
        addBuiltin("SIN", Ast::SIN);
        addBuiltin("SIZEOF", Ast::SIZEOF);
        addBuiltin("SQRT", Ast::SQRT);
        addBuiltin("TAN", Ast::TAN);
        addBuiltin("TYPEOF", Ast::TYPEOF);
        addBuiltin("USEDIN", Ast::USEDIN);
        addBuiltin("VALUE", Ast::VALUE);
        addBuiltin("VALUE_IN", Ast::VALUE_IN);
        addBuiltin("VALUE_UNIQUE", Ast::VALUE_UNIQUE);

        // predefined procedures
        addBuiltin("INSERT", Ast::INSERT);
        addBuiltin("REMOVE", Ast::REMOVE);

        // predefined logical constants (TRUE, FALSE, UNKNOWN are keywords, handled by parser)

        closeScope();
    }else
        openScope(globalScope);
}

AstModel::~AstModel()
{
    for( int i = 1; i < scopes.size(); i++ )
        Declaration::deleteAll(scopes[i]);
    scopes.clear();
}

void AstModel::openScope(Declaration* scope)
{
    if( scope == 0 )
    {
        scope = new Declaration();
        scope->kind = Declaration::Scope;
    }
    scopes.push_back(scope);
}

Declaration* AstModel::closeScope(bool takeMembers)
{
    Declaration* res = 0;
    if( takeMembers )
    {
        res = scopes.back()->link;
        scopes.back()->link = 0;
        Declaration::deleteAll(scopes.back());
    }
    scopes.pop_back();
    return res;
}

Declaration* AstModel::addDecl(const QByteArray& n)
{
    Declaration* scope = scopes.back();

    Declaration* decl = new Declaration();
    decl->setName(n);
    if( scope->kind != Declaration::Scope )
        decl->outer = scope;

    scope->appendMember(decl); // don't check for name collisions here
    return decl;
}

Declaration* AstModel::findDecl(const QByteArray& name, bool recursive) const
{
    const char* n = Token::getSymbol(name.toUpper()).constData();
    for( int i = scopes.size() - 1; i >= 0; i-- )
    {
        Declaration* cur = scopes[i]->link;
        while( cur != 0 )
        {
            if( cur->n == n )
                return cur;
            else
                cur = cur->getNext();
        }
        if( !recursive )
            return 0;
    }
    return 0;
}

Declaration* AstModel::getTopScope() const
{
    for( int i = scopes.size() - 1; i >= 0; i-- )
    {
        Declaration* d = scopes[i];
        if( d->kind == Declaration::Schema || d->kind == Declaration::Function
            || d->kind == Declaration::Procedure || d->kind == Declaration::Rule
            || d->kind == Declaration::Entity )
            return d;
    }
    return 0;
}

void AstModel::cleanupGlobals()
{
    if( globalScope )
    {
        Declaration::deleteAll(globalScope);
        globalScope = 0;
        for( int i = 0; i < Type::MaxKind; i++ )
        {
            delete types[i];
            types[i] = 0;
        }
    }
}

Type* AstModel::newType(Type::Kind k)
{
    Type* t = new Type();
    t->kind = k;
    return t;
}

Type* AstModel::addType(const QByteArray& n, Type::Kind k)
{
    Type* t = newType(k);
    Declaration* d = addDecl(n);
    if( d )
    {
        d->kind = Declaration::TypeDecl;
        d->validated = true;
        d->setType(t);
        d->ownstype = true;
    }
    return t;
}

void AstModel::addBuiltin(const QByteArray& n, quint16 id)
{
    Declaration* d = addDecl(n);
    if( d )
    {
        d->kind = Declaration::ConstDecl;
        d->validated = true;
        d->id = id;
    }
}
