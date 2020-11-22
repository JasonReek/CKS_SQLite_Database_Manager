#include "table.h"

Table::Table(QWidget *parent) : QTableView(parent)
{
    Q_INIT_RESOURCE(Resources);


    this->query_mode = false;
    this->id_toggled = false;

    this->_selected_row = 0;
    this->_selected_column = 0;

    //QString path = QCoreApplication::applicationDirPath().append("/Database/LeakDetectionDatabase.db");

    this->database = QSqlDatabase::addDatabase("QSQLITE");
    //this->database.setDatabaseName(path);
    this->q_model = new QSqlQueryModel(this);
}

void Table::createNewDatabase()
{
    bool ok;
    QString database_name = QInputDialog::getText(this, tr("New Database"),
                                         tr("Database Name:"), QLineEdit::Normal,
                                         "", &ok);
    QString path = QCoreApplication::applicationDirPath().append(Database::DATABASE_DIRECTORY+database_name+".db");

    if(ok)
    {
        QFileInfo check_file(path);
        // If the database doesn't exist, create it!
        if (!check_file.exists() && !check_file.isFile())
        {
            this->setDatabase(path);
        }
        // Otherwise, rerun the create new database prompt.
        else
        {
            this->message("Database Already Exists", "Database name is already in use.");
            this->createNewDatabase();
        }
    }
}

void Table::openDatabase()
{
    QString path = QFileDialog::getOpenFileName(this, "Open Database", "", tr("Database files(*.db *.sqlite *.db3 *.sqlite3);;All files(*)"));
    if(path.length())
        this->setDatabase(path);
}

void Table::setDatabase(QString path)
{
    this->database.setDatabaseName(path);
    if(this->database.open())
    {
        this->model = new QSqlTableModel(this, this->database);
        this->model->setEditStrategy(QSqlTableModel::OnFieldChange);
        if(this->database.tables().length())
        {
            this->model->setTable(this->database.tables()[0]);
            this->model->select();
        }
        QSqlQuery query("PRAGMA foreign_keys = ON;", this->database);
        this->setModel(this->model);
        connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &Table::onCellChange);
        emit New_Database_Set();
    }
}

void Table::setTable(QString table_name)
{
    if(this->query_mode)
    {
        this->setModel(this->model);
        connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &Table::onCellChange);
        this->query_mode = false;
    }
    this->model->setTable(table_name);
    this->model->select();
    //this->toggleIdColumn(this->id_toggled);
    this->model->setHeaderData(0, Qt::Horizontal, QIcon(Icons::KEY_ID_ICON), Qt::DecorationRole);
    this->model->setHeaderData(0, Qt::Horizontal, Qt::AlignJustify, Qt::TextAlignmentRole);
    emit Column_Names_Set(this->columnNames());
}

void Table::setTableFromTree(QTreeWidgetItem *item, int column)
{
    QString table_name = item->text(column);
    QStringList table_names = this->tableNames();
    if(table_names.contains(table_name) && item->childCount() > 0)
    {
        emit Table_Name_Changed(table_name);
    }
    else if(item->parent()->childCount() > 0)
    {
        table_name = item->parent()->text(column);
        if(table_names.contains(table_name))
        {
            emit Table_Name_Changed(table_name);
            emit Column_Name_Index_Changed(item->parent()->indexOfChild(item));
        }
    }
}

int Table::rowCount()
{
    if(this->database.tables().length())
    {
        if(this->query_mode)
            return this->q_model->rowCount();
        return this->model->rowCount();
    }
    return 0;
}

int Table::columnCount()
{
    if(this->database.tables().length())
    {
        if(this->query_mode)
            return this->q_model->columnCount();
        return this->model->columnCount();
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
    return this->database.tables();
}

QStringList Table::columnNames()
{
    QStringList column_names = {};
    for(int column = 0; column < this->columnCount(); column++)
    {
        if(this->query_mode)
            column_names.append(this->q_model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString());
        else
            column_names.append(this->model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString());
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
                this->model->setFilter(QString("datetime('%2') = datetime(%1)").arg(filter_values[0], filter_values[1]));
            else if(case_sensitive)
                this->model->setFilter(QString("'%2' = %1").arg(filter_values[0], filter_values[1]));
            else
                this->model->setFilter(QString("'%2' = lower(%1)").arg(filter_values[0], filter_values[1].toLower()));
            break;
        case SQLProperties::GREATER_THAN:
            if (date_filter)
                this->model->setFilter(QString("datetime('%2') < datetime(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->model->setFilter(QString("%2 < %1").arg(filter_values[0], filter_values[1]));
            break;
        case SQLProperties::LESS_THAN:
            if (date_filter)
                this->model->setFilter(QString("datetime('%2') > datetime(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->model->setFilter(QString("%2 > %1").arg(filter_values[0], filter_values[1]));
            break;
        case SQLProperties::GREATER_OR_EQUAL_THAN:
            if (date_filter)
                this->model->setFilter(QString("datetime('%2') <= datetime(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->model->setFilter(QString("%2 <= %1").arg(filter_values[0], filter_values[1]));
            break;
        case SQLProperties::LESS_OR_EQUAL_THAN:
            if (date_filter)
                this->model->setFilter(QString("datetime('%2') >= datetime(%1)").arg(filter_values[0], filter_values[1]));
            else
                this->model->setFilter(QString("%2 >= %1").arg(filter_values[0], filter_values[1]));
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
                        this->model->setFilter(QString("datetime(%3) BETWEEN datetime('%1') AND datetime('%2')").arg(range_values[0], range_values[1], filter_values[0]));
                    else
                        this->model->setFilter(QString("%3 BETWEEN %1 AND %2").arg(range_values[0], range_values[1], filter_values[0]));
                }
            }
        }
        emit Rows_And_Columns_Changed();
    }
    else
        this->model->setTable(this->model->tableName());
    this->model->select();
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

void Table::insertValues(QString table_name, QStringList column_names, QStringList values, QList<int> data_types)
{
    int data_type = 0;
    QSqlQuery query;
    QString table_column_values;
    QString query_string;
    QString table_column_names = column_names.join(", ");

    for(int i = 0; i < values.length(); i++)
    {
        data_type = data_types.at(i);
        if(data_type == SQL_INTEGER || data_type == SQL_REAL || data_type == SQL_NULL)
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
    this->model->select();
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
    QString table_name = this->model->tableName();

    if(select->hasSelection())
    {
        index = select->selectedIndexes()[0];
        row = index.row();
        id = this->model->record(row).value(column_name).toInt();
        if(this->messageWarning(QString("Remove Row %1?").arg(row + 1), QString("Are you sure you want to remove row %1?").arg(row + 1)))
        {
            this->removeRow(table_name, column_name, id);
            this->model->select();
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
            text_list.append(this->q_model->record(row).value(column_name).toString());
        else
            text_list.append(this->model->record(row).value(column_name).toString());
    }
    text.append(text_list.join(','));
    clipboard->setText(text);
}

void Table::contextMenuEvent(QContextMenuEvent *event)
{
    QItemSelectionModel *select = this->selectionModel();
    QAction *remove_row;
    QAction *copy_cells;
    QAction *format_date;

    QMenu *menu = new QMenu(this);
    QMenu *format_column_menu;


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
            format_date = new QAction(QIcon(Icons::DATE_ICON), "To datetime values", this);
            connect(format_date, &QAction::triggered, this, &Table::formatDateColumn);
            menu->addMenu(format_column_menu);
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
    QString table_name = this->model->tableName();

    if(select->hasSelection())
    {
        index = select->currentIndex();
        column = index.column();
        id_name = this->columnNames()[0];
        column_name = this->columnNames()[column];
        date = this->model->record(0).value(column).toString();
        date_format = QInputDialog::getItem(this,
                                            QString("Select Date Format"),
                                            QString("Select date type for \"%1\":").arg(date),
                                            DateFormats::DATE_FORMAT_LIST, 0, false, &ok);
        if(ok)
        {
            QSqlQuery("PRAGMA journal_mode = OFF");
            QSqlQuery("PRAGMA synchronous = OFF");
            query.prepare(QString("UPDATE %1 SET %2 = datetime(:fdate) WHERE %3 = :id").arg(table_name, column_name, id_name));
            for(int row = 0; row < this->rowCount(); row++)
            {
                record = this->model->record(row);
                row_id = record.value(0).toString();
                date = record.value(column).toString();
                f_date = DataFormat::toDatetime(date_format, date);
                query.bindValue(":fdate", f_date);
                query.bindValue(":id", row_id);
                query.exec();
            }
            QSqlQuery("PRAGMA journal_mode = delete");
            QSqlQuery("PRAGMA synchronous = 2");
            this->model->select();
        }
    }
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
    QString path = QFileDialog::getSaveFileName(this, "Export CSV File", "", "csv(*.csv)");
    QStringList data;
    QString value = "";
    QStringList column_names = this->columnNames();
    int columns = column_names.length();
    int rows = 0;
    CSV writer;

    // Currently a query table is on display.
    if(this->queryMode())
        rows = this->q_model->rowCount();
    // Otherwise, proceed as normal.
    else
        rows = this->model->rowCount();

    if(path.length())
    {
        writer.open(path);
        writer.writeRow(column_names);
        for(int row = 0; row < rows; row++)
        {
            for(int column = 0; column < columns; column++)
            {
                if(this->query_mode)
                    value = this->q_model->record(row).value(column_names[column]).toString();
                else
                    value = this->model->record(row).value(column_names[column]).toString();
                data.append(value);
            }
            writer.writeRow(data);
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
    if(this->containsSQLWriteCommand(sql_command))
    {
        if(this->_read_only)
        {
            this->message("Read Only Mode", "Table is currently set to \"Read Only\".\n"
                                            "Cannot commit SQL changes.");
            return;
        }
        QSqlQuery query;
        query.exec(sql_command);
        this->model->setTable(this->database.tables()[0]);
        this->model->select();
        emit New_Database_Set();
    }
    else
    {
        this->q_model->setQuery(sql_command);
        this->setModel(this->q_model);
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

    if(this->query_mode)
    {
        cell_value = this->q_model->record(row).value(column_name).toString();
        qt_data_type = this->q_model->record(row).field(column).type();
    }
    else
    {
        cell_value = this->model->record(row).value(column_name).toString();
        qt_data_type = this->model->record(row).field(column).type();
        table_name = this->model->tableName();
    }

    if(this->query_mode)
        query = this->database.exec(QString("SELECT * FROM pragma_table_info('results')"));
    else
        query = this->database.exec(QString("PRAGMA table_info(%1)").arg(table_name));

    if(query.seek(column))
        data_type = query.value(2).toString();
    else
        data_type = this->sqliteTypeFromVariant(qt_data_type);

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
        if(lower_text.contains(SQLProperties::WRITE_COMMANDS[i]))
            return true;
    return false;
}
