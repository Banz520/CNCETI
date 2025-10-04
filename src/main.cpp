#include <Arduino.h>

#include "barra_superior.h"
#include "barra_inferior.h"
#include "MCUFRIEND_kbv.h"
// put function declarations here:

//archivo_prueba prueba;
MCUFRIEND_kbv miDisplay;
BarraSuperior miBarraSup(miDisplay,"Seleccionar archivo desde USB");
BarraInferior miBarraInf(miDisplay,"Presione <,> para navegar, ENTER para ejecutar");


void setup() {
  // put your setup code here, to run once:
  uint16_t id = miDisplay.readID();
  miDisplay.begin(id);
  miDisplay.setRotation(1);
  miDisplay.fillScreen(COLOR_BLANCO);
  miBarraSup.mostrar();
  miBarraInf.mostrar();
}

void loop() {
  // put your main code here, to run repeatedly:
}
