#include "csvworker.h"

CsvWorker::CsvWorker(QObject *parent) : QObject(parent)
{
    _has_headers = true;
}

void CsvWorker::readCsv(QString path, bool has_headers)
{
    QStandardItemModel *import_data_model = new QStandardItemModel();
    int present_column_count = 0;
    int start_row = 0;
    int row_count = 0;
    int column_count = 0;
    QStringList column_names;
    JFile reader;
    this->_has_headers = has_headers;

    reader.open(path, 'r');
    this->data = reader.readCsv();
    reader.close();
    // Make sure the file isn't empty.
    if(reader.rowCount())
    {
        row_count = reader.rowCount();
        column_count = reader.maxColumns();

        //emit csvRead(data.at(0), row_count, column_count);
        // For some reason there are less header labels than data columns.
        // Set column count to label length for consistency.
        // The user can uncheck the has_headers_check_box to get the remaining
        // data into the import table.
        if(column_count > this->data.at(0).length() && has_headers)
            column_count = this->data.at(0).length();

        if(this->_has_headers)
        {
            column_names = this->data.at(0);
            start_row = 1;
        }
        else
        {
            for(int i = 1; i <= column_count; i++)
                column_names.append(QString("Column %1").arg(i));
        }

        import_data_model->setRowCount(row_count-1);
        import_data_model->setColumnCount(column_count);
        import_data_model->setHorizontalHeaderLabels(column_names);
        emit csvRead(column_names);

        for(int row = start_row; row < row_count; row++)
        {
            //emit csvRowRead(row);
            if(column_count > this->data.at(row).length())
                present_column_count = this->data.at(row).length();
            for(int column = 0; column < column_count; column++)
            {
                if(present_column_count)
                {
                    if(column < present_column_count)
                        import_data_model->setItem(row-start_row, column, new QStandardItem(this->data.at(row).at(column)));
                    else
                        import_data_model->setItem(row-start_row, column, new QStandardItem(""));
                }
                else
                    import_data_model->setItem(row-start_row, column, new QStandardItem(this->data.at(row).at(column)));
            }
        }
        emit tableModelCompleted(import_data_model);
    }
}

void CsvWorker::insertDataIntoTable(QString database_path, QString table_name, QStringList column_names, IntList data_placement, int row_start, int row_end)
{
    const int EMPTY_SELECTION = -1;
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    QStringList values;
    QSqlQuery query;
    IntList dp = data_placement;
    QStringList cn;
    int column_start = 0;
    QString prepare_string;

    if(database_path.length())
    {
        database.setDatabaseName(database_path);
        if(database.open())
        {
            // Make sure to offset row to skip column names if csv has them.
            if(!this->_has_headers || row_start > 1)
                row_start--;
            // Remove offset of data placement values.
            for(int i = 0; i < dp.length(); i++)
                dp[i]--;
            // No custom ID insertion.
            if(dp.length() > 1 && column_names.length() > 1 && dp.at(0) == EMPTY_SELECTION)
                column_start = 1;
            // Create column names and "?" for values to be inserted.
            for(int i = column_start; i < column_names.length(); i++)
            {
                cn.append(column_names.at(i));
                values.append("?");
            }
            prepare_string = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(table_name, cn.join(", "), values.join(", "));
            query.prepare(prepare_string);

            QSqlQuery("PRAGMA journal_mode = OFF");
            QSqlQuery("PRAGMA synchronous = OFF");
            //#----------------------------------------------------#
            for(int row = row_start; row < row_end+row_start; row++)
            {
                for(int column = column_start; column < dp.length(); column++)
                {
                    // The value selected for this column is available in the csv data.
                    // (*SOLVED) Also check that column doesn't exceed the data placement size.
                    if(dp.at(column) < this->data.at(row).length())
                    {
                        // An empty selection was made.
                        if(dp.at(column) == EMPTY_SELECTION)
                            query.addBindValue("");
                        // Insert csv data value into the table.
                        else
                            query.addBindValue(data.at(row).at(dp.at(column)));
                    }
                    // The value selected is not available, use empty value.
                    else
                        query.addBindValue("");
                }
                query.exec();
            }
            //#----------------------------------------------------#
            QSqlQuery("PRAGMA journal_mode = delete");
            QSqlQuery("PRAGMA synchronous = 2");

            database.close();

            emit finishedInserting(database_path);
            emit readyToClose();
        }
    }

}
