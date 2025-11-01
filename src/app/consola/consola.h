#ifndef CONSOLA_H
#define CONSOLA_H

#include "menu_inicio.h"
#include "MCUFRIEND_kbv.h"
#include "controlador_sd.h"
#include "lista.h"
#include "constantes.h"
#include "gestor_widgets.h"
#include "gestor_archivos.h"

class Consola {
private:
    MCUFRIEND_kbv miDisplay;
    Lista miLista; // Única instancia de Lista
    GestorWidgets miGestorWidgets;
    MenuInicio miMenuInicio;
    //ControladorSD miControladorSD;
    GestorArchivos miGestorArchivos;
    

public:
    ControladorSD miControladorSD;
    Consola(Ch376msc &miHostUsb);
    void iniciar();
    void bucleDeEjecucion();

    #if MODO_DESARROLLADOR
    void pruebaLecturaSD();
    void pruebaLecturaUSB();
    void pruebaLecturaGcode();
    void pruebaAbrirGcodeSD();
    #endif
};

#endif