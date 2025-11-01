#ifndef TEXTOS_H
#define TEXTOS_H

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

#endif