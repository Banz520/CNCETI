#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include <Arduino.h>
#include "controlador_sd.h"
#include "controlador_usb.h"
#include "constantes.h"
#include <string.h>

/**
 * @file gestor_archivos.h
 * @brief Gestor unificado para SD y USB. Expone API para listar, navegar y leer G-code.
 *
 * @note Todas las salidas de depuración están encapsuladas con #if MODO_DESARROLLADOR
 *       y usan F() donde es apropiado para reducir uso de RAM.
 */

/**
 * @enum TipoDispositivo
 * @brief Identifica el origen actual de archivos (ninguno, SD o USB).
 */
enum class TipoDispositivo : uint8_t {
    NINGUNO = 0,
    SD,
    USB
};

/**
 * @class GestorArchivos
 * @brief Administra archivos G-code desde SD o USB y provee API no bloqueante (para USB).
 *
 * Responsable de:
 *  - Escanear directorios y construir lista de archivos G-code (buffers locales).
 *  - Navegación simple entre archivos.
 *  - Abrir/ cerrar archivos en el dispositivo seleccionado.
 *  - Leer líneas de G-code (no bloqueante con USB; "best-effort" para SD - ver nota).
 *
 * Integración: inyectar referencias a ControladorSD y ControladorUSB (ya inicializados).
 */
class GestorArchivos {
public:
    /**
     * @brief Constructor del Gestor de Archivos
     * @param refSD Referencia a controlador SD ya inicializado
     * @param refUSB Referencia a controlador USB ya inicializado
     */
    GestorArchivos(ControladorSD &refSD, ControladorUSB &refUSB);

    /**
     * @brief Inicializa el gestor (detecta dispositivo disponible)
     * @return true si hay al menos un dispositivo disponible (SD o USB)
     */
    bool inicializar();

    /**
     * @brief Obtiene el dispositivo actualmente seleccionado.
     */
    TipoDispositivo obtenerOrigen() const;

    /**
     * @brief Forzar cambio de origen (SD/USB)
     */
    void cambiarOrigen(TipoDispositivo nuevo_origen);

    /**
     * @brief Escanea el directorio especificado (o raíz si nullptr) y construye lista interna de archivos G-code.
     * @param ruta_directorio Ruta a escanear ("/" para raíz). Si nullptr se usa root.
     * @return true si se encontraron archivos G-code (lista interna no vacía).
     *
     * Nota: la operación actual realiza el escaneo en una sola llamada. Con CH376 la listación es rápida.
     * Para SD esta operación recorre el directorio usando SD.open() / openNextFile().
     */
    bool escanearDirectorio(const char* ruta_directorio = "/");

    /**
     * @brief Obtiene el número de archivos G-code encontrados por el último escaneo.
     */
    size_t obtenerCantidadArchivos() const;

    /**
     * @brief Obtiene el nombre de archivo en el índice (0..obtenerCantidadArchivos()-1)
     * @return puntero interno (no modificar) o nullptr si indice inválido.
     */
    const char* obtenerNombreArchivo(size_t indice) const;

    /**
     * @brief Navegar por la lista (direccion: +1 siguiente, -1 anterior)
     */
    void navegarLista(int direccion);

    /**
     * @brief Obtiene el índice actualmente seleccionado en la lista
     */
    size_t obtenerIndiceSeleccion() const;

    /**
     * @brief Abre el archivo seleccionado por indice para lectura
     * @return true si se abrió correctamente en el dispositivo activo
     */
    bool abrirArchivoPorIndice(size_t indice);

    /**
     * @brief Cierra el archivo actualmente abierto (si lo hay)
     */
    void cerrarArchivo();

    /**
     * @brief Lectura de la siguiente línea de G-code de forma no bloqueante cuando sea posible.
     * @return puntero a buffer interno con línea válida (sin \n ni \r) o nullptr si no hay línea lista o EOF/error.
     *
     * Implementación:
     *  - Para USB usa ControladorUSB::leerLineaNoBloqueante (no bloqueante).
     *  - Para SD usa ControladorSD::leerLineaArchivo (puede bloquear dependiendo de la implementación SD).
     *  - En ambos casos las líneas vacías o comentarios (';') son ignoradas y no devueltas.
     */
    const char* leerLineaNoBloqueante();

    /**
     * @brief Reabre / reinicia lectura del archivo actualmente seleccionado (volver al inicio).
     * @return true si la operación fue exitosa.
     */
    bool reiniciarLecturaActual();

    /**
     * @brief Indica si hay un archivo abierto en el dispositivo activo.
     */
    bool hayArchivoAbierto() const;

    /**
     * @brief Devuelve el tamaño del archivo abierto (0 si no hay archivo).
     */
    size_t obtenerTamanoArchivoActual() const;

    /**
     * @brief Devuelve la posición de lectura actual del archivo abierto (bytes).
     */
    uint32_t obtenerPosicionArchivoActual() const;

        /**
     * @brief Abre un archivo G-code por nombre en el dispositivo activo.
     * @param nombre Nombre del archivo (FAT corto, por ejemplo "PIEZA1.GCO")
     * @return true si el archivo fue encontrado y abierto correctamente
     *
     * @note La búsqueda es insensible a mayúsculas/minúsculas (strcasecmp)
     *       y valida que sea un archivo G-code (.gco / .gcode / .gc).
     */
    bool abrirArchivoPorNombre(const char* nombre);

private:
    ControladorSD &sd;     ///< referencia al controlador SD
    ControladorUSB &usb;   ///< referencia al controlador USB

    TipoDispositivo origen_actual;

    // Lista interna de nombres (copias de los nombres FAT8.3 devueltos por SD/USB)
    static const size_t NOMBRE_LONGITUD = 32; // suficiente para nombres FAT cortos
    char lista_nombres[MAX_ARCHIVOS][NOMBRE_LONGITUD];
    size_t total_archivos;
    size_t indice_seleccion;   // índice actualmente seleccionado en la lista

    // Buffer interno para lectura de línea
    static const size_t LINE_BUFFER_SZ = 256;
    char linea_buffer[LINE_BUFFER_SZ];

    // Estado de archivo abierto
    bool archivo_abierto;

    // Métodos auxiliares
    bool esGcodeNombre(const char* nombre) const;
    void limpiarListaInterna();
    void adicionarNombreLista(const char* nombre);

    // Helpers para lectura: wrapper para SD/USB
    bool leerLineaDesdeUSB();
    bool leerLineaDesdeSD();

    // Para navegación por directorio (simple)
    bool listarArchivosUSB(const char* ruta);
    bool listarArchivosSD(const char* ruta);
};

#endif // GESTOR_ARCHIVOS_H
