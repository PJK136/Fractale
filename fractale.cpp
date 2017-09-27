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

#include "fractale.h"
#include <iostream>

Fractale::Fractale()
{
    m_limite_etape = 0;
    m_mode_infini = false;
    m_limite_cote = 50000;
    init();
}

void Fractale::init()
{
    m_etape_offset = 0;
    m_cotes = Cote::getPolygone();
    emitBoundingRect();
}

void Fractale::clear()
{
    m_cotes.clear();
}

void Fractale::resetOffset()
{
    m_etape_offset = 0;
}

void Fractale::setLimiteEtape(int limite)
{
    if (m_limite_etape != limite)
    {
        m_limite_etape = limite;
        emit fractaleChanged();
    }
    else
        m_limite_etape = limite;
}

void Fractale::setModeInfini(bool mode)
{
    m_mode_infini = mode;
    emit fractaleChanged();
}

void Fractale::setLimiteCote(int limite)
{
    m_limite_cote = limite;
    if (limite < 0)
        m_etape_offset = 0;
    emit fractaleChanged();
}

unsigned short Fractale::getEtape(double zoom)
{
    if (zoom < 0)
        return 0;

    if (!Cote::alterner_poly_cote && !Cote::alterner_division)
        return !Cote::fixer_division ?  ceil(log(zoom)/log(Cote::division)) : ceil(log(zoom)/log(Cote::division/((static_cast<double>(Cote::division-Cote::nombre_de_poly_cote)/(Cote::nombre_de_poly_cote+1)))));
    else
        return !Cote::fixer_division ?  ceil(2*(log(zoom)/log(Cote::division*Cote::division_alterne))) : ceil(2*(log(zoom)/log(
                                    (Cote::division/(static_cast<double>(Cote::division-Cote::nombre_de_poly_cote)/(Cote::nombre_de_poly_cote+1)))
                                    *(Cote::division_alterne/((static_cast<double>(Cote::division_alterne-Cote::nombre_de_poly_cote_alterne)/(Cote::nombre_de_poly_cote_alterne+1)))))));
}

int Fractale::getCote(const QRectF &cadre, double zoom, std::vector<std::forward_list<QLineF> > &lignes, bool toutes_les_etapes)
{
    if (cadre.isEmpty())
        return 0;

    unsigned short etape = getEtape(zoom);
    if (!m_mode_infini && etape > m_limite_etape)
        etape = m_limite_etape;

    if (m_limite_cote >= 0)
    {
        if ((int)etape - m_etape_offset < 0)
            m_etape_offset = etape;

        int nb_cote = 0;
        do
        {
            nb_cote = 0;
            std::vector<std::forward_list<QLineF> > a;
            lignes.swap(a);
            for (Cote &cote : m_cotes)
                cote.getCote(etape - m_etape_offset, cadre, lignes, toutes_les_etapes, &nb_cote, m_limite_cote);

            if (nb_cote > m_limite_cote)
            {
                if (nb_cote > m_limite_cote && etape - m_etape_offset == 0)
                    break;
                else
                    m_etape_offset++;
            }
        } while (nb_cote > m_limite_cote);

        if(m_mode_infini)
            emit etapeChanged(etape - m_etape_offset);

        return nb_cote;
    }
    else
    {
        for (Cote &cote : m_cotes)
            cote.getCote(etape, cadre, lignes, toutes_les_etapes);

        if(m_mode_infini)
            emit etapeChanged(etape);

        return -1;
    }
}

void Fractale::getBoundingPaths(const QRectF &cadre, double zoom, QPainterPath &path)
{
    if (cadre.isEmpty())
        return;

    unsigned short etape = getEtape(zoom);
    if (!m_mode_infini && etape > m_limite_etape)
        etape = m_limite_etape;

    if (m_limite_cote >= 0)
    {
        if ((int)etape - m_etape_offset < 0)
            m_etape_offset = etape;

        for (Cote &cote : m_cotes)
            cote.getBoundingPaths(etape - m_etape_offset, cadre, path);
    }
    else
    {
        for (Cote &cote : m_cotes)
            cote.getBoundingPaths(etape, cadre, path);
    }
}

int Fractale::getPath(const QRectF &cadre, double zoom, QPainterPath &path)
{
    if (cadre.isEmpty())
        return 0;

    unsigned short etape = getEtape(zoom);
    if (!m_mode_infini && etape > m_limite_etape)
        etape = m_limite_etape;

    if (m_limite_cote >= 0)
    {
        if ((int)etape - m_etape_offset < 0)
            m_etape_offset = etape;

        int nb_cote = 0;
        do
        {
            nb_cote = 0;
            path = QPainterPath();

            for (Cote &cote : m_cotes)
                cote.getPath(etape - m_etape_offset, cadre, path, &nb_cote, m_limite_cote);

            if (nb_cote > m_limite_cote)
            {
                if (nb_cote > m_limite_cote && etape - m_etape_offset == 0)
                    break;
                else
                    m_etape_offset++;
            }
        } while (nb_cote > m_limite_cote);

        if(m_mode_infini)
            emit etapeChanged(etape - m_etape_offset);

        return nb_cote;
    }
    else
    {
        for (Cote &cote : m_cotes)
            cote.getPath(etape, cadre, path);

        if(m_mode_infini)
            emit etapeChanged(etape);

        return -1;
    }
}

int Fractale::getPolygonPaths(const QRectF &cadre, double zoom, std::vector<QPainterPath> &paths)
{
    if (cadre.isEmpty())
        return 0;

    unsigned short etape = getEtape(zoom);
    if (!m_mode_infini && etape > m_limite_etape)
        etape = m_limite_etape;

    if (m_limite_cote >= 0)
    {
        if ((int)etape - m_etape_offset < 0)
            m_etape_offset = etape;

        int nb_cote = 0;
        do
        {
            nb_cote = 0;
            /*Clean vector*/
            std::vector<QPainterPath> a;
            paths.swap(a);
            paths.push_back(QPainterPath());

            for (Cote &cote : m_cotes)
            {
                if (paths[0].isEmpty())
                    paths[0].moveTo(cote.m_segment.p1());

                paths[0].lineTo(cote.m_segment.p2());
            }

            for (Cote &cote : m_cotes)
                cote.getPolygonPaths(etape - m_etape_offset, cadre, paths, &nb_cote, m_limite_cote);

            if (nb_cote > m_limite_cote)
            {
                if (nb_cote > m_limite_cote && etape - m_etape_offset == 0)
                    break;
                else
                    m_etape_offset++;
            }
        } while (nb_cote > m_limite_cote);

        if(m_mode_infini)
            emit etapeChanged(etape - m_etape_offset);

        return nb_cote;
    }
    else
    {
        for (Cote &cote : m_cotes)
            cote.getPolygonPaths(etape, cadre, paths);

        if(m_mode_infini)
            emit etapeChanged(etape);

        return -1;
    }
}

void Fractale::emitBoundingRect()
{
    QPainterPath boundingPath;
    boundingPath.setFillRule(Qt::WindingFill);
    for (Cote &cote : m_cotes)
        boundingPath.addPath(cote.m_bounding_path);
    emit boundingRectChanged(boundingPath.boundingRect());
}

void Fractale::setNombreDeCote(int nombre_de_cote)
{
    if (nombre_de_cote < 3)
        return;

    clear();
    Cote::setNombreDeCote(nombre_de_cote);
    init();
}

void Fractale::setAlternerNbCote(bool alterner_nb_cote)
{
    if (Cote::alterner_nb_cote == alterner_nb_cote)
        return;

    clear();
    Cote::setAlternerNbCote(alterner_nb_cote);
    init();
}

void Fractale::setNombreDeCoteAlterne(int nombre_de_cote_alterne)
{
    if (!Cote::alterner_nb_cote || nombre_de_cote_alterne < 3 || Cote::nombre_de_cote_alterne == static_cast<unsigned int>(nombre_de_cote_alterne))
        return;

    clear();
    Cote::setNombreDeCoteAlterne(nombre_de_cote_alterne);
    init();
}

void Fractale::setNombreDePolyCote(int nombre_de_poly_cote)
{
    if (nombre_de_poly_cote < 1)
        return;

    clear();
    Cote::setNombreDePolyCote(nombre_de_poly_cote);
    emit limitDivisionChanged(1+Cote::nombre_de_poly_cote*2);
    init();
}

void Fractale::setAlternerPolyCote(bool alterner_poly_cote)
{
    if (Cote::alterner_poly_cote == alterner_poly_cote)
        return;

    clear();
    Cote::setAlternerPolyCote(alterner_poly_cote);
    emit limitDivisionAlterneChanged(1+Cote::nombre_de_poly_cote_alterne*2);
    init();
}

void Fractale::setNombreDePolyAlterne(int nombre_de_poly_cote_alterne)
{
    if (!Cote::alterner_poly_cote || nombre_de_poly_cote_alterne < 1 || Cote::nombre_de_poly_cote_alterne == static_cast<unsigned int>(nombre_de_poly_cote_alterne))
        return;

    clear();
    Cote::setNombreDePolyCoteAlterne(nombre_de_poly_cote_alterne);
    emit limitDivisionAlterneChanged(1+Cote::nombre_de_poly_cote_alterne*2);
    init();
}

void Fractale::setFixerDivision(bool fixer)
{
    if (Cote::fixer_division == fixer)
        return;

    clear();
    Cote::setFixerDivision(fixer);
    if (fixer && !Cote::alterner_division)
        emit limitDivisionAlterneChanged(Cote::division);
    if(!fixer)
    {
        emit limitDivisionChanged(1+Cote::nombre_de_poly_cote*2);
        emit limitDivisionAlterneChanged(1+Cote::nombre_de_poly_cote_alterne*2);
    }
    init();
}

void Fractale::setDivision(int division)
{
    if(!Cote::fixer_division || Cote::division == division || division % 2 == 0 || division < 1+2*Cote::nombre_de_poly_cote)
        return;

    clear();
    Cote::setDivision(division);
    if (!Cote::alterner_division)
        emit limitDivisionAlterneChanged(Cote::division);
    init();
}

void Fractale::setAlternerDivision(bool alterner)
{
    if (Cote::alterner_division == alterner)
        return;
    clear();
    Cote::setAlternerDivision(alterner);
    emit limitDivisionAlterneChanged(alterner ? 1+2*Cote::nombre_de_poly_cote_alterne : Cote::division);
    init();
}

void Fractale::setDivisionAlterne(int division)
{
    if(!Cote::fixer_division || !Cote::alterner_division || Cote::division_alterne == division || division % 2 == 0 || division < 1+2*Cote::nombre_de_poly_cote_alterne)
        return;

    clear();
    Cote::setDivisionAlterne(division);
    init();
}
