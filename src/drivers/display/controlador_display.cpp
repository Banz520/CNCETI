#include "controlador_display.h"
#include "constantes.h"


void Display::iniciar(){
    tft.begin(tft.readID());
    tft.setRotation(3);
    tft.fillScreen(COLOR_BLANCO);
}