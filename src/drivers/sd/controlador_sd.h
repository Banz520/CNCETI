#ifndef CONTROLADOR_SD_H
#define CONTROLADOR_SD_H

#include "SD.h"
#include "constantes.h"

class ControladorSD {
private:
    File directorio_raiz;
    static const size_t MAX_ARCHIVOS = 50;
    static const size_t MAX_LONGITUD_NOMBRE = 64;
    
    // Buffer pre-asignado para nombres de archivo
    char bufferArchivos[MAX_ARCHIVOS][MAX_LONGITUD_NOMBRE];
    size_t contadorArchivos;
    
    // Métodos privados para responsabilidad única
    
    bool esArchivoGcode(const char* nombreArchivo);
    void agregarArchivoALista(const char* nombreArchivo);

public:
    ControladorSD();
    ~ControladorSD();
    
    bool iniciarSD();
    const char** leerDirectorio(File dir, int numTabs = 0);
    size_t obtenerCantidadArchivos() const;
    void limpiarListaArchivos();
    const char** obtenerListaArchivosGcode();
    
    // Métodos de utilidad para acceso seguro
    const char* obtenerNombreArchivo(size_t indice) const;
    bool listaVacia() const;

    //Metodos para pruebas de desarrollo
    #if MODO_DESARROLLADOR
    void imprimirEstructuraDirectorio(File dir, int numTabs);
    #endif
};

#endif