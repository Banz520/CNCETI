#include <Arduino.h>

#include "gestor_pantalla.h"
// put function declarations here:
GestorPantalla miGestorPantalla;

void setup() {
  // put your setup code here, to run once:
  miGestorPantalla.iniciarPantalla();
  miGestorPantalla.cargarMenuInicio();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
