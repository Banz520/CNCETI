#include "gestor_widgets.h"

/**
 * @brief Constructor de GestorWidgets
 * 
 * Inicializa la referencia al display mediante lista de inicialización.
 * No realiza ninguna configuración adicional del display.
 * 
 * @param display_ref Referencia al objeto MCUFRIEND_kbv que se utilizará
 *                    para todas las operaciones gráficas
 */
GestorWidgets::GestorWidgets(MCUFRIEND_kbv &display_ref) 
    : display(display_ref) 
{
    // Inicialización si es necesaria
}

/**
 * @brief Implementación del método para dibujar una barra
 * 
 * Este método realiza las siguientes operaciones:
 * 1. Dibuja el fondo de la barra con el color especificado
 * 2. Si hay un mensaje, configura la fuente y colores del texto
 * 3. Posiciona el cursor con un margen izquierdo de 12 píxeles
 * 4. Dibuja el texto centrado verticalmente en la barra
 * 
 * @param config Estructura ConfigBarra con los parámetros de visualización
 * 
 * @warning El método no valida los parámetros de posición y tamaño.
 *          Asegúrese de que estén dentro de los límites de la pantalla.
 * 
 * @see ConfigBarra
 */
void GestorWidgets::dibujarBarra(const ConfigBarra& config){
    display.fillRect(config.pos_x, config.pos_y, config.ancho, config.alto, config.color_fondo);
        if (config.mensaje) {
            display.setCursor(config.pos_x + 12, config.pos_y + 20);
            display.setTextColor(config.color_texto);
            display.setFont(&FreeSans9pt7b);
            display.print(config.mensaje);
        }
}
