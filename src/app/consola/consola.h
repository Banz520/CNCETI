#ifndef CONSOLA_H
#define CONSOLA_H

#include "menu_inicio.h"
#include "MCUFRIEND_kbv.h"
#include "controlador_sd.h"
#include "lista.h"
#include "constantes.h"
#include "gestor_widgets.h"

class Consola {
private:
    MCUFRIEND_kbv miDisplay;
    Lista miLista; // Única instancia de Lista
    MenuInicio miMenuInicio;
    GestorWidgets miGestorWidgets;
    ControladorSD miControladorSD;

public:
    Consola();
    void iniciar();
    void bucleDeEjecucion();

    #if MODO_DESARROLLADOR
    void pruebaLecturaSD();
    #endif
};

#endif