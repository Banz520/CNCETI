#ifndef GESTOR_WIDGETS_H
#define GESTOR_WIDGETS_H

#include <Arduino.h>
#include "MCUFRIEND_kbv.h"
#include "Adafruit_GFX.h"
#include "Fonts/FreeSans9pt7b.h"
#include "widgets.h"  

/**
 * @brief Clase para gestionar y dibujar widgets en la pantalla
 * 
 * Esta clase proporciona metodos para renderizar diferentes tipos de
 * elementos de interfaz grafica en displays compatibles con MCUFRIEND_kbv.
 * 
 * @note Todos los strings estaticos deben almacenarse en PROGMEM para ahorrar RAM
 */
class GestorWidgets {
private:
    MCUFRIEND_kbv &display;

public:
    /**
     * @brief Constructor del gestor de widgets
     * @param display_ref Referencia al objeto de display para operaciones graficas
     */
    GestorWidgets(MCUFRIEND_kbv &display_ref);

    // Metodos para barras estaticas
    void dibujarBarraEstatica(const WidgetBarraEstatica& widget);
    void dibujarBarraDinamicaConTitulo(const WidgetBarraDinamica& widget, const ConfigMensajeEstatico &titulo);
    
    // Metodos para barras dinamicas
    void dibujarBarraDinamica(const WidgetBarraDinamica& widget);
    bool actualizarTextoBarraDinamica(WidgetBarraDinamica& widget, const char* nuevo_mensaje);
    
    // Metodos para ejes CNC
    void dibujarEstadoEje(const WidgetEstadoEje& widget);
    void actualizarValoresEje(const ConfigWidget& config_cuadro, const float &origen, const float &posicion, const float &destino);
    
};

#endif // GESTOR_WIDGETS_H