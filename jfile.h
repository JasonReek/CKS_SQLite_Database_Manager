#ifndef CSV_H
#define CSV_H

#include <QString>
#include <QStringList>
#include <fstream>
#include <iostream>
#include <string>
#include <direct.h>
#include <sys/stat.h>
#include <cctype>

class JFile
{
private:
    char _deliminator;
    std::ofstream _write_file;
    std::ifstream _read_file;
    int _rows;
    int _max_columns;

public:
    JFile(char deliminator=',');
    static bool isAlpha(char c);
    static bool pathExists(const QString &path);
    static bool pathExists(const std::string &path);
    static bool makeNewDir(QString directory_path);
    static bool makeNewDir(std::string directory_path);
    static QStringList csvSplit(QString text);

    void open(QString path, char mode='r');
    void open(std::string path, char mode='r');
    void close();
    QList<QStringList> readCsv();
    void writeCsv(QString value);
    void writeCsv(std::string value);
    void newRow();
    void writeCsvRow(QStringList data);
    std::string replaceAll(std::string text, const std::string &from, const std::string &to);
    std::string wrapQuotations(std::string text);
    std::string wrapCommas(std::string text);
    QString wrapQuotations(QString text);
    QString wrapCommas(QString text);
    int rowCount();
    int maxColumns();

};

#endif // CSV_H
