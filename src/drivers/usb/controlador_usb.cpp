#include "controlador_usb.h"
#include <Arduino.h>
#include <string.h>
#include "constantes.h"
/**
 * @brief Verifica si un archivo tiene extensión .gcode
 */
bool ControladorUSB::EsArchivoGcode(const char* nombre_archivo) {
    if (nombre_archivo == nullptr) return false;
    
    // Buscar el punto de la extensión
    const char* punto = strchr(nombre_archivo, '.');
    if (punto == nullptr) return false;
    
    
    const char* extension = punto + 1;
    return (strcasecmp(extension, "gcode") == 0) || 
           (strcasecmp(extension, "gc") == 0) || 
           (strcasecmp(extension, "gco") == 0);
}

/**
 * @brief Imprime información detallada del archivo actual
 */
void ControladorUSB::ImprimirInfoArchivo() {
#if MODO_DESARROLLADOR
    Serial.println("=== INFORMACIÓN DE ARCHIVO ===");
    Serial.print("Nombre: ");
    Serial.println(host_usb.getFileName());
    Serial.print("Tamaño: ");
    Serial.print(host_usb.getFileSize());
    Serial.println(" bytes");
    Serial.print("Posición cursor: ");
    Serial.println(host_usb.getCursorPos());
    Serial.print("Fecha: ");
    Serial.print(host_usb.getYear());
    Serial.print("-");
    Serial.print(host_usb.getMonth());
    Serial.print("-");
    Serial.println(host_usb.getDay());
    Serial.print("Hora: ");
    Serial.print(host_usb.getHour());
    Serial.print(":");
    Serial.print(host_usb.getMinute());
    Serial.print(":");
    Serial.println(host_usb.getSecond());
    Serial.println("==============================");
#endif
}

/**
 * @brief Configura el controlador CH376 para comunicación UART
 */
bool ControladorUSB::ConfigurarUART() {
#if MODO_DESARROLLADOR
    Serial.println("Configurando comunicación UART con CH376...");
#endif
    
    // Pequeña pausa para estabilización
    delay(100);
    
    // Verificar comunicación básica
    if (!VerificarComunicacion()) {
#if MODO_DESARROLLADOR
        Serial.println("ERROR: No se puede establecer comunicación UART");
#endif
        return false;
    }
    
#if MODO_DESARROLLADOR
    Serial.println("Comunicación UART establecida correctamente");
#endif
    return true;
}

/**
 * @brief Verifica la comunicación básica con el módulo CH376
 */
bool ControladorUSB::VerificarComunicacion() {
    // Realizar múltiples intentos de ping
    for (int intento = 0; intento < 3; intento++) {
        if (host_usb.pingDevice()) {
            return true;
        }
#if MODO_DESARROLLADOR
        Serial.print("Intento de comunicación fallido: ");
        Serial.println(intento + 1);
#endif
        delay(100);
    }
    return false;
}

// Constructores UART
ControladorUSB::ControladorUSB(HardwareSerial &puerto_serial, uint32_t velocidad) 
    : host_usb(puerto_serial, velocidad),
      dispositivo_listo(false),
      usando_uart(true) {
    memset(buffer_nombre_archivo, 0, sizeof(buffer_nombre_archivo));
    memset(buffer_lectura, 0, sizeof(buffer_lectura));
    memset(buffer_raw, 0, sizeof(buffer_raw));
}

ControladorUSB::ControladorUSB(Stream &puerto_serial) 
    : host_usb(puerto_serial),
      dispositivo_listo(false),
      usando_uart(true) {
    memset(buffer_nombre_archivo, 0, sizeof(buffer_nombre_archivo));
    memset(buffer_lectura, 0, sizeof(buffer_lectura));
    memset(buffer_raw, 0, sizeof(buffer_raw));
}

ControladorUSB::~ControladorUSB() {
    // Cerrar archivo si está abierto
    if (host_usb.driveReady()) {
        host_usb.closeFile();
    }
}

/**
 * @brief Inicializa el controlador USB
 */
bool ControladorUSB::Inicializar() {
#if MODO_DESARROLLADOR
    Serial.println("Iniciando inicialización del controlador USB...");
#endif

    // Si estamos usando UART, configurar primero
    if (usando_uart && !ConfigurarUART()) {
        return false;
    }
    
    // Inicializar el controlador
    host_usb.init();
    
    // Verificar comunicación con el dispositivo
    if (!VerificarComunicacion()) {
#if MODO_DESARROLLADOR
        Serial.println("ERROR: No se puede comunicar con el controlador CH376 después de init");
        Serial.println("Verifique:");
        Serial.println("1. Conexiones UART (TX->RX, RX->TX, GND->GND)");
        Serial.println("2. Velocidad de comunicación configurada en el módulo");
        Serial.println("3. Alimentación adecuada (3.3V o 5V según módulo)");
        Serial.println("4. Módulo CH376 en modo UART (jumpers/configuración)");
#endif
        return false;
    }
    
    // Verificar si hay un dispositivo conectado
    dispositivo_listo = host_usb.driveReady();
    
#if MODO_DESARROLLADOR
    if (dispositivo_listo) {
        Serial.println("Controlador USB inicializado correctamente");
        char info_buffer[150];
        if (ObtenerInfoSistemaArchivos(info_buffer, sizeof(info_buffer))) {
            Serial.println(info_buffer);
        }
    } else {
        Serial.println("Controlador USB inicializado, pero no hay dispositivo conectado");
        Serial.println("Conecte un dispositivo USB formateado en FAT32");
    }
#endif
    
    return true;
}

/**
 * @brief Verifica si el dispositivo USB está listo
 */
bool ControladorUSB::DispositivoListo() {
    // Actualizar estado
    host_usb.checkIntMessage();
    dispositivo_listo = host_usb.driveReady();
    return dispositivo_listo;
}

/**
 * @brief Lee todos los archivos del directorio raíz
 */
bool ControladorUSB::LeerDirectoriosRaiz(ListaArchivos* lista_archivos) {
    if (lista_archivos == nullptr) {
        return false;
    }
    
    // Inicializar lista
    lista_archivos->cantidad = 0;
    memset(lista_archivos->archivos, 0, sizeof(lista_archivos->archivos));
    
    if (!DispositivoListo()) {
#if MODO_DESARROLLADOR
        Serial.println("ERROR: Dispositivo USB no está listo");
#endif
        return false;
    }
    
    // Cambiar al directorio raíz
    if (host_usb.cd("/", false) != ANSW_USB_INT_SUCCESS) {
#if MODO_DESARROLLADOR
        Serial.println("ERROR: No se puede acceder al directorio raíz");
#endif
        return false;
    }
    
    // Listar archivos
    host_usb.resetFileList();
    
#if MODO_DESARROLLADOR
    Serial.println("=== ESTRUCTURA DEL DIRECTORIO RAÍZ ===");
    Serial.println("Nombre       | Tamaño    | Fecha");
    Serial.println("-----------------------------------");
#endif
    
    while (host_usb.listDir() == ANSW_USB_INT_SUCCESS && 
           lista_archivos->cantidad < MAX_ARCHIVOS) {
        const char* nombre = host_usb.getFileName();
        if (nombre != nullptr && strlen(nombre) > 0) {
            // Almacenar el puntero al nombre (la memoria es manejada por la librería CH376)
            lista_archivos->archivos[lista_archivos->cantidad] = nombre;
            lista_archivos->cantidad++;
            
#if MODO_DESARROLLADOR
            // Formatear salida para mejor legibilidad
            char linea[50];
            snprintf(linea, sizeof(linea), "%-12s | %-9s | %04d-%02d-%02d",
                    nombre,
                    host_usb.getFileSizeStr(),
                    host_usb.getYear(),
                    host_usb.getMonth(),
                    host_usb.getDay());
            Serial.println(linea);
#endif
        }
    }
    
#if MODO_DESARROLLADOR
    Serial.println("===================================");
    Serial.print("Total de archivos encontrados: ");
    Serial.println(lista_archivos->cantidad);
#endif
    
    return true;
}

/**
 * @brief Busca archivos .gcode en el directorio raíz
 */
bool ControladorUSB::BuscarArchivosGcode(ListaArchivos* lista_gcode) {
    return BuscarArchivosGcodeEnDirectorio("/", lista_gcode);
}

/**
 * @brief Busca archivos .gcode en un directorio específico
 */
bool ControladorUSB::BuscarArchivosGcodeEnDirectorio(const char* ruta_directorio, ListaArchivos* lista_gcode) {
    if (lista_gcode == nullptr) {
        return false;
    }
    
    // Inicializar lista
    lista_gcode->cantidad = 0;
    memset(lista_gcode->archivos, 0, sizeof(lista_gcode->archivos));
    
    if (!DispositivoListo()) {
        return false;
    }
    
    // Cambiar al directorio especificado
    if (host_usb.cd(ruta_directorio, false) != ANSW_USB_INT_SUCCESS) {
#if MODO_DESARROLLADOR
        Serial.print("ERROR: No se puede acceder al directorio: ");
        Serial.println(ruta_directorio);
#endif
        return false;
    }
    
    // Buscar archivos .gcode
    host_usb.resetFileList();
    
    while (host_usb.listDir() == ANSW_USB_INT_SUCCESS && 
           lista_gcode->cantidad < MAX_ARCHIVOS) {
        const char* nombre = host_usb.getFileName();
        if (nombre != nullptr && EsArchivoGcode(nombre)) {
            lista_gcode->archivos[lista_gcode->cantidad] = nombre;
            lista_gcode->cantidad++;
        }
    }
    
#if MODO_DESARROLLADOR
    Serial.print("Archivos GCODE encontrados en '");
    Serial.print(ruta_directorio);
    Serial.print("': ");
    Serial.println(lista_gcode->cantidad);
    
    for (uint8_t i = 0; i < lista_gcode->cantidad; i++) {
        Serial.print("  - ");
        Serial.println(lista_gcode->archivos[i]);
    }
#endif
    
    return true;
}

/**
 * @brief Obtiene información del sistema de archivos
 */
bool ControladorUSB::ObtenerInfoSistemaArchivos(char* buffer, uint16_t tamano_buffer) {
    if (buffer == nullptr || tamano_buffer == 0) {
        return false;
    }
    
    if (!DispositivoListo()) {
        strncpy(buffer, "Dispositivo no disponible", tamano_buffer);
        buffer[tamano_buffer - 1] = '\0';
        return false;
    }
    
    snprintf(buffer, tamano_buffer,
             "Sistema: FAT%d | Sectores libres: %lu/%lu | Origen: %s",
             host_usb.getFileSystem(),
             host_usb.getFreeSectors(),
             host_usb.getTotalSectors(),
             host_usb.getSource() ? "SD" : "USB");
    
    return true;
}

/**
 * @brief Abre un archivo para lectura
 */
bool ControladorUSB::AbrirArchivo(const char* nombre_archivo) {
    if (!DispositivoListo()) {
        return false;
    }
    
    host_usb.setFileName(nombre_archivo);
    uint8_t resultado = host_usb.openFile();
    
    if (resultado == ANSW_USB_INT_SUCCESS) {
#if MODO_DESARROLLADOR
        Serial.print("Archivo abierto: ");
        Serial.println(nombre_archivo);
        ImprimirInfoArchivo();
#endif
        return true;
    } else {
#if MODO_DESARROLLADOR
        Serial.print("ERROR al abrir archivo: ");
        Serial.println(nombre_archivo);
#endif
        return false;
    }
}

/**
 * @brief Cierra el archivo actual
 */
bool ControladorUSB::CerrarArchivo() {
    uint8_t resultado = host_usb.closeFile();
    
    if (resultado == ANSW_USB_INT_SUCCESS) {
#if MODO_DESARROLLADOR
        Serial.println("Archivo cerrado correctamente");
#endif
        return true;
    } else {
#if MODO_DESARROLLADOR
        Serial.println("ERROR al cerrar archivo");
#endif
        return false;
    }
}

/**
 * @brief Lee una línea del archivo actual
 */
bool ControladorUSB::LeerLineaArchivo(char* buffer, uint8_t tamano_buffer) {
    if (buffer == nullptr || tamano_buffer == 0) {
        return false;
    }
    
    // Verificar si llegamos al final del archivo
    if (host_usb.getEOF()) {
        return false;
    }
    
    // Leer hasta encontrar newline o llenar el buffer
    bool exito = host_usb.readFileUntil('\n', buffer, tamano_buffer - 1);
    
    if (exito) {
        // Asegurar terminación nula
        buffer[tamano_buffer - 1] = '\0';
    }
    
    return exito;
}

/**
 * @brief Obtiene el último código de error
 */
uint8_t ControladorUSB::ObtenerUltimoError() {
    return host_usb.getError();
}

/**
 * @brief Libera la memoria utilizada por una lista de archivos
 */
void ControladorUSB::LiberarListaArchivos(ListaArchivos* lista_archivos) {
    if (lista_archivos != nullptr) {
        // En esta implementación, los nombres son manejados por la librería CH376
        // así que solo necesitamos resetear la lista
        lista_archivos->cantidad = 0;
        memset(lista_archivos->archivos, 0, sizeof(lista_archivos->archivos));
    }
}