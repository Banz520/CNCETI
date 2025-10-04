#ifndef BARRA_INFERIOR_H
#define BARRA_INFERIOR_H

#include "barra.h"
#include "constantes.h"

class BarraInferior : public Barra {
    public: 
        BarraInferior(MCUFRIEND_kbv &display,String mensaje) : Barra(display,0,280,mensaje,COLOR_GRIS_OSCURO,COLOR_BLANCO){}

};

#endif