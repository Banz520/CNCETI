/**
 * @file main.cpp
 * @author Banzi (https://github.com/Banz520)
 * @brief Sistema de control par aun sistema CNC de tres motores 
 * con pantalla tft de 3.5 pulgadas con un teclado matricial y lectura de archivos mediante tarjetas microSD y USB
 * @version 1.3
 * @date 2025-11-04
 * 
 * @copyright Copyright (c) 2025 Creative Commons Attribution 4.0 International (CC BY 4.0)
 * 
 */

 //Nota para mi: cada que hago full clean del proyecto debo volver a especificar en el mcufriend_shield y mcufriend_special que estoy usando un shield de 16 bits
#include <Ch376msc.h>
#include <Arduino.h>
#include <MultiStepperLite.h>
#include <Keypad.h>

#include "pines.h"
#include "constantes.h"

#include "controlador_usb.h"

#include "consola.h"
#include "interprete_gcode.h"
#include "controlador_cnc.h"
#include "comando_gcode.h"

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {PIN_TECLADO_FILA_1,PIN_TECLADO_FILA_2, PIN_TECLADO_FILA_3, PIN_TECLADO_FILA_4}; 
byte colPins[COLS] = {PIN_TECLADO_COL_1, PIN_TECLADO_COL_2, PIN_TECLADO_COL_3, PIN_TECLADO_COL_4}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


Ch376msc miHostUsb(Serial2, VELOCIDAD_SERIAL2_USB);
Consola miConsola(miHostUsb);

InterpreteGcode miInterpreteGcode;
MultiStepperLite miControladorMotores(3);
ControladorCNC miControladorCNC(miControladorMotores);
ComandoGcode comando_actual,comando_anterior;

//ControladorSD miControladorSD;

char linea_gcode_buffer[256] = ""; 

bool esperando_fin_movimiento = false;
char tecla;
bool archivo_terminado = false;

// Función para limpiar buffer del keypad
void limpiarBufferKeypad() {
    #if MODO_DESARROLLADOR
    Serial.println("Limpiando buffer keypad...");
    #endif
    
    // Leer y descartar todas las teclas pendientes
    while (customKeypad.getKey() != NO_KEY) {
        delay(5);
    }
    
    // También limpiar el buffer interno
    customKeypad.getKeys();
}

void setup() {

    #if MODO_DESARROLLADOR
    Serial.begin(115200);
    #endif
    
    Serial2.begin(VELOCIDAD_SERIAL2_USB); 
    delay(2000);

    miConsola.iniciar();
    miConsola.miControladorSD.iniciarSD();
    //miControladorSD.iniciarSD();
    delay(2000);

    miControladorCNC.configurarPinesMotores();
    miControladorCNC.inicializarSteppers();
    
    //miControladorSD.abrirArchivoGcode("CAKE~1.GCO");
    limpiarBufferKeypad();
    delay(1000);
    
}

void loop() {
    // Leer teclado
    char key = customKeypad.getKey();
    
    
    if (key) {
        #if MODO_DESARROLLADOR
        Serial.print("Tecla detectada: ");
        Serial.println(key);
        #endif
    
        
        // Actualizar consola con la tecla
        miConsola.actualizar(key, comando_anterior.x, comando_actual.x, comando_actual.x,
                            comando_anterior.y, comando_actual.y, comando_actual.y,
                            comando_anterior.z, comando_actual.z, comando_actual.z,
                            linea_gcode_buffer);
        
        // Limpiar buffer después de ciertas teclas
        if (key == '*' || key == '#' || key == 'A' || key == 'B' || key == 'C' || key == 'D') {
            limpiarBufferKeypad();
        }
        
    } else {
        // Actualizar consola sin tecla
        miConsola.actualizar(' ', comando_anterior.x, comando_actual.x, comando_actual.x,
                            comando_anterior.y, comando_actual.y, comando_actual.y,
                            comando_anterior.z, comando_actual.z, comando_actual.z,
                            linea_gcode_buffer);
    }
    
    uint32_t tiempo_actual = micros();
    miConsola.actualizar(key, comando_anterior.x, comando_actual.x, comando_actual.x,comando_anterior.y, comando_actual.y, comando_actual.y,comando_anterior.z, comando_actual.z, comando_actual.z,linea_gcode_buffer);
    
    // Actualizar controlador CNC
    miControladorCNC.actualizar(tiempo_actual);
    
    // Lógica de ejecución G-code
    //if(miConsola.obtenerContextoActual() == EJECUCION && !archivo_terminado){
        
        if (!miControladorCNC.comandoEnEjecucion() && !esperando_fin_movimiento) {
            String linea_gcode = miConsola.miControladorSD.leerLineaGcode();
            
            if (linea_gcode.length() > 0) {
                linea_gcode.toCharArray(linea_gcode_buffer, sizeof(linea_gcode_buffer));
                
                #if MODO_DESARROLLADOR
                Serial.print("Procesando línea: ");
                Serial.println(linea_gcode_buffer);
                #endif
                
                if (miInterpreteGcode.procesarComando(linea_gcode)) {
                    comando_actual = miInterpreteGcode.obtenerComandoActual();
                    
                    miControladorCNC.establecerComando(comando_actual);
                    if (miControladorCNC.ejecutarComando()) {
                        esperando_fin_movimiento = true;
                        #if MODO_DESARROLLADOR
                        Serial.println(F("[Main] Comando enviado a CNC"));
                        #endif
                    }
                }
            } else {
                archivo_terminado = true;
                strcpy(linea_gcode_buffer, "FIN ARCHIVO");
                #if MODO_DESARROLLADOR
                Serial.println(F("Fin del archivo"));
                #endif
                miConsola.miControladorSD.cerrarArchivoGcode();
            }
        } 
        else if (esperando_fin_movimiento && !miControladorCNC.comandoEnEjecucion()) {
            esperando_fin_movimiento = false;
            comando_anterior = comando_actual;

            #if MODO_DESARROLLADOR
            Serial.println(F("[Main] Movimiento completado"));
            #endif
            
            delay(100);
        }
    //}
    
    delay(10);


    /*
    tecla = ' ';
    if(customKeypad.getKey()){
        tecla = customKeypad.getKey();
        Serial.print("Tecla presionada: ");
        Serial.println(tecla);
        //miConsola.actualizar(key,comando_anterior.x,comando_actual.x,comando_actual.x,comando_anterior.y,comando_actual.y,comando_actual.y,comando_anterior.z,comando_actual.z,comando_actual.z,linea_gcode_buffer);
    }
    miConsola.actualizar(tecla,comando_anterior.x,comando_actual.x,comando_actual.x,comando_anterior.y,comando_actual.y,comando_actual.y,comando_anterior.z,comando_actual.z,comando_actual.z,linea_gcode_buffer);
    /// Actualizar el estado de la CNC (importante llamar esto frecuentemente)
    miControladorCNC.actualizar();
    
    // Si no hay comando en ejecucion, leer siguiente linea del archivo
    if (!miControladorCNC.comandoEnEjecucion() && !esperando_fin_movimiento && miConsola.obtenerContextoActual() == EJECUCION) {
        String linea_gcode = miConsola.miControladorSD.leerLineaGcode();
        
        if (linea_gcode.length() > 0) {
            // Procesar el comando con el interprete
            if (miInterpreteGcode.procesarComando(linea_gcode)) {
                // Obtener los datos del comando procesado
                ComandoGcode comando = miInterpreteGcode.obtenerComandoActual();
                
                // Establecer y ejecutar el comando en la CNC
                miControladorCNC.establecerComando(comando);
                if (miControladorCNC.ejecutarComando()) {
                    esperando_fin_movimiento = true;
#if MODO_DESARROLLADOR
                    Serial.println("Comando enviado a CNC, esperando finalizacion...");
#endif
                } else {
#if MODO_DESARROLLADOR
                    Serial.println("Error al ejecutar comando en CNC");
#endif
                }
            }
        } else {
            // Fin del archivo o error de lectura
#if MODO_DESARROLLADOR
            Serial.println("Fin del archivo G-code o error de lectura");
#endif
            delay(1000);
        }
    } 
    else if (esperando_fin_movimiento && !miControladorCNC.comandoEnEjecucion()) {
        // El movimiento ha terminado, prepararse para siguiente comando
        esperando_fin_movimiento = false;
#if MODO_DESARROLLADOR
        Serial.println("Movimiento completado, listo para siguiente comando");
#endif
        delay(100); // Pequeno delay antes de siguiente comando
    }
    
    // Aqui puedes agregar otras tareas que se ejecuten concurrentemente
    // mientras los motores se mueven

    // Si no hay comando en ejecucion, leer siguiente linea del archivo
    /*
    if(miConsola.obtenerContextoActual() == EJECUCION){
        
        if (!miControladorCNC.comandoEnEjecucion() && !esperando_fin_movimiento) {
            String linea_gcode = miConsola.miControladorSD.leerLineaGcode();
            
        
            if (linea_gcode.length() > 0) {
                // Copiar a buffer para persistencia
                linea_gcode.toCharArray(linea_gcode_buffer, sizeof(linea_gcode_buffer));
                
                #if MODO_DESARROLLADOR
                Serial.print("Leyendo línea: ");
                Serial.println(linea_gcode_buffer);
                #endif

                // Procesar el comando con el interprete
                if (miInterpreteGcode.procesarComando(linea_gcode)) {
                    // Obtener los datos del comando procesado
                    comando_actual = miInterpreteGcode.obtenerComandoActual();

                    
                    
                    // Establecer y ejecutar el comando_actual en la CNC
                    miControladorCNC.establecerComando(comando_actual);
                    if (miControladorCNC.ejecutarComando()) {
                        esperando_fin_movimiento = true;
                        #if MODO_DESARROLLADOR
                            Serial.println(F("[Main]Comando enviado a CNC, esperando finalizacion..."));
                        #endif
                    } 
                    else {
                        #if MODO_DESARROLLADOR
                            Serial.println(F("Error al ejecutar comando en CNC"));
                        #endif
                    }
                }
            } 
            else {
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
   */
}