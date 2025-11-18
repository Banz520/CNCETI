#ifndef CONSOLA_H
#define CONSOLA_H

#include "menu_inicio.h"
#include "MCUFRIEND_kbv.h"
#include <UTFTGLUE.h>
#include "lista.h"
#include "constantes.h"
#include "gestor_widgets.h"
#include "gestor_archivos.h"
#include "pantalla_ejecucion.h"

/**
 * @file consola.h
 * @brief Interfaz de usuario unificada para el sistema CNC
 * 
 * @details Esta clase gestiona toda la interfaz de usuario del sistema CNC,
 * incluyendo menús, navegación de archivos, y pantalla de ejecución.
 * Integra el GestorArchivos para la gestión de archivos G-code.
 */

/**
 * @enum CONTEXTO_APP
 * @brief Contextos (pantallas) de la aplicación
 */
enum CONTEXTO_APP : uint8_t {
    MENU_INICIO,        ///< Menú principal
    MENU_ARCHIVOS_SD,   ///< Browser de archivos SD
    MENU_ARCHIVOS_USB,  ///< Browser de archivos USB
    EJECUCION,          ///< Pantalla de ejecución G-code
    CONFIGURACION       ///< Pantalla de configuración
};

/**
 * @class Consola
 * @brief Controlador principal de la interfaz de usuario
 * 
 * Gestiona:
 * - Cambio de contextos (pantallas)
 * - Navegación por archivos G-code
 * - Entrada de teclado
 * - Actualización de displays
 * - Integración con GestorArchivos
 */
class Consola {
private:
    // Componentes de display
    UTFTGLUE miDisplay;
    Lista miLista;
    GestorWidgets miGestorWidgets;
    MenuInicio miMenuInicio;
    PantallaEjecucion miPantallaEjecucion;
    
    // Referencia al gestor de archivos
    GestorArchivos &miGestorArchivos;

    // Estado de la aplicación
    CONTEXTO_APP contexto_actual;
    CONTEXTO_APP contexto_anterior;
    bool primer_actualizacion;
    
    // Variables para browser de archivos
    bool archivos_cargados;              ///< Indica si hay archivos cargados en memoria
    const char** array_nombres_archivos; ///< Array de punteros a nombres de archivo
    size_t cantidad_archivos_actual;     ///< Cantidad de archivos en el array
    
    // Métodos privados - Gestión de contexto
    
    /**
     * @brief Procesa entrada de teclado según el contexto actual
     * @param tecla Carácter de la tecla presionada
     */
    void procesarTecla(char tecla);
    
    /**
     * @brief Cambia el contexto de la aplicación
     * @param nuevo_contexto Contexto al que cambiar
     */
    void cambiarContexto(CONTEXTO_APP nuevo_contexto);
    
    /**
     * @brief Muestra la interfaz del contexto actual
     */
    void mostrarInterfazContexto();
    
    /**
     * @brief Limpia la pantalla del contexto anterior
     */
    void limpiarPantallaContextoAnterior();
    
    // Métodos privados - Browser de archivos
    
    /**
     * @brief Carga la lista de archivos del dispositivo actual en memoria
     * @return true si se cargaron archivos correctamente
     * 
     * @details Este método:
     * - Llama a miGestorArchivos.escanearDirectorio()
     * - Crea array dinámico con punteros a nombres
     * - Inicializa la clase Lista con los datos
     * - Marca archivos_cargados = true
     */
    bool cargarListaArchivos();
    
    /**
     * @brief Libera la memoria del array de nombres de archivos
     */
    void liberarListaArchivos();
    
    /**
     * @brief Muestra el browser de archivos en pantalla
     */
    void mostrarBrowserArchivos();
    
    /**
     * @brief Actualiza la selección visual en el browser
     * 
     * @details Sincroniza la selección de miLista con la de miGestorArchivos
     */
    void actualizarSeleccionArchivo();
    
    /**
     * @brief Abre el archivo actualmente seleccionado
     * @return true si se abrió correctamente
     * 
     * @details Abre el archivo seleccionado y cambia al contexto EJECUCION
     */
    bool abrirArchivoSeleccionado();
    
    /**
     * @brief Navega hacia arriba en la lista de archivos
     */
    void navegarArribaLista();
    
    /**
     * @brief Navega hacia abajo en la lista de archivos
     */
    void navegarAbajoLista();
    
    /**
     * @brief Procesa teclas en el contexto de browser de archivos
     * @param tecla Carácter de la tecla presionada
     */
    void procesarTeclaBrowserArchivos(char tecla);

public:
    /**
     * @brief Constructor de la Consola
     * @param miGestorArchivos_ref Referencia al gestor de archivos ya inicializado
     */
    Consola(GestorArchivos &miGestorArchivos_ref);

    /**
     * @brief Inicializa la consola y el display
     */
    void iniciar();
    
    /**
     * @brief Actualiza la consola (llamar en loop)
     * @param tecla Tecla presionada ('\0' si no hay tecla)
     * @param origen_x Origen X del movimiento
     * @param posicion_x Posición actual X
     * @param destino_x Destino X del movimiento
     * @param origen_y Origen Y del movimiento
     * @param posicion_y Posición actual Y
     * @param destino_y Destino Y del movimiento
     * @param origen_z Origen Z del movimiento
     * @param posicion_z Posición actual Z
     * @param destino_z Destino Z del movimiento
     * @param comando_gcode Comando G-code actual
     */
    void actualizar(char tecla, 
                   const float &origen_x, const float &posicion_x, const float &destino_x,
                   const float &origen_y, const float &posicion_y, const float &destino_y,
                   const float &origen_z, const float &posicion_z, const float &destino_z, 
                   const char* comando_gcode);
    
    /**
     * @brief Obtiene el contexto actual de la aplicación
     * @return Contexto actual
     */
    CONTEXTO_APP obtenerContextoActual() const { return contexto_actual; }
    
    // Métodos de prueba
    void pruebaLecturaSD();
    void pruebaLecturaUSB();
    void pruebaLecturaGcode();
    void pruebaAbrirGcodeSD();
};

#endif // CONSOLA_H