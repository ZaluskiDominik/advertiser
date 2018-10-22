#include "columnlistwidgetcolumn.h"

ColumnListWidgetColumn::ColumnListWidgetColumn(const QString &columnName, QWidget *parent)
    :QWidget(parent)
{
    initColumn(columnName);
    initSpaceFiller();
}

void ColumnListWidgetColumn::addField(const QString &fieldName)
{
    layout->removeWidget(spaceFiller);

    auto* field = new ColumnListWidgetField( static_cast<int>(fields.size()), fieldName, this );
    layout->addWidget(field);
    fields.push_back(field);

    layout->addWidget(spaceFiller);
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
