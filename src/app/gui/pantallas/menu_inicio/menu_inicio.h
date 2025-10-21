#ifndef MENU_INICIO_H
#define MENU_INICIO_H

#include "MCUFRIEND_kbv.h"
#include "barra_superior.h"
#include "barra_inferior.h"
#include "lista.h"

class MenuInicio {
private:
    MCUFRIEND_kbv &display;
    BarraSuperior miBarraSup;
    BarraInferior miBarraInf;
    Lista &listaOpciones;
    
    // Array en RAM - funcionará inmediatamente
    const char* opciones_menu[5] = {
        "Abrir archivo local",
        "Importar archivo desde USB", 
        "Importar archivo desde la red",
        "Calibrar ejes",
        "Otros"
    };

public: 
    MenuInicio(MCUFRIEND_kbv &display_ref, Lista &lista_ref);
    void mostrar();
};

#endif