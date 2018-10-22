#ifndef COLUMNLISTWIDGET_H
#define COLUMNLISTWIDGET_H

#include <QSplitter>
#include "columnlistwidgetcolumn.h"

class ColumnListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColumnListWidget(QWidget *parent = nullptr);

    //adds a new column
    void addColumn(const QString& columnName);

    //adds values to each column
    void addRow(const std::vector<QString>& fields);

    //return value of given field
    QString getFieldValue(unsigned row, unsigned col);

    //sets value to give field
    void setFieldValue(unsigned row, unsigned col, const QString &value);

    //removes current row
    void removeCurrentRow();

    //changes active row
    void setCurrentRow(int row);

    //returns current row index
    int getCurrentRow();

private:
    QSplitter* splitter;
    QVBoxLayout* layout;

    //which column is higlighted as active
    int currentRow = -1;

    //list of all columns
    std::vector<ColumnListWidgetColumn*> columns;

    //create layout and splitter between each column
    void initLayout();

signals:
    //edit in context menu was clicked
    void editClicked();
    //remove in context menu was clicked
    void removeClicked();

private slots:
    //row was left/right clicked
    void onRowClicked(int row);

};

#endif // COLUMNLISTWIDGET_H
