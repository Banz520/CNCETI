#include "gestor_widgets.h"

GestorWidgets::GestorWidgets(MCUFRIEND_kbv &display_ref) 
    : display(display_ref) 
{
    // Inicialización si es necesaria
}

void GestorWidgets::dibujarBarra(const ConfigBarra& config){
    display.fillRect(config.pos_x, config.pos_y, config.ancho, config.alto, config.color_fondo);
        if (config.mensaje) {
            display.setCursor(config.pos_x + 12, config.pos_y + 20);
            display.setTextColor(config.color_texto);
            display.setFont(&FreeSans9pt7b);
            display.print(config.mensaje);
        }
}
