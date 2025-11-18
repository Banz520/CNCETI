#ifndef CONSTANTES_H
#define CONSTANTES_H

// =============================================================================
// CONFIGURACIÓN DE DEPURACIÓN
// =============================================================================

/**
 * @brief Activa/desactiva mensajes de depuración por Serial
 * 
 * Cuando está en 1, todas las clases imprimen información de depuración
 * usando Serial.print() dentro de bloques #if MODO_DESARROLLADOR
 * 
 * @note Desactivar en producción para:
 *       - Ahorrar memoria Flash (~2KB)
 *       - Ahorrar memoria RAM (strings)
 *       - Mejorar rendimiento (sin prints)
 */

#define MODO_DESARROLLADOR 0

/**
 * @def DISPLAY_ANCHO
 * @brief Tamano de pixeles a lo ancho del display TFT 3.5"
 */
#define DISPLAY_ANCHO 480

/**
 * @def DISPLAY_ALTO
 * @brief Tamano de pixeles a lo alto del display TFT 3.5"
 */
#define DISPLAY_ALTO 320
//La api ya lo establece con orientacion horizontal (Landscape)
//#define DISPLAY_ORIENTACION 3

#define COLORES_INVERTIDOS

#ifdef COLORES_INVERTIDOS
//Nota: por alguna razon el controlador del driver invierte los colores
#define COLOR_BLANCO 0x0000
#define COLOR_NEGRO 0xFFFF
#define COLOR_GRIS_OSCURO 0xce99    // Invertido de 0x39a7
#define COLOR_GRIS_CLARO 0x52aa     // Invertido de 0xc5f8  
#define COLOR_ROJO 0x07ff         // Invertido de 0xf800
#define COLOR_AZUL 0xe545         // Invertido de 0x1aba
#define COLOR_VERDE 0xf11e        // Invertido de 0x0ee1
#endif

#ifndef COLORES_INVERTIDOS
//Estos son los colores no invertidos
#define COLOR_BLANCO 0xFFFF
#define COLOR_NEGRO 0x0000
#define COLOR_GRIS_OSCURO 0x39a7    
#define COLOR_GRIS_CLARO 0xc5f8      
#define COLOR_ROJO 0xf800         
#define COLOR_AZUL 0x1aba       
#define COLOR_VERDE 0x0ee1        
#endif

/**
 * @def MAX_ARCHIVOS
 * @brief Numero maximo de archivos que se pueden almacenar en la lista
 */
#define MAX_ARCHIVOS 48

/**
 * @def LONGITUD_NOMBRE_ARCHIVO
 * @brief Longitud maxima para nombres de archivo (8.3 + null terminator)
 */
#define LONGITUD_NOMBRE_ARCHIVO 13

/**
 * @def VELOCIDAD_UART_DEFECTO
 * @brief Velocidad por defecto para comunicacion UART
 */
#define VELOCIDAD_UART_DEFECTO 9600

/**
 * @def VELOCIDAD_SERIAL2
 * @brief Velocidad en Baudios para la comunicacion Serial con el modulo del puerto USB
 */
#define VELOCIDAD_SERIAL2_USB 115200

/**
 * @file constantes.h
 * @brief Constantes globales del sistema de control CNC
 * 
 * Este archivo centraliza todas las constantes de configuración
 * para facilitar ajustes y mantener consistencia.
 */





// =============================================================================
// SISTEMA DE ARCHIVOS
// =============================================================================

/**
 * @brief Número máximo de archivos G-code en la lista del gestor
 * 
 * Impacto en RAM: MAX_ARCHIVOS * 32 bytes (nombres)
 * 
 * Ejemplos:
 * - 16 archivos = 512 bytes RAM
 * - 32 archivos = 1024 bytes RAM
 * - 64 archivos = 2048 bytes RAM
 * 
 * @note Para Arduino Mega (8KB RAM), 16-32 es un buen equilibrio
 */
#define MAX_ARCHIVOS 16

/**
 * @brief Tamaño máximo de buffer para líneas de G-code
 * 
 * La mayoría de líneas G-code son < 80 caracteres, pero algunos
 * comentarios o parámetros complejos pueden ser más largos.
 * 
 * @note 256 bytes cubre >99% de casos reales
 */
#define TAMANO_BUFFER_LINEA 256

/**
 * @brief Extensiones de archivo G-code soportadas
 * 
 * El sistema reconoce automáticamente estas extensiones
 * (case-insensitive)
 */
#define EXT_GCODE_1 ".gcode"
#define EXT_GCODE_2 ".gco"
#define EXT_GCODE_3 ".gc"

// =============================================================================
// HARDWARE - PINES
// =============================================================================

// ---------------------
// Módulo SD
// ---------------------

/**
 * @brief Pin Chip Select para tarjeta SD
 * 
 * Arduino Mega típicamente usa:
 * - Pin 53 para CS (si se usa shield)
 * - Pin 10 para CS (si es módulo independiente)
 */
#define PIN_SD_CS 53

/**
 * @brief Pines SPI fijos en Arduino Mega
 * 
 * Estos pines son usados automáticamente por la librería SD.h
 * No requieren configuración manual, pero se documentan aquí:
 * 
 * - MOSI: Pin 51
 * - MISO: Pin 50
 * - SCK:  Pin 52
 * - CS:   Pin 53 (configurable)
 */

// ---------------------
// Módulo USB (CH376)
// ---------------------

/**
 * @brief Pin Chip Select para CH376 (SPI)
 */
#define PIN_CH376_CS 49

/**
 * @brief Pin de interrupción para CH376
 * 
 * Arduino Mega pines de interrupción:
 * - INT0: Pin 2
 * - INT1: Pin 3
 * - INT2: Pin 21
 * - INT3: Pin 20
 * - INT4: Pin 19
 * - INT5: Pin 18
 * 
 * @note El CH376 funciona mejor con interrupciones habilitadas
 */
#define PIN_CH376_INT 2

/**
 * @brief Velocidad SPI para CH376
 * 
 * Valores típicos:
 * - SPI_SCK_KHZ(125)   : 125 kHz  (más confiable)
 * - SPI_SCK_KHZ(1000)  : 1 MHz    (velocidad media)
 * - SPI_SCK_KHZ(2000)  : 2 MHz    (más rápido)
 * 
 * @note Valores más altos pueden causar errores con cables largos
 */
#define CH376_SPI_SPEED SPI_SCK_KHZ(125)

// =============================================================================
// TIMEOUTS Y TEMPORIZACIÓN
// =============================================================================

/**
 * @brief Timeout para esperar respuesta de CH376 (ms)
 */
#define TIMEOUT_CH376_MS 1000

/**
 * @brief Timeout para inicialización de SD (ms)
 */
#define TIMEOUT_SD_INIT_MS 2000

/**
 * @brief Delay entre intentos de reconexión USB (ms)
 */
#define DELAY_RECONEXION_USB_MS 500

/**
 * @brief Intervalo para verificar cambios en dispositivos (ms)
 * 
 * Cada cuánto verificar si se conectó/desconectó USB
 */
#define INTERVALO_CHECK_DISPOSITIVOS_MS 1000

// =============================================================================
// LÍMITES Y VALIDACIONES
// =============================================================================

/**
 * @brief Longitud máxima de nombre de archivo (incluyendo '\0')
 * 
 * FAT 8.3: 8 chars + punto + 3 ext + null = 13 bytes
 * FAT LFN: Hasta 255 chars, pero CH376 solo soporta 8.3
 * 
 * @note 32 bytes da margen para rutas relativas si se implementan
 */
#define MAX_LONGITUD_NOMBRE_ARCHIVO 32

/**
 * @brief Tamaño máximo de chunk para lectura no bloqueante USB
 * 
 * Cantidad de caracteres a leer por ciclo de loop en modo no bloqueante
 * 
 * Valores recomendados:
 * - 4-8:  Muy responsive, múltiples ciclos por línea
 * - 16-32: Balance rendimiento/responsividad
 * - 64+:  Más rápido pero puede bloquear brevemente
 */
#define CHUNK_LECTURA_USB 8

// =============================================================================
// CÓDIGOS DE RESPUESTA CH376
// =============================================================================

/**
 * @brief Códigos de respuesta del módulo CH376
 * 
 * Estos códigos son retornados por las funciones del controlador USB
 * y deben ser comparados para validar operaciones.
 * 
 * @note Definidos aquí para referencia. La librería Ch376msc.h
 *       contiene las definiciones oficiales.
 */

 /*
#ifndef ANSW_USB_INT_SUCCESS
    #define ANSW_USB_INT_SUCCESS 0x14  ///< Operación exitosa
#endif

#ifndef ANSW_USB_INT_CONNECT
    #define ANSW_USB_INT_CONNECT 0x15  ///< Dispositivo USB conectado
#endif

#ifndef ANSW_USB_INT_DISCONNECT
    #define ANSW_USB_INT_DISCONNECT 0x16  ///< Dispositivo USB desconectado
#endif

#ifndef ANSW_USB_INT_DISK_READ
    #define ANSW_USB_INT_DISK_READ 0x1D  ///< Lectura de disco pendiente
#endif

#ifndef ANSW_USB_INT_DISK_WRITE
    #define ANSW_USB_INT_DISK_WRITE 0x1E  ///< Escritura de disco pendiente
#endif

#ifndef ANSW_ERR_MISS_FILE
    #define ANSW_ERR_MISS_FILE 0x42  ///< Archivo no encontrado
#endif
*/
// =============================================================================
// CONFIGURACIÓN DE PANTALLA (para referencia)
// =============================================================================

/**
 * @brief Líneas visibles en pantalla para browser de archivos
 */
#define LINEAS_PANTALLA_BROWSER 8

/**
 * @brief Caracteres por línea en pantalla
 */
#define CHARS_POR_LINEA_PANTALLA 30

// =============================================================================
// MACROS DE UTILIDAD
// =============================================================================

/**
 * @brief Macro para imprimir solo en modo desarrollador
 * 
 * Uso:
 * @code
 * DEBUG_PRINT("Valor: ");
 * DEBUG_PRINTLN(variable);
 * @endcode
 */
#if MODO_DESARROLLADOR
    #define DEBUG_PRINT(x)   Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif

/**
 * @brief Macro para convertir bytes a kilobytes
 */
#define BYTES_TO_KB(bytes) ((bytes) / 1024)

/**
 * @brief Macro para calcular elementos en un array
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief Macro para limitar un valor entre mín y máx
 */
#define CONSTRAIN(val, min_val, max_val) \
    ((val) < (min_val) ? (min_val) : ((val) > (max_val) ? (max_val) : (val)))

// =============================================================================
// VALIDACIÓN DE CONFIGURACIÓN
// =============================================================================

// Verificar que MAX_ARCHIVOS no sea demasiado grande
#if MAX_ARCHIVOS > 64
    #warning "MAX_ARCHIVOS > 64 puede consumir mucha RAM"
#endif

// Verificar que el buffer de línea sea razonable
#if TAMANO_BUFFER_LINEA > 512
    #warning "TAMANO_BUFFER_LINEA > 512 puede ser excesivo"
#endif

#if TAMANO_BUFFER_LINEA < 80
    #error "TAMANO_BUFFER_LINEA debe ser al menos 80"
#endif

// Verificar que el chunk de lectura sea razonable
#if CHUNK_LECTURA_USB > 64
    #warning "CHUNK_LECTURA_USB > 64 puede bloquear el loop"
#endif

#if CHUNK_LECTURA_USB < 1
    #error "CHUNK_LECTURA_USB debe ser al menos 1"
#endif

// =============================================================================
// INFORMACIÓN DE COMPILACIÓN
// =============================================================================

/**
 * @brief Versión del sistema de gestión de archivos
 */
#define VERSION_GESTOR_ARCHIVOS "1.0.0"

/**
 * @brief Fecha de última modificación
 */
#define FECHA_VERSION "2025-11-17"

/**
 * @brief Uso estimado de RAM por el sistema de archivos
 * 
 * Cálculo:
 * - ControladorUSB: ~130 bytes
 * - ControladorSD: ~80 bytes
 * - GestorArchivos: ~512 bytes (con MAX_ARCHIVOS=16)
 * - Total: ~720 bytes
 */
#define RAM_ESTIMADA_GESTOR \
    (130 + 80 + (MAX_ARCHIVOS * MAX_LONGITUD_NOMBRE_ARCHIVO) + TAMANO_BUFFER_LINEA)

// Mostrar información en compilación
#if MODO_DESARROLLADOR
    #pragma message "=========================================="
    #pragma message "  Sistema de Gestión de Archivos CNC"
    #pragma message "=========================================="
    #pragma message "Versión: " VERSION_GESTOR_ARCHIVOS
    #pragma message "Fecha: " FECHA_VERSION
    #pragma message "------------------------------------------"
    
    // Convertir número a string para pragma message
    #define STRINGIFY(x) #x
    #define TOSTRING(x) STRINGIFY(x)
    
    #pragma message "MAX_ARCHIVOS: " TOSTRING(MAX_ARCHIVOS)
    #pragma message "TAMANO_BUFFER_LINEA: " TOSTRING(TAMANO_BUFFER_LINEA)
    #pragma message "CHUNK_LECTURA_USB: " TOSTRING(CHUNK_LECTURA_USB)
    #pragma message "------------------------------------------"
    #pragma message "RAM estimada: ~720 bytes"
    #pragma message "=========================================="
#endif


#endif