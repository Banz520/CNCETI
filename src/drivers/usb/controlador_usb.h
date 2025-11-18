#ifndef CONTROLADOR_USB_H
#define CONTROLADOR_USB_H

#include <Arduino.h>
#include "Ch376msc.h"
#include "constantes.h"

/**
 * @file controlador_usb.h
 * @brief Capa de abstracción para la gestión de archivos USB mediante CH376.
 * 
 * @details Esta clase encapsula las operaciones del módulo CH376 proporcionando
 * una API de alto nivel para lectura de archivos G-code. Optimizada para uso
 * no bloqueante y mínimo consumo de RAM mediante reutilización de buffers.
 */

/**
 * @class ControladorUSB
 * @brief Controlador para gestión de archivos en unidades USB mediante CH376.
 * 
 * Proporciona una interfaz simplificada para:
 * - Detección y validación de dispositivos USB
 * - Listado de archivos en directorio raíz
 * - Apertura/cierre de archivos para lectura
 * - Lectura no bloqueante línea por línea
 * - Consulta de información de archivos (tamaño, posición)
 * 
 * @note Diseñada para integrarse con GestorArchivos como capa de abstracción.
 */
class ControladorUSB {
private:
    Ch376msc& host_usb;               ///< Referencia al controlador CH376 inicializado
    bool archivo_abierto;             ///< Estado del archivo actual
    
    // Buffer estático interno para lectura no bloqueante
    static const uint8_t BUFFER_LINEA_USB = 128;
    char buffer_lectura[BUFFER_LINEA_USB];
    uint8_t indice_buffer;            ///< Posición actual en buffer de lectura
    bool linea_en_progreso;           ///< Flag para lectura incremental

public:
    /**
     * @brief Constructor del controlador USB.
     * @param referencia_host Referencia a instancia Ch376msc ya inicializada
     */
    explicit ControladorUSB(Ch376msc& referencia_host);

    // ========================================
    // MÉTODOS DE ESTADO Y VALIDACIÓN
    // ========================================

    /**
     * @brief Verifica si el dispositivo USB está conectado y montado.
     * @return true si hay dispositivo USB listo para operaciones
     * @note Llama internamente a checkIntMessage() para actualizar estado
     */
    bool dispositivoListo();

    /**
     * @brief Verifica si el shield CH376 está respondiendo correctamente.
     * @return true si el shield está operativo
     */
    bool shieldConectado();

    /**
     * @brief Indica si hay un archivo abierto actualmente.
     * @return true si hay archivo abierto para lectura
     */
    bool archivoAbierto() const { return archivo_abierto; }

    /**
     * @brief Verifica si se alcanzó el final del archivo abierto.
     * @return true si EOF, false en caso contrario
     */
    bool finDeArchivo();

    // ========================================
    // GESTIÓN DE ARCHIVOS
    // ========================================

    /**
     * @brief Lista archivos del directorio raíz del USB.
     * @param callback Función que recibe cada nombre de archivo encontrado
     * @return true si la operación fue exitosa
     * 
     * @note El callback es invocado por cada archivo. Operación relativamente
     * rápida ya que CH376 maneja el listado internamente.
     * 
     * Ejemplo de uso:
     * @code
     * usb.listarArchivos([](const char* nombre) {
     *     Serial.println(nombre);
     * });
     * @endcode
     */
    bool listarArchivos(void (*callback)(const char* nombre));

    /**
     * @brief Abre un archivo por nombre para lectura.
     * @param nombre Nombre del archivo (formato FAT 8.3)
     * @return true si se abrió correctamente
     * 
     * @note Cierra archivo previo si existe. Valida conexión USB antes de abrir.
     */
    bool abrirArchivo(const char* nombre);

    /**
     * @brief Cierra el archivo actualmente abierto.
     * @return true si se cerró correctamente o no había archivo abierto
     */
    bool cerrarArchivo();

    // ========================================
    // LECTURA NO BLOQUEANTE
    // ========================================

    /**
     * @brief Lee una línea completa del archivo de forma no bloqueante.
     * 
     * @param buffer Buffer destino para la línea leída
     * @param tamano_buffer Tamaño máximo del buffer (incluyendo '\0')
     * @return true cuando se completó la lectura de una línea completa
     * 
     * @details Implementación no bloqueante mediante lectura incremental:
     * - Lee pequeños chunks por llamada (8 chars max)
     * - Acumula caracteres hasta encontrar '\n'
     * - Ignora '\r' automáticamente
     * - Retorna false mientras la línea no esté completa
     * - Reutiliza buffer interno para minimizar RAM
     * 
     * @note Debe llamarse repetidamente en loop() hasta que retorne true
     * 
     * Ejemplo de uso:
     * @code
     * char linea[128];
     * if (usb.leerLineaNoBloqueante(linea, sizeof(linea))) {
     *     procesarGcode(linea);
     * }
     * @endcode
     */
    bool leerLineaNoBloqueante(char* buffer, uint8_t tamano_buffer);

    /**
     * @brief Lee un bloque de bytes del archivo.
     * @param buffer Buffer destino
     * @param cantidad Número de bytes a leer
     * @return Número de bytes leídos efectivamente
     * 
     * @note Para uso avanzado. Lectura normal usa leerLineaNoBloqueante()
     */
    uint8_t leerBloque(uint8_t* buffer, uint8_t cantidad);

    // ========================================
    // INFORMACIÓN Y NAVEGACIÓN
    // ========================================

    /**
     * @brief Obtiene el nombre del archivo actualmente abierto.
     * @return Puntero al nombre del archivo o nullptr si no hay archivo abierto
     * @note El puntero es válido mientras el archivo esté abierto
     */
    const char* obtenerNombreArchivoActual();

    /**
     * @brief Obtiene el tamaño total del archivo abierto.
     * @return Tamaño en bytes, 0 si no hay archivo abierto
     */
    size_t obtenerTamanoArchivo();

    /**
     * @brief Obtiene la posición actual del cursor de lectura.
     * @return Posición en bytes desde inicio del archivo
     * @note Útil para calcular progreso: (posición / tamaño) * 100
     */
    uint32_t obtenerPosicionActual();

    /**
     * @brief Reinicia el archivo al inicio para releer.
     * @return true si se reposicionó correctamente
     * @note Implementado mediante reapertura del archivo (limitación CH376)
     */
    bool reiniciarArchivo();

    // ========================================
    // UTILIDADES
    // ========================================

    /**
     * @brief Verifica cambios en conexión USB (attach/detach).
     * @return true si hubo cambio de estado
     * @note Debe llamarse periódicamente para detectar desconexiones
     */
    bool verificarCambioEstado();
};

#endif // CONTROLADOR_USB_H