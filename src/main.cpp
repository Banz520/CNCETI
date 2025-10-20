#include "consola.h"
#include <Arduino.h>
#include "controlador_sd.h"
#include "constantes.h"

Consola miConsola;

void setup() {
    #if MODO_DESARROLLADOR
        Serial.begin(115200);
    #endif
    
    ControladorSD miControladorSD;
    miControladorSD.iniciarSD();
    miControladorSD.leerDirectorio(miControladorSD.directorio_raiz,0);
    //miConsola.iniciar();
}

void loop() {
    //miConsola.bucleDeEjecucion();
    
    /**/
    
}