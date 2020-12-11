#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include "importdialog.h"

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit Highlighter(QTextDocument *parent = nullptr);
    void setSQLCommands();
    void setTableNames(QStringList table_names, QStringList column_names);
    void setColumnNames(QStringList column_names);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat sqlReadFormat;
    QTextCharFormat sqlWriteFormat;
    QTextCharFormat tableFormat;
    QTextCharFormat columnFormat;
    QTextCharFormat funcFormat;
    QStringList createRegexList(QStringList list);

signals:

};

#endif // HIGHLIGHTER_H
