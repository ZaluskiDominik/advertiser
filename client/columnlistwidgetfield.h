#ifndef COLUMNLISTWIDGETFIELD_H
#define COLUMNLISTWIDGETFIELD_H

#include <QLabel>
#include <QAction>

class ColumnListWidgetField : public QLabel
{
    Q_OBJECT
public:
    explicit ColumnListWidgetField(int _rowIndex, const QString& name, QWidget* parent = nullptr);

    //highlight row as currently selected
    void setMarked(bool marked = true);

protected:
    void contextMenuEvent(QContextMenuEvent* e);
    void mousePressEvent(QMouseEvent* e);

signals:
    //remove in context menu was clicked
    void remove();
    //edit in context menu was clicked
    void edit();
    //left or right click
    void clicked(int rowIndex);

private:
    //default style sheet applied to this field
    QString defaultStyleSheet;

    //in which row this field is
    int rowIndex;

    //context menu's actions
    QAction removeAction;
    QAction editAction;

    //initialize context menu's actiona
    void initActions();

    //set default style sheet to this field
    void initDefaultStyleSheet();
};

#endif // COLUMNLISTWIDGETFIELD_H
