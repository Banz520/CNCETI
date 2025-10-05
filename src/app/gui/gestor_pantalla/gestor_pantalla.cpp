#include "gestor_pantalla.h"

GestorPantalla::GestorPantalla() : miDisplay(), miMenuInicio(miDisplay.tft){
    
}

void GestorPantalla::iniciarPantalla(){
    miDisplay.iniciar();
}

void GestorPantalla::cargarMenuInicio(){
    miMenuInicio.mostrar();
}