#include "barra.h"
#include "Fonts/FreeSans9pt7b.h"
#include <avr/pgmspace.h>

Barra::Barra(MCUFRIEND_kbv &display_ref, uint16_t posicion_x, uint16_t posicion_y, 
             const __FlashStringHelper* mensaje, uint16_t color_fondo, uint16_t color_texto) 
    : display(display_ref), 
      posicion_x(posicion_x), 
      posicion_y(posicion_y),
      mensaje(mensaje),
      color_fondo(color_fondo), 
      color_texto(color_texto) 
{
    // Lista de inicialización más eficiente
}

void Barra::mostrar() {
    display.fillRect(posicion_x, posicion_y, ancho, alto, color_fondo);
    display.setCursor(12, posicion_y + 20);
    display.setTextColor(color_texto);
    display.setFont(&FreeSans9pt7b);
    display.print(mensaje); // print maneja FlashStringHelper automáticamente
}

void Barra::set_mensaje(const __FlashStringHelper* nuevo_mensaje) {
    this->mensaje = nuevo_mensaje;
}