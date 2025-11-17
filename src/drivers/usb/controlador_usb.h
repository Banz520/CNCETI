#ifndef CONTROLADOR_USB_H
#define CONTROLADOR_USB_H
#include <Arduino.h>
#include "ch376msc.h"
#include "constantes.h"

/**
 * @brief Controlador para la gestión de archivos en una unidad USB mediante CH376.
 * 
 * Esta clase encapsula las operaciones básicas de lectura y gestión de archivos
 * en un dispositivo USB conectado al módulo CH376, utilizando una referencia
 * a una instancia ya inicializada del controlador CH376MSC.
 * 
 * Su diseño está pensado para integrarse dentro del GestorArchivos, permitiendo
 * lectura no bloqueante y lógica unificada con otros dispositivos (como SD).
 */
class ControladorUSB {
private:
    Ch376msc& host_usb;        ///< Referencia al objeto CH376MSC ya inicializado
    bool archivo_abierto = false; ///< Indica si hay un archivo actualmente abierto

public:
    static const uint8_t TAMANO_BUFFER = 128; ///< Tamaño máximo de buffer de lectura

    /**
     * @brief Constructor del controlador USB.
     * @param referencia_host Referencia a una instancia inicializada de CH376MSC.
     */
    explicit ControladorUSB(Ch376msc& referencia_host);

    // ===========================================================
    // MÉTODOS DE ESTADO Y CONFIGURACIÓN
    // ===========================================================

    /**
     * @brief Verifica si el dispositivo USB está conectado y listo.
     * @return true si el dispositivo está montado y accesible, false si no.
     */
    bool dispositivoListo();

    /**
     * @brief Indica si hay un archivo abierto actualmente.
     * @return true si hay archivo abierto, false en caso contrario.
     */
    bool archivoAbierto() const { return archivo_abierto; }

    /**
     * @brief Verifica si se ha alcanzado el final del archivo abierto.
     * @return true si el archivo llegó al EOF, false en caso contrario.
     */
    bool finDeArchivo();

    // ===========================================================
    // MÉTODOS DE GESTIÓN DE ARCHIVOS
    // ===========================================================

    /**
     * @brief Lista los archivos del directorio raíz del USB.
     * @param callback Función callback que recibe el nombre de cada archivo encontrado.
     * @return true si la operación se realizó correctamente, false si hubo error.
     * @note Este método itera de forma bloqueante pero rápida, ya que el CH376
     *       devuelve la lista de archivos preprocesada internamente.
     */
    bool listarArchivos(void (*callback)(const char* nombre));

    /**
     * @brief Abre un archivo por nombre.
     * @param nombre Nombre del archivo a abrir.
     * @return true si el archivo se abrió correctamente, false si falló.
     */
    bool abrirArchivo(const char* nombre);

    /**
     * @brief Cierra el archivo actualmente abierto.
     * @return true si se cerró correctamente o no había archivo abierto.
     */
    bool cerrarArchivo();

    // ===========================================================
    // MÉTODOS DE LECTURA
    // ===========================================================

    /**
     * @brief Lee una línea de texto del archivo de forma no bloqueante.
     * 
     * Lee progresivamente una línea de texto del archivo abierto, sin bloquear
     * el flujo del programa. Retorna true únicamente cuando la línea completa
     * está lista para procesarse.
     * 
     * @param buffer Buffer donde almacenar la línea leída.
     * @param tamano_buffer Tamaño máximo del buffer (incluyendo el terminador nulo).
     * @return true si se completó la lectura de una línea, false si aún no.
     */
    bool leerLineaNoBloqueante(char* buffer, uint8_t tamano_buffer);

    // ===========================================================
    // MÉTODOS DE INFORMACIÓN
    // ===========================================================

    /**
     * @brief Obtiene el nombre del archivo actualmente abierto.
     * @return Puntero al nombre del archivo o nullptr si no hay archivo abierto.
     */
    const char* obtenerNombreArchivoActual();

    /**
     * @brief Obtiene el tamaño total del archivo abierto.
     * @return Tamaño en bytes, o 0 si no hay archivo abierto.
     */
    size_t obtenerTamanoArchivo();

    /**
     * @brief Obtiene la posición actual del puntero de lectura.
     * @return Posición actual en bytes desde el inicio del archivo.
     */
    uint32_t obtenerPosicionActual();
};

#endif