#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{

}

void Highlighter::highlightBlock(const QString &text)
{
    QRegularExpressionMatchIterator match_iterator;
    QRegularExpressionMatch match;

    for (const HighlightingRule &rule : qAsConst(highlightingRules))
    {
        match_iterator = rule.pattern.globalMatch(text.toLower());
        while (match_iterator.hasNext())
        {
            match = match_iterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
}

void Highlighter::setSQLCommands()
{
    HighlightingRule rule;

    this->sqlReadFormat.setForeground(QColor("#00CCFF"));

    const QStringList sqlReadPatterns = this->createRegexList(SQLProperties::READ_COMMANDS);

    for (const QString &pattern : sqlReadPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = sqlReadFormat;
        this->highlightingRules.append(rule);
    }

    this->sqlWriteFormat.setForeground(QColor("#FF0000"));

    const QStringList sqlWritePatterns = this->createRegexList(SQLProperties::WRITE_COMMANDS);

    for (const QString &pattern : sqlWritePatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = sqlWriteFormat;
        this->highlightingRules.append(rule);
    }
}

void Highlighter::setTableNames(QStringList table_names, QStringList column_names)
{
    QStringList _table_names = table_names;
    int table_count = _table_names.length();
    HighlightingRule rule;
    this->tableFormat.setForeground(QColor("#00FF7F"));
    this->tableFormat.setFontWeight(QFont::Bold);

    QStringList tablePatterns = {};
    for(int i = 0; i < table_count; i++)
        tablePatterns.append(QString("\\b"+_table_names[i].toLower()+"\\b"));

    for (const QString &pattern : tablePatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = tableFormat;
        this->highlightingRules.append(rule);
    }

    this->setSQLCommands();
    this->setColumnNames(column_names);
    this->rehighlight();
}

void Highlighter::setColumnNames(QStringList column_names)
{
    QStringList _column_names = column_names;
    int column_count = _column_names.length();
    HighlightingRule rule;
    this->columnFormat.setForeground(QColor("#00FF7F"));

    QStringList columnPatterns = {};
    for(int i = 0; i < column_count; i++)
        columnPatterns.append(QString("\\b"+_column_names[i].toLower()+"\\b"));

    for (const QString &pattern : columnPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = columnFormat;
        this->highlightingRules.append(rule);
    }
}

QStringList Highlighter::createRegexList(QStringList string_list)
{
    QStringList regex_list = {};
    int string_count = string_list.length();

    for(int i = 0; i<string_count; i++)
        regex_list.append("\\b"+string_list[i].toLower()+"\\b");

    return regex_list;
}
