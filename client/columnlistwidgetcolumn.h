#ifndef COLUMNLISTWIDGETCOLUMN_H
#define COLUMNLISTWIDGETCOLUMN_H

#include <QWidget>
#include <vector>
#include <QVBoxLayout>
#include "columnlistwidgetfield.h"

class ColumnListWidgetColumn : public QWidget
{
    Q_OBJECT
    friend class ColumnListWidget;

public:
    explicit ColumnListWidgetColumn(const QString& columnName, QWidget* parent = nullptr);

    //adds a field in column
    void addField(const QString& fieldName);

    //removes a field from column
    void removeField(int row);

private:
    //column's header
    QLabel* headerLabel;

    QVBoxLayout* layout;

    //list of all fields in column
    std::vector<ColumnListWidgetField*> fields;

    //dummy widget that will take all available space in layout on bottom
    QWidget* spaceFiller;

    //sets layout and adds column's header
    void initColumn(const QString& columnName);

    void initSpaceFiller();
};

#endif // COLUMNLISTWIDGETCOLUMN_H
