#include "newtabledialog.h"


NewTableDialog::NewTableDialog(Table* table, QWidget *parent) : QDialog(parent)
{
    this->column_name = "";
    this->data_type = "";
    this->column_properties = {};
    this->_success = false;
    this->_table = table;
    QRegExp table_name_exp("[A-z_]*$");
    QRegExpValidator *name_vali = new QRegExpValidator(table_name_exp, this);

    this->main_layout = new QVBoxLayout(this);
    this->table_name_entry = new QLineEdit(this);
    this->table_name_entry->setValidator(name_vali);
    this->table_id_entry = new QLineEdit(this);
    this->table_id_entry->setValidator(name_vali);
    this->table_id_entry->setText("id");
    this->user_defined_table = new QTableWidget(2, 3, this);
    this->add_col_button = new QPushButton("Add Column", this);
    this->rmv_col_button = new QPushButton("Remove Column", this);
    this->cancel_button = new QPushButton("Cancel", this);
    this->enter_button = new QPushButton("Enter", this);
    this->column_count_label = new QLabel(this);
    this->column_count_label->setText(QString("Columns: %1").arg(this->user_defined_table->columnCount()));

    QWidget *button_filler = new QWidget(this);
    button_filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QGroupBox *table_settings_area = new QGroupBox("Settings", this);
    QFormLayout *table_settings_layout = new QFormLayout(table_settings_area);
    this->table_edit_area = new QGroupBox("Table", this);
    QVBoxLayout *new_table_layout = new QVBoxLayout(this->table_edit_area);

    this->user_defined_table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    //this->user_defined_table->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->user_defined_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //this->user_defined_table->verticalHeader()->setStretchLastSection(true);
    //this->user_defined_table->horizontalHeader()->setStretchLastSection(true);

    // Button Setup
    QWidget *button_row = new QWidget(this);
    QHBoxLayout *button_layout = new QHBoxLayout(button_row);
    button_layout->addWidget(this->rmv_col_button);
    button_layout->addWidget(this->add_col_button);
    button_layout->addWidget(button_filler);
    button_layout->addWidget(this->column_count_label);
    button_layout->setContentsMargins(0,0,0,0);

    this->setWindowTitle("Create New Table");

    IconLabel *table_name_icon_label = new IconLabel("Table name:", Icons::TABLE_ICON, this, 24, 24);
    IconLabel *table_id_icon_label = new IconLabel("Table id:", Icons::KEY_ID_ICON, this, 24, 24);
    QWidget *table_info_row = new QWidget(this);
    QGridLayout *table_info_grid = new QGridLayout(table_info_row);
    table_info_grid->setContentsMargins(0,0,0,0);
    table_info_grid->addWidget(table_name_icon_label,0,0);
    table_info_grid->addWidget(table_id_icon_label,0,1);
    table_info_grid->addWidget(this->table_name_entry,1,0);
    table_info_grid->addWidget(this->table_id_entry,1,1);

    table_settings_layout->addRow(table_info_row);

    new_table_layout->addWidget(this->user_defined_table);
    new_table_layout->addWidget(button_row);
    QWidget *new_table_area_filler = new QWidget(this);
    new_table_area_filler->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    new_table_layout->addWidget(new_table_area_filler);
    this->user_defined_table->setVerticalHeaderLabels(QStringList{"Column Name", "Data Type"});
    for(int column = 0; column < this->user_defined_table->columnCount(); column++)
    {
        this->user_defined_table->setCellWidget(0, column, new QLineEdit(this));
        static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column))->setValidator(name_vali);
        connect(static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column)), &QLineEdit::textEdited, this,
                &NewTableDialog::setColumnPropertiesWhenEdited);
        this->user_defined_table->setCellWidget(1, column, new QComboBox(this));
        static_cast<QComboBox*>(this->user_defined_table->cellWidget(1, column))->addItems(this->sql_types);
        static_cast<QComboBox*>(this->user_defined_table->cellWidget(1, column))->setCurrentIndex(2);
        connect(static_cast<QComboBox*>(this->user_defined_table->cellWidget(1, column)),
                &QComboBox::currentTextChanged, this,
                &NewTableDialog::setColumnPropertiesWhenCurrentChanged);
    }

    QWidget *bottom_row = new QWidget(this);
    QHBoxLayout *bottom_layout = new QHBoxLayout(bottom_row);
    QWidget *bottom_filler = new QWidget(this);
    bottom_filler->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    bottom_layout->addWidget(bottom_filler);
    bottom_layout->addWidget(this->enter_button);
    bottom_layout->addWidget(this->cancel_button);
    bottom_layout->setContentsMargins(0,0,0,0);

    // Column Properties:
    //-------------------------------------------------------------------------
    this->column_properties_area = new QGroupBox("Column Properties", this);
    this->column_properties_area->setMaximumWidth(300);
    this->column_prop_list_widget = new QListWidget(this);
    this->column_prop_list_widget->setMinimumWidth(275);
    this->column_prop_list_widget->setMaximumWidth(275);
    this->column_prop_list_widget->setAlternatingRowColors(true);

    // PROPERTIES
    //-------------------
    //# Column Name
    this->column_prop_list_widget->addItem("Column name:");
    //# Data Type
    this->column_prop_list_widget->addItem("Data type:");
    //# Foreign Key Checkbox
    this->column_prop_list_widget->addItem("");
    this->has_f_key_checkbox = new QCheckBox("Set as foreign key", this);
    this->has_f_key_checkbox->setDisabled(true);
    QListWidgetItem *has_f_key_item = new QListWidgetItem(this->column_prop_list_widget);
    this->column_prop_list_widget->setItemWidget(has_f_key_item, this->has_f_key_checkbox);
    this->column_prop_list_widget->addItem("");
    //# Reference Table Names Combobox
    this->ref_table_names_combo = new QComboBox(this);
    this->ref_table_names_combo->addItems(this->_table->tableNames());
    this->ref_table_names_combo->setDisabled(true);

    this->column_prop_list_widget->addItem("Reference table:");
    QListWidgetItem *table_reference_item = new QListWidgetItem(this->column_prop_list_widget);
    this->column_prop_list_widget->setItemWidget(table_reference_item, this->ref_table_names_combo);

    //# Reference Table Names Combobox
    this->column_prop_list_widget->addItem("");
    this->ref_col_names_combo = new QComboBox(this);
    this->ref_col_names_combo->addItems(this->_table->columnNames(this->ref_table_names_combo->currentText()));
    this->ref_col_names_combo->setDisabled(true);

    this->column_prop_list_widget->addItem("Reference id:");
    QListWidgetItem *ref_col_item = new QListWidgetItem(this->column_prop_list_widget);
    this->column_prop_list_widget->setItemWidget(ref_col_item, this->ref_col_names_combo);
    //-------------------
    QVBoxLayout *column_prop_layout = new QVBoxLayout(this->column_properties_area);
    column_prop_layout->addWidget(this->column_prop_list_widget);
    this->column_prop_list_widget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    //-------------------------------------------------------------------------

    QWidget *table_edit_space_row = new QWidget(this);
    QHBoxLayout *table_edit_space_layout = new QHBoxLayout(table_edit_space_row);
    table_edit_space_layout->setContentsMargins(0,0,0,0);
    table_edit_space_layout->addWidget(this->table_edit_area);
    table_edit_space_layout->addWidget(this->column_properties_area);

    this->main_layout->addWidget(table_settings_area);
    this->main_layout->addWidget(table_edit_space_row);
    this->main_layout->addWidget(bottom_row);

    connect(this->table_name_entry, &QLineEdit::textEdited, this, &NewTableDialog::editTableName);
    connect(this->rmv_col_button, &QPushButton::clicked, this, &NewTableDialog::removeColumn);
    connect(this->add_col_button, &QPushButton::clicked, this, &NewTableDialog::addColumn);
    connect(this->cancel_button, &QPushButton::clicked, this, &NewTableDialog::close);
    connect(this->enter_button, &QPushButton::clicked, this, &NewTableDialog::enterPressed);
    connect(this->user_defined_table->selectionModel(), &QItemSelectionModel::currentColumnChanged, this, &NewTableDialog::setColumnProperties);
    connect(this->ref_table_names_combo, &QComboBox::currentTextChanged, this, &NewTableDialog::setNewColumnNames);
    connect(this->has_f_key_checkbox, &QCheckBox::stateChanged, this, &NewTableDialog::addColumnProperty);
    connect(this->ref_table_names_combo, &QComboBox::currentTextChanged, this, &NewTableDialog::updateColPropOnTableChange);
    connect(this->ref_col_names_combo, &QComboBox::currentTextChanged, this, &NewTableDialog::updateColPropOnIdChange);

    this->setMinimumSize(600, 390);
    //this->setMaximumHeight(this->height());
}

void NewTableDialog::editTableName(QString table_name)
{
    if(table_name.length() > 0)
        this->table_edit_area->setTitle(QString("Table (%1)").arg(table_name));
    else
        this->table_edit_area->setTitle("Table");
}

void NewTableDialog::addColumn()
{
    int column_count = this->user_defined_table->columnCount();
    int next_column = column_count + 1;
    QRegExp table_name_exp("[A-z_]*$");
    QRegExpValidator *name_vali = new QRegExpValidator(table_name_exp, this);

    if(next_column == 2)
        this->rmv_col_button->setEnabled(true);

    this->user_defined_table->setColumnCount(next_column);

    this->user_defined_table->setCellWidget(0, column_count, new QLineEdit(this));
    static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column_count))->setValidator(name_vali);
    connect(static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column_count)),
            &QLineEdit::textEdited, this, &NewTableDialog::setColumnPropertiesWhenEdited);

    this->user_defined_table->setCellWidget(1, column_count, new QComboBox(this));
    static_cast<QComboBox*>(this->user_defined_table->cellWidget(1, column_count))->addItems(this->sql_types);
    static_cast<QComboBox*>(this->user_defined_table->cellWidget(1, column_count))->setCurrentIndex(2);
    connect(static_cast<QComboBox*>(this->user_defined_table->cellWidget(1, column_count)),
            &QComboBox::currentTextChanged, this,
            &NewTableDialog::setColumnPropertiesWhenCurrentChanged);
    this->column_count_label->setText(QString("Columns: %1").arg(next_column));
    this->column_prop_list_widget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
}

void NewTableDialog::removeColumn()
{
    int column_count = this->user_defined_table->columnCount();

    if(column_count > 1)
    {
        // Remove column properties if it exists on this column.
        if(this->column_name.length() && this->column_properties.contains(this->column_name))
            this->column_properties.remove(this->column_name);

        column_count--;
        this->user_defined_table->setColumnCount(column_count);
        if(column_count == 1)
            this->rmv_col_button->setDisabled(true);
        this->column_count_label->setText(QString("Columns: %1").arg(column_count));
        this->column_prop_list_widget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    }
}

void NewTableDialog::enterPressed()
{
    QStringList column_names = {};
    QString entry_value = "";
    QString table_name = this->table_name_entry->text();
    QString table_id = this->table_id_entry->text();
    QList<int> data_types = {};
    int index = 0;

    if(table_name.length() < 1)
    {
        this->messageError(QString("Table name entry is empty."));
        return;
    }
    if(this->_table->tableNames().contains(table_name))
    {
        this->messageError(QString("Table name already exists."));
        return;
    }
    if(table_id.length() < 1)
    {
        this->messageError(QString("Table id entry is empty."));
        return;
    }

    for(int row = 0; row < this->user_defined_table->rowCount(); row++)
    {
        for(int column = 0; column < this->user_defined_table->columnCount(); column++)
        {
            if(row == 0)
            {
                entry_value = static_cast<QLineEdit*>(this->user_defined_table->cellWidget(row, column))->text();
                if(entry_value.length() > 0)
                    column_names.append(entry_value);
                else
                {
                    this->messageError(QString("Column %1's name entry is empty.").arg(column + 1));
                    return;
                }
            }
            else
            {
                index = static_cast<QComboBox*>(this->user_defined_table->cellWidget(row, column))->currentIndex();
                data_types.append(Table::getSqlType(index));
            }
        }
    }
    if(this->column_properties.count())
    {
        this->_table->createTable(table_name, table_id, column_names, data_types, this->column_properties);
    }
    else
    {
        this->_table->createTable(table_name, table_id, column_names, data_types);
    }

    this->_success = true;
    this->close();
}

void NewTableDialog::messageError(QString message)
{
    QMessageBox *msg;
    msg = new QMessageBox();

    msg->setWindowTitle("Entry Warning");
    msg->setText(message);
    msg->setIcon(QMessageBox::Warning);
    msg->setStandardButtons(QMessageBox::Ok);
    msg->exec();

    msg->deleteLater();
}

void NewTableDialog::setColumnProperties(QModelIndex current, QModelIndex previous)
{
    int column = current.column();
    this->column_name = static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column))->text();
    this->data_type = static_cast<QComboBox*>(this->user_defined_table->cellWidget(1, column))->currentText();

    bool integer_is_set = (this->data_type == SQL::TYPE_INTEGER);

    this->has_f_key_checkbox->setEnabled(integer_is_set);

    // Set Column Name
    this->column_prop_list_widget->item(0)->setText(QString("Column Name: %1").arg(this->column_name));
    this->column_prop_list_widget->item(1)->setText(QString("Data Type: %1").arg(this->data_type));
    if(this->column_properties.contains(this->column_name))
        this->has_f_key_checkbox->setChecked(true);
    else
        this->has_f_key_checkbox->setChecked(false);

}

void NewTableDialog::setColumnPropertiesWhenEdited(QString text)
{
    this->column_name = text;
    int column_index = this->user_defined_table->currentIndex().column();

    if(this->column_name.length() && this->columnNames().count(this->column_name) > 1)
    {
        static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column_index))->clear();
        this->column_prop_list_widget->item(0)->setText("Column Name:");
        this->messageError(QString("Column \"%1\" already exists.").arg(text));
        return;
    }
    // Set Column Name
    this->column_prop_list_widget->item(0)->setText(QString("Column Name: %1").arg(this->column_name));
}

void NewTableDialog::setColumnPropertiesWhenCurrentChanged(QString text)
{
    this->data_type = text;
    bool integer_is_set = (this->data_type == SQL::TYPE_INTEGER);

    // Set Column Name
    this->column_prop_list_widget->item(1)->setText(QString("Data Type: %1").arg(this->data_type));

    this->has_f_key_checkbox->setEnabled(integer_is_set);

    if(!integer_is_set && this->has_f_key_checkbox->isChecked())
        this->has_f_key_checkbox->setChecked(false);
}

void NewTableDialog::setNewColumnNames(QString table_name)
{
    this->ref_col_names_combo->clear();
    this->ref_col_names_combo->addItems(this->_table->columnNames(table_name));
}

void NewTableDialog::addColumnProperty(bool f_key_id_checked)
{
    int column_index = this->user_defined_table->currentIndex().column();
    this->ref_table_names_combo->setEnabled(f_key_id_checked);
    this->ref_col_names_combo->setEnabled(f_key_id_checked);

    if(f_key_id_checked)
    {
        if(this->column_name.length() && this->data_type == SQL::TYPE_INTEGER && !this->column_properties.contains(this->column_name))
        {
            static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column_index))->setReadOnly(true);
            this->column_properties[this->column_name] = ColumnData();
            this->column_properties[this->column_name].reference_table = this->ref_table_names_combo->currentText();
            this->column_properties[this->column_name].reference_id = this->ref_col_names_combo->currentText();
        }
    }
    else
    {
        static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column_index))->setReadOnly(false);
        if(this->column_properties.contains(this->column_name))
            this->column_properties.remove(this->column_name);
    }
}

void NewTableDialog::updateColPropOnTableChange(QString table_name)
{
    if(this->has_f_key_checkbox->isChecked())
    {
        if(this->column_name.length() && this->column_properties.contains(this->column_name))
            this->column_properties[this->column_name].reference_table = table_name;
    }
}

void NewTableDialog::updateColPropOnIdChange(QString id_name)
{
    if(this->has_f_key_checkbox->isChecked())
    {
        if(this->column_name.length() && this->column_properties.contains(this->column_name))
            this->column_properties[this->column_name].reference_id = id_name;
    }
}

QStringList NewTableDialog::columnNames()
{
    QStringList column_names = {};
    for(int column = 0; column < this->user_defined_table->columnCount(); column++)
        column_names.append(static_cast<QLineEdit*>(this->user_defined_table->cellWidget(0, column))->text());
    return column_names;
}

bool NewTableDialog::columnNameExists(QString column_name)
{
    return this->columnNames().contains(column_name);
}

