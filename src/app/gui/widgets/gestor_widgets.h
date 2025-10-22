#ifndef GESTOR_WIDGETS_H
#define GESTOR_WIDGETS_H

#include <Arduino.h>
#include "MCUFRIEND_kbv.h"
#include "Adafruit_GFX.h"
#include "Fonts/FreeSans9pt7b.h"

/**
 * @brief Esta esta estructura se utiliza para configurar un widget de barra
 *  
 */
struct ConfigBarra {
    uint16_t pos_x, pos_y, ancho, alto;
    uint16_t color_fondo, color_texto;
    const __FlashStringHelper* mensaje;
};

class GestorWidgets{
private:
    MCUFRIEND_kbv &display;
public:

    GestorWidgets(MCUFRIEND_kbv &display_ref);
    void dibujarBarra(const ConfigBarra& config);
};

#endif