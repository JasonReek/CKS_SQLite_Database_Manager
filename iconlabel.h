#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QLabel>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QIcon>

enum IconIds
{
    TABLE_ID,
    COLUMN_ID,
    KEY_ID,
    FILTER_ID,
    SQL_ID,
    FUNC_ID,
};

namespace Icons
{ 
    const QString TABLE_ICON = QStringLiteral(":/Resources/Icons/table.png");
    const QString ADD_TABLE_ICON = QStringLiteral(":/Resources/Icons/add_table.png");
    const QString RMV_TABLE_ICON = QStringLiteral(":/Resources/Icons/remove_table.png");
    const QString RMV_ROW_ICON = QStringLiteral(":/Resources/Icons/delete_row.png");
    const QString COLUMN_ICON = QStringLiteral(":/Resources/Icons/column_cube.png");
    const QString KEY_ID_ICON = QStringLiteral(":/Resources/Icons/key.png");
    const QString FILTER_ICON = QStringLiteral(":/Resources/Icons/filter.png");
    const QString SQL_ICON = QStringLiteral(":/Resources/Icons/sql.png");
    const QString DB_ICON = QStringLiteral(":/Resources/Icons/db.png");
    const QString NEW_DB_ICON = QStringLiteral(":/Resources/Icons/new_db.png");
    const QString REFRESH_ICON = QStringLiteral(":/Resources/Icons/refresh.png");
    const QString DATE_ICON = QStringLiteral(":/Resources/Icons/date.png");
    const QString CASE_ICON = QStringLiteral(":/Resources/Icons/case.png");
    const QString BLANK_ICON = QStringLiteral(":/Resources/Icons/blank.png");
    const QString EXPORT_ICON = QStringLiteral(":/Resources/Icons/export.png");
    const QString INSERT_ICON = QStringLiteral(":/Resources/Icons/insert.png");
    const QString CONVERT_ICON = QStringLiteral(":/Resources/Icons/convert.png");
    const QString RW_ICON = QStringLiteral(":/Resources/Icons/read_write.png");
    const QString CICADA_ICON = QStringLiteral(":/Resources/Icons/cicada.png");
    const QString ITRON_ICON = QStringLiteral(":/Resources/Icons/itron.png");
    const QString FUNC_ICON = QStringLiteral(":/Resources/Icons/function.png");
    const QString SQLITE_ICON = QStringLiteral(":/Resources/Icons/sqlite.png");
    const QString SQL_EDIT_ICON = QStringLiteral(":/Resources/Icons/sqledit.png");
    const QString SQL_TUT_ICON = QStringLiteral(":/Resources/Icons/tutorial.png");
    const QString IMPORT_FILE_ICON = QStringLiteral(":/Resources/Icons/import.png");
    const QString IMPORT_MENU_ICON = QStringLiteral(":/Resources/Icons/import_menu.png");
    const QString ACCESS_DB_ICON = QStringLiteral(":/Resources/Icons/access_db.png");
}

class IconLabel : public QWidget
{
    Q_OBJECT
public:
    IconLabel(QString text, QString image_src, QWidget *parent = nullptr, int w=16, int h=16, bool has_fill = true);
    QLabel *label();

private:
    QHBoxLayout *_layout;
    QLabel *_label;
    QLabel *_icon;
    QWidget *_fill;
    QWidget *_set_widget;


public slots:
    void setText(QString text);
    void setIcon(QString image_src, int w=16, int h=16);
    void setTextInteractionFlags(Qt::TextInteractionFlag flag);
    void addWidget(QWidget *widget = nullptr);

signals:
    void iconChanged();
};

#endif // ICONLABEL_H
