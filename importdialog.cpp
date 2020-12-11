#include "importdialog.h"

ImportDialog::ImportDialog(Table* table, QWidget *parent) : QDialog(parent)
{
    this->setMinimumWidth(300);
    this->current_row = 0;
    this->main_layout = new QVBoxLayout(this);

    QWidget *bottom_row = new QWidget(this);
    QHBoxLayout *bottom_layout = new QHBoxLayout(bottom_row);
    bottom_layout->setContentsMargins(0,0,0,0);
    QWidget *filler = new QWidget(this);
    filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->insert_button = new QPushButton("Insert Row(s)");
    this->cancel_button = new QPushButton("Cancel", this);

    //this->main_layout->setContentsMargins(0,0,0,0);
    this->setWindowTitle("Import csv File to Table");
    this->_table = table;
    this->csv_headers = QStringList();
    this->setupCsvBrowser();
    this->setupDatabaseTable();
    this->setupImportTable();
    this->setupCsvThread();

    bottom_layout->addWidget(filler);
    bottom_layout->addWidget(this->insert_button);
    bottom_layout->addWidget(this->cancel_button);
    this->main_layout->addWidget(bottom_row);

    connect(this->insert_button, &QPushButton::clicked, this, &ImportDialog::insertDataIntoTable);
    connect(this->cancel_button, &QPushButton::clicked, this, &ImportDialog::close);

}

void ImportDialog::setupCsvBrowser()
{
    this->csv_browser_area = new QGroupBox("Import csv File", this);
    this->csv_file_browser_entry = new QLineEdit(this);
    this->csv_file_browser_entry->setReadOnly(true);
    this->csv_file_browser_button = new QPushButton("Browse", this);
    connect(this->csv_file_browser_button, &QPushButton::clicked, this, &ImportDialog::importCsv);
    this->has_headers_chkbox = new QCheckBox("csv file has column names", this);
    this->has_headers_chkbox->setChecked(true);
    QGridLayout *csv_browser_layout = new QGridLayout(this->csv_browser_area);
    //#----------------------------------------------------------------------#
    csv_browser_layout->addWidget(this->csv_file_browser_entry,0,0);
    csv_browser_layout->addWidget(this->csv_file_browser_button,0,1);
    csv_browser_layout->addWidget(this->has_headers_chkbox,1,0);
    //#----------------------------------------------------------------------#
    this->main_layout->addWidget(this->csv_browser_area);
}

void ImportDialog::setupDatabaseTable()
{
    QStringList tables = this->_table->tableNames();
    int table_count = tables.length();
    this->database_table_area = new QGroupBox("Database Table", this);
    this->table_combo = new QComboBox(this);
    this->database_table = new QTableWidget(this);
    this->database_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    if(table_count)
    {
        this->table_combo->addItems(tables);
        this->setDatabaseTable(this->table_combo->currentText());
    }
    QVBoxLayout *database_table_layout = new QVBoxLayout(this->database_table_area);
    //#----------------------------------------------------------------------#
    database_table_layout->addWidget(new QLabel("Select the table where the import data will be written.", this));
    database_table_layout->addWidget(this->table_combo);
    database_table_layout->addWidget(new QLabel("Match imported data columns to the table columns.", this));
    database_table_layout->addWidget(this->database_table);
    //#----------------------------------------------------------------------#
    connect(this->table_combo, &QComboBox::currentTextChanged, this, &ImportDialog::setDatabaseTable);
    this->main_layout->addWidget(database_table_area);
}

void ImportDialog::setupImportTable()
{
    this->import_table_area = new QGroupBox("Imported csv Data", this);
    this->import_table = new QTableView(this);
    this->import_table->setEditTriggers(QTableView::NoEditTriggers);
    this->start_index_spinbox = new QSpinBox(this);
    this->end_index_spinbox = new QSpinBox(this);
    this->start_index_spinbox->setMinimum(1);
    this->end_index_spinbox->setMinimum(2);
    connect(this->start_index_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImportDialog::changeEndMin);
    connect(this->end_index_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImportDialog::changeStartMax);
    QWidget *filler = new QWidget(this);
    QWidget *range_row = new QWidget(this);
    filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QVBoxLayout *import_table_layout = new QVBoxLayout(this->import_table_area);
    QHBoxLayout *range_layout = new QHBoxLayout(range_row);
    range_layout->setContentsMargins(0,0,0,0);
    //#----------------------------------------------------------------------#
    import_table_layout->addWidget(this->import_table);
    range_layout->addWidget(filler);
    range_layout->addWidget(new QLabel("Insert rows (inclusively) from"));
    range_layout->addWidget(this->start_index_spinbox);
    range_layout->addWidget(new QLabel(" to ", this));
    range_layout->addWidget(this->end_index_spinbox);
    import_table_layout->addWidget(range_row);
    //#----------------------------------------------------------------------#
    this->main_layout->addWidget(this->import_table_area);
}

void ImportDialog::setDatabaseTable(QString table_name)
{
    this->database_table->clear();
    QStringList column_names = this->_table->columnNames(table_name);
    int column_count = column_names.length();
    if(column_count)
    {
        this->database_table->setRowCount(1);
        this->database_table->setColumnCount(column_count);
        this->database_table->setHorizontalHeaderLabels(column_names);
        for(int column = 0; column < column_count; column++)
        {
            this->database_table->setCellWidget(0, column, new QComboBox(this));
            if(!this->csv_headers.isEmpty())
                static_cast<QComboBox*>(this->database_table->cellWidget(0, column))->addItems(this->csv_headers);
        }
        this->database_table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        this->database_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
}

void ImportDialog::importCsv()
{
    QStringList column_names = {};
    QList<QStringList> data;
    JFile reader;
    QString path = QFileDialog::getOpenFileName(this, "Open csv File", "", "csv(*.csv)");

    if(path.length())
    {
        this->csv_file_browser_entry->setText(path);
        emit csvPathRead(path, this->has_headers_chkbox->isChecked());
    }
}

void ImportDialog::setImportTable(QStringList column_names)
{
    //this->import_table->clear();
    this->csv_headers = column_names;
    // Set column names for the combos in the database table.
    if(!this->csv_headers.isEmpty())
    {
        this->csv_headers.insert(0, ColumnNames::EMPTY);
        for(int column = 0; column < this->database_table->columnCount(); column++)
        {
            static_cast<QComboBox*>(this->database_table->cellWidget(0, column))->clear();
            static_cast<QComboBox*>(this->database_table->cellWidget(0, column))->addItems(this->csv_headers);
        }
    }
}

/*
void ImportDialog::writeToImportTable(int csv_row)
{

}
*/

void ImportDialog::setupCsvThread()
{
    CsvWorker *csv_worker = new CsvWorker();
    QThread *csv_thread = new QThread(this);
    connect(this, &ImportDialog::dialogClosed, csv_thread, &QThread::quit);

    // ImportDialog signals to CsvWorker
    connect(this, &ImportDialog::csvPathRead, csv_worker, &CsvWorker::readCsv);
    connect(this, &ImportDialog::insertionRequested, csv_worker, &CsvWorker::insertDataIntoTable);

    // CsvWorker signals to ImportDialog
    connect(csv_worker, &CsvWorker::csvRead, this, &ImportDialog::setImportTable);
    //connect(csv_worker, &CsvWorker::csvRowRead, this, &ImportDialog::writeToImportTable);
    connect(csv_worker, &CsvWorker::tableModelCompleted, this, &ImportDialog::setImportTableModel);
    connect(csv_worker, &CsvWorker::finishedInserting, this->_table, &Table::setDatabase);
    connect(csv_worker, &CsvWorker::readyToClose, this, &ImportDialog::close);

    csv_worker->moveToThread(csv_thread);

    csv_thread->start();
}

void ImportDialog::setImportTableModel(QStandardItemModel *model)
{
    int rows = 0;
    this->import_table->setModel(model);
    rows = this->import_table->model()->rowCount();
    this->end_index_spinbox->setMaximum(rows);
    this->end_index_spinbox->setValue(rows);
    this->start_index_spinbox->setMaximum(rows-1);
}

void ImportDialog::changeStartMax(int value)
{
    this->start_index_spinbox->setMaximum(value - 1);
}

void ImportDialog::changeEndMin(int value)
{
    this->end_index_spinbox->setMinimum(value + 1);
}

void ImportDialog::insertDataIntoTable()
{
    qRegisterMetaType<IntList>("IntList");
    IntList data_placement;
    for(int i = 0; i < this->database_table->columnCount(); i++)
        data_placement.append(static_cast<QComboBox*>(this->database_table->cellWidget(0, i))->currentIndex());

    QString db_path = this->_table->databasePath();
    QString table_name = this->table_combo->currentText();
    QStringList column_names = this->_table->columnNames(table_name);
    int row_start = this->start_index_spinbox->value();
    int row_end = this->end_index_spinbox->value();
    this->_table->closeDatabase();
    emit insertionRequested(db_path,
                            table_name,
                            column_names,
                            data_placement,
                            row_start,
                            row_end);
}

void ImportDialog::closeEvent(QCloseEvent* event)
{
    emit dialogClosed();
    event->isAccepted();
}
