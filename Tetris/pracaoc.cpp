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


#include "pracaoc.h"


pracAOC::pracAOC(): QWidget()
{
	mainWin=new QMainForm();
	mainWin->setupUi(this);


    for(int y=0; y<CONTAINERH; y++)
        for(int x=0; x<CONTAINERW; x++)
            if((CONTAINERH-y)<=4 || x<2 || (CONTAINERW-x)<=2)
                container[y*CONTAINERW+x] = 1;
            else
                container[y*CONTAINERW+x] = 0;


    assignColors();
    createFigures();

    qsrand(QTime::currentTime().msec());

    nfIndex=qrand()%7;
    newFigure = true;
    giro = 0;

    start = false;
    pause = false;

    points = 0;

    mainWin->buttonDerecha->setText(QString());
    mainWin->buttonDerecha->setIconSize(QSize(30,30));
    mainWin->buttonDerecha->setIcon(QIcon("../pracaoc/arrow_right.png"));

    mainWin->buttonIzquierda->setText(QString());
    mainWin->buttonIzquierda->setIconSize(QSize(30,30));
    mainWin->buttonIzquierda->setIcon(QIcon("../pracaoc/arrow_left.png"));

    mainWin->buttonAbajo->setText(QString());
    mainWin->buttonAbajo->setIconSize(QSize(30,30));
    mainWin->buttonAbajo->setIcon(QIcon("../pracaoc/arrow_down.png"));

    mainWin->buttonGirar->setText(QString());
    mainWin->buttonGirar->setIconSize(QSize(30,30));
    mainWin->buttonGirar->setIcon(QIcon("../pracaoc/arrow_turn_right.png"));



    connect(&timer, SIGNAL(timeout()), this, SLOT(play()));
    connect(mainWin->buttonComenzar, SIGNAL(clicked()), this, SLOT(comenzar_parar()));
    connect(mainWin->buttonPausar, SIGNAL(clicked()), this, SLOT(pausar_continuar()));
    connect(mainWin->spinBoxFallTime, SIGNAL(valueChanged(double)), this, SLOT(cambiar_tiempo_caida(double)));
    connect(mainWin->groupBoxPruebas, SIGNAL(clicked(bool)), this, SLOT(set_modo_pruebas(bool)));
    connect(mainWin->buttonVaciar, SIGNAL(clicked()), this, SLOT(prueba_vaciar()));
    connect(mainWin->buttonBorrarFilas, SIGNAL(clicked()), this, SLOT(prueba_borrar_filas()));
    connect(mainWin->buttonAnnadirF, SIGNAL(clicked()), this, SLOT(prueba_annadir_fila()));
    connect(mainWin->buttonInsertarF, SIGNAL(clicked()), this, SLOT(prueba_insertar_figura()));
    connect(mainWin->buttonBorrarF, SIGNAL(clicked()), this, SLOT(prueba_borrar_figura()));
    connect(mainWin->buttonHuecoLibre, SIGNAL(clicked()), this, SLOT(prueba_hueco_libre()));
    connect(mainWin->buttonPrev, SIGNAL(clicked()), this, SLOT(prueba_selecciona_anterior()));
    connect(mainWin->buttonSig, SIGNAL(clicked()), this, SLOT(prueba_selecciona_siguiente()));

    timer.setInterval(mainWin->spinBoxFallTime->value()*1000.);
}


pracAOC::~pracAOC()
{
}


void pracAOC::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

    //Draw container
    int xI, yI, w, h;

    xI = mainWin->containerArea->x();
    yI = mainWin->containerArea->y();
    w = mainWin->containerArea->width();
    h = mainWin->containerArea->height();

    painter.fillRect(xI,yI,w,h,Qt::white);

    if(start || mainWin->groupBoxPruebas->isChecked())
        painter.setOpacity(1.);
    else
        painter.setOpacity(0.2);

    for(int y=0; y<CONTAINERH-4; y++)
        for(int x=2, xP=0; x<CONTAINERW-2; x++, xP++)
        {

            painter.fillRect(xI+xP*SQUARESIZE, yI+y*SQUARESIZE, SQUARESIZE, SQUARESIZE, colors[container[y*CONTAINERW+x]*2]);
            painter.fillRect(xI+xP*SQUARESIZE+SQUAREMARGIN, yI+y*SQUARESIZE+SQUAREMARGIN, SQUARESIZE-SQUAREMARGIN*2, SQUARESIZE-SQUAREMARGIN*2, colors[container[y*CONTAINERW+x]*2+1]);
            if(container[y*CONTAINERW+x]>0)
                painter.drawRect(xI+xP*SQUARESIZE, yI+y*SQUARESIZE, SQUARESIZE, SQUARESIZE);
        }

    if(mainWin->groupBoxPruebas->isChecked())
    {
        int x_selarea = (x_seleccionada-2)*SQUARESIZE;
        int y_selarea = y_seleccionada*SQUARESIZE;
        int w_selarea = SQUARESIZE*4;
        int h_selarea = SQUARESIZE*4;
        if(x_selarea+w_selarea >= mainWin->containerArea->width())
            w_selarea = mainWin->containerArea->width()-x_selarea;
        if(y_selarea+h_selarea >= mainWin->containerArea->height())
            h_selarea = mainWin->containerArea->height()-y_selarea;

        painter.setPen(QPen ( QBrush ( color_selarea ), 2));
        painter.drawRect(xI+x_selarea, yI+y_selarea, w_selarea, h_selarea);
        painter.setPen(QPen ( QBrush ( Qt::black ), 1));

    }


    //Draw next figure
    QPoint p = mainWin->figureArea->pos();
    xI = mainWin->frameFigure->mapToParent(p).x();
    yI = mainWin->frameFigure->mapToParent(p).y();
    w = mainWin->figureArea->width();
    h = mainWin->figureArea->height();

    painter.setOpacity(1.);

    painter.fillRect(xI,yI,w,h,Qt::white);

    int yLimitFig;

    if(mainWin->groupBoxPruebas->isChecked())
    {
        yLimitFig = 4;
    }
    else
    {
        giro_seleccionado = 0;
        yI+=SQUARESIZE;
        yLimitFig = 3;
    }

    for(int y=0; y<yLimitFig; y++)
        for(int x=0; x<4; x++)
        {

            painter.fillRect(xI+x*SQUARESIZE, yI+y*SQUARESIZE, SQUARESIZE, SQUARESIZE, colors[figures[nfIndex][giro_seleccionado][y][x]*2]);
            painter.fillRect(xI+x*SQUARESIZE+SQUAREMARGIN, yI+y*SQUARESIZE+SQUAREMARGIN, SQUARESIZE-SQUAREMARGIN*2, SQUARESIZE-SQUAREMARGIN*2, colors[figures[nfIndex][giro_seleccionado][y][x]*2+1]);
            if(figures[nfIndex][giro_seleccionado][y][x]>0)
                painter.drawRect(xI+x*SQUARESIZE, yI+y*SQUARESIZE, SQUARESIZE, SQUARESIZE);
        }

 	
}

void pracAOC::assignColors()
{
    colors[0]=QColor(255,255,255);
    colors[1]=QColor(255,255,255);
    colors[2]=QColor(0,0,155);
    colors[3]=QColor(0,0,255);
    colors[4]=QColor(0,155,0);
    colors[5]=QColor(0,255,0);
    colors[6]=QColor(155,0,0);
    colors[7]=QColor(255,0,0);
    colors[8]=QColor(0,155,155);
    colors[9]=QColor(0,255,255);
    colors[10]=QColor(155,155,0);
    colors[11]=QColor(255,255,0);
    colors[12]=QColor(155,0,155);
    colors[13]=QColor(255,0,255);
    colors[14]=QColor(255,100,0);
    colors[15]=QColor(255,165,0);



}

void pracAOC::createFigures()
{
    FigureType fig;

    fig[0][0]=0;      fig[0][1]=1;      fig[0][2]=1;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=1;      fig[1][2]=1;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[0][0], fig, 16);
    memcpy(figures[0][1], fig, 16);
    memcpy(figures[0][2], fig, 16);
    memcpy(figures[0][3], fig, 16);

    fig[0][0]=2;      fig[0][1]=2;      fig[0][2]=0;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=2;      fig[1][2]=2;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[1][0], fig, 16);
    memcpy(figures[1][2], fig, 16);

    fig[0][0]=0;      fig[0][1]=0;      fig[0][2]=2;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=2;      fig[1][2]=2;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=2;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[1][1], fig, 16);
    memcpy(figures[1][3], fig, 16);


    fig[0][0]=0;      fig[0][1]=3;      fig[0][2]=3;      fig[0][3]=0;
    fig[1][0]=3;      fig[1][1]=3;      fig[1][2]=0;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[2][0], fig, 16);
    memcpy(figures[2][2], fig, 16);

    fig[0][0]=0;      fig[0][1]=3;      fig[0][2]=0;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=3;      fig[1][2]=3;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=3;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[2][1], fig, 16);
    memcpy(figures[2][3], fig, 16);


    fig[0][0]=4;      fig[0][1]=0;      fig[0][2]=0;      fig[0][3]=0;
    fig[1][0]=4;      fig[1][1]=4;      fig[1][2]=4;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[3][0], fig, 16);


    fig[0][0]=0;      fig[0][1]=4;      fig[0][2]=4;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=4;      fig[1][2]=0;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=4;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[3][1], fig, 16);

    fig[0][0]=4;      fig[0][1]=4;      fig[0][2]=4;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=0;      fig[1][2]=4;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[3][2], fig, 16);

    fig[0][0]=0;      fig[0][1]=0;      fig[0][2]=4;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=0;      fig[1][2]=4;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=4;      fig[2][2]=4;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[3][3], fig, 16);


    fig[0][0]=0;      fig[0][1]=0;      fig[0][2]=5;      fig[0][3]=0;
    fig[1][0]=5;      fig[1][1]=5;      fig[1][2]=5;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[4][0], fig, 16);

    fig[0][0]=0;      fig[0][1]=5;      fig[0][2]=0;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=5;      fig[1][2]=0;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=5;      fig[2][2]=5;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[4][1], fig, 16);

    fig[0][0]=5;      fig[0][1]=5;      fig[0][2]=5;      fig[0][3]=0;
    fig[1][0]=5;      fig[1][1]=0;      fig[1][2]=0;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[4][2], fig, 16);

    fig[0][0]=0;      fig[0][1]=5;      fig[0][2]=5;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=0;      fig[1][2]=5;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=5;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[4][3], fig, 16);

    fig[0][0]=6;      fig[0][1]=6;      fig[0][2]=6;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=6;      fig[1][2]=0;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[5][0], fig, 16);

    fig[0][0]=0;      fig[0][1]=0;      fig[0][2]=6;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=6;      fig[1][2]=6;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=6;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[5][1], fig, 16);

    fig[0][0]=0;      fig[0][1]=6;      fig[0][2]=0;      fig[0][3]=0;
    fig[1][0]=6;      fig[1][1]=6;      fig[1][2]=6;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[5][2], fig, 16);

    fig[0][0]=0;      fig[0][1]=6;      fig[0][2]=0;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=6;      fig[1][2]=6;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=6;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[5][3], fig, 16);

    fig[0][0]=7;      fig[0][1]=7;      fig[0][2]=7;      fig[0][3]=7;
    fig[1][0]=0;      fig[1][1]=0;      fig[1][2]=0;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=0;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=0;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[6][0], fig, 16);
    memcpy(figures[6][2], fig, 16);

    fig[0][0]=0;      fig[0][1]=7;      fig[0][2]=0;      fig[0][3]=0;
    fig[1][0]=0;      fig[1][1]=7;      fig[1][2]=0;      fig[1][3]=0;
    fig[2][0]=0;      fig[2][1]=7;      fig[2][2]=0;      fig[2][3]=0;
    fig[3][0]=0;      fig[3][1]=7;      fig[3][2]=0;      fig[3][3]=0;

    memcpy(figures[6][1], fig, 16);
    memcpy(figures[6][3], fig, 16);


}

void pracAOC::play()
{
    if(newFigure)
    {
        xFig = (CONTAINERW-4)/2;
        yFig = 0;
        giro = 0;

        if(tetris::huecoLibre(container, CONTAINERW, figures[nfIndex][giro], xFig, yFig))
        {
            tetris::insertaPieza(container, CONTAINERW, figures[nfIndex][giro], xFig, yFig);
            newFigure=false;
            fIndex=nfIndex;
            nfIndex=qrand()%7;

        }
        else
            comenzar_parar();
    }
    else
    {
        tetris::borraPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);
        if(tetris::huecoLibre(container, CONTAINERW, figures[fIndex][giro], xFig, yFig+1))
            yFig++;
        else
            newFigure=true;
        tetris::insertaPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);
        if(newFigure)
        {
            nFigures++;
            nNewFigures++;
            int nfilas=tetris::borraFilas(container, CONTAINERW, CONTAINERH-4);
            if(nfilas>0)
                nNewFigures = 0;
            points+=nfilas;
            mainWin->lcdPuntuacion->display(points);
            if(mainWin->gBAddRows->isChecked())
            {
                if(nFigures>=mainWin->spinFromFigs->value() && nNewFigures>=mainWin->spinEveryFigs->value())
                {
                    bool emptyRow = true;
                    for(int c=2; c<CONTAINERW-2; c++)
                        if(container[c]!=0)
                            emptyRow = false;
                    nNewFigures = 0;
                    generateRow();
                    tetris::annadeFila(container, CONTAINERW, CONTAINERH-4, newRow);
                    if(!emptyRow)
                        comenzar_parar();
                }
            }
        }
    }

    update();

}

void pracAOC::mueveDerecha()
{
    if(!newFigure)
    {
        tetris::borraPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);
        if(tetris::huecoLibre(container, CONTAINERW, figures[fIndex][giro], xFig+1, yFig))
            xFig++;
        tetris::insertaPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);

        update();
    }

}

void pracAOC::mueveIzquierda()
{
    if(!newFigure)
    {
        tetris::borraPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);
        if(tetris::huecoLibre(container, CONTAINERW, figures[fIndex][giro], xFig-1, yFig))
            xFig--;
        tetris::insertaPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);

        update();
    }

}


void pracAOC::giraPieza()
{
    if(mainWin->groupBoxPruebas->isChecked())
    {
        giro_seleccionado = (giro_seleccionado+1)%4;
    }
    else
    {
        if(!newFigure)
        {
            tetris::borraPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);
            if(tetris::huecoLibre(container, CONTAINERW, figures[fIndex][(giro+1)%4], xFig, yFig+1))
                giro=(giro+1)%4;
            tetris::insertaPieza(container, CONTAINERW, figures[fIndex][giro], xFig, yFig);


        }
    }

    update();

}

void pracAOC::comenzar_parar()
{
    start = !start;

    if(start)
    {
        mainWin->buttonComenzar->setText(QString("Parar"));

        mainWin->buttonPausar->setText("Pausar");
        pause = false;
        mainWin->buttonPausar->setEnabled(true);

        points = 0;
        nFigures = 0;
        nNewFigures = 0;

        mainWin->lcdPuntuacion->display(points);

        tetris::vaciaContenedor(container, CONTAINERW, CONTAINERH-4);

        newFigure = true;
        giro = 0;

        connect(mainWin->buttonDerecha, SIGNAL(clicked()), this, SLOT(mueveDerecha()));
        connect(mainWin->buttonIzquierda, SIGNAL(clicked()), this, SLOT(mueveIzquierda()));
        connect(mainWin->buttonAbajo, SIGNAL(clicked()), this, SLOT(play()));
        connect(mainWin->buttonGirar, SIGNAL(clicked()), this, SLOT(giraPieza()));

        timer.start();

        update();

    }
    else
    {
        mainWin->buttonComenzar->setText(QString("Comenzar"));

        mainWin->buttonPausar->setEnabled(false);

        disconnect(mainWin->buttonDerecha, SIGNAL(clicked()), this, SLOT(mueveDerecha()));
        disconnect(mainWin->buttonIzquierda, SIGNAL(clicked()), this, SLOT(mueveIzquierda()));
        disconnect(mainWin->buttonAbajo, SIGNAL(clicked()), this, SLOT(play()));
        disconnect(mainWin->buttonGirar, SIGNAL(clicked()), this, SLOT(giraPieza()));

        timer.stop();

        update();
    }

}

void pracAOC::pausar_continuar()
{
    pause = !pause;

    if(pause)
    {
        mainWin->buttonPausar->setText(QString("Continuar"));
        disconnect(mainWin->buttonDerecha, SIGNAL(clicked()), this, SLOT(mueveDerecha()));
        disconnect(mainWin->buttonIzquierda, SIGNAL(clicked()), this, SLOT(mueveIzquierda()));
        disconnect(mainWin->buttonAbajo, SIGNAL(clicked()), this, SLOT(play()));
        disconnect(mainWin->buttonGirar, SIGNAL(clicked()), this, SLOT(giraPieza()));

        timer.stop();
    }
    else
    {
        mainWin->buttonPausar->setText(QString("Pausar"));
        connect(mainWin->buttonDerecha, SIGNAL(clicked()), this, SLOT(mueveDerecha()));
        connect(mainWin->buttonIzquierda, SIGNAL(clicked()), this, SLOT(mueveIzquierda()));
        connect(mainWin->buttonAbajo, SIGNAL(clicked()), this, SLOT(play()));
        connect(mainWin->buttonGirar, SIGNAL(clicked()), this, SLOT(giraPieza()));

        timer.start();
    }


}

void pracAOC::cambiar_tiempo_caida(double)
{
    timer.setInterval(mainWin->spinBoxFallTime->value()*1000.);
}

void pracAOC::generateRow()
{
    int nempty = 0;

    for(int i=0; i<CONTAINERW; i++)
    {
        if(i<2 || (CONTAINERW-i)<=2)
            newRow[i] = 1;
        else
            newRow[i] = qrand()%8;
        if(newRow[i]==0)
            nempty++;
    }

    if(nempty==0)
        newRow[2] = 0;

}

void pracAOC::set_modo_pruebas(bool iniciar)
{
    if(iniciar)
    {
        if(start && !pause)
            pausar_continuar();
        connect(mainWin->buttonGirar, SIGNAL(clicked()), this, SLOT(giraPieza()));
    }
    else
        disconnect(mainWin->buttonGirar, SIGNAL(clicked()), this, SLOT(giraPieza()));
    color_selarea = Qt::gray;
    update();

}

void pracAOC::prueba_vaciar()
{
    tetris::vaciaContenedor(container, CONTAINERW, CONTAINERH-4);
    color_selarea = Qt::gray;
    update();
}

void pracAOC::prueba_borrar_filas()
{
    tetris::borraFilas(container, CONTAINERW, CONTAINERH-4);
    color_selarea = Qt::gray;
    update();
}

void pracAOC::prueba_annadir_fila()
{
    generateRow();
    tetris::annadeFila(container, CONTAINERW, CONTAINERH-4, newRow);
    color_selarea = Qt::gray;
    update();

}

void pracAOC::prueba_insertar_figura()
{
    tetris::insertaPieza(container, CONTAINERW, figures[nfIndex][giro_seleccionado], x_seleccionada, y_seleccionada);
    color_selarea = Qt::gray;
    update();
}

void pracAOC::prueba_borrar_figura()
{
    tetris::borraPieza(container, CONTAINERW, figures[nfIndex][giro_seleccionado], x_seleccionada, y_seleccionada);
    color_selarea = Qt::gray;
    update();
}

void pracAOC::prueba_hueco_libre()
{
    if(tetris::huecoLibre(container, CONTAINERW, figures[nfIndex][giro_seleccionado], x_seleccionada, y_seleccionada))
        color_selarea = Qt::green;
    else
        color_selarea = Qt::red;
    update();
}

void pracAOC::prueba_selecciona_anterior()
{
    nfIndex = (nfIndex+6)%7;
    giro_seleccionado = 0;
    color_selarea = Qt::gray;
    update();

}

void pracAOC::prueba_selecciona_siguiente()
{
    nfIndex = (nfIndex+1)%7;
    giro_seleccionado = 0;
    color_selarea = Qt::gray;
    update();

}

void pracAOC::mousePressEvent ( QMouseEvent *e )
{
    if ( e->button() == Qt::LeftButton )
    {
        QPoint p(e->x(), e->y());
        QPoint pFrame = mainWin->containerArea->mapFromParent(p);

        if(QRect(0,0, mainWin->containerArea->width(), mainWin->containerArea->height()).contains(pFrame))
        {
            x_seleccionada = (int) rint(pFrame.x()/SQUARESIZE) + 2;
            y_seleccionada = (int) rint(pFrame.y()/SQUARESIZE);
            color_selarea = Qt::gray;
            update();
        }
    }
}

