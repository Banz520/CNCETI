#include "pantalla_ejecucion.h"
#include "textos.h"
#include "widgets.h"

/**
 * @def POSICION_BARRA_INFERIOR
 * @brief Posicion en pixeles en el eje Y del display para dibujar la barra inferior
 * 
 */
#define POSICION_BARRA_INFERRIOR 280

/**
 * @brief La medida de altura en pixeles para dibujar las barras
 * 
 */
#define ALTURA_BARRA 40

#define ANCHO_BARRA_INFO 200
#define ALTURA_BARRA_INFO 60

#define ANCHO_ESTADO_EJE 133
#define ALTO_ESTADO_EJE 100

#define POS_Y_ESTADO_EJE 60
#define POS_Y_INFO 207
#include "pantalla_ejecucion.h"

/**
 * @brief Constructor - inicializa configuraciones constantes
 */
PantallaEjecucion::PantallaEjecucion(MCUFRIEND_kbv &display_ref, GestorWidgets &gestor_ref)
    : display(display_ref)
    , miGestorWidgets(gestor_ref)
    , config_eje_x{20, POS_Y_ESTADO_EJE, ANCHO_ESTADO_EJE, ALTO_ESTADO_EJE, COLOR_GRIS_CLARO}
    , config_eje_y{173,POS_Y_ESTADO_EJE , ANCHO_ESTADO_EJE, ALTO_ESTADO_EJE, COLOR_GRIS_CLARO}
    , config_eje_z{326, POS_Y_ESTADO_EJE, ANCHO_ESTADO_EJE, ALTO_ESTADO_EJE, COLOR_GRIS_CLARO}
    , cuadro_gcode{20, POS_Y_INFO, ANCHO_BARRA_INFO, ALTURA_BARRA_INFO, COLOR_GRIS_CLARO}
    , cuadro_cortadora{259, POS_Y_INFO, ANCHO_BARRA_INFO, ALTURA_BARRA_INFO, COLOR_GRIS_CLARO}
    , color_texto_valores(COLOR_NEGRO)  // Color constante para valores numericos
{
    // Constructor vacio - configuraciones se inicializan en lista
}

/**
 * @brief Muestra la pantalla de ejecucion del sistema CNC
 */
void PantallaEjecucion::mostrar() {
    display.fillScreen(COLOR_BLANCO);

    // Configurar elementos estaticos
    const WidgetBarraEstatica barra_superior = {
        {0, 0, DISPLAY_ANCHO, ALTURA_BARRA, COLOR_GRIS_CLARO},
        {COLOR_NEGRO, MENU_PRINCIPAL_TEXT,&FreeSans9pt7b}
    };
    
    const WidgetBarraEstatica barra_inferior = {
        {0, POSICION_BARRA_INFERRIOR, DISPLAY_ANCHO, ALTURA_BARRA, COLOR_GRIS_OSCURO},
        {COLOR_BLANCO, BARRA_INF_TEXT,&FreeSans9pt7b}
    };
   

    const WidgetBarraDinamica barra_gcode = {
        cuadro_gcode,
        {COLOR_NEGRO, "Iniciando lectura",nullptr}
    };

    const ConfigMensajeEstatico titulo_gcode = {
        COLOR_NEGRO,F("Codigo G en ejecucion:"),&FreeSans9pt7b
    };

     const WidgetBarraDinamica barra_cortadora = {
        cuadro_cortadora,
        {COLOR_NEGRO, "Iniciando lectura",nullptr}
    };

    const ConfigMensajeEstatico titulo_cortadora = {
        COLOR_NEGRO,F("Velocidad Cortadora:"),&FreeSans9pt7b
    };

    const WidgetEstadoEje estado_eje_x = {
        config_eje_x,
        {COLOR_AZUL, F("EJE X:"),&FreeSans9pt7b}  
    };
    
    const WidgetEstadoEje estado_eje_y = {
        config_eje_y,
        {COLOR_VERDE, F("EJE Y:"),&FreeSans9pt7b}
    };
    
    const WidgetEstadoEje estado_eje_z = {
        config_eje_z,
        {COLOR_ROJO, F("EJE Z:"),&FreeSans9pt7b}
    };

    // Dibujar interfaz
    miGestorWidgets.dibujarBarraEstatica(barra_superior);
    miGestorWidgets.dibujarEstadoEje(estado_eje_x);
    miGestorWidgets.dibujarEstadoEje(estado_eje_y);
    miGestorWidgets.dibujarEstadoEje(estado_eje_z);
    miGestorWidgets.dibujarBarraDinamicaConTitulo(barra_gcode,titulo_gcode );
    miGestorWidgets.dibujarBarraDinamicaConTitulo(barra_cortadora,titulo_cortadora);
    miGestorWidgets.dibujarBarraEstatica(barra_inferior);
}

/**
 * @brief Actualiza los datos numericos de los ejes
 * 
 * Usa el metodo directo para evitar creacion de estructuras temporales.
 * CERO bytes de memoria temporal utilizados.
 * 
 * @param origen_x Valor de origen del eje X
 * @param posicion_x Posicion actual del eje X
 * @param destino_x Valor de destino del eje X
 * @param origen_y Valor de origen del eje Y
 * @param posicion_y Posicion actual del eje Y  
 * @param destino_y Valor de destino del eje Y
 * @param origen_z Valor de origen del eje Z
 * @param posicion_z Posicion actual del eje Z
 * @param destino_z Valor de destino del eje Z
 */
void PantallaEjecucion::actualizarDatos(const float &origen_x, const float &posicion_x, const float &destino_x,
    const float &origen_y, const float &posicion_y, const float &destino_y,
    const float &origen_z, const float &posicion_z, const float &destino_z, const char* comando_gcode) {
    
    const WidgetBarraDinamica barra_gcode = {
        cuadro_gcode,
        {COLOR_NEGRO, comando_gcode,nullptr}        
    };

     const WidgetBarraDinamica barra_cortadora = {
        cuadro_cortadora,
        {COLOR_NEGRO, comando_gcode,nullptr}        
    };
    miGestorWidgets.actualizarValoresEje(config_eje_x, origen_x, posicion_x, destino_x);
    miGestorWidgets.actualizarValoresEje(config_eje_y, origen_y, posicion_y, destino_y);
    miGestorWidgets.actualizarValoresEje(config_eje_z, origen_z, posicion_z, destino_z);
    miGestorWidgets.dibujarBarraDinamica(barra_gcode);
    miGestorWidgets.dibujarBarraDinamica(barra_cortadora);
    
}