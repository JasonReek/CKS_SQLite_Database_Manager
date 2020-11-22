#ifndef TABLE_H
#define TABLE_H

#include "csv.h"
#include "dataformat.h"
#include "iconlabel.h"

#include <QLabel>
#include <QWidget>
#include <QFileDialog>
#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QCoreApplication>
#include <QStandardItemModel>
#include <Qt>
#include <QBrush>
#include <QColor>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QClipboard>
#include <QGuiApplication>
#include <QKeySequence>
#include <QVBoxLayout>
#include <QItemSelectionModel>
#include <QInputDialog>
#include <QFileInfo>

namespace SQL
{
    const int SQL_NULL = 0;
    const int SQL_INTEGER = 1;
    const int SQL_TEXT = 2;
    const int SQL_REAL = 3;
    const int SQL_BLOB = 4;
    const int SQL_CLOB = 5;
    const QString DRIVER = QStringLiteral("QSQLITE");
    const QString DATABASE_NAME = QStringLiteral("SmartController.db");
    const QString TYPE_NULL = QStringLiteral("NULL");
    const QString TYPE_INTEGER = QStringLiteral("INTEGER");
    const QString TYPE_TEXT = QStringLiteral("TEXT");
    const QString TYPE_REAL = QStringLiteral("REAL");
    const QString TYPE_BLOB = QStringLiteral("BLOB");
    const QString TYPE_CLOB = QStringLiteral("CLOB");
}

namespace SQLProperties
{
    const QStringList QuickFilterTypes = {QStringLiteral("="), QStringLiteral(">"), QStringLiteral("<"),
                                          QStringLiteral(">="), QStringLiteral("<="), QStringLiteral("Between")};
    const int EQUAL = 0;
    const int GREATER_THAN = 1;
    const int LESS_THAN = 2;
    const int GREATER_OR_EQUAL_THAN = 3;
    const int LESS_OR_EQUAL_THAN = 4;
    const int BETWEEN = 5;

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

namespace Database
{
    const QString DATABASE_DIRECTORY = QStringLiteral("/Database/");
}

struct ColumnData
{
    QString reference_table;
    QString reference_id;
};

class Table : public QTableView
{
    Q_OBJECT

private:
    bool id_toggled;
    bool query_mode;
    QSqlDatabase database;
    QSqlTableModel *model;
    QSqlQueryModel *q_model;
    int _selected_row;
    int _selected_column;
    bool _read_only;

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

    Table(QWidget *parent = nullptr);
    int rowCount();
    int columnCount();
    QStringList tableNames();
    QStringList columnNames();
    QStringList columnNames(QString table_name);
    QStringList allColumnNames();
    QString SQLType(int SQL_type_id);

    void createNewDatabase();
    void setDatabase(QString path);
    void createTable(QString table_name, QString table_id, QStringList column_names, QList<int> data_types);
    void createTable(QString table_name, QString table_id, QStringList column_names, QList<int> data_types, QMap <QString, ColumnData> column_properties);
    void removeTable(QString table_name);
    void insertValues(QString table_name, QStringList column_names, QStringList values, QList<int> data_types);
    void insertRow(QString table_name, QStringList column_names);
    void removeRow(QString table_name, QString column_name, int id);
    void removeSelectedRow();
    void runSQL(QString sql_command);
    void message(QString title, QString message);
    bool messageWarning(QString title, QString message);
    static int getSqlType(int index);
    bool singleRowSelected(QModelIndexList index_list);
    bool singleColumnSelected(QModelIndexList index_list);
    virtual void contextMenuEvent(QContextMenuEvent *event);
    bool isReadOnly();
    bool queryMode();
    static bool containsSQLWriteCommand(QString text);
    static QString sqliteTypeFromVariant(QVariant data_type);

public slots:
    void openDatabase();
    void toggleIdColumn(bool toggled);
    void setTable(QString table_name);
    void setTableFromTree(QTreeWidgetItem *item, int column);
    void valueFilter(int filter_type, QStringList filter_values, QString table_name, bool filter_empty_cells, bool case_sensitive, bool date_filter);
    void setToReadOnly(bool read_only);
    void exportCSVFile();
    void copyCells();
    void onCellChange(QModelIndex current, QModelIndex previous);
    void formatDateColumn();

signals:
    void Column_Names_Set(QStringList);
    void Column_Name_Index_Changed(int);
    void Table_Name_Changed(QString);
    void New_Database_Set();
    void Query_Was_Executed(bool);
    void Rows_And_Columns_Changed();
    void Data_Preview(QString cell_value, QString data_type, QString column_name, int row, int column);
};

#endif // TABLE_H
