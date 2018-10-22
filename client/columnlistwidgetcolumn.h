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

private:
    //column's header
    QLabel* headerLabel;

    QVBoxLayout* layout;

    //list of all fields in column
    std::vector<ColumnListWidgetField*> fields;

    QWidget* spaceFiller;

    //sets layout and adds column's header
    void initColumn(const QString& columnName);

    void initSpaceFiller();
};

#endif // COLUMNLISTWIDGETCOLUMN_H
