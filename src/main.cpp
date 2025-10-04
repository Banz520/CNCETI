#include <Arduino.h>
//#include "archivo_prueba.h"
#include "barra_superior.h"
#include "MCUFRIEND_kbv.h"
// put function declarations here:
int myFunction(int, int);
//archivo_prueba prueba;
MCUFRIEND_kbv miDisplay;
BarraSuperior miBarraSup(miDisplay,"Seleccionar archivo desde USB");


void setup() {
  // put your setup code here, to run once:
  uint16_t id = miDisplay.readID();
  miDisplay.begin(id);
  miDisplay.setRotation(1);
  miDisplay.fillScreen(COLOR_BLANCO);
  miBarraSup.mostrar();
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}