#ifndef CONTROLADOR_SD_H
#define CONTROLADOR_SD_H

#include "SD.h"
#include "constantes.h"

class ControladorSD {
private:
    File directorio_raiz;
    File archivo_actual;

    //static const size_t MAX_ARCHIVOS = 50;
    static const size_t MAX_LONGITUD_NOMBRE = 64;
    static const size_t TAMANO_BUFFER_LINEA = 256;
    
    // Buffer pre-asignado para nombres de archivo
    char buffer_archivos[MAX_ARCHIVOS][MAX_LONGITUD_NOMBRE];
    size_t contador_archivos;
    
    // Métodos privados
    bool esArchivoGcode(const char* nombre_archivo);
    void agregarArchivoALista(const char* nombre_archivo);
    bool esLineaComentario(const char* linea);
    bool leerLineaArchivoOptimizada(File& archivo, char* buffer, size_t tamano_buffer);

public:
    ControladorSD();
    ~ControladorSD();
    
    bool iniciarSD();
    const char** leerDirectorio(File dir, int num_tabs = 0);
    size_t obtenerCantidadArchivos() const;
    void limpiarListaArchivos();
    const char** obtenerListaArchivosGcode();
    const char* obtenerNombreArchivo(size_t indice) const;
    bool listaVacia() const;

    // Métodos para lectura de G-code optimizados
    bool abrirArchivoGcode(const char* nombre_archivo);
    void cerrarArchivoGcode();
    bool archivoAbierto();
    const char* leerLineaGcode();
    bool buscarArchivoPorNombre(const char* nombre_archivo);
    bool buscarEnArchivo(unsigned long posicion);
    
    // Nuevos métodos optimizados
    bool reiniciarLecturaArchivo();
    size_t obtenerTamanoArchivoActual();
    unsigned long obtenerPosicionActual();
    const char* obtenerNombreArchivoActual();
    bool esFinDeArchivo();
    
    #if MODO_DESARROLLADOR
    void imprimirEstructuraDirectorio(File dir, int num_tabs);
    void modoDesarrolladorLeerArchivo();
    #endif
};

#endif