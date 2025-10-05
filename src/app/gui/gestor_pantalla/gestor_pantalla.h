#ifndef GESTOR_PANTALLA_H
#define GESTOR_PANTALLA_H

#include "controlador_display.h"
#include "menu_inicio.h"

class GestorPantalla{
private:
    Display miDisplay;
    MenuInicio miMenuInicio;
    
public:
    
    GestorPantalla();

    void iniciarPantalla();

    void cargarMenuInicio();
   
};


#endif