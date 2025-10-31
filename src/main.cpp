#include "controlador_usb.h"
#include "SoftwareSerial.h"
#include "constantes.h"
#include "consola.h"

//ControladorUSB controlador_usb(Serial1, 9600);
Consola miConsola;

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600); 

    miConsola.iniciar();
    miConsola.pruebaLecturaSD();
    delay(2000);
    miConsola.pruebaLecturaUSB();
    
}

void loop() {
    
  
}