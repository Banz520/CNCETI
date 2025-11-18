#ifndef CONTROLADOR_SD_H
#define CONTROLADOR_SD_H

#include <SD.h>
#include "constantes.h"

/**
 * @file controlador_sd.h
 * @brief Capa de abstracción para la gestión de archivos en tarjeta SD.
 * 
 * @details Esta clase encapsula las operaciones de la librería SD.h estándar
 * proporcionando una API consistente para lectura de archivos G-code.
 * Optimizada para reutilización de buffers y mínimo consumo de RAM.
 */

/**
 * @class ControladorSD
 * @brief Controlador para gestión de archivos en tarjeta SD.
 * 
 * Proporciona una interfaz simplificada para:
 * - Inicialización y validación de tarjeta SD
 * - Verificación de existencia de archivos
 * - Apertura/cierre de archivos para lectura
 * - Lectura línea por línea
 * - Listado de archivos en directorio raíz
 * - Navegación por archivos
 * 
 * @note Diseñada para integrarse con GestorArchivos como capa de abstracción.
 *       La lectura de líneas puede ser bloqueante dependiendo de la
 *       implementación de la librería SD.h.
 */
class ControladorSD {
private:
    File archivo_actual;      ///< Archivo actualmente abierto
    bool inicializada;        ///< Estado de inicialización de la SD

public:
    /**
     * @brief Constructor del controlador SD.
     */
    ControladorSD();

    /**
     * @brief Destructor: libera recursos y cierra archivos.
     */
    ~ControladorSD();

    // ========================================
    // INICIALIZACIÓN Y ESTADO
    // ========================================

    /**
     * @brief Inicializa la tarjeta SD.
     * @param pin_cs Pin Chip Select para comunicación SPI
     * @return true si la inicialización fue exitosa
     * 
     * @note Debe llamarse una vez al inicio antes de cualquier operación.
     */
    bool iniciar(uint8_t pin_cs);

    /**
     * @brief Verifica si la SD está lista para operaciones.
     * @return true si está inicializada correctamente
     */
    bool estaLista() const;

   /**
     * @brief Indica si hay un archivo abierto actualmente.
     * @return true si hay archivo abierto
     * @note No es const porque File::operator bool() no es const en la librería SD
     */
    bool archivoAbierto() { 
        return archivo_actual; 
    }

    /**
     * @brief Verifica si se alcanzó el final del archivo.
     * @return true si EOF o no hay archivo abierto
     */
    bool esFinDeArchivo();

    // ========================================
    // GESTIÓN DE ARCHIVOS
    // ========================================

    /**
     * @brief Verifica si existe un archivo.
     * @param nombre Nombre completo del archivo (puede incluir ruta)
     * @return true si el archivo existe
     */
    bool existeArchivo(const char* nombre);

    /**
     * @brief Abre un archivo para lectura.
     * @param nombre Nombre del archivo a abrir
     * @return true si se abrió correctamente
     * 
     * @note Cierra archivo previo si existe.
     */
    bool abrirArchivo(const char* nombre);

    /**
     * @brief Cierra el archivo actualmente abierto.
     */
    void cerrarArchivo();

    /**
     * @brief Lista archivos del directorio raíz.
     * @param max_archivos Límite de archivos a listar
     * @return Cantidad de archivos encontrados
     * 
     * @note Imprime la lista por Serial en modo desarrollador.
     */
    size_t listarArchivosRaiz(size_t max_archivos = 20);

    /**
     * @brief Lista archivos de un directorio específico.
     * @param ruta Ruta del directorio ("/" para raíz)
     * @param callback Función que recibe cada nombre de archivo
     * @return true si la operación fue exitosa
     * 
     * Ejemplo de uso:
     * @code
     * sd.listarDirectorio("/", [](const char* nombre) {
     *     Serial.println(nombre);
     * });
     * @endcode
     */
    bool listarDirectorio(const char* ruta, void (*callback)(const char* nombre));

    // ========================================
    // LECTURA
    // ========================================

    /**
     * @brief Lee una línea del archivo actualmente abierto.
     * @param buffer Buffer donde almacenar la línea
     * @param tamano Tamaño máximo del buffer (incluyendo '\0')
     * @return true si se leyó una línea, false si EOF o error
     * 
     * @details Lee hasta encontrar '\n' o llenar el buffer.
     * - Ignora '\r' automáticamente
     * - Añade '\0' al final
     * - Retorna false en EOF
     * 
     * @warning Esta operación puede ser bloqueante dependiendo del tamaño
     *          de la línea y la velocidad de lectura de la tarjeta SD.
     */
    bool leerLineaArchivo(char* buffer, size_t tamano);

    /**
     * @brief Lee un bloque de bytes del archivo.
     * @param buffer Buffer destino
     * @param cantidad Número de bytes a leer
     * @return Número de bytes leídos efectivamente
     */
    int leerBloque(void* buffer, uint16_t cantidad);

    // ========================================
    // NAVEGACIÓN
    // ========================================

    /**
     * @brief Reinicia la lectura al inicio del archivo.
     * @return true si se reposicionó correctamente
     */
    bool reiniciarLectura();

    /**
     * @brief Mueve el cursor a una posición específica.
     * @param posicion Posición en bytes desde el inicio
     * @return true si la operación fue exitosa
     */
    bool moverCursor(uint32_t posicion);

    // ========================================
    // INFORMACIÓN
    // ========================================

    /**
     * @brief Obtiene el tamaño del archivo abierto.
     * @return Tamaño en bytes, 0 si no hay archivo abierto
     */
    size_t obtenerTamanoArchivo();

    /**
     * @brief Obtiene la posición actual del cursor.
     * @return Posición en bytes desde el inicio
     */
    uint32_t obtenerPosicionActual();

    /**
     * @brief Obtiene el nombre del archivo actualmente abierto.
     * @return Puntero al nombre o nullptr si no hay archivo abierto
     */
    const char* obtenerNombreArchivoActual();

    /**
     * @brief Obtiene bytes disponibles para lectura.
     * @return Número de bytes disponibles desde posición actual
     */
    int obtenerBytesDisponibles();
};

#endif // CONTROLADOR_SD_H