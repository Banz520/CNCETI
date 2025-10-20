#include "controlador_usb.h"

/*
ControladorUSB::ControladorUSB():
    usb_host(),
    dispositivo_almacenamiento(&usb_host),
    lector_particiones(),
    sistema_fat(nullptr),
    fat_montado(false),
    dispositivo_conectado(false),
    cantidad_archivos(0)
{}


uint8_t ControladorUSB::inicializar_usb_host() {
    uint8_t estado_conexion_usb_host = usb_host.Init();
    //El usb host se inicio con exito si retorna 0
    if (estado_conexion_usb_host == 0) {
        //Se logro la conexion con el shield usb host
        Serial.println(F("Se logro la conexion con el shield usb host"));
        return 0;

        
    }
    else if(estado_conexion_usb_host == -1){
            //No se logro iniciar el shield usb host
            Serial.println(F("No se logro iniciar el shield usb host"));
            return 1;
    }
    else if(estado_conexion_usb_host == -2){
        Serial.println(F("Error en los registros del host"));
        return 3;
    }
    else if(estado_conexion_usb_host == -3){
        Serial.println(F("Error en con el oscilador"));
        return 4;
    }
    else if(estado_conexion_usb_host == -4){
        Serial.println(F("No device connected"));
        return 5;
    }
    else if(estado_conexion_usb_host == -5){
        Serial.println(F("Illegal bus state"));
        return 6;
    }
    else if(estado_conexion_usb_host == -6){
        Serial.println(F("Unknown state"));
        return 7;
    }
    else {
        //No se encontro conexion con el shield usb host
        Serial.println(F("No se encontro conexion con el shield usb host"));
        return 2;
    }
        
    
    
}

void ControladorUSB::tarea() {
    actualizar_estado_usb_host();
    actualizar_estado_conexion();
}

void ControladorUSB::actualizar_estado_usb_host() {
    usb_host.Task();
}

void ControladorUSB::actualizar_estado_conexion() {
    bool conectado = dispositivo_almacenamiento.GetAddress();
    if (conectado && !dispositivo_conectado) {
        manejar_conexion();
    } else if (!conectado && dispositivo_conectado) {
        manejar_desconexion();
    }
}

void ControladorUSB::manejar_conexion() {
    dispositivo_conectado = true;
    Serial.println("Se detecto un dispositivo en el puerto USB");
    Serial.println("Intentando abrir sistema de archivos del dispositivos...");
    montar_fat_si_posible();
}

void ControladorUSB::manejar_desconexion() {
    dispositivo_conectado = false;
    desmontar_fat();
    limpiar_lista_archivos();
    liberar_sistema_fat();
    mostrar_mensaje_desconexion();
}

void ControladorUSB::montar_fat_si_posible() {
    if (dispositivo_almacenamiento.LUNIsGood(0)) {
        configurar_estructura_almacenamiento();
        montar_sistema_fat();
    } else {
        mostrar_error_lun();
    }
}

void ControladorUSB::mostrar_error_lun() {
    Serial.println(F("LUN 0 no válida."));
}

void ControladorUSB::configurar_estructura_almacenamiento() {
    estructura_almacenamiento.Reads       = *UHS_USB_BulkOnly_Read;
    estructura_almacenamiento.Writes      = *UHS_USB_BulkOnly_Write;
    estructura_almacenamiento.Status      = *UHS_USB_BulkOnly_Status;
    estructura_almacenamiento.Initialize  = *UHS_USB_BulkOnly_Initialize;
    estructura_almacenamiento.Commit      = *UHS_USB_BulkOnly_Commit;
    estructura_almacenamiento.TotalSectors = dispositivo_almacenamiento.GetCapacity(0);
    estructura_almacenamiento.SectorSize   = dispositivo_almacenamiento.GetSectorSize(0);

    struct DatosPrivados {
        uint8_t controlador;
        uint8_t unidad_logica;
    };
    static DatosPrivados datos_privados = { 0, 0 };
    estructura_almacenamiento.private_data = &datos_privados;
}

void ControladorUSB::montar_sistema_fat() {
    if (!lector_particiones.Init(&estructura_almacenamiento)) {
        part_t* particion = lector_particiones.GetPart(0);
        if (particion && (particion->type == 0x0B || particion->type == 0x0C || particion->type == 0x0E)) {
            sistema_fat = new PFAT(&estructura_almacenamiento, 0, particion->firstSector);
        } else {
            sistema_fat = new PFAT(&estructura_almacenamiento, 0, 0);
        }
        verificar_montaje_fat();
    }
}

void ControladorUSB::verificar_montaje_fat() {
    if (!sistema_fat->MountStatus()) {
        fat_montado = true;
        mostrar_mensaje_montaje_exitoso();
        escanear_directorio("0:/");
    } else {
        mostrar_error_montaje_fat();
        liberar_sistema_fat();
    }
}

void ControladorUSB::mostrar_mensaje_montaje_exitoso() {
    Serial.println(F("Sistema de archivos FAT montado correctamente."));
}

void ControladorUSB::mostrar_error_montaje_fat() {
    Serial.println(F("Error al montar el sistema FAT."));
}

void ControladorUSB::liberar_sistema_fat() {
    if (sistema_fat) {
        delete sistema_fat;
        sistema_fat = nullptr;
    }
}

void ControladorUSB::desmontar_fat() {
    fat_montado = false;
}

void ControladorUSB::mostrar_mensaje_desconexion() {
    Serial.println(F("Memoria USB desconectada."));
}

void ControladorUSB::limpiar_lista_archivos() {
    cantidad_archivos = 0;
}

void ControladorUSB::escanear_directorio(const char* ruta) {
    cantidad_archivos = 0;
    if (!sistema_fat) return;
    DIR dir;
    FILINFO fno;
    FRESULT res = f_opendir(&dir, ruta);
    if (res != FR_OK) return;
    while (cantidad_archivos < max_archivos) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;
        if (fno.fattrib & AM_DIR) continue;
        agregar_archivo_a_lista(fno.fname);
    }
}

void ControladorUSB::agregar_archivo_a_lista(const char* nombre) {
    strncpy(nombres_archivos[cantidad_archivos], nombre, tam_nombre - 1);
    nombres_archivos[cantidad_archivos][tam_nombre - 1] = '\0';
    cantidad_archivos++;
}

bool ControladorUSB::esta_conectado() const {
    return dispositivo_conectado;
}

bool ControladorUSB::esta_montado() const {
    return fat_montado;
}

uint8_t ControladorUSB::obtener_cantidad_archivos() const {
    return cantidad_archivos;
}

const char* ControladorUSB::obtener_nombre_archivo(uint8_t indice) const {
    if (indice < cantidad_archivos) {
        return nombres_archivos[indice];
    }
    return nullptr;
}

bool ControladorUSB::leer_archivo(const char* nombre_archivo, Stream& salida) {
    if (!sistema_fat || !fat_montado) {
        return false;
    }
    return leer_y_escribir_archivo(nombre_archivo, salida);
}

bool ControladorUSB::leer_y_escribir_archivo(const char* nombre_archivo, Stream& salida) {
    FIL archivo;
    UINT bytes_leidos;
    uint8_t buffer[64];
    FRESULT resultado = f_open(&archivo, nombre_archivo, FA_READ);
    if (resultado) return false;
    do {
        resultado = f_read(&archivo, buffer, sizeof(buffer), &bytes_leidos);
        if (resultado || bytes_leidos == 0) break;
        escribir_buffer_en_salida(buffer, bytes_leidos, salida);
    } while (bytes_leidos == sizeof(buffer));
    f_close(&archivo);
    return true;
}

void ControladorUSB::escribir_buffer_en_salida(const uint8_t* buffer, UINT bytes, Stream& salida) {
    for (UINT i = 0; i < bytes; i++) {
        salida.write(buffer[i]);
    }
}

const char* const* ControladorUSB::obtener_lista_nombres() const {
    static const char* lista[max_archivos];
    for (uint8_t i = 0; i < cantidad_archivos; i++) {
        lista[i] = nombres_archivos[i];
    }
    return lista;
}
    */