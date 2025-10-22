#ifndef MENU_INICIO_H
#define MENU_INICIO_H

#include "MCUFRIEND_kbv.h"
#include "gestor_widgets.h"
#include "lista.h"
#include "constantes.h"

class MenuInicio {
private:
    MCUFRIEND_kbv &display;
    GestorWidgets &miGestorWidgets;
    Lista &listaOpciones;    
   
    const char* opciones_menu[5] = {
        "Abrir archivo local",
        "Importar archivo desde USB", 
        "Importar archivo desde la red",
        "Calibrar ejes",
        "Otros"
    };

public: 
    MenuInicio(MCUFRIEND_kbv &display_ref,Lista &lista_ref,GestorWidgets &gestor_ref);
    void mostrar();
};

#endif