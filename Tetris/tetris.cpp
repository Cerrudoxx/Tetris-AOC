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

#include "tetris.h"

//#define WITH_SSE

void tetris::vaciaContenedor(char * container, int w, int h)
{
#ifndef WITH_SSE
	asm volatile(

        "mov %0, %%rsi;"
        "mov %1, %%eax;"
        "mov %%eax, %%ebx;"
        "sub $2, %%ebx;"
        "mov $0, %%rcx;"
        "mov %2, %%ecx;"
        "BVaciarF:"
            "mov $0, %%edi;"
            "BVaciarC:"
                "cmp $2, %%edi;"
                "jl sigVaciar;"
                "cmp %%ebx, %%edi;"
                "jge sigVaciar;"
                "movb $0, (%%rsi);"
            "sigVaciar:"
                "inc %%rsi;"
                "inc %%edi;"
                "cmp %%eax, %%edi;"
                "jl BVaciarC;"
            "loop BVaciarF;"

        :
        : "m" (container), "m" (w), "m" (h)
        : "%rax", "%rbx", "%rcx", "%rsi", "%rdi", "memory"

    );
#else
    asm volatile(
        // Por simplificación, asumimos w=16.
        // Dicho valor es el utilizado como anchura en la definición de "container"

        "mov %0, %%rsi;"
        "mov $0, %%rcx;"
        "mov %2, %%ecx;"

        "pxor %%xmm0, %%xmm0;"
        "mov $0xFFFF0000, %%eax;"
        "pinsrd $3, %%eax, %%xmm0;"
        "mov $0x0000FFFF, %%eax;"
        "pinsrd $0, %%eax, %%xmm0;"

        "BVaciarF:"
            "movdqu (%%rsi), %%xmm1;" 
            "pand %%xmm0, %%xmm1;"
            "movdqu %%xmm1, (%%rsi);" 

            "add $16, %%rsi;"
            "loop BVaciarF;"
        :
        : "m" (container), "m" (w), "m" (h)
        : "%rax","%rcx","%rsi","%xmm0","%xmm1","memory"

    );

#endif


}


void tetris::borraPieza(char * container, int w, char figure[4][4], int c, int f)
{

#ifndef WITH_SSE
    asm volatile(
        ";"

        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );
#else
    asm volatile(
        ";"

        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );


#endif



}

bool tetris::huecoLibre(char * container, int w, char figure[4][4], int c, int f)
{
    bool hayHueco = true;

#ifndef WITH_SSE
    asm volatile(
        ";"


        : "=m" (hayHueco)
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );
#else
    asm volatile(
        ";"

        : "=m" (hayHueco)
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );


#endif

    return hayHueco;
}

void tetris::insertaPieza(char * container, int w, char figure[4][4], int c, int f)
{

#ifndef WITH_SSE
    asm volatile(
        ";"

        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );

#else

    asm volatile(
        ";"

        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );

#endif


}

int tetris::borraFilas(char * container, int w, int h)
{
    int nFilas = 0;

    asm volatile(
        ";"

        : "=m" (nFilas)
        : "m" (container), "m" (w), "m" (h)
        : "memory"

    );

    return nFilas;
}

void tetris::annadeFila(char * container, int w, int h, char * row)
{
    asm volatile(
        ";"

        :
        : "m" (container), "m" (w), "m" (h), "m" (row)
        : "memory"

    );

}
