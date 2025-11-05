
#include <MultiStepperLite.h>

#include "controlador_cnc.h"
#include "constantes.h"
#include "comando_gcode.h"
#include "pines.h"

ControladorCNC::ControladorCNC(MultiStepperLite &miControladorMotores_ref):
    controlador_motores(miControladorMotores_ref),
    ejecutando_comando(false)
{
   
}


/**
 * @brief Configura el modo de salida de los pines en el arduino
 * 
 * 
 */
void ControladorCNC::configurarPinesMotores() {
    // Configurar pines de enable y direccion
    pinMode(PIN_MOTOR_X_EN, OUTPUT);
    pinMode(PIN_MOTOR_X_DIR, OUTPUT);
    digitalWrite(PIN_MOTOR_X_EN, LOW);
    digitalWrite(PIN_MOTOR_X_DIR, LOW);
    
    pinMode(PIN_MOTOR_Y_EN, OUTPUT);
    pinMode(PIN_MOTOR_Y_DIR, OUTPUT);
    digitalWrite(PIN_MOTOR_Y_EN, LOW);
    digitalWrite(PIN_MOTOR_Y_DIR, LOW);
    
    pinMode(PIN_MOTOR_Z_EN, OUTPUT);
    pinMode(PIN_MOTOR_Z_DIR, OUTPUT);
    digitalWrite(PIN_MOTOR_Z_EN, LOW);
    digitalWrite(PIN_MOTOR_Z_DIR, LOW);
}

/**
 * @brief Llama a la referencia de la clase MultiStepper para inicializar los motores
 * 
 */
void ControladorCNC::inicializarSteppers() {
    // Inicializar cada motor con su indice y pin de step
    controlador_motores.init_stepper(EJE_X, PIN_MOTOR_X_PUL); 
    controlador_motores.init_stepper(EJE_Y, PIN_MOTOR_Y_PUL); 
    controlador_motores.init_stepper(EJE_Z, PIN_MOTOR_Z_PUL);
}

long ControladorCNC::convertirMmAPasos(float distancia_mm, float pasos_por_mm) {
    return static_cast<long>(distancia_mm * pasos_por_mm);
}

unsigned long ControladorCNC::calcularDelayVelocidad(float velocidad_mm_min, float pasos_por_mm) {
    if (velocidad_mm_min <= 0) {
        return 1000; // Velocidad por defecto lenta
    }
    
    // Convertir velocidad de mm/min a microsegundos/paso
    float mm_por_segundo = velocidad_mm_min / 60.0f;
    float pasos_por_segundo = mm_por_segundo * pasos_por_mm;
    float microsegundos_por_paso = 1000000.0f / pasos_por_segundo;
    
    return static_cast<unsigned long>(microsegundos_por_paso);
}

/**
 * @brief Establece el comando gcode para ejecutar por la clase ControladorCNC
 * 
 * @param comando una referencia a un struct ComandoGCode con los parametros de comando, y posicion de ejes como datos numericos
 */
void ControladorCNC::establecerComando(const ComandoGcode& comando) {
    comando_actual = comando;
    ejecutando_comando = false;
}

/**
 * @brief Ejecuta el comando gcode actual establecido llamando a las funciones especificas de cada comando en la clase
 * 
 * @return true El comando establecido es valido
 * @return false El comando establecido no es valido o no existe
 */
bool ControladorCNC::ejecutarComando() {
    if (ejecutando_comando) {
#if MODO_DESARROLLADOR
        Serial.println(F("Error: Ya hay un comando en ejecucion"));
#endif
        return false;
    }
    
    if (comando_actual.comando == 0) {
        return false; // Comando no valido
    }
    
#if MODO_DESARROLLADOR
    Serial.print(F("[ControladorCNC::ejecutarComando]Ejecutando comando G"));
    Serial.println(comando_actual.comando);
#endif

    bool comando_aceptado = false;
    
    switch (comando_actual.comando) {
        case 0: // Movimiento rapido (G00)
        case 1: // Interpolacion lineal (G01)
            {
                // Calcular pasos para cada eje
                long pasos_x = convertirMmAPasos(comando_actual.x, pasos_por_mm_x);
                long pasos_y = convertirMmAPasos(comando_actual.y, pasos_por_mm_y);
                long pasos_z = convertirMmAPasos(comando_actual.z, pasos_por_mm_z);
                
                // Calcular delays basados en velocidad
                unsigned long delay_x, delay_y, delay_z;
                
                if (comando_actual.comando == 0) {
                    // G00: Movimiento rapido - velocidad fija rapida
                    delay_x = 1000; // 1ms entre pasos para movimiento rapido
                    delay_y = 1000;
                    delay_z = 1000;
                } else {
                    // G01: Movimiento a velocidad especificada
                    delay_x = calcularDelayVelocidad(comando_actual.velocidad, pasos_por_mm_x);
                    delay_y = calcularDelayVelocidad(comando_actual.velocidad, pasos_por_mm_y);
                    delay_z = calcularDelayVelocidad(comando_actual.velocidad, pasos_por_mm_z);
                }
                
                // Configurar direcciones
                digitalWrite(PIN_MOTOR_X_DIR, pasos_x >= 0 ? LOW : HIGH);
                digitalWrite(PIN_MOTOR_Y_DIR, pasos_y >= 0 ? LOW : HIGH);
                digitalWrite(PIN_MOTOR_Z_DIR, pasos_z >= 0 ? LOW : HIGH);
                
                // Tomar valores absolutos para los pasos
                pasos_x = abs(pasos_x);
                pasos_y = abs(pasos_y);
                pasos_z = abs(pasos_z);
                
                // Iniciar movimientos finitos para cada eje que tenga movimiento
                if (pasos_x > 0) {
                    controlador_motores.start_finite(EJE_X, delay_x, pasos_x);
                }
                if (pasos_y > 0) {
                    controlador_motores.start_finite(EJE_Y, delay_y, pasos_y);
                }
                if (pasos_z > 0) {
                    controlador_motores.start_finite(EJE_Z, delay_z, pasos_z);
                }
                #if MODO_DESARROLLADOR
                Serial.print(F("[ ControladorCNC::ejecutarComando] Start finite - EjeX pasos: ")); Serial.print(pasos_x); Serial.print(" delayX: "); Serial.println(delay_x);
                Serial.print(F("[ ControladorCNC::ejecutarComando] Start finite - EjeY pasos: ")); Serial.print(pasos_y); Serial.print(" delayY: "); Serial.println(delay_y);
                Serial.print(F("[ ControladorCNC::ejecutarComando] Start finite - EjeZ pasos: ")); Serial.print(pasos_z); Serial.print(" delayZ: "); Serial.println(delay_z);
                #endif
                
                comando_aceptado = true;
            }
            break;
            
        case 4: // Parada programada (G04)
            {
                // Para G04, podriamos implementar un delay
                // Por ahora simplemente marcamos como completado inmediatamente
                comando_aceptado = true;
            }
            break;
            
        case 90: // Posicionamiento absoluto (G90)
        case 91: // Posicionamiento relativo (G91)
            {
                // Estos comandos afectan como se interpretan las coordenadas
                // Por ahora solo los registramos
                comando_aceptado = true;
            }
            break;
            
        default:
#if MODO_DESARROLLADOR
            Serial.print(F("Comando G no implementado: G"));
            Serial.println(comando_actual.comando);
#endif
            comando_aceptado = false;
            break;
    }
    
    if (comando_aceptado) {
        ejecutando_comando = true;
    }
    
    return comando_aceptado;
}

void ControladorCNC::actualizar(uint32_t tiempo_actual) {
    if (ejecutando_comando) {
        // Actualizar el estado de todos los motores
        #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorCNC::actualizar] Status is_running X:")); Serial.print(controlador_motores.is_running(EJE_X));
        Serial.print(" Y:"); Serial.print(controlador_motores.is_running(EJE_Y));
        Serial.print(" Z:"); Serial.println(controlador_motores.is_running(EJE_Z));
        static uint32_t last_time = 0;
        Serial.print(F("[ControladorCNC::actualizar] delta_t: "));
        Serial.println(tiempo_actual - last_time);
        last_time = tiempo_actual;
        #endif
        controlador_motores.do_tasks(tiempo_actual);
        
        // Verificar si todos los motores han terminado
        bool todos_terminados = true;
        for (int i = 0; i < 3; i++) {
            if (controlador_motores.is_running(i)) {
                todos_terminados = false;
                break;
            }
        }
        
        if (todos_terminados && ejecutando_comando) {
#if MODO_DESARROLLADOR
            Serial.println("Comando ControladorCNC completado");
#endif
            ejecutando_comando = false;
        }
    }
}

bool ControladorCNC::comandoEnEjecucion() const {
    return ejecutando_comando;
}

void ControladorCNC::detenerEmergencia() {
    // Detener todos los motores
    for (int i = 0; i < 3; i++) {
        controlador_motores.stop(i);
    }
    ejecutando_comando = false;
    
#if MODO_DESARROLLADOR
    Serial.println("EMERGENCIA: Todos los motores detenidos");
#endif
}

const ComandoGcode& ControladorCNC::obtenerComandoActual() const {
    return comando_actual;
}