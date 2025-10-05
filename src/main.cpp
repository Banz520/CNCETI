#include <Arduino.h>

#include "menu_inicio.h"
#include "barra_superior.h"
#include "MCUFRIEND_kbv.h"
// put function declarations here:

//archivo_prueba prueba;
MCUFRIEND_kbv miDisplay;
//BarraSuperior miBarraSup(miDisplay,"Hola");
MenuInicio miMenuInicio(miDisplay);


void setup() {
  // put your setup code here, to run once:
  uint16_t id = miDisplay.readID();
  miDisplay.begin(id);
  miDisplay.setRotation(3);
  miDisplay.fillScreen(COLOR_BLANCO);
  miMenuInicio.mostrar();
  //miBarraSup.mostrar();
}

void loop() {
  // put your main code here, to run repeatedly:
}
