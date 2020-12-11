#include "sqleditor.h"

SQLEditor::SQLEditor(Table *table, QWidget *parent) : QTextEdit(parent)
{
    this->setAcceptRichText(false);
    this->preserved_model = new QStandardItemModel(this);
    this->last_text = "";
    this->char_typed = NULL;
    this->last_char_typed = NULL;
    this->using_table_columns_model = false;
    this->using_function_model = false;
    this->using_sql_command_model = false;
    this->menu_visible = false;
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
    connect(static_cast<PopupList*>(c->popup()), &PopupList::visibilityChanged, this, &SQLEditor::popupStatus);
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
    if(this->using_sql_command_model || this->using_function_model)
    {
        tc.deletePreviousChar();
        tc.deletePreviousChar();
    }
    tc.insertText(completion.right(extra));

    setTextCursor(tc);
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
    QStringList sql_commands;
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
    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()+{}|:\"<>,?.;/'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    this->char_typed = e->text().right(1).toStdString()[0];

    // Bring up complete command list.

    if(this->char_typed == '?' && this->last_char_typed == '?' && !this->using_sql_command_model)
    {
        model = new QStandardItemModel(this->c);

        sql_commands = SQLProperties::READ_COMMANDS;
        sql_commands.append(SQLProperties::WRITE_COMMANDS);
        sql_commands.sort(Qt::CaseInsensitive);

        model->setRowCount(sql_commands.length());
        model->setColumnCount(1);

        for(int i = 0; i < sql_commands.length(); i++)
        {
            QStandardItem* item = new QStandardItem(sql_commands.at(i));
            item->setIcon(QIcon(QIcon(Icons::SQL_ICON)).pixmap(16,16));
            //item->setToolTip("some tool tip");
            model->setItem(i, 0, item);
        }
        c->setModel(model);
        this->using_sql_command_model = true;
    }


    // Bring up complete function list.
    else if(this->char_typed == '/' && this->last_char_typed == '.' && !this->using_function_model)
    {
        model = new QStandardItemModel(this->c);

        sql_commands = SQLProperties::FUNCTIONS;
        sql_commands.sort(Qt::CaseInsensitive);

        model->setRowCount(sql_commands.length());
        model->setColumnCount(1);

        for(int i = 0; i < sql_commands.length(); i++)
        {
            QStandardItem* item = new QStandardItem(sql_commands.at(i));
            item->setIcon(QIcon(QIcon(Icons::FUNC_ICON)).pixmap(16,16));
            //item->setToolTip("some tool tip");
            model->setItem(i, 0, item);
        }
        c->setModel(model);
        this->using_function_model = true;
    }
    else if(this->char_typed == '.' && !this->using_table_columns_model)
    {
        QTextCursor tc = this->textCursor();
        tc.movePosition(QTextCursor::Left);
        tc.movePosition(QTextCursor::Left);
        tc.select(QTextCursor::WordUnderCursor);
        this->last_text = tc.selectedText();

        if(this->_table->tableNames().contains(this->last_text))
        {
            model = new QStandardItemModel(this->c);
            column_names = this->_table->columnNames(this->last_text);
            column_names.sort(Qt::CaseInsensitive);
            model->setRowCount(column_names.length());
            model->setColumnCount(1);

            for(int i = 0; i < column_names.length(); i++)
            {
                QStandardItem* item = new QStandardItem(column_names.at(i));
                item->setIcon(QIcon(QIcon(Icons::COLUMN_ICON)).pixmap(16,16));
                //item->setToolTip("some tool tip");
                model->setItem(i, 0, item);
            }
            c->setModel(model);
            this->using_table_columns_model = true;
        }
    }
    else if(this->isUsingOtherModel())
        this->c->setModel(this->thawModel());

    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
    {
        if(!charTypedWhileUsingItsModel())
        {
            this->last_char_typed = this->char_typed;
            c->popup()->hide();
            return;
        }
    }
    this->last_char_typed = this->char_typed;

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
    return (this->using_table_columns_model || this->using_sql_command_model || this->using_function_model);
}

bool SQLEditor::charTypedWhileUsingItsModel()
{
    if(this->isUsingOtherModel())
    {
        if(this->last_char_typed == '?' && this->char_typed == '?' && this->using_sql_command_model)
            return true;
        if(this->last_char_typed == '.' && this->char_typed == '/' && this->using_function_model)
            return true;
        if(this->char_typed == '.' && this->using_table_columns_model)
            return true;
    }
    return false;
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
    QString text;
    QStandardItemModel *model = new QStandardItemModel(this->c);
    for(int row = 0; row < this->preserved_model->rowCount(); row++)
    {
        model->appendRow(this->preserved_model->item(row)->clone());
        text = this->preserved_model->item(row)->text();
    }
    this->using_table_columns_model = false;
    this->using_sql_command_model = false;
    this->using_function_model = false;
    return model;
}

void SQLEditor::popupStatus(bool visible)
{
   if(visible)
   {
       this->menu_visible = visible;
   }
   else
   {
       this->menu_visible = visible;
       this->menu_clicked = false;
   }
}
