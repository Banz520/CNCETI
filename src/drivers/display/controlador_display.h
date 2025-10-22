#ifndef CONTROLADOR_DISPLAY_H
#define CONTROLADOR_DISPLAY_H

#include <Adafruit_GFX.h>
#include "MCUFRIEND_kbv.h"

class Display{
    public:    
    MCUFRIEND_kbv tft;

    void iniciar();
};


#endif