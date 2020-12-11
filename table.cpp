#include "table.h"

Table::Table(QWidget *parent) : QTableView(parent)
{
    Q_INIT_RESOURCE(Resources);

    this->active_database_type = SQLITE;
    this->query_mode = false;
    this->id_toggled = false;
    this->access_loaded = false;

    this->_query_rows = 0;
    this->_selected_row = 0;
    this->_selected_column = 0;

    //QString path = QCoreApplication::applicationDirPath().append("/Database/LeakDetectionDatabase.db");

    this->_database = QSqlDatabase::addDatabase("QSQLITE");
    //this->database.setDatabaseName(path);
    this->q_model = new QSqlQueryModel(this);
}

Table::DatabaseType Table::databaseType()
{
    return this->active_database_type;
}

QSqlDatabase Table::activeDatabase()
{
    switch(this->active_database_type)
    {
    case SQLITE:
        return this->_database;
    case ACCESS:
        return this->access_database;
    case ORACLE:
        return this->_database;
    }
}

QSqlDatabase Table::database(DatabaseType database_type)
{
    switch(database_type)
    {
    case SQLITE:
        return this->_database;
    case ACCESS:
        return this->access_database;
    case ORACLE:
        return this->_database;
    }
}

QSqlTableModel *Table::activeTableModel()
{
    switch(this->active_database_type)
    {
    case SQLITE:
        return this->t_model;
    case ACCESS:
        return this->access_t_model;
    case ORACLE:
        return this->t_model;
    }
}

QSqlTableModel *Table::tableModel(DatabaseType database_type)
{
    switch(database_type)
    {
    case SQLITE:
        return this->t_model;
    case ACCESS:
        return this->access_t_model;
    case ORACLE:
        return this->t_model;
    }
}

QSqlQueryModel *Table::activeQueryModel()
{
    switch(this->active_database_type)
    {
    case SQLITE:
        return this->q_model;
    case ACCESS:
        return this->access_q_model;
    case ORACLE:
        return this->q_model;
    }
}

QSqlQueryModel *Table::queryModel(DatabaseType database_type)
{
    switch(database_type)
    {
    case SQLITE:
        return this->q_model;
    case ACCESS:
        return this->access_q_model;
    case ORACLE:
        return this->q_model;
    }
}

void Table::createNewDatabase()
{
    QString database_name;
    QString path;
    QString database_dir_path = QCoreApplication::applicationDirPath().append("/"+Database::DB_DIR_NAME);
    bool ok;

    // Check to make sure the database dir exists,
    // if not, create it.
    if(!JFile::pathExists(database_dir_path))
        JFile::makeNewDir(database_dir_path);

    database_name = QInputDialog::getText(this, tr("New Database"),
                                         tr("Database Name:"), QLineEdit::Normal,
                                         "", &ok);
    path = database_dir_path+"/"+database_name+".db";

    if(ok)
    {
        // If database already exists, rerun the prompt.
        if (JFile::pathExists(path))
        {
            this->message("Database Already Exists", "Database name is already in use.");
            this->createNewDatabase();
        }
        // The database doesn't exist, create it!
        else
            this->setDatabase(path);
    }
}

void Table::openDatabase()
{
    QString path;
    QString database_dir_path = QCoreApplication::applicationDirPath().append("/"+Database::DB_DIR_NAME);
    // Check to make sure the database dir exists,
    // if not, create it.
    if(!JFile::pathExists(database_dir_path))
        JFile::makeNewDir(database_dir_path);

    path = QFileDialog::getOpenFileName(this, "Open Database", database_dir_path, tr("Database files(*.db *.sqlite *.db3 *.sqlite3);;All files(*)"));
    if(path.length())
    {
        this->setDatabase(path);
        this->sqlite_database_path = path;
        this->_database_path = path;
    }
}

void Table::closeDatabase()
{
    this->activeDatabase().close();
}

void Table::importAccessDatabase()
{
    QString path;
    QString database_dir_path = QCoreApplication::applicationDirPath().append("/"+Database::DB_DIR_NAME);
    path = QFileDialog::getOpenFileName(this, "Open Database", database_dir_path, tr("Access files(*.mdb, *.accdb);;All files(*)"));
    if(path.length())
    {
        this->access_database = QSqlDatabase::addDatabase("QODBC");
        qDebug() << QSqlDatabase::isDriverAvailable("QODBC");
        this->access_database.setDatabaseName(QString("Driver={Microsoft Access Driver (*.mdb, *.accdb)};"
                                                      "DBQ=%1;").arg(path));

        if(this->access_database.open())
        {
            this->access_database_path = path;
            this->active_database_type = ACCESS;
            this->access_t_model = new QSqlTableModel(this, this->access_database);
            this->access_t_model->setEditStrategy(QSqlTableModel::OnFieldChange);
            if(this->access_database.tables().length())
            {
                this->access_t_model->setTable(this->access_database.tables()[0]);
                this->access_t_model->select();
            }
            this->setModel(this->access_t_model);
            connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &Table::onCellChange);
            emit New_Database_Set(path);
        }
        else
          qDebug() << this->access_database.lastError().text();
    }
}

void Table::importAccessDatabaseOnTheFly(QString path)
{
    if(path.length())
    {
        if(!this->access_database.open())
            this->access_database = QSqlDatabase::addDatabase("QODBC");

        this->access_database.setDatabaseName(QString("Driver={Microsoft Access Driver (*.mdb, *.accdb)};"
                                                      "DBQ=%1;").arg(path));
        if(this->access_database.open())
        {
            this->access_database_path = path;
            this->active_database_type = ACCESS;
            this->access_t_model = new QSqlTableModel(this, this->access_database);
            this->access_t_model->setEditStrategy(QSqlTableModel::OnFieldChange);
            if(this->access_database.tables().length())
            {
                this->access_t_model->setTable(this->access_database.tables()[0]);
                this->access_t_model->select();
            }
            this->setModel(this->access_t_model);
            connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &Table::onCellChange);
            emit New_Database_Set(path);
        }
        else
          qDebug() << this->access_database.lastError().text();
    }
}

void Table::setDatabase(QString path)
{
    // Database may have been closed, and used in another
    // thread, check to reopen it in the main thread.
    if(!this->_database.open())
        this->_database = QSqlDatabase::addDatabase("QSQLITE");

    this->_database.setDatabaseName(path);

    if(this->_database.open())
    {
        this->active_database_type = SQLITE;
        this->t_model = new QSqlTableModel(this, this->_database);
        this->t_model->setEditStrategy(QSqlTableModel::OnFieldChange);
        if(this->_database.tables().length())
        {
            this->t_model->setTable(this->_database.tables()[0]);
            this->t_model->select();
        }
        QSqlQuery query("PRAGMA foreign_keys = ON;", this->_database);
        this->setModel(this->t_model);
        connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &Table::onCellChange);
        emit New_Database_Set(path);
    }
}

void Table::setTable(QString table_name)
{
    if(this->query_mode)
    {
        this->setModel(this->activeTableModel());
        connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &Table::onCellChange);
        this->query_mode = false;
    }
    this->activeTableModel()->setTable(table_name);
    this->activeTableModel()->select();
    this->activeTableModel()->setHeaderData(0, Qt::Horizontal, QIcon(Icons::KEY_ID_ICON), Qt::DecorationRole);
    this->activeTableModel()->setHeaderData(0, Qt::Horizontal, Qt::AlignJustify, Qt::TextAlignmentRole);

    emit Column_Names_Set(this->columnNames());
}

void Table::setTableFromTree(QTreeWidgetItem *item, int column)
{
    QString table_name;
    QStringList table_names;
    table_name = item->text(column);

    if(item->childCount() > 0)
    {
        this->detectAndSetDatabase(table_name);
        emit Table_Name_Changed(table_name);
    }
    else if(item->parent() && item->parent()->childCount())
    {
        int index = item->parent()->indexOfChild(item);
        table_name = item->parent()->text(column);
        this->detectAndSetDatabase(table_name);
        emit Table_Name_Changed(table_name);
        emit Column_Name_Index_Changed(index);
    }
}

int Table::rowCount()
{
    if(activeDatabase().tables().length())
    {
        if(this->query_mode)
            return this->_query_rows;
        else
        {
            QSqlQuery query;
            query.exec(QString("SELECT COUNT(*) FROM %1").arg(this->activeTableModel()->tableName()));
            if(query.first())
                return query.value(0).toInt();
            return this->activeTableModel()->rowCount();
        }
    }
    return 0;
}

int Table::columnCount()
{
    if(this->activeDatabase().tables().length())
    {
        if(this->query_mode)
            return this->activeQueryModel()->columnCount();
        return this->activeTableModel()->columnCount();
    }
    return 0;
}

void Table::toggleIdColumn(bool toggled)
{
    this->id_toggled = toggled;
    if(this->id_toggled)
        this->hideColumn(0);
    else
        this->showColumn(0);
}

QStringList Table::tableNames()
{
    return this->activeDatabase().tables();
}

QStringList Table::columnNames()
{
    QStringList column_names = {};
    for(int column = 0; column < this->columnCount(); column++)
    {
        if(this->query_mode)
            column_names.append(activeQueryModel()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString());
        else
            column_names.append(activeTableModel()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString());
    }
    return column_names;
}

QStringList Table::columnNames(QString table_name)
{
    QSqlQuery query;
    QSqlRecord record;
    QStringList column_names = {};
    if(query.exec(QString("SELECT * FROM %1").arg(table_name)))
    {
        record = query.record();
        for(int column = 0; column < record.count(); column++)
            column_names.append(record.fieldName(column));
    }
    return column_names;
}

QStringList Table::allColumnNames()
{
    QStringList all_column_names = {};
    QStringList tables = this->tableNames();
    int table_count = tables.length();

    for(int i = 0; i < table_count; i++)
        all_column_names.append(this->columnNames(tables[i]));

    return all_column_names;
}

void Table::valueFilter(int filter_type, QStringList filter_values, QString table_name, bool filter_empty_cells, bool case_sensitive, bool date_filter)
{
    QStringList range_values;
    QString value_format;
    if(filter_values[1].length() > 0 || filter_empty_cells)
    {
        switch(filter_type)
        {
        case SQLProperties::EQUAL:
            if (date_filter)
                this->activeTableModel()->setFilter(QString("date('%2') = date(%1)").arg(filter_values[0], filter_values[1]));
            else if(case_sensitive)
                this->activeTableModel()->setFilter(QString("'%2' = %1").arg(filter_values[0], filter_values[1]));
            else
                this->activeTableModel()->setFilter(QString("'%2' = lower(%1)").arg(filter_values[0], filter_values[1].toLower()));
            break;
        case SQLProperties::NOT_EQUAL:
            if (date_filter)
                this->activeTableModel()->setFilter(QString("date('%2') != date(%1)").arg(filter_values[0], filter_values[1]));
            else if(case_sensitive)
                this->activeTableModel()->setFilter(QString("'%2' != %1").arg(filter_values[0], filter_values[1]));
            else
                this->activeTableModel()->setFilter(QString("'%2' != lower(%1)").arg(filter_values[0], filter_values[1].toLower()));
            break;
        case SQLProperties::GREATER_THAN:
            if (date_filter)
                this->activeTableModel()->setFilter(QString("date('%2') < date(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->activeTableModel()->setFilter(QString("%2 < %1").arg(filter_values[0], filter_values[1]));
            break;
        case SQLProperties::LESS_THAN:
            if (date_filter)
                this->activeTableModel()->setFilter(QString("date('%2') > date(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->activeTableModel()->setFilter(QString("%2 > %1").arg(filter_values[0], filter_values[1]));
            break;
        case SQLProperties::GREATER_OR_EQUAL_THAN:
            if (date_filter)
                this->activeTableModel()->setFilter(QString("date('%2') <= date(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->activeTableModel()->setFilter(QString("%2 <= %1").arg(filter_values[0], filter_values[1]));
            break;
        case SQLProperties::LESS_OR_EQUAL_THAN:
            if (date_filter)
                this->activeTableModel()->setFilter(QString("date('%2') >= date(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->activeTableModel()->setFilter(QString("%2 >= %1").arg(filter_values[0], filter_values[1]));
            break;
        case SQLProperties::BETWEEN:
            if(filter_values[1].length() >= 3 && filter_values[1].contains(','))
            {
                value_format = filter_values[1].replace(" ", "");
                value_format = value_format.replace("[", "");
                value_format = value_format.replace("]", "");
                range_values = value_format.split(',');

                if(range_values.length() == 2)
                {
                    if(date_filter)
                        this->activeTableModel()->setFilter(QString("date(%3) BETWEEN date('%1') AND date('%2')").arg(range_values[0], range_values[1], filter_values[0]));
                    else
                        this->activeTableModel()->setFilter(QString("%3 BETWEEN %1 AND %2").arg(range_values[0], range_values[1], filter_values[0]));
                }
            }
        }
        emit Rows_And_Columns_Changed();
    }
    else
        this->activeTableModel()->setTable(activeTableModel()->tableName());
    this->activeTableModel()->select();
}

QString Table::SQLType(int SQL_type_id)
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

void Table::createTable(QString table_name, QString table_id, QStringList column_names, QList<int> data_types)
{
    QSqlQuery query;
    QString table_attributes;
    if(column_names.length() == data_types.length())
    {
        for(int i = 0; i < column_names.length(); i++)
            table_attributes.append(", " + column_names.at(i) + " " + this->SQLType(data_types.at(i)));
        query.exec(QString("CREATE TABLE %1 (%3 INTEGER PRIMARY KEY AUTOINCREMENT %2)").arg(table_name, table_attributes, table_id));
    }
}

void Table::createTable(QString table_name, QString table_id, QStringList column_names, QList<int> data_types, QMap <QString, ColumnData> column_properties)
{
    QSqlQuery query;
    QString query_string;
    QString table_attributes;
    QStringList f_keys;
    QStringList keys = column_properties.keys();
    QString ref_id;
    QString ref_table;
    QString f_id;

    for(int i = 0; i < keys.length(); i++)
    {
        f_id = keys[i];
        ref_table = column_properties[f_id].reference_table;
        ref_id = column_properties[f_id].reference_id;
        f_keys.append(QString("FOREIGN KEY(%3) REFERENCES %1(%2)").arg(ref_table, ref_id, f_id));
    }
    if(column_names.length() == data_types.length())
    {
        for(int i = 0; i < column_names.length(); i++)
            table_attributes.append(", " + column_names.at(i) + " " + this->SQLType(data_types.at(i)));
        query_string = QString("CREATE TABLE %1 (%3 INTEGER PRIMARY KEY AUTOINCREMENT %2, %4)").arg(table_name, table_attributes,
                                                                                     table_id, f_keys.join(",\n"));
        query.exec(query_string);
    }
}

void Table::removeTable(QString table_name)
{
    QSqlQuery query;
    query.exec(QString("DROP TABLE %1").arg(table_name));
}

void Table::insertValues(QString table_name, QStringList column_names, QStringList values)
{
    /*
    int data_type = 0;
    QSqlQuery query;
    QString table_column_values;
    QString query_string;
    QString table_column_names = column_names.join(", ");
    QString sql_query_string;


    sql_query_string = QString("INSERT INTO employee (%1) VALUES (%2)").arg(
                    columnNames().join(','));
    query.prepare(sql_query_string);
    for(int i = 0; i < values.length(); i++)
    {
            if(i == values.length() - 1)
                table_column_values.append(values.at(i));
            else
                table_column_values.append(values.at(i)+", ");
        }
        else
        {
            if(i == values.length() - 1)
                table_column_values.append("'"+values.at(i)+"'");
            else
                table_column_values.append("'"+values.at(i)+"', ");
        }

    }

    query_string = QString("INSERT INTO %1(%2) VALUES(%3)").arg(table_name, table_column_names, table_column_values);

    query.exec(query_string);
    */
}

void Table::insertRow(QString table_name, QStringList column_names)
{
    QSqlQuery query;
    QStringList values;
    int column_count = column_names.length();
    int column_start = 1;
    QString table_column_names = column_names.join(", ");
    QString table_column_values;

    if(column_names[0].toLower() == "id")
    {
        values.append(QString("%1").arg(this->rowCount() + 1));

        for(int i = column_start; i<column_count; i++)
            values.append("''");
        table_column_values = values.join(", ");

        query.exec(QString("INSERT INTO %1(%2) VALUES(%3)").arg(table_name, table_column_names, table_column_values));
    }
    else
    {
        query.exec(QString("INSERT INTO %1 DEFAULT VALUES").arg(table_name));
    }
    this->activeTableModel()->select();
}

void Table::removeRow(QString table_name, QString column_name, int id)
{
    QSqlQuery query;
    query.exec(QString("DELETE FROM %1 WHERE %2 = %3;").arg(table_name, column_name).arg(id));
}

int Table::getSqlType(int index)
{
    switch(index)
    {
    case SQL::SQL_NULL:
        return SQL_NULL;
    case SQL::SQL_INTEGER:
        return SQL_INTEGER;
    case SQL::SQL_TEXT:
        return SQL_TEXT;
    case SQL::SQL_REAL:
        return SQL_REAL;
    case SQL::SQL_BLOB:
        return SQL_BLOB;
    case SQL::SQL_CLOB:
        return SQL_CLOB;
    default:
        return SQL_NULL;
    }
}

bool Table::singleRowSelected(QModelIndexList index_list)
{
    if(index_list.length() < 1)
        return false;

    int current_row = index_list[0].row();
    for(int i = 0; i < index_list.length(); i++)
        if(current_row != index_list[i].row())
            return false;
    return true;
}

bool Table::singleColumnSelected(QModelIndexList index_list)
{
    if(index_list.length() < 1)
        return false;

    int current_column = index_list[0].column();
    for(int i = 0; i < index_list.length(); i++)
        if(current_column != index_list[i].column())
            return false;
    return true;
}

void Table::removeSelectedRow()
{
    QItemSelectionModel *select = this->selectionModel();
    QModelIndex index;
    int id = 0;
    int row = 0;
    QString column_name = columnNames().at(0);
    QString table_name = this->t_model->tableName();

    if(select->hasSelection())
    {
        index = select->selectedIndexes()[0];
        row = index.row();
        id = this->t_model->record(row).value(column_name).toInt();
        if(this->messageWarning(QString("Remove Row %1?").arg(row + 1), QString("Are you sure you want to remove row %1?").arg(row + 1)))
        {
            this->removeRow(table_name, column_name, id);
            this->t_model->select();
        }
    }
}

void Table::copyCells()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    QItemSelectionModel *select = this->selectionModel();
    QModelIndexList indexes = select->selectedIndexes();
    int index_count = indexes.length();
    QString text;
    QStringList text_list = {};
    QStringList column_names = this->columnNames();
    QString column_name;
    int row = indexes[0].row();
    int column = indexes[0].column();

    for(int i = 0; i < index_count; i++)
    {
        if(row != indexes[i].row())
        {
            text.append(text_list.join(',')+'\n');
            text_list.clear();
        }
        row = indexes[i].row();
        column = indexes[i].column();
        column_name = column_names[column];
        if(this->query_mode)
            text_list.append(this->activeQueryModel()->record(row).value(column_name).toString());
        else
            text_list.append(this->activeTableModel()->record(row).value(column_name).toString());
    }
    text.append(text_list.join(','));
    clipboard->setText(text);
}

void Table::contextMenuEvent(QContextMenuEvent *event)
{
    QItemSelectionModel *select = this->selectionModel();
    QAction *remove_row = nullptr;
    QAction *copy_cells = nullptr;
    QAction *format_date = nullptr;

    QAction *format_lower = nullptr;
    QAction *format_upper = nullptr;
    QAction *format_capital_1 = nullptr;
    QAction *format_capital_2 = nullptr;

    QMenu *menu = new QMenu(this);
    QMenu *format_column_menu;

    if(this->tableNames().length())
    {
        copy_cells = new QAction("Copy", this);
        copy_cells->setShortcut(QKeySequence::Copy);
        connect(copy_cells, &QAction::triggered, this, &Table::copyCells);
        menu->addAction(copy_cells);
        if(!this->isReadOnly() && !this->query_mode)
        {
            if(this->singleColumnSelected(select->selectedIndexes()))
            {
                menu->addSeparator();
                format_column_menu = new QMenu("Format Column", this);
                format_column_menu->setIcon(QIcon(Icons::CONVERT_ICON));

                format_lower = new QAction(QIcon(Icons::CASE_ICON), "To lowercase", this);
                connect(format_lower, &QAction::triggered, this, &Table::formatToLower);
                format_upper = new QAction(QIcon(Icons::CASE_ICON), "To uppercase", this);
                connect(format_upper, &QAction::triggered, this, &Table::formatToUpper);
                format_capital_1 = new QAction(QIcon(Icons::CASE_ICON), "To capitalized 1", this);
                connect(format_capital_1, &QAction::triggered, this, &Table::formatCapital_1);
                format_capital_2 = new QAction(QIcon(Icons::CASE_ICON), "To capitalized 2", this);
                connect(format_capital_2, &QAction::triggered, this, &Table::formatCapital_2);
                format_date = new QAction(QIcon(Icons::DATE_ICON), "To date values", this);
                connect(format_date, &QAction::triggered, this, &Table::formatDateColumn);

                menu->addMenu(format_column_menu);

                format_column_menu->addAction(format_lower);
                format_column_menu->addAction(format_upper);
                format_column_menu->addAction(format_capital_1);
                format_column_menu->addAction(format_capital_2);
                format_column_menu->addSeparator();
                format_column_menu->addAction(format_date);
            }
            if(this->singleRowSelected(select->selectedIndexes()))
            {
                menu->addSeparator();
                remove_row = new QAction(QIcon(Icons::RMV_ROW_ICON), "Remove Row", this);
                connect(remove_row, &QAction::triggered, this, &Table::removeSelectedRow);
                menu->addAction(remove_row);
            }
        }
        menu->popup(event->globalPos());
    }
}

void Table::formatDateColumn()
{
    /* Will attempt to convert an entire table column of date strings
       into the datetime format.*/
    QModelIndex index;
    int column;
    bool ok;
    QString date_format;
    QString date;
    QString f_date;
    QString column_name;
    QString id_name;
    QString row_id;
    QSqlQuery query;
    QSqlRecord record;
    QItemSelectionModel *select = this->selectionModel();
    QString table_name = this->activeTableModel()->tableName();

    if(select->hasSelection())
    {
        index = select->currentIndex();
        column = index.column();
        id_name = this->columnNames()[0];
        column_name = this->columnNames()[column];
        date = this->activeTableModel()->record(0).value(column).toString();
        date_format = QInputDialog::getItem(this,
                                            QString("Select Date Format"),
                                            QString("Select date type for \"%1\":").arg(date),
                                            DateFormats::DATE_FORMAT_LIST, 0, false, &ok);
        if(ok)
        {
            QSqlQuery("PRAGMA journal_mode = OFF");
            QSqlQuery("PRAGMA synchronous = OFF");
            query.prepare(QString("UPDATE %1 SET %2 = date(:fdate) WHERE %3 = :id").arg(table_name, column_name, id_name));
            for(int row = 0; row < this->rowCount(); row++)
            {
                record = this->activeTableModel()->record(row);
                row_id = record.value(0).toString();
                date = record.value(column).toString();
                f_date = DataFormat::toDatetime(date_format, date);
                query.bindValue(":fdate", f_date);
                query.bindValue(":id", row_id);
                query.exec();
            }
            QSqlQuery("PRAGMA journal_mode = delete");
            QSqlQuery("PRAGMA synchronous = 2");
            this->activeTableModel()->select();
        }
    }
}

void Table::formatCapital_1()
{
    this->formatCapitalizeColumn(' ');
}

void Table::formatCapital_2()
{
    this->formatCapitalizeColumn('.');
}

void Table::formatCapitalizeColumn(char delimiter)
{
    QModelIndex index;
    QString column_name;
    QString capitalized_text;
    QString table_name = this->activeTableModel()->tableName();
    QItemSelectionModel *select = this->selectionModel();
    QString id_name;
    QString row_id;
    QSqlQuery query;
    QSqlRecord record;
    std::string text;
    int column;
    bool delimiter_found;

    if(select->hasSelection())
    {
        index = select->currentIndex();
        column = index.column();
        id_name = this->columnNames()[0];
        column_name = this->columnNames()[column];
        text = this->activeTableModel()->record(0).value(column).toString().toLower().toStdString();

        QSqlQuery("PRAGMA journal_mode = OFF");
        QSqlQuery("PRAGMA synchronous = OFF");
        query.prepare(QString("UPDATE %1 SET %2 = :ctext WHERE %3 = :id").arg(table_name, column_name, id_name));
        for(int row = 0; row < this->rowCount(); row++)
        {
            record = this->activeTableModel()->record(row);
            row_id = record.value(0).toString();
            text = record.value(column).toString().toLower().toStdString();
            delimiter_found = true;
            for(int j = 0; j < static_cast<int>(text.length()); j++)
            {
                if(JFile::isAlpha(text[j]) && delimiter_found)
                {
                    if(text[j] != ' ')
                    {
                        text[j] = toupper(text[j]);
                        delimiter_found = false;
                    }
                }
                if(!delimiter_found)
                    delimiter_found = (text[j] == delimiter);
            }
            capitalized_text = QString(text.c_str());

            query.bindValue(":ctext", capitalized_text);
            query.bindValue(":id", row_id);
            query.exec();
        }
        QSqlQuery("PRAGMA journal_mode = delete");
        QSqlQuery("PRAGMA synchronous = 2");
        this->activeTableModel()->select();
    }
}

void Table::setCaseForFormat(bool lower)
{
    QModelIndex index;
    QString column_name;
    QString capitalized_text;
    QString table_name = this->activeTableModel()->tableName();
    QItemSelectionModel *select = this->selectionModel();
    QString id_name;
    QString row_id;
    QSqlQuery query;
    QSqlRecord record;
    QString text;
    int column;

    if(select->hasSelection())
    {
        index = select->currentIndex();
        column = index.column();
        id_name = this->columnNames()[0];
        column_name = this->columnNames()[column];
        if(lower)
            text = this->activeTableModel()->record(0).value(column).toString().toLower();
        else
            text = this->activeTableModel()->record(0).value(column).toString().toUpper();

        QSqlQuery("PRAGMA journal_mode = OFF");
        QSqlQuery("PRAGMA synchronous = OFF");
        query.prepare(QString("UPDATE %1 SET %2 = :ctext WHERE %3 = :id").arg(table_name, column_name, id_name));
        for(int row = 0; row < this->rowCount(); row++)
        {
            record = this->activeTableModel()->record(row);
            row_id = record.value(0).toString();
            if(lower)
                text = record.value(column).toString().toLower();
            else
                text = record.value(column).toString().toUpper();
            query.bindValue(":ctext", text);
            query.bindValue(":id", row_id);
            query.exec();
        }
        QSqlQuery("PRAGMA journal_mode = delete");
        QSqlQuery("PRAGMA synchronous = 2");
        this->activeTableModel()->select();
    }
}

void Table::formatToUpper()
{
    this->setCaseForFormat(false);
}

void Table::formatToLower()
{
    this->setCaseForFormat(true);
}

void Table::message(QString title, QString message)
{
    QMessageBox *msg;
    msg = new QMessageBox(this);

    msg->setWindowTitle(title);
    msg->setText(message);
    msg->setIcon(QMessageBox::Warning);
    msg->exec();
    msg->deleteLater();
}

bool Table::messageWarning(QString title, QString message)
{
    QMessageBox *msg;
    bool accepted = false;
    msg = new QMessageBox(this);

    msg->setWindowTitle(title);
    msg->setText(message);
    msg->setIcon(QMessageBox::Warning);
    msg->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    accepted = (msg->exec() == QMessageBox::Yes);
    msg->deleteLater();
    return accepted;
}

void Table::setToReadOnly(bool read_only)
{
    this->_read_only = read_only;
    if(this->_read_only)
        this->setEditTriggers(QTableView::NoEditTriggers);
    else
        this->setEditTriggers(QTableView::DoubleClicked);
}

bool Table::isReadOnly()
{
    return this->_read_only;
}

void Table::exportCSVFile()
{
    QMessageBox *msg = new QMessageBox(this);
    QString path = QFileDialog::getSaveFileName(this, "Export csv File", "", "csv(*.csv)");
    QStringList data;
    QString value = "";
    QStringList column_names = this->columnNames();
    int columns = column_names.length();
    int rows = 0;
    JFile writer;

    // Currently a query table is on display.
    if(this->queryMode())
        rows = this->rowCount();
    // Otherwise, proceed as normal.
    else
        rows = this->rowCount();

    if(path.length())
    {
        writer.open(path, 'w');
        writer.writeCsvRow(column_names);
        for(int row = 0; row < rows; row++)
        {
            for(int column = 0; column < columns; column++)
            {
                if(this->query_mode)
                    value = this->activeQueryModel()->record(row).value(column_names[column]).toString();
                else
                    value = this->activeTableModel()->record(row).value(column_names[column]).toString();
                data.append(value);
            }
            writer.writeCsvRow(data);
            data.clear();
        }
        writer.close();
    msg->setWindowTitle("Export Complete");
    msg->setText(QString("File \"%1\" exported sucessfully!").arg(path));
    msg->setIcon(QMessageBox::Information);
    msg->setStandardButtons(QMessageBox::Ok);
    msg->exec();
    msg->deleteLater();
    }
}

void Table::runSQL(QString sql_command)
{
    QString table_name = this->activeTableModel()->tableName();
    QSqlQuery query;
    if(this->containsSQLWriteCommand(sql_command))
    {
        if(this->_read_only)
        {
            this->message("Read Only Mode", "Table is currently set to \"Read Only\".\n"
                                            "Cannot commit SQL changes.");
            return;
        }
        query.exec(sql_command);

        emit New_Database_Set();
        if(this->tableNames().contains(table_name))
            this->activeTableModel()->setTable(table_name);
        else
            this->activeTableModel()->setTable(this->activeDatabase().tables()[0]);
        this->activeTableModel()->select();

    }
    else
    {
        query.exec(sql_command);

        this->_query_rows = 0;
        while(query.next())
            this->_query_rows++;
        this->activeQueryModel()->setQuery(query);
        this->setModel(this->activeQueryModel());

        connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &Table::onCellChange);
        this->query_mode = true;

        emit Query_Was_Executed(false);
    }
}

QString Table::sqliteTypeFromVariant(QVariant data_type)
{
    if(data_type == QVariant::Int)
        return SQL::TYPE_INTEGER;
    return "~"+SQL::TYPE_TEXT;
}

void Table::onCellChange(QModelIndex current, QModelIndex previous)
{
    QString cell_value;
    int row = current.row();
    int column = current.column();
    QString column_name = this->columnNames()[column];
    QSqlQuery query;
    QString  sqlite_data_type;
    QVariant qt_data_type;
    QString data_type;
    QString table_name;
    QString date_query_string;
    bool is_date = false;

    if(this->query_mode)
    {
        cell_value = this->activeQueryModel()->record(row).value(column_name).toString();
        qt_data_type = this->activeQueryModel()->record(row).field(column).type();
    }
    else
    {
        table_name = this->activeTableModel()->tableName();
        cell_value = this->activeTableModel()->record(row).value(column_name).toString();

        date_query_string = QString("SELECT strftime('%m/%d/%Y', %2) FROM %1 WHERE %2 = '%3' AND (%2 IS date(%2))").arg(table_name, column_name, cell_value);
        query = this->activeDatabase().exec(date_query_string);
        if(query.first())
        {
            cell_value = query.value(0).toString();
            is_date = true;
        }
        qt_data_type = this->activeTableModel()->record(row).field(column).type();
    }

    if(this->query_mode)
    {
        data_type = this->sqliteTypeFromVariant(qt_data_type);
    }
    else
    {
        query = this->activeDatabase().exec(QString("PRAGMA table_info(%1)").arg(table_name));
        if(query.seek(column))
            data_type = query.value(2).toString();
        else
            data_type = this->sqliteTypeFromVariant(qt_data_type);
        if(is_date)
            data_type = QString("%1 (Date)").arg(data_type);
    }

    emit cellChanged(column);
    emit Data_Preview(cell_value, data_type, column_name, row, column);
}

bool Table::queryMode()
{
    return this->query_mode;
}

bool Table::containsSQLWriteCommand(QString text)
{
    QString lower_text = text.toLower();
    int command_count = SQLProperties::WRITE_COMMANDS.length();
    for(int i = 0; i < command_count; i++)
        if(lower_text.contains(SQLProperties::WRITE_COMMANDS[i].toLower()))
            return true;
    return false;
}

QString Table::databasePath()
{
    return this->_database_path;
}

void Table::detectAndSetDatabase(QString table_name)
{
    bool table_exists = this->tableNames().contains(table_name);
    if(this->databaseType() != SQLITE && !table_exists)
        this->setDatabase(this->sqlite_database_path);
    else if(this->databaseType() != ACCESS && !table_exists)
        this->importAccessDatabaseOnTheFly(this->access_database_path);
}
