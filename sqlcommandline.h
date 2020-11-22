#ifndef SQLCOMMANDLINE_H
#define SQLCOMMANDLINE_H

#include "sqleditor.h"

class SQLCommandLine : public QWidget
{
    Q_OBJECT
private:
    SQLEditor *_editor;

public:
    SQLCommandLine(SQLEditor *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

};

#endif // SQLCOMMANDLINE_H
