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

#include "fractalewidget.h"
#include "fractale.h"
#include <QPainter>
#include <QPicture>
#include <QWheelEvent>
#include <QTimer>
#include <QColorDialog>
#include <algorithm>

FractaleWidget::FractaleWidget(QWidget *parent) :
    QWidget(parent), m_pos_mouse(0,0), m_timer_adjust(this), m_pen(QColor(0,0,0), 0), m_brush(QColor(255,223,0))
{
    m_fractale = nullptr;
    m_zoom = 350;
    m_cadre.setSize(size()/m_zoom);
    m_cadre.moveBottomLeft(QPointF(0,0));
    m_offset = QPointF(0,0);
    m_clicked = false;
    m_couleur = 0;
    m_afficher_toutes_les_etapes = false;
    m_antialiase = true;
    m_debug_mode = false;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    connect(&m_timer_adjust, SIGNAL(timeout()), this, SLOT(adjustFractale()));
    updateZoom(350);
    setCursor(Qt::OpenHandCursor);
}

FractaleWidget::~FractaleWidget()
{

}

void FractaleWidget::setFractale(Fractale *fractale)
{
    m_fractale = fractale;
    m_fractale->emitBoundingRect();
}

Qt::GlobalColor FractaleWidget::getColor(unsigned int etape)
{
    switch(etape%7)
    {
    case 0:
        return Qt::black;
        break;
    case 1:
        return Qt::red;
        break;
    case 2:
        return Qt::green;
        break;
    case 3:
        return Qt::blue;
        break;
    case 4:
        return Qt::magenta;
        break;
    case 5:
        return Qt::yellow;
        break;
    case 6:
        return Qt::cyan;
        break;
    default:
        return Qt::black;
        break;
    }
}

void FractaleWidget::drawBoundingPath(QPainter &painter, int nb_cote)
{
    QPainterPath path;
    m_fractale->getBoundingPaths(m_cadre, m_zoom, path);
    painter.setPen(QPen(Qt::red, 0));
    painter.setBrush(Qt::yellow);
    path.setFillRule(Qt::WindingFill);
    painter.drawPath(path);
    QString text("Nombre de cote : ");
    text.push_back(QString::number(nb_cote));
    painter.drawText(QRectF(0.0, 0.0, 0+20/m_zoom, 0+20/m_zoom), text);
}

void FractaleWidget::paintEvent(QPaintEvent *)
{
    if (!m_fractale)
        return;

    QPainter painter(this);
    QTransform transformation;
    transformation.scale(m_zoom, m_zoom);
    QPointF offset = -m_cadre.topLeft()+m_offset;
    transformation.translate(offset.x(), offset.y());
    painter.setWorldTransform(transformation);
    painter.setRenderHint(QPainter::Antialiasing, m_antialiase);

    if (m_couleur != 2)
    {
        std::vector<std::forward_list<QLineF> > lignes;
        if (m_debug_mode)
            drawBoundingPath(painter, m_fractale->getCote(m_cadre, m_zoom, lignes, m_afficher_toutes_les_etapes));
        else
            m_fractale->getCote(m_cadre, m_zoom, lignes, m_afficher_toutes_les_etapes);
        for (unsigned int i = 0; i < lignes.size(); i++)
        {
            if (m_couleur)
                painter.setPen(QPen(getColor(i), 0));
            else
                painter.setPen(m_pen);
            for (QLineF &ligne : lignes[i])
                painter.drawLine(ligne);
        }
    }
    else if (!m_afficher_toutes_les_etapes)
    {
        QPainterPath path;

        if (m_debug_mode)
            drawBoundingPath(painter, m_fractale->getPath(m_cadre, m_zoom, path));
        else
            m_fractale->getPath(m_cadre, m_zoom, path);
        path.setFillRule(Qt::WindingFill);
        painter.setPen(m_pen);
        painter.setBrush(m_brush);
        painter.drawPath(path);
    }
    else
    {
        std::vector<QPainterPath> paths;
        if (m_debug_mode)
            drawBoundingPath(painter, m_fractale->getPolygonPaths(m_cadre, m_zoom, paths));
        else
            m_fractale->getPolygonPaths(m_cadre, m_zoom, paths);
        painter.setPen(m_pen);
        for (unsigned int i = 0; i < paths.size(); i++)
        {
            paths[i].setFillRule(Qt::WindingFill);
            painter.setBrush(getColor(i));
            painter.drawPath(paths[i]);
        }
    }

    if (m_need_adjust_rendu)
        m_timer_adjust.start(500);
}

QImage FractaleWidget::getFractaleImage()
{
    QImage image(size(), QImage::Format_RGB32);
    render(&image);
    return image;
}

void FractaleWidget::adjustFractale()
{
    if (m_need_adjust_rendu)
    {
        m_need_adjust_rendu = false;
        m_fractale->resetOffset();
        update();
    }
}

void FractaleWidget::setCouleur(int couleur)
{
    m_couleur = couleur;
    update();
}

void FractaleWidget::askColorPen()
{
    QColor color(QColorDialog::getColor(m_pen.color(), this));
    if (color.isValid())
        m_pen.setColor(color);
    update();
}

void FractaleWidget::askColorBrush()
{
    QColor color(QColorDialog::getColor(m_brush.color(), this));
    if (color.isValid())
        m_brush.setColor(color);
    update();
}

void FractaleWidget::setAfficherToutesLesEtapes(bool afficher)
{
    m_afficher_toutes_les_etapes = afficher;
    update();
}

void FractaleWidget::setAntialiase(bool antialiase)
{
    m_antialiase = antialiase;
    update();
}

void FractaleWidget::setDebugMode(bool debug)
{
    m_debug_mode = debug;
    update();
}

void FractaleWidget::updatePosX(int x)
{
    if (!m_on_update_interval && m_cadre.left() != x/m_zoom)
    {
        m_cadre.moveLeft(x/m_zoom);
        adjustPosCadre();
        update();
    }
}

void FractaleWidget::updatePosY(int y)
{
    if (!m_on_update_interval && m_cadre.top() != y/m_zoom)
    {
        m_cadre.moveTop(y/m_zoom);
        adjustPosCadre();
        update();
    }
}

void FractaleWidget::emitPosCadreScrollBar()
{
    emit posXChanged(m_cadre.x()*m_zoom);
    emit posYChanged(m_cadre.y()*m_zoom);
}

void FractaleWidget::updateBoundingRect(const QRectF &bounding_rect)
{
    m_boundingRect = bounding_rect;
    m_cadre.moveCenter(m_boundingRect.center());
    updateInterval();
    update();
}

void FractaleWidget::updateInterval()
{
    m_on_update_interval = true;
    m_max = QPointF(m_boundingRect.right() - m_cadre.width(), m_boundingRect.bottom() - m_cadre.height());

    if (m_max.x() < m_boundingRect.left())
        m_max.setX(m_boundingRect.left());

    if (m_max.y() < m_boundingRect.top())
        m_max.setY(m_boundingRect.top());

    if (m_cadre.width() > m_boundingRect.width())
    {
        m_cadre.moveLeft(m_boundingRect.left());
        m_offset.setX((m_cadre.width()-m_boundingRect.width())/2);
    }
    else
        m_offset.setX(0);

    if (m_cadre.height() > m_boundingRect.height())
    {
        m_cadre.moveTop(m_boundingRect.top());
        m_offset.setY((m_cadre.height()-m_boundingRect.height())/2);
    }
    else
        m_offset.setY(0);

    adjustPosCadre();

    if (m_boundingRect.left()*m_zoom > INT_MAX || m_boundingRect.left()*m_zoom < INT_MIN
            || m_max.x()*m_zoom > INT_MAX || m_max.x()*m_zoom < INT_MIN)
        emit intervalXChanged(false, 0, 0, 0);
    else
        emit intervalXChanged(true, static_cast<int>(m_boundingRect.left()*m_zoom), static_cast<int>(m_max.x()*m_zoom), static_cast<int>(size().width()/10));

    if (m_boundingRect.top()*m_zoom > INT_MAX || m_boundingRect.top()*m_zoom < INT_MIN
            || m_max.y()*m_zoom > INT_MAX || m_max.y()*m_zoom < INT_MIN)
        emit intervalYChanged(false, 0,0,0);
    else
        emit intervalYChanged(true, static_cast<int>(m_boundingRect.top()*m_zoom), static_cast<int>(m_max.y()*m_zoom), static_cast<int>(size().height()/10));

    emitPosCadreScrollBar();
    m_on_update_interval = false;
}

void FractaleWidget::adjustPosCadre()
{
    if (m_cadre.left() > m_max.x())
        m_cadre.moveLeft(m_max.x());
    else if (m_cadre.left() < m_boundingRect.left())
        m_cadre.moveLeft(m_boundingRect.left());

    if (m_cadre.top() > m_max.y())
        m_cadre.moveTop(m_max.y());
    else if (m_cadre.top() < m_boundingRect.top())
        m_cadre.moveTop(m_boundingRect.top());
}

void FractaleWidget::updateZoom(double newZoom)
{
    m_need_adjust_rendu = true;

    if (newZoom == m_zoom)
        return;

    if (newZoom < 1)
        newZoom = 1;
    else if (newZoom > 1000000000000000)
        newZoom = 1000000000000000;

    if (!m_pos_mouse.isNull() && m_offset.isNull())
    {
        QPointF posSourisScene = m_pos_mouse/m_zoom + m_cadre.topLeft();
        QPointF vecteurSourisCadreScene = m_pos_mouse/m_zoom; // + m_cadre.topLeft() - m_cadre.topLeft()
        double facteurZoom = newZoom/m_zoom;
        m_cadre.setTopLeft(posSourisScene + -vecteurSourisCadreScene/facteurZoom);
    }

    m_zoom = newZoom;
    m_cadre.setSize(static_cast<QSizeF>(size())/m_zoom);
    updateInterval();
    update();
}


void FractaleWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    double numSteps = numDegrees / 15.0;
    m_pos_mouse = event->pos();
    emit zoomChanged(m_zoom * pow(1.25, numSteps));
}

void FractaleWidget::resizeEvent(QResizeEvent *)
{
    m_need_adjust_rendu = true;

    if (m_pos_mouse.isNull())
    {
        m_cadre.setSize(static_cast<QSizeF>(size())/m_zoom);
        m_fractale->emitBoundingRect();
    }
    else
    {
        m_cadre.setSize(static_cast<QSizeF>(size())/m_zoom);
        updateInterval();
        update();
    }
}

void FractaleWidget::mousePressEvent(QMouseEvent *event)
{
    m_clicked = true;
    m_pos_mouse = event->pos();
    setCursor(Qt::ClosedHandCursor);
}

void FractaleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_pos_mouse = event->pos();
    if (event->button() == Qt::LeftButton)
        emit zoomChanged(m_zoom * 2.5);
    else if (event->button() == Qt::RightButton)
        emit zoomChanged(m_zoom / 2.5);
}

void FractaleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_clicked)
        return;

    m_need_adjust_rendu = true;
    QPointF vecteurSouris = m_pos_mouse - event->pos();
    if (!m_offset.x())
        m_cadre.moveLeft(m_cadre.left() + vecteurSouris.x()/m_zoom);
    if (!m_offset.y())
        m_cadre.moveTop(m_cadre.top() + vecteurSouris.y()/m_zoom);
    m_pos_mouse = event->pos();
    adjustPosCadre();
    emitPosCadreScrollBar();
    update();
}

void FractaleWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_clicked = false;
    setCursor(Qt::OpenHandCursor);
}
