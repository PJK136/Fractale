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

#ifndef FMAINWINDOWS_H
#define FMAINWINDOWS_H

#include <QMainWindow>
#include "fractale.h"

namespace Ui {
class FMainWindows;
}

class FMainWindows : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FMainWindows(QWidget *parent = 0);
    ~FMainWindows();
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void setIntervalXScrollBar(bool valid, int minX, int maxX, int stepX);
    void setIntervalYScrollBar(bool valid, int minY, int maxY, int stepY);
    void setMinimumDivision(int min);
    void setMinimumDivisionAlterne(int min);
    void saveFractale();
    void resetEtapeOnChange();
    void modePresentation();
    void aPropos();
private:
    Ui::FMainWindows *ui;
    Fractale m_fractale;
};

#endif // FMAINWINDOWS_H
