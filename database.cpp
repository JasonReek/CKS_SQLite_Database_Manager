#include "database.h"

Database::Database()
{
    if(QSqlDatabase::isDriverAvailable(SQL::DRIVER))
    {
        QString path = QCoreApplication::applicationDirPath() + "/database/SmartController.db";
        this->database = QSqlDatabase::addDatabase(SQL::DRIVER);
        this->database.setDatabaseName(path);
        if(!this->database.open())
        {

        }
        else if(!this->database.tables().contains("Age"))
        {
            this->createTable("Age", {"Year", "Month", "Day"}, {SQL_INTEGER, SQL_INTEGER, SQL_INTEGER});
        }
        else if(!this->database.tables().contains("Names"))
        {
            this->createTable("Names", {"First", "Last"}, {SQL_TEXT, SQL_TEXT});
        }
    }

}

QString Database::SQLType(int SQL_type_id)
{
    switch(SQL_type_id)
    {
    case SQL_NULL:
        return SQL::TYPE_NULL;
    case SQL_INTEGER:
        return SQL::TYPE_INTEGER;
    case SQL_REAL:
        return SQL::TYPE_REAL;
    case SQL_TEXT:
        return SQL::TYPE_TEXT;
    case SQL_BLOB:
        return SQL::TYPE_BLOB;
    case SQL_CLOB:
        return SQL::TYPE_CLOB;
    default:
        return SQL::TYPE_NULL;
    }
}

// QSqlQuery query("CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT)");
void Database::createTable(QString table_name, QStringList column_names, QList<int> data_types)
{
    QString table_query;
    if(column_names.length() == data_types.length())
    {
        QString table_attributes;
        for(int i = 0; i < column_names.length(); i++)
            table_attributes.append(", " + column_names.at(i) + " " + this->SQLType(data_types.at(i)));
        table_query = QString("CREATE TABLE %1 (id INTEGER PRIMARY KEY%2)").arg(table_name, table_attributes);
        QSqlQuery query(table_query);
    }
}

// if(!query.exec("INSERT INTO people(name) VALUES('Eddie Guerrero')"))
void Database::insertValuesIntoTable(QString table_name, QStringList column_names, QStringList values, QList<int> data_types)
{
    int data_type = 0;
    QSqlQuery query;
    QString table_values;
    QString query_string;
    QString table_column_names = column_names.join(", ");

    for(int i = 0; i < values.length(); i++)
    {
        data_type = data_types.at(i);
        if(data_type == SQL_INTEGER || data_type == SQL_REAL || data_type == SQL_NULL)
        {
            if(i == values.length() - 1)
                table_values.append(values.at(i));
            else
                table_values.append(values.at(i)+", ");
        }
        else
        {
            if(i == values.length() - 1)
                table_values.append("'"+values.at(i)+"'");
            else
                table_values.append("'"+values.at(i)+"', ");
        }

    }

    query_string = QString("INSERT INTO %1(%2) VALUES(%3)").arg(table_name, table_column_names, table_values);

    query.exec(query_string);
}

QString Database::readValue(QString table_name, QString column_name, int id)
{
    QSqlQuery query;
    query.prepare(QString("SELECT %2 FROM %1 WHERE id = ?").arg(table_name, column_name));
    query.addBindValue(id);
    query.exec();
    if(query.first())
        return query.value(0).toString();
    return "No Value";
}

QStringList Database::tableNames()
{
    return this->database.tables();
}

QMap<QString, QStringList> Database::table(QString table_name)
{
    QMap <QString, QStringList> table_values;
    QSqlQuery query;
    QSqlRecord record;

    query.exec(QString("SELECT * FROM %1").arg(table_name));
    if(query.first())
    {
        do
        {
        record = query.record();
        for(int column = 0; column < record.count(); column++)
            table_values[record.fieldName(column)].append(record.value(column).toString());

        }while(query.next());
    }
    return table_values;
}

QStringList Database::tableColumnNames(QString table_name)
{
    QSqlQuery query;
    QSqlRecord record;
    QStringList column_names = {};
    query.prepare(QString("SELECT * FROM %1").arg(table_name));
    query.exec();
    if(query.first())
    {
        record = query.record();
        for(int column = 0; column < record.count(); column++)
            column_names.append(record.fieldName(column));
    }
    return column_names;
}

void Database::updateValueFromCellEdit(QString table_name, QString column_name, QString value, int id)
{
    int row_id = id;
    QSqlQuery query;
    query.exec(QString("UPDATE %1 SET %2 = '%3' WHERE id = %4").arg(table_name, column_name, value).arg(row_id));
}

QSqlDatabase *Database::getDatabase()
{
    return &database;
}
