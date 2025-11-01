/**
 * @file interprete_gcode.h
 * @brief Interprete de codigos G para sistema CNC embebido
 * @author Tu Nombre
 * @date 2024
 */

#ifndef INTERPRETE_GCODE_H
#define INTERPRETE_GCODE_H

#include <Arduino.h>
#include "controlador_cnc.h"
#include "comando_gcode.h"


/**
 * @class InterpreteGcode
 * @brief Clase para interpretar y procesar comandos G-code
 * 
 * Esta clase se encarga de parsear comandos G-code y almacenar los datos
 * en una estructura ComandoGcode para su posterior uso.
 */
class InterpreteGcode {
private:
    ComandoGcode comando_actual_; ///< Estructura con los datos del comando actual
    
    /**
     * @brief Extrae valor numerico de una cadena
     * @param cadena Cadena de texto a procesar
     * @param prefijo Prefijo a buscar (ej: "X", "Y", etc.)
     * @return Valor numerico extraido
     */
    float extraerValor(const String& cadena, const String& prefijo);
    
    /**
     * @brief Procesa comando de interpolacion lineal (G00, G01)
     */
    void procesarInterpolacionLineal();
    
    /**
     * @brief Procesa comando de interpolacion circular (G02, G03)  
     */
    void procesarInterpolacionCircular();
    
    /**
     * @brief Procesa comando de parada programada (G04)
     */
    void procesarParadaProgramada();
    
    /**
     * @brief Procesa comando de seleccion de unidades (G20, G21)
     */
    void procesarSeleccionUnidades();

public:
    /**
     * @brief Constructor de la clase
     */
    InterpreteGcode();
    
    /**
     * @brief Procesa una linea de codigo G y almacena los datos en la estructura interna
     * @param comando Cadena con el comando G-code a procesar
     * @return true si el comando fue procesado correctamente, false en caso contrario
     */
    bool procesarComando(const String& comando);
    
    /**
     * @brief Obtiene una copia del comando G-code actualmente procesado
     * @return Estructura ComandoGcode con los datos del comando
     */
    ComandoGcode obtenerComandoActual() const;
    
    /**
     * @brief Establece los valores del comando G-code actual
     * @param comando Estructura ComandoGcode con los nuevos valores
     */
    void establecerComandoActual(const ComandoGcode& comando);
    
    /**
     * @brief Reinicia los valores de la estructura comando_actual_
     */
    void reiniciarValores();
    
    /**
     * @brief Verifica si hay un comando G-code valido almacenado
     * @return true si hay un comando valido, false en caso contrario
     */
    bool hayComandoValido() const;
};

#endif // INTERPRETE_GCODE_H