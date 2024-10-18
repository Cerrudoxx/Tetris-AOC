/***************************************************************************
 *   Copyright (C) 2022 by pilar                                           *
 *   pilarb@unex.es                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef PRACAOC_H
#define PRACAOC_H

#include <ui_mainForm.h>
#include <QtCore>
#include <QtWidgets/QLabel>
#include <QPainter>
#include <QtWidgets/QFileDialog>
#include <QMouseEvent>
#include <iostream>
#include "tetris.h"
#include <QtWidgets/QColorDialog>


#define SQUARESIZE 35
#define SQUAREMARGIN 5
#define CONTAINERW 16
#define CONTAINERH 21
#define NFIGURES 7

typedef char FigureType[4][4];

using namespace Ui;

class pracAOC:public QWidget
{
    Q_OBJECT

public:
    pracAOC();
    ~pracAOC();
		
		

private:

		QMainForm * mainWin;

        QTimer timer;

        char container[CONTAINERH*CONTAINERW];

        QColor colors[(NFIGURES+1)*2];

        FigureType figures[NFIGURES][4];

        int fIndex, nfIndex, giro;

        bool newFigure;

        int xFig, yFig;

        int giro_seleccionado=0;
        int x_seleccionada=2, y_seleccionada=0;
        QColor color_selarea = Qt::gray;

        int points;

        bool start;
        bool pause;

        int nFigures, nNewFigures;
        char newRow[CONTAINERW];

        void assignColors();
        void createFigures();
        void generateRow();

public slots:

		void paintEvent(QPaintEvent *);
        void play();
        void mueveDerecha();
        void mueveIzquierda();
        void giraPieza();
        void comenzar_parar();
        void pausar_continuar();
        void cambiar_tiempo_caida(double);
        void set_modo_pruebas(bool);
        void prueba_vaciar();
        void prueba_borrar_filas();
        void prueba_annadir_fila();
        void prueba_insertar_figura();
        void prueba_borrar_figura();
        void prueba_hueco_libre();
        void prueba_selecciona_anterior();
        void prueba_selecciona_siguiente();
protected:
    void mousePressEvent(QMouseEvent *);


};




#endif
