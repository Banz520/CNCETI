#ifndef BARRA_ALERTA_H
#define BARRA_ALERTA_H

#include "barra.h"
#include "constantes.h"
class BarraAlerta : public Barra { 
   
    public:
        BarraAlerta(MCUFRIEND_kbv &display,const char* mensaje) : Barra(display,0,0,mensaje,COLOR_ROJO,COLOR_BLANCO){}
};

#endif