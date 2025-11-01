#ifndef CONSTANTES_H
#define CONSTANTES_H

/**
 * @def MODO_DESARROLLADOR
 * @brief Directiva del compilador para activar funciones de pruebas de desarrollador
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

#define COLOR_BLANCO 0xFFFF
#define COLOR_NEGRO 0x0000
#define COLOR_GRIS_OSCURO 0x39a7
#define COLOR_GRIS_CLARO 0xc5f8
#define COLOR_ROJO 0xf800
#define COLOR_AZUL 0x1aba


/**
 * @def MAX_ARCHIVOS
 * @brief Numero maximo de archivos que se pueden almacenar en la lista
 */
#define MAX_ARCHIVOS 50

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

#endif