#ifndef CSV_H
#define CSV_H

#include <QString>
#include <QStringList>
#include <fstream>
#include <iostream>
#include <string>

class CSV
{
private:
    char _deliminator;
    std::ofstream _file;

public:
    CSV(char deliminator=',');
    void open(QString path);
    void open(std::string path);
    void close();
    void write(QString value);
    void write(std::string value);
    void newRow();
    void writeRow(QStringList data);
    std::string replaceAll(std::string text, const std::string &from, const std::string &to);
    std::string wrapQuotations(std::string text);
    std::string wrapCommas(std::string text);
    QString wrapQuotations(QString text);
    QString wrapCommas(QString text);
};

#endif // CSV_H
