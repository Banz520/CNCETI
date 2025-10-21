#ifndef CONSOLA_H
#define CONSOLA_H

#include "menu_inicio.h"
#include "MCUFRIEND_kbv.h"
#include "barra_alerta.h"
//#include "controlador_usb.h"
#include "controlador_sd.h"
#include "lista.h"

class Consola{
private:
    MCUFRIEND_kbv miDisplay;
    MenuInicio miMenuInicio;
    Lista miLista;
    BarraAlerta miBarraAlerta;
    ControladorSD miControladorSD;
    //ControladorUSB miControladorUSB;

public:

    Consola();
    void iniciar();
    void bucleDeEjecucion();

    //Metodos de pruebas de desarrollo
    #if MODO_DESARROLLADOR
    void pruebaLecturaSD();
    #endif
};

#endif

