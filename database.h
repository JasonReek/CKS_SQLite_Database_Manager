#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql>
#include <QList>
#include <QCoreApplication>

namespace SQL
{
    const QString DRIVER = "QSQLITE";
    const QString DATABASE_NAME = "SmartController.db";
    const QString TYPE_NULL = "NULL";
    const QString TYPE_INTEGER = "INTEGER";
    const QString TYPE_TEXT = "TEXT";
    const QString TYPE_REAL = "REAL";
    const QString TYPE_BLOB = "BLOB";
    const QString TYPE_CLOB = "CLOB";
}

class Database
{
private:
    QSqlDatabase database;

public:
    enum SQLType
    {
      SQL_NULL,
      SQL_INTEGER,
      SQL_TEXT,
      SQL_REAL,
      SQL_BLOB,
      SQL_CLOB
    };

    Database();
    ~Database(){};
    QString SQLType(int SQL_type_id);
    void createTable(QString table_name, QStringList column_names, QList<int> data_types);
    void insertValuesIntoTable(QString table_name, QStringList column_names, QStringList values, QList<int> data_types);
    QString readValue(QString table_name, QString column_name, int id);
    QStringList tableNames();
    QStringList tableColumnNames(QString table_name);
    QMap<QString, QStringList> table(QString table_name);
    void updateValueFromCellEdit(QString table_name, QString column_name, QString value, int id);
    QSqlDatabase *getDatabase();
};

#endif // DATABASE_H
