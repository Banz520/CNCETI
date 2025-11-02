#ifndef PANTALLA_EJECUCION_H
#define PANTALLA_EJECUCION_H

#include "MCUFRIEND_kbv.h"
#include "gestor_widgets.h"
#include "lista.h"
#include "constantes.h"

class PantallaEjecucion {
private:
    MCUFRIEND_kbv &display;
    GestorWidgets &miGestorWidgets;
    const ConfigWidget config_eje_x;
    const ConfigWidget config_eje_y;
    const ConfigWidget config_eje_z;
    const ConfigWidget cuadro_gcode;
    const ConfigWidget cuadro_cortadora;

    const uint16_t color_texto_valores;
    
public: 
    PantallaEjecucion(MCUFRIEND_kbv &display_ref, GestorWidgets &gestor_ref);
    void mostrar();
    void actualizarDatos(const float &origen_x, const float &posicion_x, const float &destino_x,
                        const float &origen_y, const float &posicion_y, const float &destino_y,
                        const float &origen_z, const float &posicion_z, const float &destino_z, const char* comando_gcode);
};

#endif