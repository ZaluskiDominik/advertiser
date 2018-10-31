#include "columnlistwidget.h"

ColumnListWidget::ColumnListWidget(QWidget *parent)
    :QWidget(parent)
{
    initLayout();
}

void ColumnListWidget::addColumn(const QString &columnName)
{
    auto* column = new ColumnListWidgetColumn(columnName, this);
    columns.push_back(column);
    splitter->addWidget(column);
}

void ColumnListWidget::addRow(const std::vector<QString> &fields)
{
    for (unsigned int i = 0 ; i < fields.size() ; i++)
    {
        //add field to column
        columns[i]->addField(fields[i]);
        //connect to field's click signal
        QObject::connect(columns[i]->fields.back(), SIGNAL(clicked(int)), this, SLOT(onRowClicked(int)));
        //propagate further edit and remove signals
        QObject::connect(columns[i]->fields.back(), SIGNAL(edit()), this, SIGNAL(editClicked()));
        QObject::connect(columns[i]->fields.back(), SIGNAL(remove()), this, SIGNAL(removeClicked()));
    }
}

QString ColumnListWidget::getFieldValue(unsigned row, unsigned col)
{
    return columns[col]->fields[row]->text();
}

void ColumnListWidget::setFieldValue(unsigned row, unsigned col, const QString& value)
{
    columns[col]->fields[row]->setText(value);
}

void ColumnListWidget::removeCurrentRow()
{
    //remove in each column field with index equal to currentIndex
    for (auto i = columns.begin() ; i != columns.end() ; i++)
        (*i)->removeField(currentRow);

    //set current row to unselected state
    currentRow = -1;
}

void ColumnListWidget::setCurrentRow(int row)
{
    for (auto i = columns.begin() ; i != columns.end() ; i++)
    {
        //if a row is higlighted remove that higlighting
        if (currentRow != -1)
            (*i)->fields[static_cast<unsigned>(currentRow)]->setMarked(false);

        //set higlighting to the new curent row
        (*i)->fields[static_cast<unsigned>(row)]->setMarked();
    }

    //change current row index
    currentRow = row;
}

int ColumnListWidget::getCurrentRow()
{
    return currentRow;
}

void ColumnListWidget::initLayout()
{
    layout = new QVBoxLayout(this);
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    splitter = new QSplitter(this);
    splitter->setHandleWidth(1);
    splitter->setChildrenCollapsible(false);
    layout->addWidget(splitter);
}

void ColumnListWidget::onRowClicked(int row)
{
    setCurrentRow(row);
}
