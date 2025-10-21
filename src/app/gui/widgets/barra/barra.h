#ifndef BARRA_H
#define BARRA_H

#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"

class Barra {
private:
    MCUFRIEND_kbv &display; // Referencia, no copia
    static const uint16_t ancho = 480, alto = 40;
    uint16_t posicion_x, posicion_y, color_fondo, color_texto;
    const __FlashStringHelper* mensaje; // Para strings en PROGMEM

public:
    Barra(MCUFRIEND_kbv &display_ref, uint16_t posicion_x, uint16_t posicion_y, 
          const __FlashStringHelper* mensaje, uint16_t color_fondo, uint16_t color_texto);

    void mostrar();
    void set_mensaje(const __FlashStringHelper* nuevo_mensaje);
};

#endif