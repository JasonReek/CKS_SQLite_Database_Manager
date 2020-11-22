#include "sqlcommandline.h"

SQLCommandLine::SQLCommandLine(SQLEditor *editor) : QWidget(editor)
{
    this->_editor = editor;
}

QSize SQLCommandLine::sizeHint() const
{
    return QSize(this->_editor->lineNumberAreaWidth(), 0);
}

void SQLCommandLine::paintEvent(QPaintEvent *event)
{
    this->_editor->lineNumberAreaPaintEvent(event);
}
