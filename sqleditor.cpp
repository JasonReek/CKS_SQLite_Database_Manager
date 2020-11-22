#include "sqleditor.h"

SQLEditor::SQLEditor(Table *table, QWidget *parent) : QTextEdit(parent)
{
    this->setAcceptRichText(false);
    this->preserved_model = new QStandardItemModel(this);
    this->last_text = "";
    this->period_selection = false;
    this->using_other_model = false;
    this->menu_popped_up = false;
    this->menu_clicked = false;

    this->_table = table;
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor("#323232"));
    palette.setColor(QPalette::Text, QColor("#EFEFEF"));
    this->setPalette(palette);

    this->highlighter = new Highlighter(this->document());
}

SQLEditor::~SQLEditor()
{
}

void SQLEditor::rehighlight()
{
    this->highlighter->rehighlight();
}

void SQLEditor::setHlForTablesAndCols(QStringList table_names, QStringList column_names)
{
    this->highlighter->setTableNames(table_names, column_names);
}

void SQLEditor::setCompleter(QCompleter *completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    connect(c, QOverload<const QString &>::of(&QCompleter::activated),this, &SQLEditor::insertCompletion);
    connect(c, QOverload<const QModelIndex &>::of(&QCompleter::highlighted), this, &SQLEditor::checkIfModelChanged);
}

void SQLEditor::insertCompletion(const QString &completion)
{
    this->menu_clicked = true;
    if (this->c->widget() != this)
        return;

    QTextCursor tc = this->textCursor();
    int extra = completion.length(); //- c->completionPrefix().length();

    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.select(QTextCursor::WordUnderCursor);
    tc.removeSelectedText();
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
    if(this->using_other_model)
        this->c->setModel(this->thawModel());
}

QString SQLEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void SQLEditor::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void SQLEditor::keyPressEvent(QKeyEvent *e)
{
    QStringList column_names;
    QStandardItemModel *model = nullptr;

    if (c && c->popup()->isVisible())
    {
        // The following keys are forwarded by the completer to the widget
       switch (e->key())
       {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }
    if(this->menu_popped_up)
    {
        if(this->using_other_model)
            this->c->setModel(this->thawModel());
        this->menu_popped_up = false;
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()+{}|:\".<>?,/;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if(e->text().right(1) == ".")
    {
        QTextCursor tc = this->textCursor();
        tc.movePosition(QTextCursor::Left);
        tc.movePosition(QTextCursor::Left);
        tc.select(QTextCursor::WordUnderCursor);
        this->last_text = tc.selectedText();

        if(this->_table->tableNames().contains(this->last_text))
        {
            // Catches if the user fumbles with text entry. This prevents model overwrite.
            if(this->using_other_model)
                this->c->setModel(this->thawModel());
            else
            {
                this->period_selection = true;
                this->preserveModel(this->c->model());
                model = new QStandardItemModel(this->c);
                column_names = this->_table->columnNames(this->last_text);
                column_names.sort(Qt::CaseInsensitive);
                model->setRowCount(column_names.length());
                model->setColumnCount(1);

                for(int i = 0; i < column_names.length(); i++)
                {
                    QStandardItem* item = new QStandardItem(column_names[i]);
                    item->setIcon(QIcon(QIcon(Icons::COLUMN_ICON)).pixmap(16,16));
                    //item->setToolTip("some tool tip");
                    model->setItem(i, 0, item);
                }
                c->setModel(model);
                this->using_other_model = true;
            }
        }

    }
    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
    {
        if(!this->period_selection)
        {
            c->popup()->hide();
            return;
        }
        this->period_selection = false;
    }

    if (completionPrefix != c->completionPrefix())
    {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}

QCompleter *SQLEditor::completer() const
{
    return c;
}

bool SQLEditor::isUsingOtherModel()
{
    return this->using_other_model;
}

void SQLEditor::preserveModel(QAbstractItemModel *model)
{
    QMap <int, QVariant> item_data;
    this->preserved_model->clear();
    for(int row = 0; row < model->rowCount(); row++)
    {
        item_data = model->itemData(model->index(row, 0));
        this->preserved_model->appendRow(new QStandardItem(item_data[1].value<QIcon>(), item_data[0].toString()));
    }

}

QAbstractItemModel *SQLEditor::thawModel()
{
    QStandardItemModel *model = new QStandardItemModel(this->c);
    for(int row = 0; row < this->preserved_model->rowCount(); row++)
        model->appendRow(this->preserved_model->item(row)->clone());
    this->using_other_model = false;
    return model;
}

void SQLEditor::checkIfModelChanged(const QModelIndex &index)
{
    this->menu_popped_up = true;

}
