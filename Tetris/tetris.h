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

#ifndef TETRIS_H
#define TETRIS_H

#include <QtCore>

namespace tetris{
    extern void vaciaContenedor(char * container, int w, int h);
    extern void borraPieza(char * container, int w, char figure[4][4], int c, int f);
    extern bool huecoLibre(char * container, int w, char figure[4][4], int c, int f);
    extern void insertaPieza(char * container, int w, char figure[4][4], int c, int f);
    extern int borraFilas(char * container, int w, int h);
    extern void annadeFila(char * container, int w, int h, char * row);

}

#endif
