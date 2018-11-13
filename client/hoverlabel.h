#ifndef HOVERWIDGET_H
#define HOVERWIDGET_H

#include <QLabel>

class HoverLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HoverLabel(QColor _hoverColor, QString _hoverText, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent*e);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    //background color when mouse is over the widget
    QColor hoverColor;

    //text shown when mouse is hovering over the widget
    QString hoverText;

    //whether mouse is over the widget
    bool isHovered = false;
};

#endif // HOVERWIDGET_H
