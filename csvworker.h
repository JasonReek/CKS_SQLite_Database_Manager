#ifndef CSVWORKER_H
#define CSVWORKER_H

#include "jfile.h"

#include <QObject>
#include <QThread>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSqlDatabase>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QDebug>
#include <QMetaType>


typedef QList<int> IntList;

namespace ColumnNames
{
    const QString EMPTY = QStringLiteral("--Empty--");
}

class CsvWorker : public QObject
{
    Q_OBJECT
private:
    QList<QStringList> data;
    bool _has_headers;
public:
    CsvWorker(QObject *parent = nullptr);

public slots:
    void readCsv(QString path, bool has_headers);
    void insertDataIntoTable(QString database_path,
                             QString table_name,
                             QStringList column_names,
                             IntList data_placement,
                             int row_start,
                             int row_end);

signals:
    void csvRead(QStringList column_names);
    void csvRowRead(int csv_row);
    void tableModelCompleted(QStandardItemModel *model);
    void readyToClose();
    void finishedInserting(QString path);

};

#endif // CSVWORKER_H
