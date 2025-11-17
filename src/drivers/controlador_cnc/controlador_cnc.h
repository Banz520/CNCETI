#ifndef CONTROLADOR_CNC_H
#define CONTROLADOR_CNC_H

#include "MultiStepperLite.h"
#include "comando_gcode.h"

enum Motor: uint8_t{
    EJE_X,
    EJE_Y,
    EJE_Z
};

/**
 * @class ControladorCNC
 * @brief Controlador ControladorCNC que ejecuta comandos G-code usando MultiStepperLite
 * 
 * Esta clase recibe comandos G-code estructurados y los ejecuta controlando
 * los motores paso a paso mediante la libreria MultiStepperLite.
 */
class ControladorCNC {
private:
    
    ComandoGcode comando_actual; 
    bool ejecutando_comando;
    
    
    // Configuracion de pasos por milimetro (ajustar segun tu mecanica)
    const float pasos_por_mm_x = 80.0f;
    const float pasos_por_mm_y = 80.0f;
    const float pasos_por_mm_z = 80.0f;
    
    /**
     * @brief Convierte distancia en mm a pasos de motor
     * @param distancia_mm Distancia en milimetros
     * @param pasos_por_mm Pasos por milimetro del eje
     * @return Numero de pasos a ejecutar
     */
    long convertirMmAPasos(float distancia_mm, float pasos_por_mm);
    
    /**
     * @brief Calcula el delay entre pasos en microsegundos basado en la velocidad
     * @param velocidad_mm_min Velocidad en mm/minuto
     * @param pasos_por_mm Pasos por milimetro del eje
     * @return Delay en microsegundos entre pasos
     */
    unsigned long calcularDelayVelocidad(float velocidad_mm_min, float pasos_por_mm);
    
    

public:
    MultiStepperLite &controlador_motores;

    /**
     * @brief Constructor de la clase ControladorCNC
     * @param pin_enable_x Pin enable del motor X
     * @param pin_step_x Pin step del motor X
     * @param pin_dir_x Pin direction del motor X
     * @param pin_enable_y Pin enable del motor Y
     * @param pin_step_y Pin step del motor Y
     * @param pin_dir_y Pin direction del motor Y
     * @param pin_enable_z Pin enable del motor Z
     * @param pin_step_z Pin step del motor Z
     * @param pin_dir_z Pin direction del motor Z
     */
    ControladorCNC(MultiStepperLite &miControladorMotores_ref);
    
    /**
     * @brief Configura los pines de control de los motores
     */
    void configurarPinesMotores();
    
    /**
     * @brief Inicializa el controlador de motores
     */
    void inicializarMotores();
    
    /**
     * @brief Establece un comando G-code para ejecucion
     * @param comando Estructura con los datos del comando G-code
     */
    void establecerComando(const ComandoGcode& comando);
    
    /**
     * @brief Ejecuta el comando G-code actualmente establecido
     * @return true si el comando fue aceptado para ejecucion, false en caso de error
     */
    bool ejecutarComando();
    
    /**
     * @brief Actualiza el estado de los motores (debe llamarse frecuentemente en el loop)
     */
    void actualizar(uint32_t tiempo_actual,float *posicion_motor);
    
    /**
     * @brief Verifica si hay un comando en ejecucion
     * @return true si hay un comando en ejecucion, false si esta listo para nuevo comando
     */
    bool comandoEnEjecucion() const;
    
    /**
     * @brief Detiene inmediatamente todos los motores
     */
    void detenerEmergencia();
    
    /**
     * @brief Obtiene el comando actual en ejecucion
     * @return Referencia constante al comando actual
     */
    const ComandoGcode& obtenerComandoActual() const;
};

#endif // CNC_H