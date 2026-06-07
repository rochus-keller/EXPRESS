#ifndef EXPAST_H
#define EXPAST_H

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

#include <QByteArray>
#include <Express/ExpRowCol.h>
#include <QVariant>
#include <QHash>

namespace Exp
{
namespace Ast
{
    class Declaration;

    enum Builtin {
        // built-in constants
        CONST_E, PI, SELF, INDETERMINATE,
        // built-in functions
        ABS, ACOS, ASIN, ATAN, BLENGTH, COS, EXISTS, EXP, FORMAT,
        HIBOUND, HIINDEX, LENGTH, LOBOUND, LOINDEX, LOG, LOG2, LOG10,
        NVL, ODD, ROLESOF, SIN, SIZEOF, SQRT, TAN, TYPEOF, USEDIN,
        VALUE, VALUE_IN, VALUE_UNIQUE,
        // built-in procedures
        INSERT, REMOVE,
        BuiltinMax
    };

    class Type;

    class Node
    {
    public:
        enum Meta { T, D, E, S }; // Type, Declaration, Expression, Statement
        uint meta : 2;
#ifndef _DEBUG
        uint kind : 6;
#endif

        uint validated : 1;
        uint inList : 1;  // private, used for linked-list management
        uint forward : 1; // Type and Expression NameRefs

        // Type flags:
        uint optional_ : 1;  // OPTIONAL attribute
        uint unique_ : 1;    // UNIQUE constraint on aggregation element type
        uint fixed_ : 1;     // FIXED width for STRING/BINARY
        uint extensible_ : 1; // EXTENSIBLE ENUMERATION/SELECT
        uint genericEntity_ : 1; // GENERIC_ENTITY SELECT

        // Declaration flags:
        uint abstract_ : 1;   // ABSTRACT ENTITY or ABSTRACT SUPERTYPE
        uint supertype_ : 1;  // SUPERTYPE declaration present
        uint ownstype : 1;    // this declaration owns its type (should delete it)
        uint hasErrors : 1;
        uint varParam : 1;    // VAR parameter (procedure params)
        uint derived_ : 1;    // derived attribute
        uint inverse_ : 1;    // inverse attribute
        uint redeclared_ : 1; // redeclared (renamed) attribute

        // Expression flags:
        uint byVal : 1;

        // 25 bits used

        RowCol pos;

        Type* type() const { return _ty; }
        void setType(Type*);

        Node():meta(0),
    #ifndef _DEBUG
            kind(0),
    #endif
            validated(0),inList(0),forward(0),
            optional_(0),unique_(0),fixed_(0),extensible_(0),genericEntity_(0),
            abstract_(0),supertype_(0),ownstype(0),hasErrors(0),
            varParam(0),derived_(0),inverse_(0),redeclared_(0),
            byVal(0),_ty(0) {}
        ~Node();
    private:
        Type* _ty;
    };

    class Expression;
    class Statement;

    typedef QPair<QByteArray,QByteArray> Quali;

    class Type : public Node
    {
    public:
        enum Kind {
            Undefined,
            NoType,
            // simple types
            BINARY,
            BOOLEAN,
            INTEGER,
            LOGICAL,
            NUMBER,
            REAL,
            STRING,
            // aggregation types
            ARRAY,
            BAG,
            LIST,
            SET,
            // constructed types
            ENUMERATION,
            SELECT,
            // generalized types
            AGGREGATE,
            GENERIC,
            GENERIC_ENTITY,
            // reference
            NameRef,    // unresolved named type reference; resolved by validator
            MaxKind
        };

        static const char* name[];

#ifdef _DEBUG
        Kind kind;
#endif

        union {
            Quali* quali;        // NameRef: qualified name reference
            Expression* expr;    // STRING/BINARY/REAL: width/precision
                                 // ARRAY/BAG/LIST/SET: low bound (high = expr->next)
            QByteArray* text;    // ENUM/SELECT: basedOn name
                                 // AGGREGATE/GENERIC/GENERIC_ENTITY: typeLabel
        };
        // base/element type for aggregations, REAL precision, STRING/BINARY width uses expr
        // for ENUMERATION: subs has enumerators; for SELECT: subs has select items
        QList<Declaration*> subs;
        Declaration* decl; // resolved declaration for NameRef; not owned

        Type():expr(0),decl(0) { meta = T; kind = Undefined; }
        ~Type();
    };

    class Declaration : public Node
    {
    public:
        enum Kind {
            Invalid,
            Scope,        // helper scope
            Schema,
            Entity,       
            TypeDecl,     // TYPE
            ConstDecl,    // CONSTANT
            Attribute,    // explicit entity attribute (also derived, inverse)
            Function,     // FUNCTION
            Procedure,    // PROCEDURE
            Rule,         // RULE
            ParamDecl,
            LocalDecl,    // LOCAL
            AliasDecl,    // ALIAS
            Enumerator,   // enumeration value
            SelectItem,   // select type item (ident reference)
            Use,          // USE
            Reference,    // REFERENCE
            Imported,     // USE/REFERENCE (imported)
            SubtypeConstraint, // SUBTYPE_CONSTRAINT
            Supertype,     // SUBTYPE OF (parent1, parent2, ...)
            RuleFor,      // RULE FOR (entity1, entity2, ...)
            TotalOver,    // TOTAL_OVER entities
            Max
        };
        // NOTE: abstract_supertype in constraint body reuses abstract_ flag
        static const char* s_name[];

#ifdef _DEBUG
        Kind kind;
#endif
        enum { IdWidth = 16, NoSlot = (1 << IdWidth) - 1 };
        Declaration* link;   // first member of scope (linked via next)
        Declaration* outer;  // owning declaration
        Declaration* next;   // next declaration in same scope
        Statement* body;     // statement body for functions/procedures/rules

        QByteArray name;
        const char* n; // canonical name for comparison
        enum Visi { NA, Private, Public };
        uint visi : 2;
        uint id : IdWidth;   // built-in code or param/local number

        QVariant data;       // value for ConstDecl; schema version string; import info; constraintFor

        union {
            Expression* expr;       // const value expression, supertype_expression (ONEOF, AND, ANDOR) and constraint body
            Declaration* helper;    // for TypeDecl/Attribute: subsidiary decls
        };

        Declaration():next(0),link(0),outer(0),body(0),n(0),id(NoSlot),expr(0),visi(0)
            { meta = D; kind = Invalid; }

        QList<Declaration*> getParams() const;
        Declaration* find(const QByteArray& name, bool recursive = true) const;
        Declaration* getLast() const;
        Declaration* getNext() const { return next; }
        void appendMember(Declaration*);
        static void deleteAll(Declaration*);
        QByteArray scopedName() const;
        void setName(const QByteArray& name);

        ~Declaration();
    private:
        friend class AstModel;
    };
    typedef QList<Declaration*> DeclList;

    class Expression : public Node
    {
    public:
        enum Kind {
            Invalid,
            // Unary operators
            Plus, Minus, Not,
            // Relational operators
            Eq, Neq, Lt, Leq, Gt, Geq, In, Like,
            InstEq, InstNeq,  // :=: and :<>:
            // Additive operators
            Add, Sub, Or, Xor,
            // Multiplicative operators
            Mul, Fdiv, Div, Mod, And,
            Combine,  // || string/binary concatenation
            // Power
            Power,    // **
            // References and access
            DeclRef,  // resolved reference to declaration; val holds Declaration*
            NameRef,  // unresolved name reference; val holds QByteArray name
            Select,   // '.' attribute selection
            Index,    // '[' index/slice ']'
            GroupRef,  // '\' group qualifier
            Call,     // function/procedure/entity constructor call
            // Literals
            Literal,  // val holds the literal value
            ConstVal, // resolved constant value
            // Aggregation
            AggInit,  // [...] aggregate initializer
            Range,    // element : repetition
            // Query
            Query,    // QUERY(var <* source | condition)
            // Interval
            Interval, // {expr1 op1 expr2 op2 expr3}
            // Supertype expression nodes
            OneOf,    // ONEOF(...)
            AndOr,    // ANDOR
            // Self
            Self,
            MAX
        };
#ifdef _DEBUG
        Kind kind;
#endif
        QVariant val;      // literal value, name (QByteArray), Declaration* for DeclRef/Select
        Expression* lhs;   // left operand
        Expression* rhs;   // right operand
        Expression* next;  // sibling list (args, elements, case labels)

        void appendRhs(Expression*);
        static int getCount(const Expression* list);
        static void append(Expression* list, Expression* elem);

        Expression(Kind k = Invalid, const RowCol& rc = RowCol()):
            lhs(0),rhs(0),next(0) { meta = E; kind = k; pos = rc; }
        ~Expression();
    };

    typedef QList<Expression*> ExpList;

    class Statement : public Node
    {
    public:
        enum Kind {
            Invalid,
            StatBlock,    // head of statement sequence
            Assig,        // assignment :=
            Call,         // procedure call (including built-in)
            If,           // IF ... THEN ... END_IF
            Elsif,        // ELSE IF (chained via next)
            Else,         // ELSE branch
            Case,         // CASE ... OF ... END_CASE
            CaseAction,   // case_label : stmt
            Alias,        // ALIAS ... FOR ... END_ALIAS
            Escape,       // ESCAPE (break from REPEAT)
            Skip,         // SKIP (continue in REPEAT)
            Return,       // RETURN [(...)]
            Repeat,       // REPEAT ... END_REPEAT
            Compound,     // BEGIN ... END
            Null,         // ; (null statement)
        };
#ifdef _DEBUG
        Kind kind;
#endif
        RowCol pos;
        Expression* lhs;   // assig lhs, case selector, alias target, repeat increment var
        Expression* rhs;    // assig rhs, condition, return expr, case labels, alias qualifiers
        Statement* body;    // then-part, case body, loop body, alias body

        Statement(Kind k = Invalid, const RowCol& p = RowCol()):pos(p),lhs(0),rhs(0),
            next(0),body(0) { kind = k; }
        Statement* getLast() const;
        Statement* getNext() const { return next; }
        void append(Statement* s);
        static void deleteAll(Statement*);
    private:
        ~Statement();
        Statement* next;    // linked list of statements
    };

    struct SchemaData {
        QString sourcePath;
        QByteArray schemaName;
        QByteArray versionId;
        RowCol end;
    };

    class Symbol
    {
    public:
        enum Kind { Invalid, Schema, Decl, DeclRef, Lval };
        quint8 kind;
        quint8 len;
        RowCol pos;
        Declaration* decl;
        Symbol* next;
        Symbol():kind(Invalid),len(0),decl(0),next(0){}
        static void deleteAll(Symbol*);
    };

    typedef QList<Symbol*> SymList;

    struct Xref {
        Symbol* syms;
        QHash<Declaration*,SymList> uses;
        QHash<Declaration*,DeclList> subs;
        Xref():syms(0){}
    };

    class AstModel
    {
    public:
        AstModel();
        ~AstModel();

        void openScope(Declaration* scope);
        Declaration* closeScope(bool takeMembers = false);
        Declaration* addDecl(const QByteArray& name);
        Declaration* findDecl(const QByteArray& name, bool recursive = true) const;
        Declaration* getTopScope() const;

        Type* getType(quint8 basicType) const { return types[basicType]; }

        static void cleanupGlobals();
        static Declaration* getGlobalScope() { return globalScope; }
    protected:
        Type* newType(Type::Kind k);
        Type* addType(const QByteArray& name, Type::Kind k);
        void addBuiltin(const QByteArray& name, quint16 id);
    private:
        QList<Declaration*> scopes;
        static Declaration* globalScope;
        static Type* types[Type::MaxKind];
    };
}
}

Q_DECLARE_METATYPE(Exp::Ast::Declaration*)
Q_DECLARE_METATYPE(Exp::Ast::Expression*)
Q_DECLARE_METATYPE(Exp::Ast::Symbol*)
Q_DECLARE_METATYPE(Exp::Ast::SchemaData)

#endif // EXPAST_H
