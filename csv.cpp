#include "csv.h"

CSV::CSV(char deliminator)
{
    this->_deliminator = deliminator;

}

void CSV::open(QString path)
{
    if(path.length())
        this->_file.open(path.toStdString().c_str());
}

void CSV::open(std::string path)
{
    if(path.length())
        this->_file.open(path.c_str());
}

void CSV::close()
{
    if(this->_file)
        this->_file.close();
}

void CSV::write(QString value)
{
    if(this->_file)
        this->_file << value.toStdString() << this->_deliminator;
}

void CSV::write(std::string value)
{
    if(this->_file)
        this->_file << value << this->_deliminator;
}

void CSV::newRow()
{
    if(this->_file)
        this->_file << '\n';
}

void CSV::writeRow(QStringList data)
{
    QString data_format;

    if(this->_file)
    {
        for(int i = 0; i < data.length(); i++)
        {
            data_format = data[i].remove('\n');
            if(data_format.contains('"'))
                data_format = this->wrapQuotations(data_format);
            else if(data_format.contains(','))
                data_format = this->wrapCommas(data_format);
            if(data.length() > 1)
                this->_file << data_format.toStdString() << this->_deliminator;
            else
                this->_file << data_format.toStdString();
        }
        this->_file << '\n';
    }
}

std::string CSV::replaceAll(std::string text, const std::string &from, const std::string &to)
{
    size_t start_position = 0;
    while((start_position = text.find(from, start_position)) != std::string::npos)
    {
        text.replace(start_position, from.length(), to);
        start_position += to.length();
    }
    return text;
}

std::string CSV::wrapQuotations(std::string text)
{
    return this->replaceAll(text, "\"", "\"\"");
}

std::string CSV::wrapCommas(std::string text)
{
    return '"'+text+'"';
}

QString CSV::wrapQuotations(QString text)
{
    QString f_text;
    f_text = text.replace('"', "\"\"");
    return '"'+f_text+'"';
}

QString CSV::wrapCommas(QString text)
{
    return '"'+text+'"';
}




