#ifndef CONSTANTES_H
#define CONSTANTES_H

#define DISPLAY_ANCHO 480
#define DISPLAY_ALTO 320
#define COLOR_BLANCO 0xFFFF
#define COLOR_NEGRO 0x0000
#define COLOR_GRIS_OSCURO 0x39a7
#define COLOR_GRIS_CLARO 0xc5f8
#define COLOR_ROJO 0xf800
#define COLOR_AZUL 0x1aba
#define MODO_DESARROLLADOR 1

#include <avr/pgmspace.h>

#define MENU_PRINCIPAL_TEXT F("Menu principal - Seleccione una opcion")
#define BARRA_INF_TEXT F("Utilice las teclas <,> para navegar, ENTER para selec.")
#define SIN_ALERTA_TEXT F("Sin alerta")

// Puedes agregar textos predefinidos para alertas si lo necesitas
#define ALERTA_ERROR_TEXT F("Error")
#define ALERTA_ADVERTENCIA_TEXT F("Advertencia")
#define ALERTA_INFO_TEXT F("Información")

// Opciones del menú en PROGMEM...
// Strings individuales en PROGMEM
const char OP_ABRIR_LOCAL[] PROGMEM = "Abrir archivo local";
const char OP_IMPORTAR_USB[] PROGMEM = "Importar archivo desde USB";
const char OP_IMPORTAR_RED[] PROGMEM = "Importar archivo desde la red";
const char OP_CALIBRAR[] PROGMEM = "Calibrar ejes";
const char OP_OTROS[] PROGMEM = "Otros";

// Array de punteros en PROGMEM - VERIFICA QUE TENGA 'PROGMEM'
const char* const OPCIONES_MENU[] PROGMEM = {
    OP_ABRIR_LOCAL,
    OP_IMPORTAR_USB, 
    OP_IMPORTAR_RED,
    OP_CALIBRAR,
    OP_OTROS
};

const uint8_t NUM_OPCIONES_MENU = 5;


#endif