#include "controlador_display.h"
#include "constantes.h"

Display::Display()
{    
    
}

void Display::iniciar(){
    tft.begin(tft.readID());
    tft.setRotation(3);
    tft.fillScreen(COLOR_BLANCO);
}