The goal of this project is to build an EXPRESS parser, code model, navigator (similar to https://github.com/rochus-keller/ActiveOberon) and other tools suitable to study the various models published by the STEP (ISO 10303) standard series, and to experiment with interesting alternative uses of the language.

This project is work in progress.

## What is EXPRESS

EXPRESS is a formal information modeling language in development and use since 1982 and eventually standardized as ISO 10303-11 in 1994. It is the data definition backbone of [STEP](https://en.wikipedia.org/wiki/ISO_10303) (STandard for the Exchange of Product model data), and provides a rigorous, machine-processable way to specify data schemas. Schemas are entities with typed attributes, inheritance hierarchies, aggregation types, constraint rules, and algorithmic functions, covering the full lifecycle of industrial product data.

Its "contenders" at the time were e.g. IDEF1X (a graphical entity-relationship method developed for the US Department of Defense), or ASN.1 (a standarized specification language for telecommunications protocols and data), and in later years UML with OCL (Object Constraint Language), XML Schema or OWL (Web Ontology Language).

EXPRESS originally started as part of the PDDI (Product Definition Data Interface) program by the US Air Force at the McDonnell Douglas Corporation, headed by Douglas Schenck. The language was first called “PDDI DSL” (Data Specification Language). PDDI and later PDES (Product Data Exchange using STEP) were pioneering CAD/CAM data exchange programs, which laid the methodological foundation for the international STEP standard series, which is used today for product data exchange.

Unlike general-purpose programming languages, EXPRESS is purpose-built for defining *what data looks like* and *what invariants it must satisfy*. Its syntax is Pascal-like: schemas, entities, types, functions, procedures, and rules compose in a block-structured, strongly typed style directly descended from the Wirth tradition. Keywords are globally reserved, identifiers are case-insensitive, and the type system includes novel features such as SELECT (discriminated union over entity types), ONEOF/AND/ANDOR supertype constraints, and built-in aggregation types (SET, BAG, LIST, ARRAY) with bound specifications.

## Why is EXPRESS interesting

I came in contact with ISO 10303 and the EXPRESS language many years ago when I was working as a systems engineering consultant and mangager for government projects. EXPRESS is fascinating for different reasons. I'm generally interested in programming language design and implemented many parsers and compilers over the years, particularly in the Algol and Wirth lineage of programming languages (such as [Oberon](https://github.com/rochus-keller/Oberon), [ActiveOberon](https://github.com/rochus-keller/ActiveOberon), [Simula 67](https://github.com/rochus-keller/simula/), and more recently [Luon](https://github.com/rochus-keller/Luon) and [Micron](https://github.com/rochus-keller/Micron)). So EXPRESS was on my list for a long time for this reason, but also because I wanted a tool to do specific analyses on the STEP models. 

And 2026 also marks the **40th anniversary of EXPRESS**: in January 1986, Douglas Schenck submitted the first complete language specification bearing the EXPRESS name to ISO/TC 184/SC 4/WG 1. The grammar and core semantics were already stable by 1986; what followed was documentation and consensus, not redesign.

For many years I'm also interested in [MUMPS](https://en.wikipedia.org/wiki/MUMPS) (Massachusetts General Hospital Utility Multi-Programming System, which celebrates 60th anniversary in 2026!) and the integration of programming languages with (no-sql) databases (see e.g. [Udb](https://github.com/rochus-keller/udb) and [Sdb](https://github.com/rochus-keller/sdb)). I want to explore EXPRESS as a *typed counterpart to MUMPS*: a language that unifies data definition, data manipulation, and query within a single formally specified notation, but with the strong typing, declarative constraints, and algebraic rigor that MUMPS deliberately omitted. EXPRESS already provides data definition (schemas, entities, types) and constraint specification (WHERE rules, UNIQUE constraints, global RULE declarations). Extending it with behavioral semantics would yield a language occupying a unique point in the design space: Wirth-style syntactic clarity, MUMPS-like integration of data and computation, and formal verification capabilities enabled by its declarative constraint system.

Yet a further goal is to study extensions of EXPRESS with enhanced constraint capabilities and behavioral semantics sufficient for formal verification: proving that schema invariants are preserved under all valid state transitions, that derived attributes are consistent, and that subtype constraints are satisfiable, and from that to eventually [generate formally verified business applications](https://rochuskeller.substack.com/p/why-rust-solves-a-problem-we-no-longer), all based on an established language accessible to "normal" people (i.e. without a PhD in formal methods).

In that context, it is interesting to note that a parallel and equally instructive story unfolded in ISO 15926, the standard for lifecycle data integration of process plants (oil, gas, chemical, nuclear). Its Part 2 data model (ISO 15926-2:2003), the generic upper ontology of classes, relationships, and temporal parts is *written in EXPRESS*. When the community later pursued Semantic Web implementation, they created Part 7 (template methodology based on first-order logic) and Part 8 (OWL/RDF implementation, ISO/TS 15926-8:2011), which translates the EXPRESS-native data model into OWL declarations. The OWL version is explicitly described as a derived representation, not a replacement. The original EXPRESS specification remains the normative source. 

The lesson from both STEP and ISO 15926 is consistent: every attempt to replace EXPRESS has either failed outright, produced a partial mapping that loses essential semantics, or ended up using EXPRESS as the underlying normative substrate. 

#### Status on June 7, 2026

The parser finally works and generates an AST (ready for validation and further processing). It is able to parse my ~1000 EXPRESS files (~37 MB of code) in ~5 seconds on my Lenovo T480. I even found a good dozen syntax errors in these files (some of which from the official standards). The grammar was derived from the one included with ISO 10303-11:2004 using my [EbnfStudio](https://github.com/rochus-keller/ebnfstudio/). I even added exact LL(k) calculation to EbnfStudio to make sure it doesn't overlook an ambiguity. Next I will implement a cross-referencing tool with semantic navigation.


#### Precompiled versions

Not available at this time.

### Build Steps

Follow these steps if you want to build the application yourself:

1. Make sure a Qt 5.x (libraries and headers) version compatible with your C++ compiler is installed on your system.
1. Download the source code from https://github.com/rochus-keller/Express/archive/master.zip and unpack it.
1. Goto the unpacked directory and execute `QTDIR/bin/qmake ExpTest.pro` (see the Qt documentation concerning QTDIR).
1. Run make; after a couple of seconds you will find the executable in the build directory.

Alternatively you can open the *.pro files using QtCreator and build everything there.

## Support
If you need support or would like to post issues or feature requests please use the Github issue list at https://github.com/rochus-keller/Express/issues or send an email to the author.



 
