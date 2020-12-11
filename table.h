#ifndef TABLE_H
#define TABLE_H

#include "csvworker.h"
#include "dataformat.h"
#include "iconlabel.h"

#include <QFileDialog>
#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <Qt>
#include <QBrush>
#include <QColor>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QSqlError>
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

namespace SQL
{
    const int SQL_NULL = 0;
    const int SQL_INTEGER = 1;
    const int SQL_TEXT = 2;
    const int SQL_REAL = 3;
    const int SQL_BLOB = 4;
    const int SQL_CLOB = 5;
    const QString DRIVER = QStringLiteral("QSQLITE");
    const QString TYPE_NULL = QStringLiteral("NULL");
    const QString TYPE_INTEGER = QStringLiteral("INTEGER");
    const QString TYPE_TEXT = QStringLiteral("TEXT");
    const QString TYPE_REAL = QStringLiteral("REAL");
    const QString TYPE_BLOB = QStringLiteral("BLOB");
    const QString TYPE_CLOB = QStringLiteral("CLOB");
}

namespace SQLProperties
{
    const QStringList QuickFilterTypes = {QStringLiteral("="), QStringLiteral("!="), QStringLiteral(">"), QStringLiteral("<"),
                                          QStringLiteral(">="), QStringLiteral("<="), QStringLiteral("Between")};
    const int EQUAL = 0;
    const int NOT_EQUAL = 1;
    const int GREATER_THAN = 2;
    const int LESS_THAN = 3;
    const int GREATER_OR_EQUAL_THAN = 4;
    const int LESS_OR_EQUAL_THAN = 5;
    const int BETWEEN = 6;

    const QStringList READ_COMMANDS = {QStringLiteral("AND"), QStringLiteral("BEGIN"), QStringLiteral("BETWEEN"),
                                       QStringLiteral("IN"), QStringLiteral("NOT"),
                                       QStringLiteral("LIKE"), QStringLiteral("INTO"), QStringLiteral("INDEX"),
                                       QStringLiteral("GLOB"), QStringLiteral("OR"), QStringLiteral("IS"),
                                       QStringLiteral("UNIQUE"), QStringLiteral("NULL"), QStringLiteral("NULL"),
                                       QStringLiteral("SELECT"), QStringLiteral("BEFORE"), QStringLiteral("PRIMARY"),
                                       QStringLiteral("FROM"), QStringLiteral("TABLE"), QStringLiteral("WHERE"),
                                       QStringLiteral("KEY"), QStringLiteral("VIRTUAL"), QStringLiteral("VIEW"),
                                       QStringLiteral("BY"),  QStringLiteral("COUNT"), QStringLiteral("EXISTS"),
                                       QStringLiteral("GROUP"), QStringLiteral("INNER"),
                                       QStringLiteral("JOIN"), QStringLiteral("HAVING"), QStringLiteral("VALUES"),
                                       QStringLiteral("ORDER"), QStringLiteral("PRAGMA"), QStringLiteral("RELEASE"),
                                       QStringLiteral("ON"), QStringLiteral("DISTINCT"), QStringLiteral("LIMIT"),
                                       QStringLiteral("LEFT"), QStringLiteral("CROSS"), QStringLiteral("SELF"),
                                       QStringLiteral("FULL"), QStringLiteral("OUTER"), QStringLiteral("HAVING"),
                                       QStringLiteral("UNION"), QStringLiteral("EXCEPT"), QStringLiteral("INTERSECT"),
                                       QStringLiteral("CASE"), QStringLiteral("AS")};


    const QStringList WRITE_COMMANDS = {QStringLiteral("DROP"), QStringLiteral("UPDATE"),
                                        QStringLiteral("DELETE"), QStringLiteral("CREATE"), QStringLiteral("REPLACE"),
                                        QStringLiteral("ALTER"), QStringLiteral("RENAME"), QStringLiteral("COMMIT"),
                                        QStringLiteral("DETACH"), QStringLiteral("INSERT"), QStringLiteral("REINDEX"),
                                        QStringLiteral("TRANSACTION"), QStringLiteral("ROLLBACK"), QStringLiteral("VACCUM")};

    const QStringList FUNCTIONS = {QStringLiteral("abs"), QStringLiteral("avg"), QStringLiteral("changes"),
                                   QStringLiteral("count"), QStringLiteral("date"), QStringLiteral("datetime"),
                                   QStringLiteral("group_concat"), QStringLiteral("instr"), QStringLiteral("length"),
                                   QStringLiteral("lower"), QStringLiteral("ltrim"), QStringLiteral("max"),
                                   QStringLiteral("min"), QStringLiteral("random"), QStringLiteral("replace"),
                                   QStringLiteral("round"),QStringLiteral("rtrim"), QStringLiteral("strftime"),
                                   QStringLiteral("substr"), QStringLiteral("sum"),QStringLiteral("time"),
                                   QStringLiteral("trim"), QStringLiteral("typeof"),QStringLiteral("upper")};
}

namespace Database
{
    const QString DATABASE_DIRECTORY = QStringLiteral("/database/");
    const QString DB_DIR_NAME = QStringLiteral("database");
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
    QString sqlite_database_path;
    QString access_database_path;
    int _query_rows;
    bool id_toggled;
    bool query_mode;
    bool access_loaded;
    QSqlDatabase _database;
    QSqlTableModel *t_model;
    QSqlQueryModel *q_model;
    QSqlTableModel *access_t_model;
    QSqlQueryModel *access_q_model;
    int _selected_row;
    int _selected_column;
    bool _read_only;
    QString _database_path;
    void setCaseForFormat(bool lower);
    void formatCapitalizeColumn(char delimiter = ' ');
    QSqlDatabase access_database;


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

    enum DatabaseType
    {
        SQLITE,
        ACCESS,
        ORACLE
    };

    DatabaseType active_database_type;
    Table(QWidget *parent = nullptr);
    int rowCount();
    int columnCount();
    QStringList tableNames();
    QStringList columnNames();
    QStringList columnNames(QString table_name);
    QStringList allColumnNames();
    QString SQLType(int SQL_type_id);

    QSqlDatabase activeDatabase();
    QSqlDatabase database(DatabaseType database_type);
    QSqlTableModel *activeTableModel();
    QSqlTableModel *tableModel(DatabaseType database_type);
    QSqlQueryModel *activeQueryModel();
    QSqlQueryModel *queryModel(DatabaseType database_type);
    DatabaseType databaseType();

    void createNewDatabase();
    void createTable(QString table_name, QString table_id, QStringList column_names, QList<int> data_types);
    void createTable(QString table_name, QString table_id, QStringList column_names, QList<int> data_types, QMap <QString, ColumnData> column_properties);
    void removeTable(QString table_name);
    void insertValues(QString table_name, QStringList column_names, QStringList values);
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
    void detectAndSetDatabase(QString table_name);
    QString databasePath();
    static bool containsSQLWriteCommand(QString text);
    static QString sqliteTypeFromVariant(QVariant data_type);

public slots:
    void openDatabase();
    void setDatabase(QString path);
    void closeDatabase();
    void toggleIdColumn(bool toggled);
    void setTable(QString table_name);
    void setTableFromTree(QTreeWidgetItem *item, int column);
    void valueFilter(int filter_type, QStringList filter_values, QString table_name, bool filter_empty_cells, bool case_sensitive, bool date_filter);
    void setToReadOnly(bool read_only);
    void exportCSVFile();
    void copyCells();
    void onCellChange(QModelIndex current, QModelIndex previous);
    void formatDateColumn();
    void formatToLower();
    void formatToUpper();
    void formatCapital_1();
    void formatCapital_2();
    void importAccessDatabase();
    void importAccessDatabaseOnTheFly(QString path);

signals:
    void cellChanged(int column_index);
    void Column_Names_Set(QStringList);
    void Column_Name_Index_Changed(int);
    void Table_Name_Changed(QString);
    void New_Database_Set(QString path = "");
    void Query_Was_Executed(bool);
    void Rows_And_Columns_Changed();
    void Data_Preview(QString cell_value, QString data_type, QString column_name, int row, int column);
    void sqlWriteCommandExecuted();
    void newImportedDatabaseSet(QString path = "");
};

#endif // TABLE_H
