#ifndef GESTOR_WIDGETS_H
#define GESTOR_WIDGETS_H

#include <Arduino.h>
#include "MCUFRIEND_kbv.h"
#include "Adafruit_GFX.h"
#include "Fonts/FreeSans9pt7b.h"

/**
 * @brief Estructura para configurar los parámetros de un widget de tipo barra
 * 
 * Esta estructura contiene todos los parámetros necesarios para dibujar
 * una barra en la pantalla, incluyendo posición, dimensiones, colores y texto.
 * @param pos_x uint16t La posicion de la pantalla donde se dibujara la esquina superior izquierda de la barra( 0 < pos_x < 480)
 * @param pos_y uint16t La posicion de la pantalla donde se dibujara la esquina superior izquierda de la barra( 0 < pos_x < 320)
 * @param ancho uint16_t El ancho de la barra a la dibujar (0 < ancho < 480)
 * @param alto uint16_t El alto de la barra a la dibujar (0 < ancho < 320)
 * @param color_fondo uint16_t Color de fondo de la barra a dibujar, toma un color rgb de 16 bits en hexadecimal
 * @param color_texto uint16_t Color de del texto, toma un color rgb de 16 bits en hexadecimal
 * @param mensaje __FlashStringHelper* Mensaje a mostrar en la barra, toma un puntero a una cadena guardada en la memoria flash
 */
struct ConfigBarra {
    uint16_t pos_x, pos_y, ancho, alto;
    uint16_t color_fondo, color_texto;
    const __FlashStringHelper* mensaje;
};

/**
 * @brief Clase para gestionar y dibujar widgets en la pantalla
 * 
 * Esta clase proporciona métodos para renderizar diferentes tipos de
 * elementos de interfaz gráfica en displays compatibles con MCUFRIEND_kbv.
 * 
 * 
 * @note Todos los strings deben almacenarse en PROGMEM para ahorrar RAM
 */
class GestorWidgets{
private:
    MCUFRIEND_kbv &display;
public:

    GestorWidgets(MCUFRIEND_kbv &display_ref);
    void dibujarBarra(const ConfigBarra& config);
};

#endif