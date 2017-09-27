/* Copyright © 2012-2013 Paul DU
 * This file is part of Fractale.

 * Fractale is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Fractale is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Fractale.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef FRACTALEWIDGET_H
#define FRACTALEWIDGET_H

#include <QWidget>
#include <QRectF>
#include <QTimer>
#include <QPen>
#include <QBrush>

class Fractale;

class FractaleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FractaleWidget(QWidget *parent = 0);
    void setFractale(Fractale *fractale);
    QImage getFractaleImage();
    virtual ~FractaleWidget();
    void emitPosCadreScrollBar();

signals:
    void intervalXChanged(bool valid, int minX, int maxX, int stepX);
    void intervalYChanged(bool valid, int minY, int maxY, int stepY);
    void posXChanged(int x);
    void posYChanged(int y);
    void zoomChanged(double zoom);

public slots:
    void setCouleur(int couleur);
    void askColorPen();
    void askColorBrush();
    void setAfficherToutesLesEtapes(bool afficher);
    void setAntialiase(bool antialiase);
    void updateBoundingRect(const QRectF &bounding_rect);
    void updateInterval();
    void updatePosX(int x); // x = xscene * zoom
    void updatePosY(int y); // y = yscene * zoom
    void updateZoom(double newZoom);
    void adjustFractale();
    void setDebugMode(bool debug);

protected:
    void adjustPosCadre();
    inline Qt::GlobalColor getColor(unsigned int etape);
    inline void drawBoundingPath(QPainter &painter, int nb_cote);
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    Fractale *m_fractale;

    QRectF m_cadre;

    QRectF m_boundingRect;
    QPointF m_max;
    QPointF m_offset;

    double m_zoom;
    QPointF m_pos_mouse;
    bool m_clicked;

    bool m_on_update_interval;

    bool m_need_adjust_rendu;
    QTimer m_timer_adjust;

    unsigned int m_couleur;
    QPen m_pen;
    QBrush m_brush;

    bool m_afficher_toutes_les_etapes;
    bool m_antialiase;
    bool m_debug_mode;
};

#endif // FRACTALEWIDGET_H
