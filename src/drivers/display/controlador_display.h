#ifndef CONTROLADOR_DISPLAY_H
#define CONTROLADOR_DISPLAY_H

#include <Adafruit_GFX.h>
#include "MCUFRIEND_kbv.h"

class Display{
    private:  
    
    public:
    MCUFRIEND_kbv tft;
    Display();
    void iniciar();
};


#endif