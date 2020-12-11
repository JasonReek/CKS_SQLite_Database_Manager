#ifndef DATAFORMAT_H
#define DATAFORMAT_H

#include <QString>
#include <QStringList>
#include <QMap>

namespace DateFormats
{
const QStringList DATE_FORMAT_LIST = {
    QStringLiteral("MM/DD/YYYY"),QStringLiteral("DD/MM/YYYY"),QStringLiteral("YYYY/MM/DD"),
    QStringLiteral("Month D, YYYY"), QStringLiteral("M/D/YYYY"), QStringLiteral("D/M/YYYY"),
    QStringLiteral("YYYY/M/D"), QStringLiteral("D Month, YYYY"), QStringLiteral("YYYY, Month D"),
    QStringLiteral("Mon-DD-YYYY"), QStringLiteral("MM-DD-YYYY"), QStringLiteral("MM-DD-YYYY"),
    QStringLiteral("DD-MM-YYYY"), QStringLiteral("D-M-YYYY"), QStringLiteral("YYYY-MM-DD"),
    QStringLiteral("YYYY-M-D"), QStringLiteral("DD-Mon-YYYY"), QStringLiteral("YYYY-Mon-DD"),
    QStringLiteral("Mon DD, YYYY"), QStringLiteral("DD Mon, YYYY"), QStringLiteral("YYYY, Mon DD")
};

const QString D = QStringLiteral("D");           // Day with not leading zeros
const QString DD = QStringLiteral("DD");         // Day with leading zeros
const QString M = QStringLiteral("M");           // Month with no leading zeros
const QString MM = QStringLiteral("MM");         // Month with leading zeros
const QString MON = QStringLiteral("Mon");       // Month abreviated name
const QString MONTH = QStringLiteral("Month");   // Month full name
const QString YYYY = QStringLiteral("YYYY");     // Year

const QMap<QString, QString> MONTH_ABRV_TO_NUMBER
{
    { "jan", "01" },
    { "feb", "02" },
    { "mar", "03" },
    { "apr", "04" },
    { "may", "05" },
    { "jun", "06" },
    { "jul", "07" },
    { "aug", "08" },
    { "sep", "09" },
    { "oct", "10" },
    { "nov", "11" },
    { "dec", "12" }
};

const QMap<QString, QString> MONTH_TO_NUMBER
{
    { "january", "01" },
    { "february", "02" },
    { "march", "03" },
    { "april", "04" },
    { "may", "05" },
    { "june", "06" },
    { "july", "07" },
    { "august", "08" },
    { "september", "09" },
    { "october", "10" },
    { "november", "11" },
    { "december", "12" }
};

}

class DataFormat
{
private:
    static QString formatDate(QStringList date_format_list, QStringList date_list, QString original_value);
    static QString monthFormat(QString month_format, QString month);
    static QString dayFormat(QString day_format, QString day);

public:
    DataFormat();
    static QString toDatetime(QString date_format, QString date);

};

#endif // DATAFORMAT_H
