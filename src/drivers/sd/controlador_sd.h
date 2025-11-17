#ifndef CONTROLADOR_SD_H
#define CONTROLADOR_SD_H

#include <SD.h>
#include "constantes.h"

/**
 * @class ControladorSD
 * @brief Encapsula la interacción de bajo nivel con la tarjeta SD usando la librería estándar SD.h
 * @details Esta clase se encarga únicamente de la comunicación con el hardware SD:
 *           - Inicialización del módulo
 *           - Verificación de conexión y disponibilidad
 *           - Apertura, lectura y cierre de archivos
 *           - Listado de archivos en el directorio raíz
 *           Toda la lógica de filtrado o manipulación de archivos G-code se maneja en el GestorArchivos.
 */
class ControladorSD {
private:
    File archivo_actual;      ///< Archivo actualmente abierto para lectura
    bool inicializada;        ///< Indica si la SD fue inicializada correctamente

public:
    /**
     * @brief Constructor de la clase ControladorSD
     */
    ControladorSD();

    /**
     * @brief Inicializa la tarjeta SD
     * @param pin_cs Pin Chip Select de la tarjeta SD
     * @return true si la inicialización fue exitosa
     */
    bool iniciar(uint8_t pin_cs);

    /**
     * @brief Verifica si la SD está lista para operaciones
     * @return true si está inicializada correctamente
     */
    bool estaLista() const;

    /**
     * @brief Verifica si existe un archivo con el nombre dado
     * @param nombre Nombre completo del archivo
     * @return true si el archivo existe
     */
    bool existeArchivo(const char* nombre);

    /**
     * @brief Abre un archivo en modo lectura
     * @param nombre Nombre del archivo a abrir
     * @return true si se abrió correctamente
     */
    bool abrirArchivo(const char* nombre);

    /**
     * @brief Cierra el archivo actualmente abierto
     */
    void cerrarArchivo();

    /**
     * @brief Lee una línea del archivo actualmente abierto
     * @param buffer Buffer donde se almacenará la línea
     * @param tamano Tamaño máximo del buffer
     * @return true si se leyó una línea correctamente, false si EOF o error
     */
    bool leerLineaArchivo(char* buffer, size_t tamano);

    /**
     * @brief Reinicia la lectura del archivo al inicio
     * @return true si se logró reposicionar correctamente
     */
    bool reiniciarLectura();

    /**
     * @brief Obtiene el tamaño del archivo actualmente abierto
     * @return Tamaño del archivo en bytes, 0 si no hay archivo abierto
     */
    size_t obtenerTamanoArchivo();

    /**
     * @brief Verifica si se alcanzó el fin del archivo
     * @return true si EOF
     */
    bool esFinDeArchivo();

    /**
     * @brief Lista los archivos del directorio raíz
     * @param max_archivos Máximo número de archivos a listar
     * @return Cantidad de archivos encontrados
     */
    size_t listarArchivosRaiz(size_t max_archivos = 20);

    /**
     * @brief Destructor: cierra archivos y libera recursos
     */
    ~ControladorSD();
};

#endif // CONTROLADOR_SD_H
