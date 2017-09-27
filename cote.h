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

#ifndef COTE_H
#define COTE_H

#include <QLineF>
#include <QPainterPath>
#include <QRectF>
#include <QTransform>
#include <forward_list>
#include <cmath>

class Fractale;

class Cote
{
public:
    Cote(QLineF cote, unsigned short etape);
    Cote(QLineF cote, unsigned short etape_parent, unsigned short etape_generation);
    void getCote(unsigned short etape, const QRectF &cadre, std::vector<std::forward_list<QLineF> > &lignes, bool toutes_les_etapes, int *nb_cote = nullptr, const int limite = -1);
    void getBoundingPaths(unsigned short etape, const QRectF &cadre, QPainterPath &path);
    void getPath(unsigned short etape, const QRectF &cadre, QPainterPath &path, int *nb_cote = nullptr, const int limite = -1);
    void getPolygonPaths(unsigned short etape, const QRectF &cadre, std::vector<QPainterPath> &paths, int *nb_cote = nullptr, const int limite = -1);
    static inline QPointF calculerVecteurOrthogonal(const QPointF &vecteur, bool clockwise);
    static std::list<Cote> getPolygone();
    static void setNombreDeCote(unsigned int nb_cote);
    static void setAlternerNbCote(bool alterner);
    static void setNombreDeCoteAlterne(unsigned int nb_cote_alterne);
    static void setNombreDePolyCote(unsigned int nb_poly_cote);
    static void setAlternerPolyCote(bool alterner);
    static void setNombreDePolyCoteAlterne(unsigned int nb_poly_cote_alterne);
    static void setFixerDivision(bool fixer);
    static void setDivision(unsigned int div);
    static void setAlternerDivision(bool alterner);
    static void setDivisionAlterne(unsigned int div);
    static inline double getDivision(unsigned int etape);
protected:
    QLineF m_segment;
    QPainterPath m_bounding_path;
    std::list<Cote> m_sous_cotes;
    unsigned short m_etape_parent;
    unsigned short m_etape_generation;

    inline void calculateBoundingPath();
    inline void ajouterSousCote(QPointF &from, const QPointF &vecteur, bool sameParent = false);
    void calculer(unsigned short etape);
    inline bool estDansCadre(const QRectF &cadre);
    inline void effacer();

    static unsigned int nombre_de_cote;
    static bool alterner_nb_cote;
    static unsigned int nombre_de_cote_alterne;

    static unsigned int nombre_de_poly_cote;
    static bool alterner_poly_cote;
    static unsigned int nombre_de_poly_cote_alterne;

    static unsigned int division;
    static unsigned int division_alterne;

    static bool fixer_division;
    static bool alterner_division;

    static double angle, angle_alterne;
    static QTransform rotation_anti_horaire, rotation_anti_horaire_alterne;
    static QTransform rotation_horaire, rotation_horaire_alterne;
    static double rayon, rayon_alterne;
    static double apotheme, apotheme_alterne;
    static double hauteur, hauteur_alterne;

    friend class Fractale;
};

QPointF Cote::calculerVecteurOrthogonal(const QPointF &vecteur, bool clockwise)
{
    if (clockwise)
        return QPointF(-vecteur.y(), vecteur.x());
    else
        return QPointF(vecteur.y(), -vecteur.x());
}

double Cote::getDivision(unsigned int etape)
{
    if (!alterner_poly_cote)
        return pow(division, etape);
    else
        return pow(division, ceil((double)etape/2))*pow(division_alterne, floor((double)etape/2));
}

#endif // COTE_H
