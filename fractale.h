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

#ifndef FRACTALE_H
#define FRACTALE_H

#include <QObject>
#include <forward_list>
#include "cote.h"

class Fractale : public QObject
{
    Q_OBJECT
public:
    Fractale();
    unsigned short getEtape(double zoom);
    int getCote(const QRectF &cadre, double zoom, std::vector<std::forward_list<QLineF> > &lignes, bool toutes_les_etapes);
    void getBoundingPaths(const QRectF &cadre, double zoom, QPainterPath &path);
    int getPath(const QRectF &cadre, double zoom, QPainterPath &path);
    int getPolygonPaths(const QRectF &cadre, double zoom, std::vector<QPainterPath> &paths);
    void emitBoundingRect();
    void resetOffset();
signals:
    void fractaleChanged();
    void boundingRectChanged(const QRectF&);
    void limitDivisionChanged(int);
    void limitDivisionAlterneChanged(int);
    void etapeChanged(int);
public slots:
    void setNombreDeCote(int nombre_de_cote);
    void setAlternerNbCote(bool alterner_nb_cote);
    void setNombreDeCoteAlterne(int nombre_de_cote_alterne);
    void setNombreDePolyCote(int nombre_de_poly_cote);
    void setAlternerPolyCote(bool alterner_poly_cote);
    void setNombreDePolyAlterne(int nombre_de_poly_cote_alterne);
    void setFixerDivision(bool fixer);
    void setDivision(int division);
    void setAlternerDivision(bool alterner);
    void setDivisionAlterne(int division);
    void setLimiteEtape(int limite);
    void setModeInfini(bool mode);
    void setLimiteCote(int limite);
protected:
    void init();
    void clear();
    std::list<Cote> m_cotes;
    unsigned int m_limite_etape;
    bool m_mode_infini;
    int m_limite_cote;
    unsigned short m_etape_offset;
};

#endif // FRACTALE_H
