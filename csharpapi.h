#ifndef CSHARPAPI_H
#define CSHARPAPI_H

#include <QObject>

#include <Qsci/qsciglobal.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercustom.h>

class CSharpApi : public QsciLexerCPP
{
    Q_OBJECT
public:
    CSharpApi(QObject *parent);

    virtual ~CSharpApi();

    const char *language() const;

    QColor defaultColor(int style) const;

    bool defaultEolFill(int style) const;

    QFont defaultFont(int style) const;

    QColor defaultPaper(int style) const;

    const char *keywords(int set) const;

    QString description(int style) const;

    QStringList autoCompletionWordSeparators() const;

    QColor paperColor() const;

    void setPaperColor(QColor paperColor);

    void styleText(int start, int end);

private:
    CSharpApi(const CSharpApi &);
    CSharpApi &operator=(const CSharpApi &);
    QStringList autocomplete;
    QColor m_paperColor;
};

#endif
