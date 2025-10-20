#ifndef CONTROLADOR_USB_H
#define CONTROLADOR_USB_H

/*
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
    uint8_t inicializar_usb_host();
    void actualizar_estado_usb_host();
    void actualizar_estado_conexion();
    void manejar_conexion();
    void manejar_desconexion();
    void montar_fat_si_posible();
    void mostrar_error_lun();
    void configurar_estructura_almacenamiento();
    void montar_sistema_fat();
    void verificar_montaje_fat();
    void mostrar_mensaje_montaje_exitoso();
    void mostrar_error_montaje_fat();
    void liberar_sistema_fat();
    void desmontar_fat();
    void mostrar_mensaje_desconexion();
    void limpiar_lista_archivos();
    void agregar_archivo_a_lista(const char* nombre);
    bool leer_y_escribir_archivo(const char* nombre_archivo, Stream& salida);
    void escribir_buffer_en_salida(const uint8_t* buffer, UINT bytes, Stream& salida);
    void tarea();
    bool esta_conectado() const;
    bool esta_montado() const;
    uint8_t obtener_cantidad_archivos() const;
    const char* obtener_nombre_archivo(uint8_t indice) const;
    bool leer_archivo(const char* nombre_archivo, Stream& salida);
    const char* const* obtener_lista_nombres() const;
};
*/
#endif