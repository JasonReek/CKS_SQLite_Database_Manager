#include "popuplist.h"

PopupList::PopupList(QWidget *parent) : QListView(parent)
{

}

void PopupList::hideEvent(QHideEvent *event)
{
    emit this->visibilityChanged(false);
}

void PopupList::showEvent(QShowEvent *event)
{
    emit this->visibilityChanged(true);
}
