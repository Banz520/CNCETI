#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H


#include "constantes.h"
#include"Ch376msc.h"
#include "SD.h"
#include <Arduino.h>

class GestorArchivos{
private:

    File directorio_raiz_sd;
    File archivo_actual_sd;

    Ch376msc &hostUsb;
   
    
    //static const uint8_t MAX_ARCHIVOS = 48;
    static const uint8_t MAX_LONGITUD_NOMBRE = 12;
    static const size_t TAMANO_BUFFER_LINEA = 256;
    
    char buffer_nombres_archivos[MAX_ARCHIVOS * MAX_LONGITUD_NOMBRE];
    const char* lista_archivos[MAX_ARCHIVOS];
    uint8_t contador_archivos = 0;

    bool EsArchivoGcode(const char* archivo);

public:

    // Se pasa al constructor de la clase del host el puerto de comunicacion serial UART que utilizara y la velocidad
    GestorArchivos(Ch376msc &miHostUsb_ref);
    ~GestorArchivos();
    SDClass hostSD;

    bool iniciarPuertoUSB();

    bool iniciarPuertoSD();

    const char** obtenerListaArchivosSD(const char* directorio, uint8_t* cantidad);
    const char** obtenerListaArchivosUSB(const char* directorio, uint8_t* cantidad);
    const char** obtenerListaArchivosUSBDebug(const char* directorio, uint8_t* cantidad);
    


#if MODO_DESARROLLADOR


#endif
};

#endif