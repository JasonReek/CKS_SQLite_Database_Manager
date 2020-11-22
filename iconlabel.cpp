#include "iconlabel.h"

IconLabel::IconLabel(QString text, QString image_src, QWidget *parent, int w, int h, bool has_fill) : QWidget(parent)
{
    this->_set_widget = nullptr;

    this->_layout = new QHBoxLayout(this);
    this->_layout->setContentsMargins(0,0,0,0);
    this->_label = new QLabel(text, this);
    this->_icon = new QLabel(this);
    this->_icon->setPixmap(QIcon(image_src).pixmap(w,h));

    this->_layout->addWidget(this->_icon);
    this->_layout->addWidget(this->_label);
    if(has_fill)
        this->_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void IconLabel::setText(QString text)
{
    this->_label->setText(text);
}

void IconLabel::setIcon(QString image_src, int w, int h)
{
    this->_icon->setPixmap(QIcon(image_src).pixmap(w, h));
    emit iconChanged();
}

void IconLabel::setTextInteractionFlags(Qt::TextInteractionFlag flag)
{
    this->_label->setTextInteractionFlags(flag);
}

void IconLabel::addWidget(QWidget *widget)
{
    this->_layout->addWidget(widget);
}

QLabel *IconLabel::label()
{
    return this->_label;
}


