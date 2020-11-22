#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>

namespace SQLCommands
{
const QStringList READ_COMMANDS = {QStringLiteral("AND"), QStringLiteral("BEGIN"), QStringLiteral("BETWEEN"),
                                   QStringLiteral("EXISTS"), QStringLiteral("IN"), QStringLiteral("NOT"),
                                   QStringLiteral("LIKE"), QStringLiteral("INTO"), QStringLiteral("INDEX"),
                                   QStringLiteral("GLOB"), QStringLiteral("OR"), QStringLiteral("IS"),
                                   QStringLiteral("UNIQUE"), QStringLiteral("NULL"), QStringLiteral("NULL"),
                                   QStringLiteral("SELECT"), QStringLiteral("BEFORE"), QStringLiteral("PRIMARY"),
                                   QStringLiteral("FROM"), QStringLiteral("TABLE"), QStringLiteral("WHERE"),
                                   QStringLiteral("KEY"), QStringLiteral("VIRTUAL"), QStringLiteral("VIEW"), QStringLiteral("BY"),
                                   QStringLiteral("COUNT"), QStringLiteral("EXISTS"), QStringLiteral("GROUP"), QStringLiteral("INNER"),
                                   QStringLiteral("JOIN"), QStringLiteral("HAVING"), QStringLiteral("VALUES"),
                                   QStringLiteral("ORDER"), QStringLiteral("PRAGMA"), QStringLiteral("RELEASE"), QStringLiteral("ON")};

const QStringList WRITE_COMMANDS = {QStringLiteral("AS"), QStringLiteral("DROP"), QStringLiteral("UPDATE"),
                                    QStringLiteral("INSERT"), QStringLiteral("DELETE"), QStringLiteral("CREATE"),
                                  QStringLiteral("ALTER"), QStringLiteral("RENAME"), QStringLiteral("COMMIT"),
                                    QStringLiteral("DETACH"), QStringLiteral("INSERT"), QStringLiteral("REINDEX")};

}

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
    QStringList createRegexList(QStringList list);

signals:

};

#endif // HIGHLIGHTER_H
