#ifndef CONTROLADOR_USB_H
#define CONTROLADOR_USB_H


#include <Usb.h>
#include <masstorage.h>
#include <Storage.h>
#include <PCPartition.h>
#include <FAT/FAT.h>

class ControladorUSB{
private:
    USB usb_host;
    BulkOnly dispositivo_almacenamiento;
    storage_t estructura_almacenamiento;
    PCPartition lector_particiones;
    PFAT* sistema_fat;
    bool fat_montado;
    bool dispositivo_conectado;

    static const uint8_t max_archivos = 20;
    static const uint8_t tam_nombre = 32;
    char nombres_archivos[max_archivos][tam_nombre];
    uint8_t cantidad_archivos;

    void montar_fat();
    void escanear_directorio(const char* ruta);

public:

    ControladorUSB();
    void iniciar();
    void tarea();
    bool esta_conectado() const;
    bool esta_montado() const;
    uint8_t obtener_cantidad_archivos() const;
    const char* obtener_nombre_archivo(uint8_t indice) const;
    bool leer_archivo(const char* nombre_archivo, Stream& salida);
    const char* const* obtener_lista_nombres() const;
};

#endif