#include "barra.h"
#include "Fonts/FreeSans12pt7b.h"

Barra::Barra(MCUFRIEND_kbv &display_ref,uint16_t posicion_x, uint16_t posicion_y,String mensaje,uint16_t color_fondo,uint16_t color_texto) : display(display_ref){
    this->posicion_x = posicion_x;
    this->posicion_y = posicion_y;
    this->mensaje = mensaje;
    this->color_fondo = color_fondo;
    this->color_texto = color_texto;
}

void Barra::mostrar(){
    display.fillRect(posicion_x,posicion_y,ancho,alto,color_fondo);
    display.setCursor(12,posicion_y + 20);
    display.setTextColor(color_texto);
    display.setFont(&FreeSans12pt7b);
    display.print(mensaje);

}