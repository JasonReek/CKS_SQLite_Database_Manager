#ifndef NEWTABLEDIALOG_H
#define NEWTABLEDIALOG_H

#include "sqleditor.h"

#include <QDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSizePolicy>
#include <QAbstractScrollArea>
#include <QListWidget>
#include <QListWidgetItem>
#include <QItemSelectionModel>


class NewTableDialog : public QDialog
{
    Q_OBJECT
private:
    bool _success;
    QLineEdit *table_name_entry;
    QLineEdit *table_id_entry;
    QTableWidget *user_defined_table;
    QStringList sql_types{"NULL", "INTEGER", "TEXT", "REAL", "BLOB", "CLOB"};
    QVBoxLayout *main_layout;
    QGroupBox *table_edit_area;
    QGroupBox *column_properties_area;
    QListWidget *column_prop_list_widget;
    QPushButton *add_col_button;
    QPushButton *rmv_col_button;
    QPushButton *cancel_button;
    QPushButton *enter_button;
    QLabel *column_count_label;
    Table *_table;
    QComboBox *ref_table_names_combo;
    QCheckBox *has_f_key_checkbox;
    QComboBox *ref_col_names_combo;
    QMap <QString, ColumnData> column_properties;
    QString column_name;
    QString data_type;

public:
    explicit NewTableDialog(Table* table, QWidget *parent = nullptr);
    bool success(){return this->_success;};
    QStringList columnNames();
    bool columnNameExists(QString column_name);

public slots:
    void editTableName(QString table_name);
    void addColumn();
    void removeColumn();
    void messageError(QString error);
    void enterPressed();
    void setColumnProperties(QModelIndex current, QModelIndex previous);
    void setColumnPropertiesWhenEdited(QString text);
    void setColumnPropertiesWhenCurrentChanged(QString text);
    void addColumnProperty(bool f_key_id_checked);
    void updateColPropOnTableChange(QString table_name);
    void updateColPropOnIdChange(QString id_name);
    void setNewColumnNames(QString table_name);

signals:
    void Create_New_Table(QString table_name, QStringList values, QList<int> sql_types);
};

#endif // NEWTABLEDIALOG_H
