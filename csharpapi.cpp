#include "csharpapi.h"
#include "Qsci/qsciapis.h"

#include <qcolor.h>
#include <qfont.h>

QStringList autocompletion = {
    "Time",
    "Time.deltaTime The interval in seconds from the last frame to the current one (Read Only).",
    ".AddForce(Vector3 force, ForceMode mode = ForceMode.Force) Adds a force to the Rigidbody.",

    "abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char", "checked",
    "class", "const", "continue", "decimal", "default", "delegate", "do", "double", "else",
    "enum", "event", "explicit", "extern", "false", "finally", "fixed", "float", "for",
    "foreach", "goto", "if", "implicit", "in", "int", "interface", "internal", "is", "lock",
    "long", "namespace", "new", "null", "object", "operator", "out", "override", "params",
    "private", "protected", "public", "vreadonly", "ref", "return", "sbyte", "sealed",
    "short", "sizeof", "stackalloc", "static", "string", "struct", "switch", "this",
    "throw", "true", "try", "typeof", "uint", "ulong", "unchecked", "unsafe", "ushort",
    "using", "virtual", "void", "while", "volatile", "and", "or", "async", "await", "var", "let", "not",
};

CSharpApi::CSharpApi(QObject *parent) : QsciLexerCPP(parent)
{
    QsciAPIs *api = new QsciAPIs(this);
    foreach(const QString &word, autocompletion) {
        api->add(word);
    }
    api->prepare();
    setAPIs(api);
};

CSharpApi::~CSharpApi(void) {};

QStringList CSharpApi::autoCompletionWordSeparators() const
{
    QStringList wl;

    wl << ".";

    return wl;
}


const char *CSharpApi::language() const
{
    return "C#";
}

QColor CSharpApi::defaultColor(int style) const
{
    if (style == VerbatimString)
        return QColor(0x00,0x7f,0x00);

    if (style == GlobalClass)
        return QColor(0x00,0x7f,0x00);

    return QsciLexerCPP::defaultColor(style);
}

bool CSharpApi::defaultEolFill(int style) const
{
    if (style == VerbatimString)
        return true;

    return QsciLexerCPP::defaultEolFill(style);
}

QFont CSharpApi::defaultFont(int style) const
{
    if (style == VerbatimString)
#if defined(Q_OS_WIN)
        return QFont("Courier New",10);
#elif defined(Q_OS_MAC)
        return QFont("Courier", 12);
#else
        return QFont("Bitstream Vera Sans Mono",9);
#endif

    return QsciLexerCPP::defaultFont(style);
}

const char *CSharpApi::keywords(int set) const
{
    if(set == 1)
        return "abstract as base bool break byte case catch char checked "
               "class const continue decimal default delegate do double else "
               "enum event explicit extern false finally fixed float for "
               "foreach goto if implicit in int interface internal is lock "
               "long namespace new null object operator out override params "
               "private protected public readonly ref return sbyte sealed "
               "short sizeof stackalloc static string struct switch this "
               "throw true try typeof uint ulong unchecked unsafe ushort "
               "using virtual void while volatile and or async await var let not";

    return 0;
}

QString CSharpApi::description(int style) const
{
    if (style == VerbatimString)
        return tr("Verbatim string");

    return QsciLexerCPP::description(style);
}

QColor CSharpApi::defaultPaper(int style) const
{
    if (style == VerbatimString)
        return QColor(0xe0,0xff,0xe0);

    return QsciLexerCPP::defaultPaper(style);
}
