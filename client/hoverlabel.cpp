#include "hoverlabel.h"
#include <QPainter>

HoverLabel::HoverLabel(QColor _hoverColor, QString _hoverText, QWidget *parent)
    :QLabel(parent)
{
    hoverColor = _hoverColor;
    hoverText = _hoverText;
    setCursor(Qt::PointingHandCursor);
}

void HoverLabel::paintEvent(QPaintEvent* e)
{
    QPainter p(this);

    //if mouse is over the widget
    if (isHovered)
    {
        //draw hover background
        p.setBrush(hoverColor);
        p.drawRect(0, 0, width(), height());

        //draw hover text
        p.drawText(0, 0, width(), height(), Qt::AlignCenter, hoverText);
    }
    else
        QLabel::paintEvent(e);
}

void HoverLabel::enterEvent(QEvent*)
{
    isHovered = true;
    update();
}

void HoverLabel::leaveEvent(QEvent*)
{
    isHovered = false;
    update();
}
