#include <Ch376msc.h>
#include <Arduino.h>
#include <MultiStepperLite.h>

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
ComandoGcode comando_actual,comando_anterior;

bool esperando_fin_movimiento = false;



void setup() {

    #if MODO_DESARROLLADOR
    Serial.begin(115200);
    #endif
    
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

    miConsola.mostrarPantallaEjecucion();
    delay(1000);
    
}

void loop() {
    // Actualizar el estado de la CNC (importante llamar esto frecuentemente)
    miControladorCNC.actualizar();
    
    // Si no hay comando en ejecucion, leer siguiente linea del archivo
    if (!miControladorCNC.comandoEnEjecucion() && !esperando_fin_movimiento) {
        const char* linea_gcode = miConsola.miControladorSD.leerLineaGcode();
        
        if (linea_gcode != nullptr) {
            // Procesar el comando con el interprete
            if (miInterpreteGcode.procesarComando(linea_gcode)) {
                // Obtener los datos del comando procesado
                comando_actual = miInterpreteGcode.obtenerComandoActual();

                miConsola.actualizar(comando_anterior.x,comando_actual.x,comando_actual.x,comando_anterior.y,comando_actual.y,comando_actual.y,comando_anterior.z,comando_actual.z,comando_actual.z,linea_gcode);
                
                // Establecer y ejecutar el comando_actual en la CNC
                miControladorCNC.establecerComando(comando_actual);
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
        comando_anterior = comando_actual;
        #if MODO_DESARROLLADOR
            Serial.println(F("Movimiento completado, listo para siguiente comando"));
        #endif
            delay(1000); // Pequeno delay antes de siguiente comando
    }
    
   
}
