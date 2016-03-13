#pragma once

#include <QProxyStyle>
#include <QStyle>
#include <QStyleOption>

class PlaylistViewStyle: public QProxyStyle {
public:
    PlaylistViewStyle(QStyle* style = 0) :QProxyStyle(style)
    {}

    void drawPrimitive ( PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const{
        if (element == QStyle::PE_IndicatorItemViewItemDrop && !option->rect.isNull()){
            QStyleOption opt(*option);
            opt.rect.setLeft(0);
            if (widget) opt.rect.setRight(widget->width());
            QProxyStyle::drawPrimitive(element, &opt, painter, widget);
            return;
        }
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};
