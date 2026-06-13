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

#include "ExpValidator2.h"
#include "ExpToken.h"
#include <QtDebug>
using namespace Exp;
using namespace Ast;

Validator2::Validator2(AstModel* m, Importer* i, bool haveXref)
    :mdl(m),imp(i),curSchema(0),curFunction(0),first(0),last(0)
{
    Q_UNUSED(haveXref);
}

Validator2::~Validator2()
{
}

bool Validator2::validate(Declaration* s)
{
    if( s == 0 || s->kind != Declaration::Schema )
        return false;
    if( s->validated )
        return !s->hasErrors;
    s->validated = true;
    curSchema = s;
    sourcePath = s->data.toString();
    Schema(s);
    s->hasErrors = !errors.isEmpty();
    return !s->hasErrors;
}

Xref Validator2::takeXref()
{
    Xref res;
    res.syms = first;
    res.uses = xref;
    res.subs = subs;
    first = last = 0;
    xref.clear();
    subs.clear();
    return res;
}

void Validator2::Schema(Declaration* s)
{
    scopeStack.push_back(s);

    // resolve all USE/REFERENCE imports
    Declaration* d = s->link;
    while( d )
    {
        if( d->kind == Declaration::Use || d->kind == Declaration::Reference )
            ImportDecl(d);
        d = d->next;
    }

    // resolve all Type::NameRef early (forward reference support)
    d = s->link;
    while( d )
    {
        if( (d->kind == Declaration::TypeDecl) &&
            d->type() && d->type()->kind == Type::NameRef && !d->type()->validated )
            resolveNameRefType(d->type(), d->pos);
        d = d->next;
    }

    // validate all declarations
    d = s->link;
    while( d )
    {
        if( d->kind != Declaration::Use && d->kind != Declaration::Reference )
        {
            Q_ASSERT(d->kind != Declaration::Imported);
            decl(d);
        }
        d = d->next;
    }

    scopeStack.pop_back();
}

void Validator2::ImportDecl(Declaration* useOrRef)
{
    if( useOrRef->validated )
        return;
    useOrRef->validated = true;
    useOrRef->hasErrors = true;
    // TODO it's not actually a decl markDecl(useOrRef);

    if( !imp )
        return; // no importer; cannot resolve external schemas

    // Build Import struct from the Use/Reference declaration
    Import impInfo;
    impInfo.name = useOrRef->name;
    impInfo.importer = useOrRef;
    impInfo.importedAt = useOrRef->pos;

    Declaration* foreignSchema = imp->loadSchema(impInfo);
    if( !foreignSchema )
    {
        error(useOrRef->pos, QString("cannot resolve schema '%1'").arg(useOrRef->name.constData()));
        return;
    }

    useOrRef->data = QVariant::fromValue(foreignSchema);
    useOrRef->hasErrors = false;

    // Import specific items listed as Imported children (if no items are listed,
    // all symbols are implicitly imported)
    Declaration* item = useOrRef->link;
    while( item )
    {
        if( item->kind == Declaration::Imported )
        {
            Q_ASSERT(item->validated == false);
            item->validated = true;
            // item->name is the local name, item->data the original name in the foreign schema
            QByteArray foreignName = item->data.toByteArray();
            Declaration* resolved = foreignSchema->find(sym(foreignName), false);
            if( !resolved )
            {
                error(item->pos, QString("'%1' not found in schema '%2'")
                      .arg(foreignName.constData()).arg(useOrRef->name.constData()));
                item->hasErrors = true;
            }else
            {
                markRef(resolved, item->pos);
                item->data = QVariant::fromValue(resolved); // replace the name by the decl ref
            }
        }
        item = item->next;
    }
}

void Validator2::DeclSeq(Declaration* d)
{
    while( d )
    {
        decl(d);
        d = d->next;
    }
}

bool Validator2::decl(Declaration* d)
{
    if( d == 0 )
        return false;
    if( d->validated )
        return !d->hasErrors;
    d->validated = true;
    markDecl(d);

    switch( d->kind )
    {
    case Declaration::ConstDecl:
        ConstDecl(d);
        break;
    case Declaration::TypeDecl:
        TypeDecl(d);
        break;
    case Declaration::Attribute:
        AttributeDecl(d);
        break;
    case Declaration::Function:
        FunctionDecl(d);
        break;
    case Declaration::Procedure:
        ProcedureDecl(d);
        break;
    case Declaration::Rule:
        RuleDecl(d);
        break;
    case Declaration::SubtypeConstraint:
        SubtypeConstraintDecl(d);
        break;
    case Declaration::ParamDecl:
    case Declaration::LocalDecl:
        if( d->type() )
            Type_(d->type());
        if( d->expr )
            Expr(d->expr);
        break;
    case Declaration::Enumerator:
    case Declaration::SelectItem:
    case Declaration::AliasDecl:
    case Declaration::Supertype:
    case Declaration::RuleFor:
    case Declaration::TotalOver:
        // these are handled by their parent
        break;
    default:
        break;
    }
    return !d->hasErrors;
}

void Validator2::ConstDecl(Declaration* d)
{
    if( d->type() )
        Type_(d->type());
    if( d->expr )
        Expr(d->expr);
}

void Validator2::TypeDecl(Declaration* d)
{
    if( d->type() )
    {
        if( d->type()->kind == Type::ENTITY )
            EntityDecl(d);
        else
            Type_(d->type());
    }

    // validate WHERE rules in the body (if any)
    if( d->body )
        Body(d->body);
}

void Validator2::EntityDecl(Declaration* d)
{
    scopeStack.push_back(d);

    // Resolve supertype references (SUBTYPE OF parent names)
    resolveSupertypes(d->type());

    QList<Type*> seen;
    checkCircularSupertypes(d->type(), seen);

    // Validate supertype expression (ONEOF, AND, ANDOR)
    if( d->expr ) // TODO: attached to decl or type?
        supertypeExpr(d->expr);

    // Validate members: attributes, local declarations
    if( d->type() )
        foreach( Declaration* member, d->type()->subs )
        {
            if( member->kind == Declaration::Attribute )
                decl(member);
        }


    scopeStack.pop_back();
}

void Validator2::AttributeDecl(Declaration* d)
{
    if( d->type() )
        Type_(d->type());
    // derived attribute has expr
    if( d->derived_ && d->expr )
        Expr(d->expr);
}

void Validator2::FunctionDecl(Declaration* d)
{
    scopeStack.push_back(d);
    curFunction = d;

    // return type
    if( d->type() )
        Type_(d->type());

    // parameters
    Declaration* member = d->link;
    while( member )
    {
        if( member->kind == Declaration::ParamDecl || member->kind == Declaration::LocalDecl )
            decl(member);
        member = member->next;
    }

    // body
    if( d->body )
        Body(d->body);

    curFunction = 0;
    scopeStack.pop_back();
}

void Validator2::ProcedureDecl(Declaration* d)
{
    scopeStack.push_back(d);
    curFunction = d;

    // parameters
    Declaration* member = d->link;
    while( member )
    {
        if( member->kind == Declaration::ParamDecl || member->kind == Declaration::LocalDecl )
            decl(member);
        member = member->next;
    }

    // body
    if( d->body )
        Body(d->body);

    curFunction = 0;
    scopeStack.pop_back();
}

void Validator2::RuleDecl(Declaration* d)
{
    scopeStack.push_back(d);
    curFunction = d;

    // Resolve RULE FOR entities
    Declaration* member = d->link;
    while( member )
    {
        if( member->kind == Declaration::RuleFor )
        {
            Q_ASSERT(member->validated == false);
            member->validated = true;
            Q_ASSERT(member->n == 0);
            Declaration* entity = find(sym(member->name), member->pos);
            if( entity == 0 )
            {
                error(member->pos, QString("'%1' cannot be resolved").arg(member->name.constData()));
                member->hasErrors = true;
            }else if( entity->kind != Declaration::TypeDecl || entity->type() == 0 || entity->type()->kind != Type::ENTITY )
            {
                error(member->pos, QString("'%1' is not an entity (RULE FOR)").arg(member->name.constData()));
                member->hasErrors = true;
            }else
                member->data = QVariant::fromValue(entity);
        }
        else if( member->kind == Declaration::LocalDecl )
            decl(member);
        member = member->next;
    }

    // body
    if( d->body )
        Body(d->body);

    curFunction = 0;
    scopeStack.pop_back();
}

void Validator2::SubtypeConstraintDecl(Declaration* d)
{
    // d->data holds the FOR entity name
    QByteArray entityName = d->data.toByteArray();
    if( !entityName.isEmpty() )
    {
        Declaration* entity = find(sym(entityName), d->pos);
        if( entity && (entity->kind != Declaration::TypeDecl || entity->type() == 0 || entity->type()->kind != Type::ENTITY) )
            error(d->pos, QString("'%1' is not an entity (SUBTYPE_CONSTRAINT FOR)").arg(entityName.constData()));
    }

    // resolve TotalOver entities
    if( d->type() )
        foreach( Declaration* member, d->type()->subs)
        {
            if( member->kind == Declaration::TotalOver )
            {
                Q_ASSERT(member->validated == false);
                member->validated = true;
                Q_ASSERT(member->n);
                Declaration* entity = find(member->n, member->pos);
                if( entity == 0 )
                {
                    error(member->pos, QString("'%1' cannot be resolved").arg(member->name.constData()));
                    member->hasErrors = true;
                }else if( entity && (entity->kind != Declaration::TypeDecl || entity->type() == 0 || entity->type()->kind != Type::ENTITY) )
                {
                    error(member->pos, QString("'%1' is not an entity (TOTAL_OVER)").arg(member->name.constData()));
                    member->hasErrors = true;
                }else
                    member->data = QVariant::fromValue(entity);
            }
            member = member->next;
        }

    // constraint expression (ONEOF, AND, ANDOR)
    if( d->expr ) // TODO: here or in t->expr?
        supertypeExpr(d->expr);
}

bool Validator2::Type_(Type* t)
{
    if( t == 0 )
        return false;
    if( t->validated )
        return true;
    t->validated = true;

    switch( t->kind )
    {
    case Type::NameRef:
        return NameRefType(t);
    case Type::ARRAY:
    case Type::BAG:
    case Type::LIST:
    case Type::SET:
        return AggregationType(t);
    case Type::BINARY:
    case Type::STRING:
        if( t->expr )
            Expr(t->expr);
        break;
    case Type::REAL:
        if( t->expr )
            Expr(t->expr);
        break;
    case Type::ENUMERATION:
        // subs holds Enumerator declarations
        break;
    case Type::SELECT:
        // subs holds SelectItem declarations; could resolve each one
        for( int i = 0; i < t->subs.size(); i++ )
        {
            Declaration* si = t->subs[i];
            if( si && si->kind == Declaration::SelectItem )
            {
                Q_ASSERT(si->n);
                Declaration* resolved = find(si->n, si->pos);
                if( resolved )
                    markRef(resolved, si->pos);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

bool Validator2::NameRefType(Type* t)
{
    resolveNameRefType(t, t->pos);
    return t->decl != 0;
}

bool Validator2::AggregationType(Type* t)
{
    // bounds in t->expr (low) and t->expr->next (high)
    if( t->expr )
    {
        Expr(t->expr);
        if( t->expr->next )
            Expr(t->expr->next);
    }
    // element type stored in Node::type() of the Type node is the base type
    // Actually the element type is stored via setType on the parent or separate mechanism
    // For now, validate the element type if set
    if( t->type() )
        Type_(t->type());
    return true;
}

void Validator2::Body(Statement* s)
{
    if( s == 0 )
        return;
    StatSeq(s);
}

void Validator2::StatBlock(Statement* s)
{
    if( s == 0 )
        return;
    StatSeq(s);
}

void Validator2::StatSeq(Statement* s)
{
    while( s )
    {
        Statement_(s);
        s = s->getNext();
    }
}

Statement* Validator2::Statement_(Statement* s)
{
    if( s == 0 )
        return 0;
    switch( s->kind )
    {
    case Statement::Assig:
        Assig(s);
        break;
    case Statement::Call:
        CallStmt(s);
        break;
    case Statement::If:
        IfStat(s);
        break;
    case Statement::Case:
        CaseStat(s);
        break;
    case Statement::Alias:
        AliasStat(s);
        break;
    case Statement::Repeat:
        RepeatStat(s);
        break;
    case Statement::Return:
        ReturnStat(s);
        break;
    case Statement::Compound:
        CompoundStat(s);
        break;
    case Statement::Escape:
        if( repeatStack.isEmpty() )
            error(s->pos, "ESCAPE outside REPEAT");
        break;
    case Statement::Skip:
        if( repeatStack.isEmpty() )
            error(s->pos, "SKIP outside REPEAT");
        break;
    case Statement::Null:
        break;
    default:
        break;
    }
    return s;
}

void Validator2::Assig(Statement* s)
{
    if( s->lhs )
        Expr(s->lhs);
    if( s->rhs )
        Expr(s->rhs);
}

void Validator2::CallStmt(Statement* s)
{
    if( s->lhs )
        Expr(s->lhs);
}

Statement* Validator2::IfStat(Statement* s)
{
    // condition
    if( s->rhs )
        Expr(s->rhs);
    // then-part
    if( s->body )
        StatSeq(s->body);
    // else/elsif chain; stored as next statements of kind Elsif/Else
    Statement* n = s->getNext();
    while( n && (n->kind == Statement::Elsif || n->kind == Statement::Else) )
    {
        if( n->kind == Statement::Elsif )
        {
            if( n->rhs )
                Expr(n->rhs);
            if( n->body )
                StatSeq(n->body);
        }
        else if( n->kind == Statement::Else )
        {
            if( n->body )
                StatSeq(n->body);
        }
        n = n->getNext();
    }
    return s;
}

Statement* Validator2::CaseStat(Statement* s)
{
    // selector
    if( s->lhs )
        Expr(s->lhs);
    // case actions in body
    if( s->body )
    {
        Statement* ca = s->body;
        while( ca )
        {
            if( ca->kind == Statement::CaseAction )
            {
                // labels in rhs
                if( ca->rhs )
                {
                    Expression* lbl = ca->rhs;
                    while( lbl )
                    {
                        Expr(lbl);
                        lbl = lbl->next;
                    }
                }
                // action body
                if( ca->body )
                    StatSeq(ca->body);
            }
            else if( ca->kind == Statement::Else )
            {
                if( ca->body )
                    StatSeq(ca->body);
            }
            ca = ca->getNext();
        }
    }
    return s;
}

void Validator2::AliasStat(Statement* s)
{
    // alias target expression
    if( s->rhs )
        Expr(s->rhs);
    // alias body
    if( s->body )
        StatSeq(s->body);
}

void Validator2::RepeatStat(Statement* s)
{
    repeatStack.push_back(s);
    // increment control in lhs (if any)
    if( s->lhs )
        Expr(s->lhs);
    // condition/bounds in rhs
    if( s->rhs )
        Expr(s->rhs);
    // body
    if( s->body )
        StatSeq(s->body);
    repeatStack.pop_back();
}

void Validator2::ReturnStat(Statement* s)
{
    if( s->rhs )
        Expr(s->rhs);
}

void Validator2::CompoundStat(Statement* s)
{
    if( s->body )
        StatSeq(s->body);
}

bool Validator2::Expr(Expression* e)
{
    if( e == 0 )
        return false;

    switch( e->kind )
    {
    // Unary
    case Expression::Plus:
    case Expression::Minus:
    case Expression::Not:
        return unaryOp(e);
    // Relational
    case Expression::Eq: case Expression::Neq:
    case Expression::Lt: case Expression::Leq:
    case Expression::Gt: case Expression::Geq:
    case Expression::In: case Expression::Like:
    case Expression::InstEq: case Expression::InstNeq:
        return relation(e);
    // Additive
    case Expression::Add: case Expression::Sub:
    case Expression::Or: case Expression::Xor:
        return arithOp(e);
    // Multiplicative
    case Expression::Mul: case Expression::Fdiv:
    case Expression::Div: case Expression::Mod:
    case Expression::And:
        return mulOp(e);
    // Combine
    case Expression::Combine:
        return combineOp(e);
    // Power
    case Expression::Power:
        return powerOp(e);
    // References
    case Expression::DeclRef:
        return declRef(e);
    case Expression::NameRef:
        return nameRef(e);
    case Expression::Select:
        return select(e);
    case Expression::Index:
        return index(e);
    case Expression::GroupRef:
        return groupRef(e);
    case Expression::Call:
        return call(e);
    // Literals
    case Expression::Literal:
    case Expression::ConstVal:
        return literal(e);
    // Aggregation
    case Expression::AggInit:
        return aggInit(e);
    case Expression::Range:
        return range(e);
    // Query
    case Expression::Query:
        return query(e);
    // Interval
    case Expression::Interval:
        return interval(e);
    // Supertype
    case Expression::OneOf:
    case Expression::AndOr:
        return supertypeExpr(e);
    // Self
    case Expression::Self:
        return selfExpr(e);
    default:
        return false;
    }
}

bool Validator2::ConstExpr(Expression* e)
{
    return Expr(e);
}

bool Validator2::relation(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);
    return true;
}

bool Validator2::unaryOp(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    return true;
}

bool Validator2::arithOp(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);
    return true;
}

bool Validator2::mulOp(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);
    return true;
}

bool Validator2::combineOp(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);
    return true;
}

bool Validator2::powerOp(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);
    return true;
}

bool Validator2::declRef(Expression* e)
{
    // already resolved
    Declaration* d = e->val.value<Declaration*>();
    if( d )
        markRef(d, e->pos);
    return d != 0;
}

bool Validator2::nameRef(Expression* e)
{
    QByteArray name = e->val.toByteArray();
    if( name.isEmpty() )
        return false;

    Declaration* d = find(sym(name), e->pos);
    if( d == 0 )
    {
        // d = find(sym(name), e->pos); // TEST
        error(e->pos,QString("cannot resolve '%1'").arg(name.constData()));
        e->forward = true;
        return false;
    }

    // Resolve: convert NameRef to DeclRef
    // qDebug() << "resolved" << d->scopedName() << "from" << curSchema->name;
    e->kind = Expression::DeclRef;
    e->val = QVariant::fromValue(d);
    markRef(d, e->pos);
    return true;
}

bool Validator2::select(Expression* e)
{
    // e->lhs is the base expression (already parsed)
    if( e->lhs )
        Expr(e->lhs);
    // e->val holds the selected attribute name (QByteArray)
    // TODO: resolve attribute in entity type of lhs
    return true;
}

bool Validator2::index(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    // rhs is index expression(s)
    if( e->rhs )
    {
        Expr(e->rhs);
        // second index (slice) via rhs->next
        if( e->rhs->next )
            Expr(e->rhs->next);
    }
    return true;
}

bool Validator2::groupRef(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    // e->val holds group entity name
    QByteArray groupName = e->val.toByteArray();
    if( !groupName.isEmpty() )
    {
        Declaration* d = find(sym(groupName), e->pos);
        if( d )
            markRef(d, e->pos);
    }
    return true;
}

bool Validator2::call(Expression* e)
{
    // e->lhs is the callee expression (NameRef or DeclRef)
    if( e->lhs )
        Expr(e->lhs);

    // e->rhs is argument list (linked via next)
    Expression* arg = e->rhs;
    while( arg )
    {
        Expr(arg);
        arg = arg->next;
    }

    // check built-in function arguments
    if( e->lhs && e->lhs->kind == Expression::DeclRef )
    {
        Declaration* callee = e->lhs->val.value<Declaration*>();
        if( callee && callee->id != Declaration::NoSlot )
        {
            // it's a built-in
            QList<Expression*> args;
            arg = e->rhs;
            while( arg ) { args.append(arg); arg = arg->next; }
            Type* ret = 0;
            checkBuiltinArgs(callee->id, args, &ret, e->pos);
        }
    }
    return true;
}

bool Validator2::literal(Expression* e)
{
    Q_UNUSED(e);
    return true;
}

bool Validator2::aggInit(Expression* e)
{
    // elements in rhs list
    Expression* elem = e->rhs;
    while( elem )
    {
        Expr(elem);
        elem = elem->next;
    }
    return true;
}

bool Validator2::range(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);
    return true;
}

bool Validator2::query(Expression* e)
{
    // e->lhs: query variable (NameRef)
    // e->rhs: source expression
    // e->rhs->next: condition expression (if present)
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
    {
        Expr(e->rhs);
        if( e->rhs->next )
            Expr(e->rhs->next);
    }
    return true;
}

bool Validator2::interval(Expression* e)
{
    // {low op1 val op2 high}
    // lhs = low, rhs = val; or encoding depends on parser
    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);
    return true;
}

bool Validator2::selfExpr(Expression* e)
{
    Q_UNUSED(e);
    return true;
}

bool Validator2::supertypeExpr(Expression* e)
{
    if( e == 0 )
        return false;
    switch( e->kind )
    {
    case Expression::OneOf:
        // arguments in rhs list (each is a NameRef to entity or sub-expression)
    {
        Expression* arg = e->rhs;
        while( arg )
        {
            if( arg->kind == Expression::NameRef )
                nameRef(arg);
            else
                supertypeExpr(arg);
            arg = arg->next;
        }
    }
        break;
    case Expression::AndOr:
    case Expression::And:
        if( e->lhs )
        {
            if( e->lhs->kind == Expression::NameRef )
                nameRef(e->lhs);
            else
                supertypeExpr(e->lhs);
        }
        if( e->rhs )
        {
            if( e->rhs->kind == Expression::NameRef )
                nameRef(e->rhs);
            else
                supertypeExpr(e->rhs);
        }
        break;
    case Expression::NameRef:
        nameRef(e);
        break;
    default:
        Expr(e);
        break;
    }
    return true;
}

bool Validator2::assigCompat(Type* lhs, Type* rhs)
{
    if( lhs == 0 || rhs == 0 )
        return true; // unknown types pass
    lhs = deref(lhs);
    rhs = deref(rhs);
    if( lhs == rhs )
        return true;
    if( lhs->kind == rhs->kind )
        return true;
    // NUMBER compatible with REAL compatible with INTEGER
    if( (lhs->kind == Type::NUMBER || lhs->kind == Type::REAL || lhs->kind == Type::INTEGER) &&
        (rhs->kind == Type::NUMBER || rhs->kind == Type::REAL || rhs->kind == Type::INTEGER) )
        return true;
    // LOGICAL compatible with BOOLEAN
    if( (lhs->kind == Type::LOGICAL || lhs->kind == Type::BOOLEAN) &&
        (rhs->kind == Type::LOGICAL || rhs->kind == Type::BOOLEAN) )
        return true;
    // aggregation kinds compatible if same container type
    // TODO: deeper checking of element types
    return true; // conservative: allow for now
}

bool Validator2::assigCompat(Type* lhs, Expression* rhs)
{
    Q_UNUSED(lhs);
    Q_UNUSED(rhs);
    return true; // TODO
}

bool Validator2::paramCompat(Declaration* formal, Expression* actual)
{
    Q_UNUSED(formal);
    Q_UNUSED(actual);
    return true; // TODO
}

void Validator2::resolveNameRefType(Type* t, const RowCol& pos)
{
    if( t == 0 || t->kind != Type::NameRef )
        return;
    if( t->decl )
        return; // already resolved

    QByteArray name;
    if( t->quali )
    {
        // qualified: quali->first is schema, quali->second is type
        if( !t->quali->second.isEmpty() )
            name = t->quali->second;
        else
            name = t->quali->first;
    }

    if( name.isEmpty() )
        return;

    Declaration* d = find(sym(name), pos, false);
    if( d == 0 )
    {
        // could be a forward reference; mark for later
        t->forward = true;
        return;
    }

    if( d->kind != Declaration::TypeDecl )
    {
        // d = find(sym(name), pos); // TEST
        error(pos, QString("'%1' is not a type or entity").arg(name.constData()));
        return;
    }

    t->decl = d;
    markRef(d, pos);
}

Declaration* Validator2::find(const char* n, const RowCol& pos, bool searchInEntity)
{
    Q_UNUSED(pos);
    if( n == 0 )
        return 0;

    Q_ASSERT( !scopeStack.isEmpty() );
    Declaration* back = scopeStack.back();
    if( searchInEntity && back->kind == Declaration::TypeDecl && back->type() && back->type()->kind == Type::ENTITY )
    {
        Declaration* d = findInEntity(back->type(), n);
        if( d )
            return d;
    }
    Declaration* d = back->find(n);
    if( d )
        return d;

    Declaration* global = AstModel::getGlobalScope();
    if( global )
    {
        Declaration* d = global->find(n, false);
        if( d )
            return d;
    }

    if( curSchema )
    {
        Declaration* d = curSchema->findInImports(n);
        if( d )
            return d;
    }
    return 0;
}

Declaration* Validator2::findInEntity(Type* entity, const char* n)
{
    if( entity == 0 )
        return 0;

    // Search local members
    Declaration* d = entity->find(n, false);
    if( d )
        return d;

    // Search supertypes (Supertype kind children of entity)
    // TODO: this belongs into entity->find
    foreach( Declaration* member, entity->subs)
    {
        if( member->kind == Declaration::Supertype )
        {
            Declaration* parent = member->data.value<Declaration*>();
            if( !parent && !member->validated )
            {
                Q_ASSERT(member->n == 0);
                const char* mn = sym(member->name);
                Declaration* scope = entity->decl ? entity->decl->outer : curSchema;
                if (scope)
                    parent = scope->find(mn, true);
                if (!parent)
                {
                    Declaration* global = AstModel::getGlobalScope();
                    if( global )
                        parent = global->find(mn, false);
                }
                if (!parent && curSchema)
                    parent = curSchema->findInImports(mn);
            }

            if( parent && parent->kind == Declaration::TypeDecl && parent->type() && parent->type()->kind == Type::ENTITY && parent->type() != entity )
            {
                d = findInEntity(parent->type(), n);
                if( d )
                    return d;
            }
        }
    }
    return 0;
}

Type* Validator2::deref(Type* t)
{
    if( t == 0 )
        return 0;
    // follow NameRef chain
    int limit = 100; // guard against cycles
    while( t->kind == Type::NameRef && t->decl && limit-- > 0 )
    {
        Declaration* d = t->decl;
        if( d->type() && d->type() != t )
            t = d->type();
        else
            break;
    }
    return t;
}

bool Validator2::checkBuiltinArgs(quint16 builtin, const QList<Expression*>& args,
                                   Type** ret, const RowCol& pos)
{
    int n = args.size();

    switch( builtin )
    {
    case ABS:
        if( n != 1 ) {
            error(pos, "ABS expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::NUMBER);
        break;
    case ACOS: case ASIN: case ATAN:
    case COS: case SIN: case TAN:
    case Ast::EXP: case LOG: case LOG2:
    case LOG10: case SQRT:
        if( n != 1 ) {
            error(pos, QString("math function expects 1 argument"));
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::REAL);
        break;
    case BLENGTH: case LENGTH:
        if( n != 1 ) {
            error(pos, "LENGTH/BLENGTH expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::INTEGER);
        break;
    case EXISTS:
        if( n != 1 ) {
            error(pos, "EXISTS expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::BOOLEAN);
        break;
    case FORMAT:
        if( n != 2 ) {
            error(pos, "FORMAT expects 2 arguments");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::STRING);
        break;
    case HIBOUND: case LOBOUND: case HIINDEX: case LOINDEX:
        if( n != 1 ) {
            error(pos, "bound/index function expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::INTEGER);
        break;
    case NVL:
        if( n != 2 ) {
            error(pos, "NVL expects 2 arguments");
            return false;
        }
        break;
    case ODD:
        if( n != 1 ) {
            error(pos, "ODD expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::LOGICAL);
        break;
    case ROLESOF:
        if( n != 1 ) {
            error(pos, "ROLESOF expects 1 argument");
            return false;
        }
        break;
    case SIZEOF:
        if( n != 1 ) {
            error(pos, "SIZEOF expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::INTEGER);
        break;
    case TYPEOF:
        if( n != 1 ) {
            error(pos, "TYPEOF expects 1 argument");
            return false;
        }
        break;
    case USEDIN:
        if( n < 1 || n > 2 ) {
            error(pos, "USEDIN expects 1 or 2 arguments");
            return false;
        }
        break;
    case VALUE:
        if( n != 1 ) {
            error(pos, "VALUE expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::NUMBER);
        break;
    case VALUE_IN:
        if( n != 2 ) {
            error(pos, "VALUE_IN expects 2 arguments");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::LOGICAL);
        break;
    case VALUE_UNIQUE:
        if( n != 1 ) {
            error(pos, "VALUE_UNIQUE expects 1 argument");
            return false;
        }
        if( ret )
            *ret = mdl->getType(Type::LOGICAL);
        break;
    case INSERT:
        if( n != 3 ) {
            error(pos, "INSERT expects 3 arguments");
            return false;
        }
        break;
    case REMOVE:
        if( n != 2 ) {
            error(pos, "REMOVE expects 2 arguments");
            return false;
        }
        break;
    default:
        error(pos, "unknown built-in");
        return false;
        break;
    }
    return true;
}

void Validator2::resolveSupertypes(Type* entity)
{
    if( !entity )
        return;
    // Walk the entity's Supertype kind children and resolve them
    foreach( Declaration* member, entity->subs)
    {
        if( member->kind == Declaration::Supertype )
        {
            Q_ASSERT(member->validated == false);
            member->validated = true;
            Q_ASSERT(member->n == 0);
            const char* n = sym(member->name);
            Declaration* parent = find(n, member->pos);
            if( parent == 0 )
            {
                // parent = find(member->name, member->pos); // TEST
                error(member->pos, QString("cannot resolve supertype '%1'").arg(member->name.constData()));
                member->hasErrors = true;
            }
            else if( parent->kind != Declaration::TypeDecl || parent->type() == 0 || parent->type()->kind != Type::ENTITY )
            {
                error(member->pos, QString("'%1' is not an entity (SUBTYPE OF)").arg(member->name.constData()));
                member->hasErrors = true;
            }
            else
            {
                markRef(parent, member->pos);
                member->data = QVariant::fromValue(parent);
                // register subtype relationship
                Q_ASSERT(entity->decl);
                subs[parent].append(entity->decl);
            }
        }
        member = member->next;
    }
}

bool Validator2::checkCircularSupertypes(Type* entity, QList<Type*>& seen)
{
    if( entity == 0 )
        return false;
    if( seen.contains(entity) )
    {
        Q_ASSERT(entity->decl);
        error(entity->pos, QString("circular supertype hierarchy involving '%1'").arg(entity->decl->name.constData()));
        return true;
    }
    seen.append(entity);

    foreach( Declaration* member, entity->subs)
    {
        if( member->kind == Declaration::Supertype )
        {
            Declaration* parent = member->data.value<Declaration*>();
            if( !parent && !member->validated )
            {
                const char* mn = sym(member->name);
                Declaration* scope = entity->decl ? entity->decl->outer : curSchema;
                if (scope)
                    parent = scope->find(mn, true);
                if (!parent)
                {
                    Declaration* global = AstModel::getGlobalScope();
                    if( global )
                        parent = global->find(mn, false);
                }
                if (!parent && curSchema)
                    parent = curSchema->findInImports(mn);
            }

            if( parent && parent->kind == Declaration::TypeDecl && parent->type() && parent->type()->kind == Type::ENTITY )
            {
                if( checkCircularSupertypes(parent->type(), seen) )
                    return true;
            }
        }
    }
    seen.removeLast();
    return false;
}

void Validator2::invalid(const char* what, const RowCol& pos)
{
    error(pos, QString("invalid %1").arg(what));
}

bool Validator2::error(const RowCol& pos, const QString& msg) const
{
    errors << Error(msg, pos, sourcePath);
    return false;
}

void Validator2::markDecl(Declaration* d)
{
    if( d == 0 )
        return;
    Symbol* sym = new Symbol();
    sym->kind = Symbol::Decl;
    sym->pos = d->pos;
    sym->len = d->name.size();
    sym->decl = d;

    if( last )
        last->next = sym;
    else
        first = sym;
    last = sym;

    xref[d].append(sym);
}

Symbol* Validator2::markRef(Declaration* d, const RowCol& pos)
{
    if( d == 0 )
        return 0;
    Symbol* sym = new Symbol();
    sym->kind = Symbol::DeclRef;
    sym->pos = pos;
    sym->len = d->name.size();
    sym->decl = d;

    if( last )
        last->next = sym;
    else
        first = sym;
    last = sym;

    xref[d].append(sym);
    return sym;
}

Symbol* Validator2::markUnref(int len, const RowCol& pos)
{
    Symbol* sym = new Symbol();
    sym->kind = Symbol::Invalid;
    sym->pos = pos;
    sym->len = len;

    if( last )
        last->next = sym;
    else
        first = sym;
    last = sym;
    return sym;
}

const char *Validator2::sym(const QByteArray & name)
{
    return Token::getSymbol(name.toUpper()).constData();
}
