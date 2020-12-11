#include "jfile.h"

JFile::JFile(char deliminator)
{
    this->_rows = 0;
    this->_max_columns = 0;
    this->_deliminator = deliminator;

}

bool JFile::isAlpha(char c)
{
    return(c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool JFile::pathExists(const QString &path)
{
  struct stat buffer;
  return (stat (path.toStdString().c_str(), &buffer) == 0);
}

bool JFile::pathExists(const std::string &path)
{
  struct stat buffer;
  return (stat (path.c_str(), &buffer) == 0);
}

void JFile::open(QString path, char mode)
{
    mode = tolower(mode);
    if(path.length() && mode == 'r')
        this->_read_file.open(path.toStdString().c_str());
    else if(path.length() && mode == 'w')
        this->_write_file.open(path.toStdString().c_str());
}

void JFile::open(std::string path, char mode)
{
    mode = tolower(mode);
    if(path.length() && mode == 'r')
        this->_read_file.open(path.c_str());
    else if(path.length() && mode == 'w')
        this->_write_file.open(path.c_str());
}

void JFile::close()
{
    if(this->_write_file)
        this->_write_file.close();
}

// Note - Windows only solution
bool JFile::makeNewDir(QString directory_path)
{
    if(directory_path.length() && !JFile::pathExists(directory_path))
       return (_mkdir(directory_path.toStdString().c_str()) == 0);
    return false;
}

// Note - Windows only solution
bool JFile::makeNewDir(std::string directory_path)
{
    if(directory_path.length() && !JFile::pathExists(directory_path))
        return (_mkdir(directory_path.c_str()) == 0);
     return false;
}

QList<QStringList> JFile::readCsv()
{
    QList<QStringList> data = {};
    QStringList csv_row;
    QString q_line;
    std::string line;
    int number_of_columns = 0;

    this->_rows = 0;
    this->_max_columns = 0;
    if(this->_read_file.is_open())
    {
        this->_read_file.clear();
        this->_read_file.seekg(0);
        while(this->_read_file)
        {
            std::getline(this->_read_file, line);
            q_line = QString(line.c_str());
            csv_row = JFile::csvSplit(q_line);
            number_of_columns = csv_row.length();
            if(number_of_columns)
            {
                data.append(csv_row);
                if(number_of_columns > this->_max_columns)
                    this->_max_columns = number_of_columns;
                this->_rows++;
            }
        }
    }
    return data;
}

void JFile::writeCsv(QString value)
{
    if(this->_write_file.is_open())
        this->_write_file << value.toStdString() << this->_deliminator;
}

void JFile::writeCsv(std::string value)
{
    if(this->_write_file.is_open())
        this->_write_file << value << this->_deliminator;
}

void JFile::newRow()
{
    if(this->_write_file.is_open())
        this->_write_file << '\n';
}

void JFile::writeCsvRow(QStringList data)
{
    QString data_format;

    if(this->_write_file.is_open())
    {
        for(int i = 0; i < data.length(); i++)
        {
            data_format = data[i].remove('\n');
            data_format = data_format.trimmed();
            if(data_format.contains('"'))
                data_format = this->wrapQuotations(data_format);
            else if(data_format.contains(','))
                data_format = this->wrapCommas(data_format);
            if(data.length() > 1)
                this->_write_file << data_format.toStdString() << this->_deliminator;
            else
                this->_write_file << data_format.toStdString();
        }
        this->_write_file << '\n';
    }
}

std::string JFile::replaceAll(std::string text, const std::string &from, const std::string &to)
{
    size_t start_position = 0;
    while((start_position = text.find(from, start_position)) != std::string::npos)
    {
        text.replace(start_position, from.length(), to);
        start_position += to.length();
    }
    return text;
}

std::string JFile::wrapQuotations(std::string text)
{
    return this->replaceAll(text, "\"", "\"\"");
}

std::string JFile::wrapCommas(std::string text)
{
    return '"'+text+'"';
}

QString JFile::wrapQuotations(QString text)
{
    QString f_text;
    f_text = text.replace('"', "\"\"");
    return '"'+f_text+'"';
}

QString JFile::wrapCommas(QString text)
{
    return '"'+text+'"';
}

QStringList JFile::csvSplit(QString text)
{
    QStringList split_text = {};
    QString split = "";

    char current_char = NULL;
    char last_char = NULL;
    int char_count = text.length();
    int quotation_count = 0;
    bool inside_quotations = false;

    for(int i = 0; i < char_count; i++)
    {
        current_char = text[i].toLatin1();
        switch(current_char)
        {
        case '"':
            if(quotation_count && quotation_count % 2 == 0)
            {
                split.append(current_char);
                inside_quotations = !inside_quotations;
            }
            else if(quotation_count == 1 && inside_quotations)
                inside_quotations = !inside_quotations;
            quotation_count++;
            break;
        case ',':
            if(inside_quotations)
                split.append(current_char);
            // Next comma separated value
            else
            {
                split_text.append(split);
                split = "";
                quotation_count = 0;
            }

            break;
        default:
            // Inside quotations or outside of quotations
            if(quotation_count && (quotation_count % 2 == 0))
                inside_quotations = !inside_quotations;
            else if(quotation_count == 1)
                inside_quotations = !inside_quotations;
            split.append(current_char);
        }
        last_char = current_char;

    }
    if(split.length())
        split_text.append(split);
    return split_text;
}

int JFile::rowCount()
{
    return this->_rows;
}

int JFile::maxColumns()
{
    return this->_max_columns;
}



