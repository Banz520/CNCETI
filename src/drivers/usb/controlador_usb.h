#ifndef CONTROLADOR_USB_H
#define CONTROLADOR_USB_H

#include <Ch376msc.h>
#include "constantes.h"





/**
 * @class ControladorUSB
 * @brief Clase que encapsula la logica para manejar dispositivos USB FAT mediante el controlador CH376
 * 
 * Esta clase proporciona una interfaz de alto nivel para operaciones de archivos FAT
 * en sistemas embebidos, con funcionalidades específicas para archivos GCODE.
 */
class ControladorUSB {
private:
    Ch376msc host_usb;                          ///< Instancia del controlador USB CH376
    bool dispositivo_listo;                     ///< Estado del dispositivo USB
    bool usando_uart;                           ///< Indica si se esta usando UART
    
    // Buffers para operaciones eficientes en memoria
    char buffer_nombre_archivo[LONGITUD_NOMBRE_ARCHIVO]; ///< Buffer para nombre de archivo
    char buffer_lectura[64];                    ///< Buffer pequeño para lectura
    uint8_t buffer_raw[64];                     ///< Buffer para datos raw

    /**
     * @brief Verifica si un archivo tiene extension .gcode
     * @param nombre_archivo Nombre del archivo a verificar
     * @return true si es archivo .gcode, false en caso contrario
     */
    bool EsArchivoGcode(const char* nombre_archivo);

    /**
     * @brief Imprime informacion detallada del archivo actual (solo modo desarrollador)
     */
    void ImprimirInfoArchivo();

    /**
     * @brief Configura el controlador CH376 para comunicacion UART
     * @return true si la configuracion fue exitosa, false en caso contrario
     */
    bool ConfigurarUART();

public:
    /**
     * @brief Estructura para almacenar lista de archivos
     */
    typedef struct {
        const char* archivos[MAX_ARCHIVOS];     ///< Array de punteros a nombres de archivo
        uint8_t cantidad;                       ///< Número de archivos en la lista
    } ListaArchivos;

    /**
     * @brief Constructor para interfaz UART por hardware
     * @param puerto_serial Referencia al puerto serial hardware
     * @param velocidad Velocidad de comunicacion (9600, 19200, 57600, 115200)
     */
    ControladorUSB(HardwareSerial &puerto_serial, uint32_t velocidad = VELOCIDAD_UART_DEFECTO);
    
    /**
     * @brief Constructor para interfaz UART por software
     * @param puerto_serial Referencia al puerto serial software  
     * @param velocidad Velocidad de comunicacion (debe coincidir con la configurada en el modulo)
     */
    ControladorUSB(Stream &puerto_serial);

    /**
     * @brief Destructor
     */
    ~ControladorUSB();

    /**
     * @brief Inicializa el controlador USB
     * @return true si la inicializacion fue exitosa, false en caso contrario
     */
    bool Inicializar();

    /**
     * @brief Verifica si el dispositivo USB esta listo para operaciones
     * @return true si el dispositivo esta listo, false en caso contrario
     */
    bool DispositivoListo();

    /**
     * @brief Lee todos los archivos del directorio raíz y los lista
     * @param lista_archivos Estructura donde se almacenaran los nombres de archivos
     * @return true si la operacion fue exitosa, false en caso contrario
     */
    bool LeerDirectoriosRaiz(ListaArchivos* lista_archivos);

    /**
     * @brief Busca archivos con extension .gcode en el directorio raíz
     * @param lista_gcode Estructura donde se almacenaran los nombres de archivos GCODE
     * @return true si la operacion fue exitosa, false en caso contrario
     */
    bool BuscarArchivosGcode(ListaArchivos* lista_gcode);

    /**
     * @brief Busca archivos con extension .gcode en un directorio específico
     * @param ruta_directorio Ruta del directorio a buscar
     * @param lista_gcode Estructura donde se almacenaran los nombres de archivos GCODE
     * @return true si la operacion fue exitosa, false en caso contrario
     */
    bool BuscarArchivosGcodeEnDirectorio(const char* ruta_directorio, ListaArchivos* lista_gcode);

    /**
     * @brief Obtiene informacion del sistema de archivos
     * @param buffer Buffer donde se almacenara la informacion
     * @param tamano_buffer Tamaño del buffer
     * @return true si se obtuvo la informacion correctamente, false en caso contrario
     */
    bool ObtenerInfoSistemaArchivos(char* buffer, uint16_t tamano_buffer);

    /**
     * @brief Abre un archivo para lectura
     * @param nombre_archivo Nombre del archivo a abrir
     * @return true si se abrio correctamente, false en caso contrario
     */
    bool AbrirArchivo(const char* nombre_archivo);

    /**
     * @brief Cierra el archivo actualmente abierto
     * @return true si se cerro correctamente, false en caso contrario
     */
    bool CerrarArchivo();

    /**
     * @brief Lee una línea del archivo actualmente abierto
     * @param buffer Buffer donde almacenar la línea leída
     * @param tamano_buffer Tamaño del buffer
     * @return true si se leyo correctamente, false en caso de error o EOF
     */
    bool LeerLineaArchivo(char* buffer, uint8_t tamano_buffer);

    /**
     * @brief Obtiene el último codigo de error
     * @return Codigo de error del controlador USB
     */
    uint8_t ObtenerUltimoError();

    /**
     * @brief Libera la memoria utilizada por una lista de archivos
     * @param lista_archivos Puntero a la estructura de lista de archivos
     */
    void LiberarListaArchivos(ListaArchivos* lista_archivos);

    /**
     * @brief Verifica la comunicacion basica con el modulo CH376
     * @return true si responde correctamente, false en caso contrario
     */
    bool VerificarComunicacion();
};

#endif // CONTROLADOR_USB_H