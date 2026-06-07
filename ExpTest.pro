QT       += core
QT       -= gui

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ..

CONFIG(debug, debug|release) {
    DEFINES += _DEBUG
}



!win32 {
    QMAKE_CXXFLAGS += -Wno-reorder -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-switch \
        -Wno-deprecated-declarations -Wno-sign-compare -Wno-parentheses -Wno-unused-parameter -Werror=return-type
}

HEADERS += \
    ExpAst.h \
    ExpLexer.h \
    ExpParser.h \
    ExpParser2.h \
    ExpRowCol.h \
    ExpSynTree.h \
    ExpToken.h \
    ExpTokenType.h

SOURCES += \
    ExpAst.cpp \
    ExpLexer.cpp \
    ExpParser.cpp \
    ExpParser2.cpp \
    ExpRowCol.cpp \
    ExpSynTree.cpp \
    ExpTest.cpp \
    ExpToken.cpp \
    ExpTokenType.cpp



