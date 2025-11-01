#ifndef COMANDO_GCODE_H
#define COMANDO_GCODE_H

#include <Arduino.h>
/**
 * @struct ComandoGcode
 * @brief Estructura para almacenar los datos de un comando G-code
 */
struct ComandoGcode {
    float x; ///< Valor del eje X
    float y; ///< Valor del eje Y  
    float z; ///< Valor del eje Z
    float velocidad; ///< Velocidad de la cortadora
    uint8_t comando; ///< Codigo G del comando
    
    /**
     * @brief Constructor que inicializa todos los valores a cero
     */
    ComandoGcode() : x(0.0f), y(0.0f), z(0.0f), velocidad(0.0f), comando(0) {}
};

#endif // COMANDO_GCODE_H