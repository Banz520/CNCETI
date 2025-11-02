
#ifndef WIDGETS_H
#define WIDGETS_H

#include <Arduino.h>
#include "Adafruit_GFX.h"

/**
 * @brief Estructura base para la configuracion geometrica y de apariencia de widgets
 * 
 * Contiene los parametros fundamentales de posicion, dimensiones y color de fondo
 * que son comunes a todos los tipos de widgets graficos.
 * 
 * @note Todos los valores son constantes
 */
struct ConfigWidget {
    const uint16_t pos_x;      
    const uint16_t pos_y;       
    const uint16_t ancho;       
    const uint16_t alto;        
    const uint16_t color_fondo;
};

/**
 * @brief Configuracion para mensajes de texto estaticos almacenados en memoria Flash
 * 
 * Optimizado para textos constantes que no cambian durante la ejecucion,
 * permitiendo ahorrar memoria RAM almacenando los strings en memoria Flash.
 * 
 * @warning El mensaje debe estar almacenado en PROGMEM usando la macro F()
 * 
 */
struct ConfigMensajeEstatico {
    const uint16_t color_texto;                 
    const __FlashStringHelper* mensaje;        
    const GFXfont *fuente;
};

/**
 * @brief Configuracion para mensajes de texto dinamicos almacenados en RAM
 * 
 * Diseñado para textos que cambian durante la ejecucion del programa.
 * Utiliza memoria RAM para permitir modificaciones en tiempo real.
 * 
 * @warning El mensaje debe apuntar a memoria valida durante todo el ciclo de vida del widget.
 *          Se recomienda usar buffers globales o estaticos para evitar punteros colgantes.
 */
struct ConfigMensajeDinamico {
    const uint16_t color_texto;     
    const char* mensaje;            
    const GFXfont *fuente;
};

/**
 * @brief Widget para barras con texto estatico no modificable
 * 
 * Combina una configuracion geometrica con un mensaje de texto estatico
 * almacenado en memoria Flash. Ideal para titulos, etiquetas y textos
 * que permanecen constantes durante la ejecucion.
 */
struct WidgetBarraEstatica {
    const ConfigWidget miConfigCuadro;              
    const ConfigMensajeEstatico miConfigMensajeEstatico; 
};

/**
 * @brief Widget para barras con texto dinamico modificable
 * 
 * Combina una configuracion geometrica con un mensaje de texto dinamico
 * que puede actualizarse durante la ejecucion. Utiliza memoria RAM para
 * permitir cambios en el contenido textual.
 */
struct WidgetBarraDinamica {
    const ConfigWidget miConfigCuadro;              
    const ConfigMensajeDinamico miConfigMensajeDinamico; 
};

/**
 * @brief Widget especializado para mostrar el estado de ejes en sistemas CNC
 * 
 * Diseñado especificamente para mostrar informacion de estado de ejes,
 * incluyendo un titulo estatico con el nombre del eje. Optimizado para
 * interfaces de maquinas CNC y sistemas de control de movimiento.
 */
struct WidgetEstadoEje {
    const ConfigWidget miConfigCuadro;              
    const ConfigMensajeEstatico miConfigMensajeEstatico; 
};

/**
 * @brief Widget para mostrar valores numericos de ejes en sistemas CNC
 * 
 * Especializado en la visualizacion de valores numericos de posicionamiento
 * como origen, posicion actual y destino. No incluye texto estatico, solo
 * los valores numericos flotantes.
 * 
 * @note Diseñado para trabajar en conjunto con WidgetEstadoEje
 * 
 */
struct WidgetValoresEje {
    const float origen;                    
    const float posicion;                   
    const float destino;                    
};

#endif // WIDGETS_CONFIG_H