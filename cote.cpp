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

#include "cote.h"

#define PI 3.14159265358979323846264338327950288419716939937511
unsigned int Cote::nombre_de_cote = 3;
unsigned int Cote::nombre_de_cote_alterne = 3;
bool Cote::alterner_nb_cote = false;
unsigned int Cote::nombre_de_poly_cote = 1;
unsigned int Cote::nombre_de_poly_cote_alterne = 1;
bool Cote::alterner_poly_cote = false;
unsigned int Cote::division = 1+Cote::nombre_de_poly_cote*2;
unsigned int Cote::division_alterne = 1+Cote::nombre_de_poly_cote*2;
bool Cote::fixer_division = false;
bool Cote::alterner_division = false;
double Cote::angle = (Cote::nombre_de_cote-2)*PI/Cote::nombre_de_cote;
double Cote::angle_alterne = (Cote::nombre_de_cote_alterne-2)*PI/Cote::nombre_de_cote_alterne;
QTransform Cote::rotation_anti_horaire = QTransform().rotateRadians(-Cote::angle);
QTransform Cote::rotation_anti_horaire_alterne = QTransform().rotateRadians(-Cote::angle_alterne);
QTransform Cote::rotation_horaire = QTransform().rotateRadians(PI-Cote::angle);
QTransform Cote::rotation_horaire_alterne = QTransform().rotateRadians(PI-Cote::angle_alterne);
double Cote::rayon = 1/(2*sin(PI/Cote::nombre_de_cote));
double Cote::rayon_alterne = 1/(2*sin(PI/Cote::nombre_de_cote_alterne));
double Cote::apotheme = 1/(2*tan(PI/Cote::nombre_de_cote));
double Cote::apotheme_alterne = 1/(2*tan(PI/Cote::nombre_de_cote_alterne));
double Cote::hauteur = Cote::nombre_de_cote % 2 ? Cote::rayon + Cote::apotheme : 2*Cote::apotheme;
double Cote::hauteur_alterne = Cote::nombre_de_cote_alterne % 2 ? Cote::rayon_alterne + Cote::apotheme_alterne : 2*Cote::apotheme_alterne;

Cote::Cote(QLineF cote, unsigned short etape)
{
    m_segment = cote;
    m_etape_parent = etape;
    m_etape_generation = etape;
    calculateBoundingPath();
}

Cote::Cote(QLineF cote, unsigned short etape_parent, unsigned short etape_generation)
{
    m_segment = cote;
    m_etape_parent = etape_parent;
    m_etape_generation = etape_generation;
    calculateBoundingPath();
}

void Cote::effacer()
{
    if (!m_sous_cotes.empty())
        m_sous_cotes.clear();
}

void Cote::calculateBoundingPath()
{
    QPointF point1, point2, point3, point4;
    point1 = m_segment.p1();
    point4 = m_segment.p2();

    double division_actuelle = getDivision(m_etape_generation);
    double hauteur_actuelle = !alterner_nb_cote ? hauteur/division_actuelle : ((hauteur+hauteur_alterne)/2)/division_actuelle;
    double division = static_cast<double>(this->division);
    double division_alterne = static_cast<double>(this->division_alterne);
    double limite = !alterner_poly_cote ? 1/(division-1) : (division_alterne+1)/(division*division_alterne-1);
    double limite_sans_premiere_etape = !alterner_poly_cote ? (1/(division-1))-1/division : ((division_alterne+1)/(division*division_alterne-1))-1/division;

    QPointF vecteur(m_segment.dx(), m_segment.dy());
    double longueur = 1/division_actuelle;
    double rayon_horizontal = (rayon/getDivision(m_etape_generation+1))+hauteur_actuelle*limite_sans_premiere_etape;
    /*Rayon/div = Expansion sur le coté latérale de la prochaine étape + Hauteur maximale des prochaines étapes
     *(La hauteur du prochain polygone sera perpendiculaire au coté d'origine)*/

    if (rayon_horizontal > longueur/2)
    {
        QPointF centre((m_segment.p1() + m_segment.p2())/2);
        point1 = centre + (vecteur/longueur)*(-rayon_horizontal);
        point4 = centre + (vecteur/longueur)*rayon_horizontal;
        vecteur = point4 - point1;
        longueur = rayon_horizontal*2;
    }

    point2 = point1 + (calculerVecteurOrthogonal(vecteur, false)/longueur)*hauteur_actuelle*limite;
    point3 = point2 + vecteur;
    point1 += (calculerVecteurOrthogonal(vecteur, true)/longueur)*hauteur_actuelle*limite_sans_premiere_etape;
    point4 = point1 + vecteur;
    m_bounding_path.moveTo(point1);
    m_bounding_path.lineTo(point2);
    m_bounding_path.lineTo(point3);
    m_bounding_path.lineTo(point4);
    m_bounding_path.closeSubpath();
}

std::list<Cote> Cote::getPolygone()
{
    std::list<Cote> polygone;

    QPointF from(0,0);
    QPointF vecteur = rotation_anti_horaire.map(QPointF(1,0));
    QPointF to = from + vecteur;

    polygone.push_back(Cote(QLineF(from, to), 0));
    from = to;

    for (unsigned int i = 0; i < nombre_de_cote-1; i++)
    {
        vecteur = rotation_horaire.map(vecteur);
        to += vecteur;
        polygone.push_back(Cote(QLineF(from, to), 0));
        from = to;
    }

    return polygone;
}

bool Cote::estDansCadre(const QRectF &cadre)
{
    if (cadre.contains(m_segment.p1()) && cadre.contains(m_segment.p2()))
        return true;
    else if (m_bounding_path.intersects(cadre))
        return true;
    else
        return false;
}

void Cote::getCote(unsigned short etape, const QRectF &cadre, std::vector<std::forward_list<QLineF> > &lignes, bool toutes_les_etapes, int *nb_cote, const int limite)
{
    if (nb_cote && limite >= 0 && *nb_cote > limite)
        return;

    while (lignes.size() <= m_etape_parent)
        lignes.push_back(std::forward_list<QLineF>());

    if (!estDansCadre(cadre))
        effacer();
    else
    {
        if (toutes_les_etapes && m_etape_parent == m_etape_generation)
        {
            lignes[m_etape_parent].push_front(m_segment);
            if (nb_cote)
                (*nb_cote)++;
        }

        if (etape == m_etape_generation)
        {
            if (!toutes_les_etapes)
            {
                lignes[m_etape_parent].push_front(m_segment);
                if (nb_cote)
                    (*nb_cote)++;
            }

            if (!m_sous_cotes.empty())
            {
                for (Cote &cote : m_sous_cotes)
                    cote.effacer();
            }
        }
        else
        {
            calculer(etape);
            for (Cote &sous_cote : m_sous_cotes)
                sous_cote.getCote(etape, cadre, lignes, toutes_les_etapes, nb_cote, limite);
        }
    }
}

void Cote::getBoundingPaths(unsigned short etape, const QRectF &cadre, QPainterPath &path)
{
    if (!estDansCadre(cadre))
        return;

    if (m_etape_parent == m_etape_generation)
    {
        path.addPath(m_bounding_path);
    }

    if (etape != m_etape_generation)
    {
        calculer(etape);
        for (Cote &sous_cote : m_sous_cotes)
            sous_cote.getBoundingPaths(etape, cadre, path);
    }
}

void Cote::getPath(unsigned short etape, const QRectF &cadre, QPainterPath &path, int *nb_cote, const int limite)
{
    if (nb_cote && limite >= 0 && *nb_cote > limite)
        return;

    if (path.isEmpty())
        path.moveTo(m_segment.p1());

    if (!estDansCadre(cadre))
    {
        path.lineTo(m_segment.p2());
        if (nb_cote)
            (*nb_cote)++;
        effacer();
    }
    else if (etape == m_etape_generation)
    {
        path.lineTo(m_segment.p2());
        if (nb_cote)
            (*nb_cote)++;
        if (!m_sous_cotes.empty())
        {
            for (Cote &cote : m_sous_cotes)
                cote.effacer();
        }
    }
    else
    {
        calculer(etape);
        for (Cote &sous_cote : m_sous_cotes)
            sous_cote.getPath(etape, cadre, path, nb_cote, limite);
    }
}

void Cote::getPolygonPaths(unsigned short etape, const QRectF &cadre, std::vector<QPainterPath> &paths, int *nb_cote, const int limite)
{
    if (nb_cote && limite >= 0 && *nb_cote > limite)
        return;

    while (paths.size() <= m_etape_generation+1u)
        paths.push_back(QPainterPath());

    if (!estDansCadre(cadre))
        effacer();
    else
    {
        if (etape == m_etape_generation)
        {
            if (!m_sous_cotes.empty())
            {
                for (Cote &cote : m_sous_cotes)
                    cote.effacer();
            }
        }
        else
        {
            calculer(etape);

            for (Cote &sous_cote : m_sous_cotes)
            {
                if (sous_cote.m_etape_parent < m_etape_parent+1)
                {
                    paths[m_etape_generation+1].closeSubpath();
                    paths[m_etape_generation+1].moveTo(sous_cote.m_segment.p2());
                    continue;
                }

                paths[m_etape_generation+1].lineTo(sous_cote.m_segment.p2());

                if (nb_cote)
                    (*nb_cote)++;
            }

            for (Cote &sous_cote : m_sous_cotes)
                sous_cote.getPolygonPaths(etape, cadre, paths, nb_cote, limite);
        }
    }
}

void Cote::ajouterSousCote(QPointF &from, const QPointF &vecteur, bool sameParent)
{
    QPointF to = from + vecteur;
    if (!sameParent)
        m_sous_cotes.push_back(Cote(QLineF(from, to), m_etape_generation + 1));
    else
        m_sous_cotes.push_back(Cote(QLineF(from, to), m_etape_parent, m_etape_generation + 1));
    from = to;
}

void Cote::calculer(unsigned short etape)
{
    if (etape == m_etape_generation)
        return;

    if (m_sous_cotes.empty())
    {
        unsigned int nb_cote = nombre_de_cote;
        QTransform *rotation_anti_horaire = &Cote::rotation_anti_horaire;
        QTransform *rotation_horaire = &Cote::rotation_horaire;
        unsigned int nb_poly_cote = nombre_de_poly_cote;
        double div_poly = division;
        if ((m_etape_generation+1)%2 == 0)
        {
            if (alterner_poly_cote || (fixer_division && alterner_division))
                div_poly = division_alterne;

            if (alterner_poly_cote)
                nb_poly_cote = nombre_de_poly_cote_alterne;

            if (alterner_nb_cote)
            {
                nb_cote = nombre_de_cote_alterne;
                rotation_anti_horaire = &rotation_anti_horaire_alterne;
                rotation_horaire = &rotation_horaire_alterne;
            }
        }

        double div_cote = !fixer_division ? div_poly : div_poly/((div_poly-nb_poly_cote)/(nb_poly_cote+1));

        QPointF vecteur_cote(m_segment.dx()/div_cote, m_segment.dy()/div_cote);
        QPointF vecteur_poly(m_segment.dx()/div_poly, m_segment.dy()/div_poly);

        QPointF from = m_segment.p1();

        ajouterSousCote(from, vecteur_cote, true);

        if (nb_cote != 4)
        {
            for (unsigned int j = 0; j < nb_poly_cote; j++)
            {
                QPointF vecteur = rotation_anti_horaire->map(vecteur_poly);
                ajouterSousCote(from, vecteur);
                for (unsigned int k = 0; k < nb_cote-2; k++)
                {
                    vecteur = rotation_horaire->map(vecteur);
                    ajouterSousCote(from, vecteur);
                }
                ajouterSousCote(from, vecteur_cote, true);
            }
        }
        else /* Optimisation pour le carré */
        {
            for (unsigned int j = 0; j < nb_poly_cote; j++)
            {
                QPointF vecteur = calculerVecteurOrthogonal(vecteur_poly, false);
                ajouterSousCote(from, vecteur);
                for (unsigned int k = 0; k < nb_cote-2; k++)
                {
                    vecteur = calculerVecteurOrthogonal(vecteur, true);
                    ajouterSousCote(from, vecteur);
                }
                ajouterSousCote(from, vecteur_cote, true);
            }
        }
    }
}

void Cote::setNombreDeCote(unsigned int nb_cote)
{
    if (nb_cote < 3)
        return;

    nombre_de_cote = nb_cote;
    angle = (nombre_de_cote-2)*PI/nombre_de_cote;
    rotation_anti_horaire.reset();
    rotation_anti_horaire.rotateRadians(-angle);
    rotation_horaire.reset();
    rotation_horaire.rotateRadians(PI-angle);
    rayon = 1/(2*sin(PI/nombre_de_cote));
    apotheme = 1/(2*tan(PI/nombre_de_cote));
    hauteur = nombre_de_cote % 2 ? rayon + apotheme : 2*apotheme;
}

void Cote::setAlternerNbCote(bool alterner)
{
    alterner_nb_cote = alterner;
}

void Cote::setNombreDeCoteAlterne(unsigned int nb_cote_alterne)
{
    if(nb_cote_alterne < 3)
        return;

    nombre_de_cote_alterne = nb_cote_alterne;
    angle_alterne = (nombre_de_cote_alterne-2)*PI/nombre_de_cote_alterne;
    rotation_anti_horaire_alterne.reset();
    rotation_anti_horaire_alterne.rotateRadians(-angle_alterne);
    rotation_horaire_alterne.reset();
    rotation_horaire_alterne.rotateRadians(PI-angle_alterne);
    rayon_alterne = 1/(2*sin(PI/nombre_de_cote_alterne));
    apotheme_alterne = 1/(2*tan(PI/nombre_de_cote_alterne));
    hauteur_alterne = nombre_de_cote_alterne % 2 ? rayon_alterne + apotheme_alterne : 2*apotheme_alterne;
}

void Cote::setNombreDePolyCote(unsigned int nb_poly_cote)
{
    if (nb_poly_cote < 1)
        return;

    nombre_de_poly_cote = nb_poly_cote;
    if (!fixer_division || 1+nombre_de_poly_cote*2 > division)
        division = 1+nombre_de_poly_cote*2;
}

void Cote::setAlternerPolyCote(bool alterner)
{
    alterner_poly_cote = alterner;
    if (!fixer_division || !alterner_division || 1+nombre_de_poly_cote_alterne*2 > division_alterne)
        division_alterne = 1+nombre_de_poly_cote_alterne*2;
}

void Cote::setNombreDePolyCoteAlterne(unsigned int nb_poly_cote_alterne)
{
    if(nb_poly_cote_alterne < 1)
        return;

    nombre_de_poly_cote_alterne = nb_poly_cote_alterne;
    if (!fixer_division || 1+nombre_de_poly_cote_alterne*2 > division_alterne)
        division_alterne = 1+nombre_de_poly_cote_alterne*2;
}

void Cote::setFixerDivision(bool fixer)
{
    fixer_division = fixer;
    if (fixer)
        division_alterne = division >= 1+2*nombre_de_poly_cote_alterne ? division : 1+2*nombre_de_poly_cote_alterne;
    else
    {
        division = 1+2*nombre_de_poly_cote;
        division_alterne = 1+2*nombre_de_poly_cote_alterne;
    }
}

void Cote::setDivision(unsigned int div)
{
    if (div % 2 == 0 || div < 1+2*nombre_de_poly_cote)
        return;

   division = div;
   if (!alterner_division && alterner_poly_cote && division > 1+2*nombre_de_poly_cote_alterne)
        division_alterne = div;
}

void Cote::setAlternerDivision(bool alterner)
{
    alterner_division = alterner;
    if (!alterner && fixer_division && division > 1+2*nombre_de_poly_cote_alterne)
            division_alterne = division;
    else if (!alterner)
        division_alterne = 1+2*nombre_de_poly_cote_alterne;
}

void Cote::setDivisionAlterne(unsigned int div)
{
    if (div % 2 == 0 || (!alterner_poly_cote && div < 1+2*nombre_de_poly_cote) || (alterner_poly_cote && div < 1+2*nombre_de_poly_cote_alterne))
        return;

    division_alterne = div;
}
