#ifndef POPUPLIST_H
#define POPUPLIST_H

#include <QListView>


class PopupList : public QListView
{
    Q_OBJECT
public:
    explicit PopupList(QWidget *parent = nullptr);

protected:
    virtual void hideEvent(QHideEvent *event);
    virtual void showEvent(QShowEvent *event);

signals:
    void visibilityChanged(bool visibility); // True when visible
};

#endif // POPUPLIST_H
