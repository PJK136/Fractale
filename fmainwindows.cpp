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

#include "fmainwindows.h"
#include "ui_fmainwindows.h"
#include <QEvent>
#include <QScrollBar>
#include <QImage>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>

FMainWindows::FMainWindows(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FMainWindows)
{
    ui->setupUi(this);
    ui->label_alterner_nb_div->setVisible(false);
    ui->alterner_division->setVisible(false);
    ui->division_alterne->setVisible(false);

    ui->xScrollBar->installEventFilter(this);
    ui->yScrollBar->installEventFilter(this);
    ui->displayZoom->setMinimum(1);
    ui->displayZoom->setMaximum(1000000000000000);

    connect(ui->actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(ui->nombre_de_cote, SIGNAL(valueChanged(int)), this, SLOT(resetEtapeOnChange()));
    connect(ui->nombre_de_cote, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setNombreDeCote(int)));

    connect(ui->alterner_nb_cote, SIGNAL(toggled(bool)), this, SLOT(resetEtapeOnChange()));
    connect(ui->alterner_nb_cote, SIGNAL(toggled(bool)), &m_fractale, SLOT(setAlternerNbCote(bool)));

    connect(ui->nombre_de_cote_alterne, SIGNAL(valueChanged(int)), this, SLOT(resetEtapeOnChange()));
    connect(ui->nombre_de_cote_alterne, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setNombreDeCoteAlterne(int)));

    connect(ui->nombre_de_poly_cote, SIGNAL(valueChanged(int)), this, SLOT(resetEtapeOnChange()));
    connect(ui->nombre_de_poly_cote, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setNombreDePolyCote(int)));

    connect(ui->alterner_poly_cote, SIGNAL(toggled(bool)), this, SLOT(resetEtapeOnChange()));
    connect(ui->alterner_poly_cote, SIGNAL(toggled(bool)), &m_fractale, SLOT(setAlternerPolyCote(bool)));

    connect(ui->nombre_de_poly_cote_alterne, SIGNAL(valueChanged(int)), this, SLOT(resetEtapeOnChange()));
    connect(ui->nombre_de_poly_cote_alterne, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setNombreDePolyAlterne(int)));

    connect(ui->fixer_division, SIGNAL(toggled(bool)), this, SLOT(resetEtapeOnChange()));
    connect(ui->fixer_division, SIGNAL(toggled(bool)), &m_fractale, SLOT(setFixerDivision(bool)));

    connect(ui->division, SIGNAL(valueChanged(int)), this, SLOT(resetEtapeOnChange()));
    connect(ui->division, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setDivision(int)));

    connect(ui->alterner_division, SIGNAL(toggled(bool)), this, SLOT(resetEtapeOnChange()));
    connect(ui->alterner_division, SIGNAL(toggled(bool)), &m_fractale, SLOT(setAlternerDivision(bool)));

    connect(ui->division_alterne, SIGNAL(valueChanged(int)), this, SLOT(resetEtapeOnChange()));
    connect(ui->division_alterne, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setDivisionAlterne(int)));

    connect(&m_fractale, SIGNAL(limitDivisionChanged(int)), this, SLOT(setMinimumDivision(int)));
    connect(&m_fractale, SIGNAL(limitDivisionAlterneChanged(int)), this, SLOT(setMinimumDivisionAlterne(int)));

    connect(ui->limite_etape, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setLimiteEtape(int)));
    connect(ui->mode_infini, SIGNAL(toggled(bool)), &m_fractale, SLOT(setModeInfini(bool)));
    connect(&m_fractale, SIGNAL(etapeChanged(int)), ui->limite_etape, SLOT(setValue(int)));
    connect(ui->limite_cote, SIGNAL(valueChanged(int)), &m_fractale, SLOT(setLimiteCote(int)));

    connect(&m_fractale, SIGNAL(boundingRectChanged(QRectF)), ui->rendu, SLOT(updateBoundingRect(QRectF)));
    connect(&m_fractale, SIGNAL(fractaleChanged()), ui->rendu, SLOT(update()));
    ui->rendu->setFractale(&m_fractale);
}

FMainWindows::~FMainWindows()
{
    delete ui;
}

bool FMainWindows::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Wheel &&
       qobject_cast<QScrollBar*>(obj))
    {
        event->ignore();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void FMainWindows::setIntervalXScrollBar(bool valid, int minX, int maxX, int stepX)
{
    ui->xScrollBar->setEnabled(valid);
    ui->xScrollBar->blockSignals(!valid);
    ui->xScrollBar->setMinimum(minX);
    ui->xScrollBar->setMaximum(maxX);
    ui->xScrollBar->setSingleStep(stepX);
    ui->xScrollBar->setPageStep(stepX*2.5);
}

void FMainWindows::setIntervalYScrollBar(bool valid, int minY, int maxY, int stepY)
{
    ui->yScrollBar->setEnabled(valid);
    ui->yScrollBar->blockSignals(!valid);
    ui->yScrollBar->setMinimum(minY);
    ui->yScrollBar->setMaximum(maxY);
    ui->yScrollBar->setSingleStep(stepY);
    ui->yScrollBar->setPageStep(stepY*2.5);
}

void FMainWindows::setMinimumDivision(int min)
{
    ui->division->setMinimum(min);
    if (!ui->fixer_division->isChecked())
        ui->division->setValue(min);
}

void FMainWindows::setMinimumDivisionAlterne(int min)
{
    ui->division_alterne->setMinimum(min);
    if (!ui->alterner_division->isChecked())
        ui->division_alterne->setValue(min);
}

void FMainWindows::saveFractale()
{
    QImage image = ui->rendu->getFractaleImage();
    image.save(QFileDialog::getSaveFileName(this, "Enregistrer une image", QString(), "Images (*.png *.gif *.jpg *.bmp)"));
}

void FMainWindows::resetEtapeOnChange()
{
    if(ui->actionRemiseAZero->isChecked())
    {
       ui->limite_etape->setValue(0);
       ui->mode_infini->setChecked(false);
    }
}

void FMainWindows::modePresentation()
{
    static bool croissant = true;
    if(!ui->actionAutomatique->isChecked())
        return;

    if (ui->mode_infini->isChecked())
        ui->mode_infini->setChecked(false);

    unsigned short max = m_fractale.getEtape(ui->displayZoom->value());

    if (ui->limite_etape->value() <= 0)
    {
        if (ui->limite_etape->value() < 0)
            ui->limite_etape->setValue(0);
        else
            ui->limite_etape->setValue(1);
        croissant = true;
    }
    else if (ui->limite_etape->value() >= max)
    {
        ui->limite_etape->setValue(max-1);
        croissant = false;
    }
    else if (croissant)
        ui->limite_etape->setValue(ui->limite_etape->value()+1);
    else
        ui->limite_etape->setValue(ui->limite_etape->value()-1);

    QTimer::singleShot(1000, this, SLOT(modePresentation()));
}

void FMainWindows::aPropos()
{
    QMessageBox msgBox(this);
    msgBox.setIconPixmap(QPixmap(":/icon/48x48.ico"));
    msgBox.setWindowTitle("À propos de Fractale");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("<h1 align='center'><strong>Fractale 1.5.1</strong></h1>"\
                   "<p align='center'>Fractale est un programme de modélisation 2D de la fractale de Von Koch avec différentes variantes et différents modes de représentation possibles.<br/><br />"\
                   "Conçu dans le cadre de l'atelier Math.En.Jean au Lycée Jean Cocteau<br />de Miramas session 2012-2013.<br /><br />"\
                   "Pour toute suggesstion ou rapport de bug, vous pouvez adresser un mail à <a href='mailto:du.paul@hotmail.fr'>du.paul@hotmail.fr</a></p>"\
                   "<p align='center'>Copyright © 2012-2013 Paul DU<p>"\
                   "<p align='justify'>Ce programme est distribué dans l'espoir qu'il sera utile, mais SANS AUCUNE GARANTIE; pas même la garantie implicite de COMMERCIABILISABILITÉ ni d'ADÉQUATION à UN OBJECTIF PARTICULIER.</p><p align='center'>Consultez la <em>GNU General Public License</em> pour plus de détails.</p>");
    msgBox.exec();
}
