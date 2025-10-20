#ifndef CONSOLA_H
#define CONSOLA_H

#include "menu_inicio.h"
#include "MCUFRIEND_kbv.h"
#include "barra_alerta.h"
#include "controlador_usb.h"

class Consola{
private:
    MCUFRIEND_kbv miDisplay;
    MenuInicio miMenuInicio;
    BarraAlerta miBarraAlerta;
    //ControladorUSB miControladorUSB;

public:

    Consola();
    void iniciar();
    void bucleDeEjecucion();

};

#endif

