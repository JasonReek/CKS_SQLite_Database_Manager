#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    // Top Table Row Widgets
    this->table = new Table(this);

    this->setWindowTitle(QString("CKS SQLite Database Manager"));

    this->_id_column_locked = true;
    this->table_selection_icon_label = new IconLabel("Select Table: ", Icons::TABLE_ICON, this);
    this->table_selection_combo = new QComboBox(this);
    this->table_selection_combo->setMinimumWidth(150);
    this->column_icon_label = new IconLabel("Column Name: ", Icons::COLUMN_ICON, this);
    this->column_name_combo = new QComboBox(this);
    this->column_name_combo->setMinimumWidth(150);

    // Table Tree Widget(s)
    this->table_tree = new QTreeWidget(this);
    this->table_tree->setMinimumWidth(225);

    // Bottom Table Row Widgets
    //-------------------------------------------------------------

    // Export Table Button
    this->export_csv_button = new QPushButton("Export", this);
    this->export_csv_button->setIcon(QIcon(Icons::EXPORT_ICON));
    this->export_csv_button->setDisabled(true);

    // Insert Row Button
    this->insert_row_button = new QPushButton("Insert Row", this);
    this->insert_row_button->setIcon(QIcon(Icons::INSERT_ICON));

    // Quick Filter Widgets
    //-------------------------------------------------------------
    this->filter_icon_label = new IconLabel("Quick Filter:", Icons::FILTER_ICON, this);
    this->filter_entry = new QLineEdit(this);

    this->filter_options_area = new QGroupBox("Quick Filter Options", this);
    QVBoxLayout *filter_options_layout = new QVBoxLayout(this->filter_options_area);

    // Date Filter
    this->date_filter_checkbox = new QCheckBox("Filter dates", this);
    this->date_filter_checkbox->setIcon(QIcon(Icons::DATE_ICON));

    // Case Sensative Filter
    this->filter_case_sens_checkbox = new QCheckBox("Filter case-sensitive", this);
    this->filter_case_sens_checkbox->setIcon(QIcon(Icons::CASE_ICON));
    this->filter_case_sens_checkbox->setChecked(true);

    // Empty Cell Filter
    this->filter_type_combo = new QComboBox(this);
    this->filter_type_combo->addItems(SQLProperties::QuickFilterTypes);
    this->filter_empty_cells_checkbox = new QCheckBox("Filter empty cells", this);
    this->filter_empty_cells_checkbox->setIcon(QIcon(Icons::BLANK_ICON));

    // Filter Area
    filter_options_layout->addWidget(this->date_filter_checkbox);
    filter_options_layout->addWidget(this->filter_case_sens_checkbox);
    filter_options_layout->addWidget(this->filter_empty_cells_checkbox);
    this->filter_button = new QPushButton("Filter", this);

    // Refresh Table Button
    this->refresh_table_button = new QPushButton(this);
    this->refresh_table_button->setIcon(QIcon(Icons::REFRESH_ICON));
    this->refresh_table_button->setIconSize(QSize(16,16));
    this->refresh_table_button->setToolTip("Refresh the table");
    this->refresh_table_button->setFlat(true);

    // SQL Command Line Widgets
    this->sql_editor = new SQLEditor(this->table, this);
    this->sql_completer = new QCompleter(this);

    this->sql_run_button = new QPushButton("Run SQL", this);

    // Data Preview Widgets
    this->data_preview_textbox = new QPlainTextEdit(this);
    this->data_preview_textbox->setReadOnly(true);
    this->data_preview_textbox->setMaximumHeight(100);
    this->data_type_label = new QLabel(this);
    this->data_type_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->table_cell_label = new QLabel(this);
    this->table_cell_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->data_column_label = new QLabel(this);
    this->data_column_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->data_table_label = new QLabel(this);
    this->data_table_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QWidget *data_preview_filler = new QWidget(this);
    data_preview_filler->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    this->rows_label = new QLabel(this);
    this->columns_label = new QLabel(this);

    QStringList headers;

    this->main_widget = new QWidget(this);
    this->main_layout = new QVBoxLayout(this->main_widget);

    this->table_area = new QGroupBox(this->tableName(), this);

    QVBoxLayout *table_layout = new QVBoxLayout(this->table_area);

    QWidget *top_table_row = new QWidget(this);
    QGridLayout *top_table_layout = new QGridLayout(top_table_row);
    QWidget *bottom_table_row = new QWidget(this);
    QHBoxLayout *bottom_table_layout = new QHBoxLayout(bottom_table_row);
    QWidget *bottom_filler = new QWidget(this);
    bottom_filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    top_table_layout->setContentsMargins(0,0,0,0);
    top_table_layout->addWidget(this->table_selection_icon_label,0,0);
    top_table_layout->addWidget(this->table_selection_combo,1,0);
    top_table_layout->addWidget(this->refresh_table_button,1,1);
    top_table_layout->addWidget(this->column_icon_label,0,2);
    top_table_layout->addWidget(this->column_name_combo,1,2);
    top_table_layout->addWidget(this->filter_icon_label,0,3);
    top_table_layout->addWidget(this->filter_entry,1,3);
    top_table_layout->addWidget(this->filter_type_combo,1,4);
    top_table_layout->addWidget(this->filter_button,1,5);

    bottom_table_layout->addWidget(this->export_csv_button);
    bottom_table_layout->addWidget(bottom_filler);
    bottom_table_layout->addWidget(this->insert_row_button);
    bottom_table_layout->setContentsMargins(0,0,0,0);

    table_layout->addWidget(top_table_row);
    table_layout->addWidget(this->table);
    table_layout->addWidget(bottom_table_row);

    QWidget *bottom_label_row = new QWidget(this);
    QHBoxLayout *bottom_label_layout = new QHBoxLayout(bottom_label_row);
    bottom_label_layout->setContentsMargins(0,0,0,0);
    QWidget *bottom_label_filler = new QWidget(this);
    bottom_label_filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    bottom_label_layout->addWidget(this->rows_label);
    bottom_label_layout->addWidget(this->columns_label);
    bottom_label_layout->addWidget(bottom_label_filler);

    this->setBottomLabels();

    // Data Preview Area
    //-------------------------------------------------------------------------------------------------
    QWidget *central_area = new QWidget(this);
    QHBoxLayout *central_layout = new QHBoxLayout(central_area);
    central_layout->setContentsMargins(0,0,0,0);

    this->data_preview_area = new QGroupBox("Data Cell Properties", this);

    QFormLayout *data_preview_layout = new QFormLayout(this->data_preview_area);
    data_preview_layout->addRow(new QLabel("Value (preview):", this));
    data_preview_layout->addRow(this->data_preview_textbox);
    data_preview_layout->addRow("Data Type:", this->data_type_label);
    data_preview_layout->addRow("Table Cell:", this->table_cell_label);
    data_preview_layout->addRow("Column:", this->data_column_label);
    data_preview_layout->addRow("Table:", this->data_table_label);
    data_preview_layout->addWidget(data_preview_filler);

    QWidget *right_column_area = new QWidget(this);
    right_column_area->setMinimumWidth(250);
    right_column_area->setMaximumWidth(300);
    QVBoxLayout *right_column_layout = new QVBoxLayout(right_column_area);
    right_column_layout->setContentsMargins(0,0,0,0);

    right_column_layout->addWidget(this->filter_options_area);
    right_column_layout->addWidget(this->data_preview_area);

    central_layout->addWidget(this->table_area);
    central_layout->addWidget(right_column_area);
    this->main_layout->addWidget(central_area);
    this->main_layout->addWidget(bottom_label_row);

    /* MainWindow Signals */
    //-------------------------------------------------------------------------------------------------
    connect(this, &MainWindow::Table_Name_Changed, this->table, &Table::setTable);
    connect(this, &MainWindow::Value_Filtered, this->table, &Table::valueFilter);
    connect(this, &MainWindow::Column_Name_Index_Changed, this->table, &Table::selectColumn);

    /* Widget Signals */
    //-------------------------------------------------------------------------------------------------
    connect(this->export_csv_button, &QPushButton::clicked, this->table, &Table::exportCSVFile);
    connect(this->insert_row_button, &QPushButton::clicked, this, &MainWindow::insertRow);
    connect(this->table_selection_combo, &QComboBox::currentTextChanged, this, &MainWindow::tableNameChanged);
    connect(this->filter_entry, &QLineEdit::returnPressed, this, &MainWindow::filterValue);
    connect(this->filter_button, &QPushButton::clicked, this, &MainWindow::filterValue);
    connect(this->column_name_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::columnNameIndexChanged);
    connect(this->sql_run_button, &QPushButton::clicked, this, &MainWindow::runSQL);
    connect(this->filter_type_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::setQuickFilterPlaceHolderText);
    connect(this->refresh_table_button, &QPushButton::clicked, this, &MainWindow::refreshTable);
    connect(this->date_filter_checkbox, &QCheckBox::clicked, this, &MainWindow::updateQuickFilterWithDate);

    /* Table Signals */
    //-------------------------------------------------------------------------------------------------
    connect(this->table, &Table::Column_Names_Set, this, &MainWindow::setColumnNameCombo);
    connect(this->table, &Table::Column_Name_Index_Changed, this, &MainWindow::setColumnNameIndex);
    connect(this->table, &Table::New_Database_Set, this, &MainWindow::reset);
    connect(this->table, &Table::Query_Was_Executed, this, &MainWindow::enableDefectiveQueryWidgets);
    connect(this->table, &Table::Rows_And_Columns_Changed, this, &MainWindow::setBottomLabels);
    connect(this->table, &Table::Data_Preview, this, &MainWindow::setDataPreview);
    connect(this->sql_editor, &SQLEditor::resetModel, this, &MainWindow::updateCompleterModel);


    this->setCentralWidget(this->main_widget);
    this->setupDockWidgets();
    this->setSQLEditor();
    this->setupTopMenu();
    this->setQuickFilterPlaceHolderText(0);
    this->enableDefectiveQueryWidgets(false);
}

void MainWindow::setupTopMenu()
{
    QMenuBar *menu = this->menuBar();
    QMenu *file = menu->addMenu(tr("&File"));
    QMenu *edit = menu->addMenu(tr("&Edit"));
    QMenu *view = menu->addMenu(tr("&View"));
    //SUB VIEW MENU
    QMenu *dock_view = view->addMenu("Docks");

    QAction *exit = new QAction("Exit", this);

    /* FILE MENU */

    // New Database
    this->new_database_action = new QAction(QIcon(Icons::NEW_DB_ICON), "New Database", this);
    this->new_database_action->setShortcut(QKeySequence::New);
    connect(this->new_database_action, &QAction::triggered, this->table, &Table::createNewDatabase);
    file->addAction(this->new_database_action);

    // Open Database
    this->open_database_action = new QAction(QIcon(Icons::DB_ICON), "Open Database", this);
    this->open_database_action->setShortcut(QKeySequence::Open);
    connect(this->open_database_action, &QAction::triggered, this->table, &Table::openDatabase);
    file->addAction(this->open_database_action);
    file->addSeparator();


    // New Table
    this->new_table_action = new QAction(QIcon(Icons::ADD_TABLE_ICON), "New Table...", this);
    this->new_table_action->setDisabled(true);
    connect(this->new_table_action, &QAction::triggered, this, &MainWindow::runNewTableDialog);
    file->addAction(this->new_table_action);
    file->addSeparator();

    // Exit
    exit->setShortcut(QKeySequence::Close);
    connect(exit, &QAction::triggered, this, &MainWindow::close);
    file->addAction(exit);

    /* EDIT MENU */

    // Set table to read only.
    this->read_only_action = new QAction(QIcon(Icons::RW_ICON), "Set table to read only", this);
    this->read_only_action->setCheckable(true);
    connect(this->read_only_action, &QAction::toggled, this->table, &Table::setToReadOnly);
    connect(this->read_only_action, &QAction::toggled, this->insert_row_button, &QPushButton::setDisabled);
    connect(this->read_only_action, &QAction::toggled, this, &MainWindow::setTableTitleToReadOnly);

    edit->addAction(this->read_only_action);
    this->read_only_action->toggle();

    edit->addSeparator();
    // Remove Table
    this->remove_table_action = new QAction(QIcon(Icons::RMV_TABLE_ICON), "Remove Table", this);
    this->remove_table_action->setDisabled(true);
    connect(this->remove_table_action, &QAction::triggered, this, &MainWindow::removeTable);
    edit->addAction(this->remove_table_action);

    /* VIEW MENU */

    /* Dock View */
    dock_view->addAction(this->table_tree_dock->toggleViewAction());
    dock_view->addAction(this->sql_dock->toggleViewAction());

}

void MainWindow::setupDockWidgets()
{
    // Table Tree Dock
    this->table_tree_dock = new QDockWidget("Database Tables", this);
    this->setTableTree();
    this->addDockWidget(Qt::LeftDockWidgetArea, this->table_tree_dock);
    connect(this->table_tree, &QTreeWidget::itemDoubleClicked, this->table, &Table::setTableFromTree);
    connect(this->table, &Table::Table_Name_Changed, this, &MainWindow::setCurrentTableName);

    // SQL Dock
    this->sql_dock = new QDockWidget("SQL", this);

    // SQL Text Editor
    this->sql_area = new QGroupBox("SQL Command Line", this);
    QVBoxLayout *sql_editor_layout = new QVBoxLayout(sql_area);
    QWidget *bottom_row = new QWidget(this);
    QHBoxLayout *bottom_layout = new QHBoxLayout(bottom_row);
    bottom_layout->setContentsMargins(0,0,0,0);

    sql_editor_layout->addWidget(this->sql_editor);
    QWidget *sql_editor_filler = new QWidget(this);
    sql_editor_filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    bottom_layout->addWidget(sql_editor_filler);
    bottom_layout->addWidget(this->sql_run_button);
    sql_editor_layout->addWidget(bottom_row);

    this->sql_dock->setWidget(this->sql_area);
    this->addDockWidget(Qt::BottomDockWidgetArea, this->sql_dock);
}

void MainWindow::setSQLEditor()
{
    this->sql_editor->setHlForTablesAndCols(this->table->tableNames(), this->table->allColumnNames());
    this->sql_editor->rehighlight();
}

void MainWindow::runSQL()
{
    this->table->runSQL(this->sql_editor->toPlainText());
}

void MainWindow::setTableTree()
{
    QList<QTreeWidgetItem*> items;
    QStringList table_names = this->table->tableNames();
    QStringList column_names;
    int table_count = table_names.length();

    //this->table_tree->setAlternatingRowColors(true);
    this->table_tree->clear();
    this->table_tree->setColumnCount(1);
    this->table_tree->setHeaderLabel("Table");

    this->table_tree->setUpdatesEnabled(false);
    for(int i = 0; i < table_count; i++)
    {
        items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(table_names.at(i))));
        column_names = this->table->columnNames(table_names.at(i));
        for(int j=0; j<column_names.length(); j++)
        {
            items.at(i)->addChild(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(column_names.at(j))));

            if(j == 0)
                items.at(i)->child(j)->setIcon(0, QIcon(QIcon(Icons::KEY_ID_ICON).pixmap(16,16)));

            else
                items.at(i)->child(j)->setIcon(0, QIcon(QIcon(Icons::COLUMN_ICON).pixmap(16,16)));

        }
        items.at(i)->setIcon(0, QIcon(Icons::TABLE_ICON));
    }
    this->table_tree->insertTopLevelItems(0, items);
    this->table_tree_dock->setWidget(this->table_tree);
    this->table_tree->setUpdatesEnabled(true);
    this->updateCompleterModel();
}

void MainWindow::runNewTableDialog()
{
    NewTableDialog *table_dialog = new NewTableDialog(this->table, this);
    table_dialog->exec();
    if(table_dialog->success())
    {
        this->setTableTree();
        this->setTableCombo(this->table->tableNames());
        this->setColumnNameCombo(this->table->columnNames());
        this->setSQLEditor();
    }
    table_dialog->deleteLater();
}

void MainWindow::removeTable()
{
    bool ok = false;
    QString table_name;
    QMessageBox *msg;
    msg = new QMessageBox(this);
    msg->setWindowTitle("Remove Table?");
    msg->setIcon(QMessageBox::Question);
    msg->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    QInputDialog *dialog = new QInputDialog(this);

    table_name = dialog->getItem(this,
                   "Remove Table",
                   "Tables:",
                   this->table->tableNames(),
                   0, false, &ok,
                   Qt::MSWindowsFixedSizeDialogHint);
    msg->setText(QString("Are you sure you want to remove the \"%1\" table?").arg(table_name));

    if(ok && table_name.length() > 0)
    {
        switch(msg->exec())
        {
        case QMessageBox::Yes:
            this->table->removeTable(table_name);
            this->reset();
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
            this->removeTable();
            break;
        default:
            break;
        }
    }
    msg->deleteLater();
    dialog->deleteLater();
}

MainWindow::~MainWindow()
{
    //delete this->database;
}

void MainWindow::setTableCombo(QStringList table_names)
{
    this->table_selection_combo->clear();
    this->table_selection_combo->addItems(table_names);
}

void MainWindow::setColumnNameCombo(QStringList column_names)
{
    this->column_name_combo->clear();
    this->column_name_combo->addItems(column_names);
    this->column_name_combo->setCurrentIndex(1);
}

void MainWindow::filterValue()
{
    QStringList filter_values;
    filter_values.append(this->column_name_combo->currentText());
    filter_values.append(this->filter_entry->text());

    emit Value_Filtered(this->filter_type_combo->currentIndex(),
                        filter_values, this->tableName(),
                        this->filter_empty_cells_checkbox->isChecked(),
                        this->filter_case_sens_checkbox->isChecked(),
                        this->date_filter_checkbox->isChecked());
}

void MainWindow::tableNameChanged(QString table_name)
{
    this->enableDefectiveQueryWidgets(true);
    emit Table_Name_Changed(table_name);
    this->setBottomLabels();
}

void MainWindow::columnNameIndexChanged(int index)
{
    this->setQuickFilterPlaceHolderText(this->filter_type_combo->currentIndex());
    emit Column_Name_Index_Changed(index);
}

void MainWindow::setCurrentTableName(QString table_name)
{
    this->table_selection_combo->setCurrentText(table_name);
}

void MainWindow::setColumnNameIndex(int index)
{
    this->column_name_combo->setCurrentIndex(index);
}

void MainWindow::insertRow()
{
    this->table->insertRow(this->tableName(), this->table->columnNames());
}

void MainWindow::reset()
{
    this->setTableTree();
    this->setTableCombo(this->table->tableNames());
    this->setColumnNameCombo(this->table->columnNames());
    this->setSQLEditor();
    this->sql_editor->rehighlight();
    this->setTableTitleToReadOnly(this->table->isReadOnly());
    this->enableDefectiveQueryWidgets(true);
    this->setBottomLabels();
    this->new_table_action->setEnabled(true);
    this->remove_table_action->setEnabled(true);
    this->export_csv_button->setEnabled(true);
}

void MainWindow::setTableTitleToReadOnly(bool toggled)
{
    if(toggled)
        this->table_area->setTitle(this->tableName()+" (Read Only)");
    else
        this->table_area->setTitle(this->tableName());
}

void MainWindow::enableDefectiveQueryWidgets(bool enable)
{
    this->read_only_action->setEnabled(enable);
    this->filter_entry->setEnabled(enable);
    this->filter_button->setEnabled(enable);
    this->filter_empty_cells_checkbox->setEnabled(enable);
    this->date_filter_checkbox->setEnabled(enable);
    this->filter_case_sens_checkbox->setEnabled(enable);
    this->column_name_combo->setEnabled(enable);

    if(enable)
        this->setTableTitleToReadOnly(this->table->isReadOnly());
    else
    {
        this->table_area->setTitle("Query Table (Read Only)");
        this->setBottomLabels();
    }
}

void MainWindow::setBottomLabels()
{
    this->rows_label->setText(QString("Rows: %1, ").arg(this->table->rowCount()));
    this->columns_label->setText(QString("Columns: %1").arg(this->table->columnCount()));
}

void MainWindow::updateQuickFilterWithDate()
{
    this->setQuickFilterPlaceHolderText(this->filter_type_combo->currentIndex());
}

void MainWindow::setQuickFilterPlaceHolderText(int index)
{
    QString date_filter_text = "";
    if(this->date_filter_checkbox->isChecked())
        date_filter_text = "(date filter)";

    switch(index)
    {
    case SQLProperties::EQUAL:
        this->filter_entry->setPlaceholderText(QString("x = %1 %2").arg(this->columnName(), date_filter_text));
        break;
    case SQLProperties::GREATER_THAN:
        this->filter_entry->setPlaceholderText(QString("x > %1 %2").arg(this->columnName(), date_filter_text));
        break;
    case SQLProperties::LESS_THAN:
        this->filter_entry->setPlaceholderText(QString("x < %1 %2").arg(this->columnName(), date_filter_text));
        break;
    case SQLProperties::GREATER_OR_EQUAL_THAN:
        this->filter_entry->setPlaceholderText(QString("x >= %1 %2").arg(this->columnName(), date_filter_text));
        break;
    case SQLProperties::LESS_OR_EQUAL_THAN:
        this->filter_entry->setPlaceholderText(QString("x <= %1 %2").arg(this->columnName(), date_filter_text));
        break;
    case SQLProperties::BETWEEN:
        this->filter_entry->setPlaceholderText(QString("[x, y] in %1 %2").arg(this->columnName(), date_filter_text));
        break;
    }
}

void MainWindow::setDataPreview(QString cell_value, QString data_type, QString column_name, int row, int column)
{
    this->data_preview_textbox->setPlainText(cell_value);
    this->data_type_label->setText(data_type);
    this->table_cell_label->setText(QString("(%1, %2)").arg(row+1).arg(column+1));
    this->data_column_label->setText(column_name);
    this->data_table_label->setText(this->tableName());
}

void MainWindow::updateCompleterModel()
{
    this->sql_completer->setModel(this->createCompleterListModel());
    this->sql_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    this->sql_completer->setCaseSensitivity(Qt::CaseInsensitive);
    this->sql_completer->setWrapAround(false);
    this->sql_editor->setCompleter(this->sql_completer);
}

void MainWindow::createCompleterSheet()
{
    CSV writer;
    QStringList table_names = this->table->tableNames();
    QStringList column_names;
    int table_count = table_names.length();
    int sql_read_cmd_count = SQLCommands::READ_COMMANDS.length();
    int sql_write_cmd_count = SQLCommands::WRITE_COMMANDS.length();
    QStringList words_used = {};
    QString word;

    writer.open(QString("completer_sheet.txt"));
    // Write SQL Read/Write Commands
    for(int i = 0; i < sql_read_cmd_count; i++)
    {
        word = SQLCommands::READ_COMMANDS[i];
        if(!words_used.contains(word) && word.length() > 2)
            writer.writeRow({word});
        words_used.append(word);
    }

    for(int i = 0; i < sql_write_cmd_count; i++)
    {
        word = SQLCommands::WRITE_COMMANDS[i];
        if(!words_used.contains(word) && word.length() > 2)
            writer.writeRow({word});
        words_used.append(word);
    }

    // List Table Names and Column Names
    for(int i = 0; i < table_count; i++)
    {
        word = table_names[i];
        if(!words_used.contains(word) && word.length() > 2)
            writer.writeRow({word});
        words_used.append(word);

        column_names = this->table->columnNames(table_names[i]);
        for(int j = 0; j < column_names.length(); j++)
        {
            word = column_names[j];
            if(!words_used.contains(word) && word.length() > 2)
                writer.writeRow({word});
            words_used.append(word);
        }

    }
    writer.close();
}

void MainWindow::refreshTable()
{
    if(this->table_selection_combo->count())
    {
        this->enableDefectiveQueryWidgets(true);
        emit Table_Name_Changed(this->tableName());
        this->setBottomLabels();
    }
}

QAbstractItemModel *MainWindow::createCompleterListModel()
{
    QStandardItemModel* model = new QStandardItemModel(this->sql_completer);
    QStringList table_names = this->table->tableNames();
    QStringList column_names;
    int table_count = table_names.length();
    int sql_read_cmd_count = SQLCommands::READ_COMMANDS.length();
    int sql_write_cmd_count = SQLCommands::WRITE_COMMANDS.length();
    QStringList words = {};
    QMap <QString, int> icons = {};
    QString word;
    int word_count = 0;

    // Get SQL read command strings
    for(int i = 0; i < sql_read_cmd_count; i++)
    {
        word = SQLCommands::READ_COMMANDS[i];
        if(!words.contains(word) && word.length() > 2)
        {
            words.append(word);
            icons[word] = SQL_ID;
        }

    }
    // Get SQL write command strings
    for(int i = 0; i < sql_write_cmd_count; i++)
    {
        word = SQLCommands::WRITE_COMMANDS[i];
        if(!words.contains(word) && word.length() > 2)
        {
            words.append(word);
            icons[word] = SQL_ID;
        }
    }

    // Get current table names and column names
    for(int i = 0; i < table_count; i++)
    {
        word = table_names[i];
        if(!words.contains(word) && word.length() > 2)
        {
            words.append(word);
            icons[word] = TABLE_ID;
        }

        column_names = this->table->columnNames(table_names[i]);
        for(int j = 0; j < column_names.length(); j++)
        {
            word = column_names[j];
            if(!words.contains(word) && word.length() > 2)
            {
                words.append(word);
                icons[word] = COLUMN_ID;
            }
        }
    }
    words.sort(Qt::CaseInsensitive);
    word_count = words.length();
    model->setRowCount(word_count);
    model->setColumnCount(1);
    for(int i = 0; i < word_count; i++)
    {
        word = words[i];

        QStandardItem* item = new QStandardItem(word);
        item->setIcon(QIcon(QIcon(this->getIconFromId(icons[word])).pixmap(16,16)));
        //item->setToolTip("some tool tip");
        model->setItem(i, 0, item);
    }

    return model;
}

QString MainWindow::tableName()
{
    return this->table_selection_combo->currentText();
}

QString MainWindow::columnName()
{
    return this->column_name_combo->currentText();
}

QString MainWindow::getIconFromId(int icon_id)
{
    switch(icon_id)
    {
    case TABLE_ID:
        return Icons::TABLE_ICON;
    case COLUMN_ID:
        return Icons::COLUMN_ICON;
    case KEY_ID:
        return Icons::KEY_ID_ICON;
    case FILTER_ID:
        return Icons::FILTER_ICON;
    case SQL_ID:
        return Icons::SQL_ICON;
    default:
        return ""; // Place a default icon here
    }
}
