#include "columnlistwidgetfield.h"
#include <QMenu>
#include <QContextMenuEvent>

ColumnListWidgetField::ColumnListWidgetField(int _rowIndex, const QString &name, QWidget *parent)
    :QLabel(parent), rowIndex(_rowIndex)
{
    setText(name);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(30);
    initActions();
    initDefaultStyleSheet();
}

void ColumnListWidgetField::setMarked(bool marked)
{
    if (marked)
    {
        //highlight field
        setStyleSheet(defaultStyleSheet + " background-color: orange;");
    }
    else
    {
        //unhighlight field
        setStyleSheet(defaultStyleSheet);
    }
}

void ColumnListWidgetField::contextMenuEvent(QContextMenuEvent *e)
{
    //if context menu was invoked by mouse click
    if (e->reason() == QContextMenuEvent::Mouse)
    {
        //show context menu
        QMenu menu(this);
        menu.addAction(&editAction);
        menu.addAction(&removeAction);
        menu.exec(e->globalPos());
    }
}

void ColumnListWidgetField::mousePressEvent(QMouseEvent *e)
{
    //send clicked signal
    if (e->button() == Qt::LeftButton || e->button() == Qt::RightButton)
        clicked(rowIndex);

    QLabel::mousePressEvent(e);
}

void ColumnListWidgetField::initActions()
{
    editAction.setText("Edytuj");
    QObject::connect(&editAction, SIGNAL(triggered()), this, SIGNAL(edit()));
    removeAction.setText("Usuń");
    QObject::connect(&removeAction, SIGNAL(triggered()), this, SIGNAL(remove()));
}

void ColumnListWidgetField::initDefaultStyleSheet()
{
    QString bcg = ( rowIndex % 2 ) ? "#ddd" : "#ccc";
    defaultStyleSheet = "padding: 6px; font-size: 14px; background-color: " + bcg + ";";
    setStyleSheet(defaultStyleSheet);
}
