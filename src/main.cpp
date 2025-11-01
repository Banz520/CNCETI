#include <Ch376msc.h>
#include <Arduino.h>

#include "controlador_usb.h"
#include "constantes.h"
#include "consola.h"
#include "interprete_gcode.h"
#include "controlador_cnc.h"
#include "comando_gcode.h"
#include "pines.h"


Ch376msc miHostUsb(Serial2, VELOCIDAD_SERIAL2_USB);
Consola miConsola(miHostUsb);

InterpreteGcode miInterpreteGcode;
MultiStepperLite miControladorMotores(3);

ControladorCNC miControladorCNC(miControladorMotores);

char buffer_lectura_archivo[255];
bool esperando_fin_movimiento = false;
ComandoGcode comando;
void setup() {
    Serial.begin(115200);
    Serial2.begin(VELOCIDAD_SERIAL2_USB); 
    delay(2000);

    miConsola.iniciar();
    miConsola.miControladorSD.iniciarSD();
    delay(2000);

    miControladorCNC.configurarPinesMotores();
    miControladorCNC.inicializarSteppers();
    
    
    if(!miConsola.miControladorSD.abrirArchivoGcode("cake.gcode")){
        miConsola.miControladorSD.abrirArchivoGcode("CAKE~1.GCO");
    } 

    
}

void loop() {
    // Actualizar el estado de la CNC (importante llamar esto frecuentemente)
    miControladorCNC.actualizar();
    
    // Si no hay comando en ejecucion, leer siguiente linea del archivo
    if (!miControladorCNC.comandoEnEjecucion() && !esperando_fin_movimiento) {
        String linea_gcode = miConsola.miControladorSD.leerLineaGcode();
        
        if (linea_gcode.length() > 0) {
            // Procesar el comando con el interprete
            if (miInterpreteGcode.procesarComando(linea_gcode)) {
                // Obtener los datos del comando procesado
                comando = miInterpreteGcode.obtenerComandoActual();
                
                // Establecer y ejecutar el comando en la CNC
                miControladorCNC.establecerComando(comando);
                if (miControladorCNC.ejecutarComando()) {
                    esperando_fin_movimiento = true;
#if MODO_DESARROLLADOR
                    Serial.println(F("Comando enviado a CNC, esperando finalizacion..."));
#endif
                } else {
#if MODO_DESARROLLADOR
                    Serial.println(F("Error al ejecutar comando en CNC"));
#endif
                }
            }
        } else {
            // Fin del archivo o error de lectura
#if MODO_DESARROLLADOR
            Serial.println(F("Fin del archivo G-code o error de lectura"));
#endif
            delay(1000);
        }
    } 
    else if (esperando_fin_movimiento && !miControladorCNC.comandoEnEjecucion()) {
        // El movimiento ha terminado, prepararse para siguiente comando
        esperando_fin_movimiento = false;
#if MODO_DESARROLLADOR
        Serial.println(F("Movimiento completado, listo para siguiente comando"));
#endif
        delay(1000); // Pequeno delay antes de siguiente comando
    }
    
   
}
