#include "gestor_widgets.h"
#include "textos.h"
#include "constantes.h"

/**
 * @brief Constructor de GestorWidgets
 * 
 * Inicializa la referencia al display mediante lista de inicializacion.
 * No realiza ninguna configuracion adicional del display.
 * 
 * @param display_ref Referencia al objeto MCUFRIEND_kbv que se utilizara
 *                    para todas las operaciones graficas
 */
GestorWidgets::GestorWidgets(MCUFRIEND_kbv &display_ref) 
    : display(display_ref) 
{
    // Inicializacion si es necesaria
}

/**
 * @brief Dibuja una barra estatica con texto en memoria Flash
 * 
 * Este metodo realiza las siguientes operaciones:
 * 1. Dibuja el fondo de la barra con el color especificado
 * 2. Configura la fuente y colores del texto
 * 3. Posiciona el cursor con un margen izquierdo de 12 pixeles
 * 4. Dibuja el texto centrado verticalmente en la barra
 * 
 * @param widget Estructura WidgetBarraEstatica con los parametros de visualizacion
 * 
 * @warning El metodo no valida los parametros de posicion y tamaño.
 *          Asegurese de que esten dentro de los limites de la pantalla.
 * 
 * @see WidgetBarraEstatica
 */
void GestorWidgets::dibujarBarraEstatica(const WidgetBarraEstatica& widget){
    const ConfigWidget& config_cuadro = widget.miConfigCuadro;
    const ConfigMensajeEstatico& config_texto = widget.miConfigMensajeEstatico;
    
    // Dibujar fondo del widget
    display.fillRect(config_cuadro.pos_x, config_cuadro.pos_y, 
                    config_cuadro.ancho, config_cuadro.alto, 
                    config_cuadro.color_fondo);
    
    // Configurar y dibujar texto
    display.setCursor(config_cuadro.pos_x + 12, config_cuadro.pos_y + 20);
    display.setTextColor(config_texto.color_texto);
    display.setFont(config_texto.fuente);
    display.print(config_texto.mensaje);
}

/**
 * @brief Dibuja una barra dinamica con texto en memoria RAM
 * 
 * Similar a dibujarBarraEstatica pero diseñado para textos que pueden
 * actualizarse durante la ejecucion del programa.
 * 
 * @param widget Estructura WidgetBarraDinamica con los parametros de visualizacion
 * 
 * @see WidgetBarraDinamica
 */
void GestorWidgets::dibujarBarraDinamica(const WidgetBarraDinamica& widget){
    const ConfigWidget& config_cuadro = widget.miConfigCuadro;
    const ConfigMensajeDinamico& config_texto = widget.miConfigMensajeDinamico;
    
    // Dibujar fondo del widget
    display.fillRect(config_cuadro.pos_x, config_cuadro.pos_y, 
                    config_cuadro.ancho, config_cuadro.alto, 
                    config_cuadro.color_fondo);
    
    // Configurar y dibujar texto
    display.setCursor(config_cuadro.pos_x + 12, config_cuadro.pos_y + 20);
    display.setTextColor(config_texto.color_texto);
    display.setFont(config_texto.fuente);
    display.print(config_texto.mensaje);
}

/**
 * @brief Dibuja una barra estatica con un titulo adicional
 * 
 * Combina la funcionalidad de una barra estatica con un titulo
 * que se dibuja por encima de la barra principal.
 * 
 * @param widget Estructura WidgetBarraEstatica con los parametros de la barra
 * @param titulo Texto del titulo a mostrar sobre la barra
 */
void GestorWidgets::dibujarBarraDinamicaConTitulo(const WidgetBarraDinamica& widget, const ConfigMensajeEstatico &titulo){
    const ConfigWidget& config_cuadro = widget.miConfigCuadro;

    // Dibujar titulo sobre la barra
    display.setCursor(config_cuadro.pos_x, config_cuadro.pos_y - 12);
    display.setTextColor(titulo.color_texto);
    display.setFont(titulo.fuente);
    display.print(titulo.mensaje);

    // Dibujar la barra principal
    dibujarBarraDinamica(widget);
}

/**
 * @brief Dibuja el widget del estado de un eje del CNC
 * 
 * Dibuja el titulo (nombre del eje) y un recuadro con las etiquetas:
 * Origen, Posicion (Actual), Destino
 * 
 * @param widget Estructura WidgetEstadoEje con la configuracion del eje
 * 
 * @note Este metodo solo dibuja las partes estaticas. Para actualizar
 *       los valores numericos usar actualizarValoresEje()
 */
void GestorWidgets::dibujarEstadoEje(const WidgetEstadoEje& widget){
    const ConfigWidget& config_cuadro = widget.miConfigCuadro;
    const ConfigMensajeEstatico& config_texto = widget.miConfigMensajeEstatico;
    
    // Dibujar titulo del eje
    display.setCursor(config_cuadro.pos_x + 49, config_cuadro.pos_y - 5);
    display.setTextColor(config_texto.color_texto);
    display.setFont(config_texto.fuente);
    display.print(config_texto.mensaje);

    // Dibujar recuadro de informacion
    display.fillRect(config_cuadro.pos_x, config_cuadro.pos_y, 
                    config_cuadro.ancho, config_cuadro.alto, 
                    config_cuadro.color_fondo);
    
    // Dibujar etiquetas estaticas
    display.setTextColor(COLOR_NEGRO);
    display.setCursor(config_cuadro.pos_x + 4, config_cuadro.pos_y + 20);
    display.print(TXT_ESTADO_EJE_ORIGEN);
    display.setCursor(config_cuadro.pos_x + 4, config_cuadro.pos_y + 54);
    display.print(TXT_ESTADO_EJE_POSICION);
    display.setCursor(config_cuadro.pos_x + 4, config_cuadro.pos_y + 88);
    display.print(TXT_ESTADO_EJE_DESTINO);
}

#define OFFSET_X_VALORES_EJE 80
/**
 * @brief Actualiza los datos numericos del origen, posicion y destino del eje del CNC
 * 
 * @param config_cuadro Configuracion grafica constante del widget del eje
 * @param valores Valores numericos actualizados del eje
 * 
 * @note Este metodo asume que el area de valores ya fue dibujada previamente
 *       con dibujarEstadoEje(). Solo actualiza los valores numericos.
 */
void GestorWidgets::actualizarValoresEje(
    const ConfigWidget& config_cuadro, 
    const float &origen, 
    const float &posicion, 
    const float &destino) {
  
    // Limpiar area de valores
    display.fillRect(config_cuadro.pos_x + OFFSET_X_VALORES_EJE, config_cuadro.pos_y, 53, config_cuadro.alto, config_cuadro.color_fondo);
    
    // Dibujar valores directamente desde los parametros
    display.setFont(&FreeSans9pt7b);
    display.setCursor(config_cuadro.pos_x + OFFSET_X_VALORES_EJE, config_cuadro.pos_y + 20);
    display.print(origen);
    
    display.setCursor(config_cuadro.pos_x + OFFSET_X_VALORES_EJE, config_cuadro.pos_y + 54);
    display.print(posicion);
    
    display.setCursor(config_cuadro.pos_x + OFFSET_X_VALORES_EJE, config_cuadro.pos_y + 88);
    display.print(destino);
}

