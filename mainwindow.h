#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "database.h"
#include "newtabledialog.h"

#include <QGridLayout>
#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <Qt>
#include <QColor>
#include <QBrush>
#include <QTreeWidget>
#include <QDockWidget>
#include <QMenuBar>
#include <QPlainTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    IconLabel *table_selection_icon_label;
    IconLabel *column_icon_label;
    QComboBox *table_selection_combo;
    QComboBox *column_name_combo;
    QAction *read_only_action;
    QAction *new_database_action;
    QAction *open_database_action;
    QAction *new_table_action;
    QAction *remove_table_action;
    Table *table;
    QWidget *main_widget;
    QGroupBox *table_area;
    QVBoxLayout *main_layout;
    bool _id_column_locked;
    QDockWidget *table_tree_dock;
    QTreeWidget *table_tree;
    QPushButton *insert_row_button;
    QPushButton *export_csv_button;
    IconLabel *filter_icon_label;
    QLineEdit *filter_entry;
    QCheckBox *filter_empty_cells_checkbox;
    QCheckBox *filter_case_sens_checkbox;
    QCheckBox *date_filter_checkbox;
    QComboBox *filter_type_combo;
    QPushButton *filter_button;
    QPushButton *sql_run_button;
    QPushButton *refresh_table_button;
    SQLEditor *sql_editor;
    QLabel *rows_label;
    QLabel *columns_label;
    QGroupBox *data_preview_area;
    QGroupBox *filter_options_area;
    QLabel *data_type_label;
    QLabel *data_table_label;
    QLabel *data_column_label;
    QLabel *table_cell_label;
    QPlainTextEdit *data_preview_textbox;
    QCompleter *sql_completer;
    QGroupBox *sql_area;
    QDockWidget *sql_dock;
    QAbstractItemModel *createCompleterListModel();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setupTopMenu();
    void setupDockWidgets();
    void setTableTree();
    void tableNameChanged(QString table_name);
    void setTableCombo(QStringList table_names);
    void setColumnNameCombo(QStringList column_names);
    void setColumnNameIndex(int index);
    void setSQLEditor();
    void runSQL();
    void filterValue();
    void columnNameIndexChanged(int index);
    void setCurrentTableName(QString table_name);
    void setQueryReadOnlyTableTitle();
    QString tableName();
    QString columnName();
    QString getIconFromId(int icon_id);

public slots:
    void setTableTitleToReadOnly(bool toggled);
    void runNewTableDialog();
    void removeTable();
    void insertRow();
    void enableDefectiveQueryWidgets(bool enable);
    void reset();
    void setBottomLabels();
    void updateQuickFilterWithDate();
    void setQuickFilterPlaceHolderText(int index);
    void setDataPreview(QString cell_value, QString data_type, QString column_name, int row, int column);
    void createCompleterSheet();
    void updateCompleterModel();
    void refreshTable();

signals:
    void Table_Name_Changed(QString);
    void Value_Filtered(int filter_type, QStringList, QString, bool, bool, bool);
    void Column_Name_Index_Changed(int);
};
#endif // MAINWINDOW_H
