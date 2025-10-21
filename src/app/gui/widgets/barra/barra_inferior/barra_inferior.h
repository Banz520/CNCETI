#ifndef BARRA_INFERIOR_H
#define BARRA_INFERIOR_H

#include "barra.h"
#include "constantes.h"

class BarraInferior : public Barra {
public: 
    BarraInferior(MCUFRIEND_kbv &display_ref, const __FlashStringHelper* mensaje) 
        : Barra(display_ref, 0, 280, mensaje, COLOR_GRIS_OSCURO, COLOR_BLANCO) {}
};

#endif