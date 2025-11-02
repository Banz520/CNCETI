#ifndef CONSOLA_H
#define CONSOLA_H

#include "menu_inicio.h"
#include "MCUFRIEND_kbv.h"
#include "controlador_sd.h"
#include "lista.h"
#include "constantes.h"
#include "gestor_widgets.h"
#include "gestor_archivos.h"
#include "pantalla_ejecucion.h"

class Consola {
private:
    MCUFRIEND_kbv miDisplay;
    Lista miLista; // Única instancia de Lista
    GestorWidgets miGestorWidgets;
    MenuInicio miMenuInicio;
    PantallaEjecucion miPantallaEjecucion;
    //ControladorSD miControladorSD;
    GestorArchivos miGestorArchivos;
    

public:
    ControladorSD miControladorSD;
    Consola(Ch376msc &miHostUsb);
    void iniciar();
    void actualizar(const float &origen_x,const float &posicion_x,const float &destino_x,const float &origen_y,const float &posicion_y,const float &destino_y,const float &origen_z,const float &posicion_z,const float &destino_z, const char* comando_gcode);
    void mostrarPantallaEjecucion();

    void pruebaLecturaSD();
    void pruebaLecturaUSB();
    void pruebaLecturaGcode();
    void pruebaAbrirGcodeSD();
    #if MODO_DESARROLLADOR
    
    #endif
};

#endif