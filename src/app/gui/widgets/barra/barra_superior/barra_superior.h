#ifndef BARRA_SUPERIOR_H
#define BARRA_SUPERIOR_H

#include "barra.h"
#include "constantes.h"

class BarraSuperior : public Barra { 
public:
    BarraSuperior(MCUFRIEND_kbv &display, const __FlashStringHelper* mensaje) 
        : Barra(display, 0, 0, mensaje, COLOR_GRIS_CLARO, COLOR_NEGRO) {}
};

#endif