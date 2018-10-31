#include "columnlistwidgetcolumn.h"

ColumnListWidgetColumn::ColumnListWidgetColumn(const QString &columnName, QWidget *parent)
    :QWidget(parent)
{
    initColumn(columnName);
    initSpaceFiller();
}

void ColumnListWidgetColumn::addField(const QString &fieldName)
{
    //remove space filler in order to add a new field to layout
    layout->removeWidget(spaceFiller);

    auto* field = new ColumnListWidgetField( static_cast<int>(fields.size()), fieldName, this );
    layout->addWidget(field);
    fields.push_back(field);

    //restore space filler to layout
    layout->addWidget(spaceFiller);
}

void ColumnListWidgetColumn::removeField(int row)
{
    //delete field
    fields[static_cast<unsigned>(row)]->deleteLater();
    //remove field from fields array
    fields.erase(fields.begin() + row);

    //decrement rowIndexes of all fields that are further in fields array
    for (auto i = fields.begin() + row ; i != fields.end() ; i++)
         (*i)->rowIndex--;
}

void ColumnListWidgetColumn::initColumn(const QString &columnName)
{
    //create verticall layout
    layout = new QVBoxLayout;
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    //create column's header
    headerLabel = new QLabel(columnName);
    headerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    headerLabel->setStyleSheet("border: 1px solid skyblue; background-color: lightblue; padding: 6px; font-size: 15px; font-weight: bold;");
    layout->addWidget(headerLabel);
}

void ColumnListWidgetColumn::initSpaceFiller()
{
    spaceFiller = new QWidget(this);
    spaceFiller->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    layout->addWidget(spaceFiller);
}
