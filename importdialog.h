#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include "table.h"
#include <QGroupBox>
#include <QTableWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

class ImportDialog : public QDialog
{
    Q_OBJECT
private:
    Table *_table;
    int current_row;
    int max_column_count;
    QVBoxLayout *main_layout;
    // Csv Setup-------------------------------#
    QGroupBox *csv_browser_area;
    QCheckBox *has_headers_chkbox;
    QLineEdit *csv_file_browser_entry;
    // Database Table--------------------------#
    IconLabel *table_icon;
    QGroupBox *database_table_area;
    QComboBox *table_combo;
    QTableWidget *database_table;
    // Import Table----------------------------#
    QGroupBox *import_table_area;
    QTableView *import_table;
    QPushButton *csv_file_browser_button;

    // Bottom Buttons--------------------------#
    QPushButton *insert_button;
    QPushButton *cancel_button;

    QSpinBox *start_index_spinbox;
    QSpinBox *end_index_spinbox;

    QStringList csv_headers;

    void setupCsvBrowser();
    void setupDatabaseTable();
    void setupImportTable();
    void setupCsvThread();

public:
    ImportDialog(Table *table, QWidget *parent = nullptr);

protected:
    virtual void closeEvent(QCloseEvent* event);

public slots:
    void importCsv();
    void setImportTable(QStringList column_names);
    void setImportTableModel(QStandardItemModel *model);
    //void writeToImportTable(int csv_row);
    void setDatabaseTable(QString table_name);
    void changeStartMax(int value);
    void changeEndMin(int value);
    void insertDataIntoTable();

signals:
    void dialogClosed();
    void csvPathRead(QString path, bool has_headers);
    void insertionRequested(QString database_path,
                            QString table_name,
                            QStringList column_names,
                            IntList data_placement,
                            int row_start,
                            int row_end);
};

#endif // IMPORTDIALOG_H
