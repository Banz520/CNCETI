#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include <Arduino.h>
#include "controlador_sd.h"
#include "controlador_usb.h"
#include "constantes.h"
#include <string.h>

/**
 * @file gestor_archivos.h
 * @brief Gestor unificado de alto nivel para archivos G-code en SD y USB.
 * 
 * @details Esta clase actúa como capa de abstracción sobre los controladores
 * SD y USB, proporcionando una API unificada para:
 * - Detección automática de dispositivos disponibles
 * - Listado y filtrado de archivos G-code
 * - Navegación por listas de archivos
 * - Apertura y lectura no bloqueante de archivos
 * - Gestión transparente del origen de datos (SD/USB)
 * 
 * Diseñada para uso en sistemas de control CNC con mínimo impacto en RAM
 * mediante reutilización de buffers y estructuras estáticas.
 */

/**
 * @enum TipoDispositivo
 * @brief Identifica el origen actual de archivos.
 */
enum class TipoDispositivo : uint8_t {
    NINGUNO = 0,  ///< Ningún dispositivo disponible
    SD,           ///< Tarjeta SD
    USB           ///< Unidad USB
};

/**
 * @class GestorArchivos
 * @brief Gestor de alto nivel para archivos G-code desde SD o USB.
 * 
 * Funcionalidades principales:
 * - Escaneo de directorios con filtrado de extensiones G-code
 * - Lista interna de archivos con navegación tipo menú
 * - Apertura de archivos por índice o nombre
 * - Lectura no bloqueante línea por línea (optimizada para USB)
 * - Cambio dinámico entre dispositivos SD/USB
 * - Validación automática de extensiones G-code (.gco, .gcode, .gc)
 * 
 * @note Esta clase NO maneja la inicialización de los controladores subyacentes.
 *       Los controladores deben ser inicializados externamente y pasados por referencia.
 */
class GestorArchivos {
public:
    /**
     * @brief Constructor del Gestor de Archivos.
     * @param refSD Referencia a controlador SD ya inicializado
     * @param refUSB Referencia a controlador USB ya inicializado
     */
    GestorArchivos(ControladorSD &refSD, ControladorUSB &refUSB);

    // ========================================
    // INICIALIZACIÓN Y CONFIGURACIÓN
    // ========================================

    /**
     * @brief Inicializa el gestor detectando dispositivos disponibles.
     * @return true si hay al menos un dispositivo disponible (SD o USB)
     * 
     * @details Establece el origen predeterminado con la siguiente prioridad:
     * 1. USB (si está conectado y montado)
     * 2. SD (si está inicializada)
     * 3. NINGUNO (si no hay dispositivos)
     */
    bool inicializar();

    /**
     * @brief Obtiene el dispositivo actualmente seleccionado.
     * @return Tipo de dispositivo actual
     */
    TipoDispositivo obtenerOrigen() const;

    /**
     * @brief Cambia manualmente el origen de archivos.
     * @param nuevo_origen Dispositivo a seleccionar (SD/USB)
     * 
     * @note Limpia la lista interna de archivos al cambiar de origen.
     */
    void cambiarOrigen(TipoDispositivo nuevo_origen);

    // ========================================
    // LISTADO Y NAVEGACIÓN
    // ========================================

    /**
     * @brief Escanea un directorio y construye lista de archivos G-code.
     * @param ruta_directorio Ruta a escanear ("/" para raíz, nullptr usa raíz)
     * @return true si se encontraron archivos G-code
     * 
     * @details Operación en una sola llamada que:
     * - Limpia lista interna previa
     * - Escanea el directorio especificado
     * - Filtra solo archivos con extensión G-code válida
     * - Almacena nombres en buffer interno (máx MAX_ARCHIVOS)
     * - Establece índice de selección en 0
     * 
     * @note Para CH376 (USB) esta operación es rápida.
     *       Para SD recorre el directorio usando openNextFile().
     */
    bool escanearDirectorio(const char* ruta_directorio = "/");

    /**
     * @brief Obtiene el número de archivos G-code en la lista actual.
     * @return Cantidad de archivos encontrados en último escaneo
     */
    size_t obtenerCantidadArchivos() const;

    /**
     * @brief Obtiene el nombre de un archivo por su índice.
     * @param indice Posición en la lista (0 a obtenerCantidadArchivos()-1)
     * @return Puntero al nombre del archivo o nullptr si índice inválido
     * 
     * @note El puntero es válido hasta el próximo escaneo o cambio de origen.
     */
    const char* obtenerNombreArchivo(size_t indice) const;

    /**
     * @brief Navega por la lista de archivos (tipo menú).
     * @param direccion +1 para siguiente, -1 para anterior
     * 
     * @details Implementa navegación circular:
     * - Al llegar al final, vuelve al inicio
     * - Al llegar al inicio, va al final
     */
    void navegarLista(int direccion);

    /**
     * @brief Obtiene el índice actualmente seleccionado.
     * @return Índice del archivo seleccionado
     */
    size_t obtenerIndiceSeleccion() const;

    // ========================================
    // GESTIÓN DE ARCHIVOS
    // ========================================

    /**
     * @brief Abre un archivo por su índice en la lista.
     * @param indice Posición del archivo en la lista
     * @return true si se abrió correctamente
     * 
     * @details Cierra archivo previo si existe.
     *          Actualiza el índice de selección si tiene éxito.
     */
    bool abrirArchivoPorIndice(size_t indice);

    /**
     * @brief Abre un archivo por nombre.
     * @param nombre Nombre del archivo (insensible a mayúsculas)
     * @return true si el archivo fue encontrado y abierto
     * 
     * @details Busca el archivo en la lista actual usando strcasecmp.
     *          Valida que la extensión sea G-code válida.
     *          Cierra archivo previo si existe.
     * 
     * @note El archivo debe estar presente en la lista actual
     *       (debe haberse ejecutado escanearDirectorio previamente).
     */
    bool abrirArchivoPorNombre(const char* nombre);

    /**
     * @brief Cierra el archivo actualmente abierto.
     */
    void cerrarArchivo();

    /**
     * @brief Indica si hay un archivo abierto.
     * @return true si hay archivo abierto para lectura
     */
    bool hayArchivoAbierto() const;

    // ========================================
    // LECTURA NO BLOQUEANTE
    // ========================================

    /**
     * @brief Lee la siguiente línea de G-code del archivo abierto.
     * @return Puntero a buffer interno con la línea o nullptr
     * 
     * @details Implementación no bloqueante:
     * - Para USB: lectura incremental sin bloquear el loop
     * - Para SD: lectura completa de línea (puede bloquear brevemente)
     * 
     * Filtrado automático:
     * - Ignora líneas vacías
     * - Ignora comentarios (líneas que inician con ';')
     * - Elimina espacios iniciales
     * 
     * @return Puntero a buffer interno o nullptr si:
     *         - No hay archivo abierto
     *         - La línea no está completa aún (USB)
     *         - Se alcanzó EOF
     *         - La línea es vacía o comentario
     * 
     * @warning El puntero es válido hasta la próxima llamada a este método.
     *          Copiar datos si se necesita persistencia.
     * 
     * Ejemplo de uso:
     * @code
     * void loop() {
     *     const char* linea = gestor.leerLineaNoBloqueante();
     *     if (linea) {
     *         procesarComandoGcode(linea);
     *     }
     * }
     * @endcode
     */
    const char* leerLineaNoBloqueante();

    /**
     * @brief Reinicia la lectura del archivo actual al inicio.
     * @return true si se reposicionó correctamente
     * 
     * @note Para USB esto implica reabrir el archivo (limitación CH376).
     *       Para SD usa seek(0).
     */
    bool reiniciarLecturaActual();

    // ========================================
    // INFORMACIÓN
    // ========================================

    /**
     * @brief Obtiene el tamaño del archivo actualmente abierto.
     * @return Tamaño en bytes, 0 si no hay archivo abierto
     */
    size_t obtenerTamanoArchivoActual() const;

    /**
     * @brief Obtiene la posición actual de lectura.
     * @return Posición en bytes desde el inicio
     */
    uint32_t obtenerPosicionArchivoActual() const;

    /**
     * @brief Calcula el progreso de lectura del archivo.
     * @return Porcentaje de progreso (0-100)
     */
    uint8_t calcularPorcentajeProgreso() const;

private:
    ControladorSD &sd;        ///< Referencia al controlador SD
    ControladorUSB &usb;      ///< Referencia al controlador USB

    TipoDispositivo origen_actual;  ///< Dispositivo actualmente seleccionado

    // Lista interna de nombres de archivo
    static const size_t NOMBRE_LONGITUD = 32;
    char lista_nombres[MAX_ARCHIVOS][NOMBRE_LONGITUD];
    size_t total_archivos;    ///< Cantidad de archivos en lista
    size_t indice_seleccion;  ///< Índice seleccionado actualmente

    // Buffer para lectura de líneas
    static const size_t LINE_BUFFER_SZ = 256;
    char linea_buffer[LINE_BUFFER_SZ];

    bool archivo_abierto;     ///< Flag de archivo abierto

    // ========================================
    // MÉTODOS AUXILIARES PRIVADOS
    // ========================================

    /**
     * @brief Verifica si un nombre tiene extensión G-code válida.
     * @param nombre Nombre del archivo a verificar
     * @return true si la extensión es .gco, .gcode o .gc (case-insensitive)
     */
    bool esGcodeNombre(const char* nombre) const;

    /**
     * @brief Limpia la lista interna de archivos.
     */
    void limpiarListaInterna();

    /**
     * @brief Adiciona un nombre a la lista interna.
     * @param nombre Nombre del archivo a adicionar
     */
    void adicionarNombreLista(const char* nombre);

    /**
     * @brief Lee una línea desde USB (wrapper para controlador).
     * @return true si la línea está completa
     */
    bool leerLineaDesdeUSB();

    /**
     * @brief Lee una línea desde SD (wrapper para controlador).
     * @return true si se leyó una línea válida
     */
    bool leerLineaDesdeSD();

    /**
     * @brief Lista archivos de USB y los adiciona a la lista interna.
     * @param ruta Ruta del directorio
     * @return true si se encontraron archivos
     */
    bool listarArchivosUSB(const char* ruta);

    /**
     * @brief Lista archivos de SD y los adiciona a la lista interna.
     * @param ruta Ruta del directorio
     * @return true si se encontraron archivos
     */
    bool listarArchivosSD(const char* ruta);
};

#endif // GESTOR_ARCHIVOS_H