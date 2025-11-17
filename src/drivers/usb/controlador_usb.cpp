#include "controlador_usb.h"
#include <string.h>

#define DEPURAR_CONTROLADOR_USB
/**
 * @brief Constructor: vincula el controlador CH376 ya inicializado.
 */
ControladorUSB::ControladorUSB(Ch376msc& referencia_host)
    : host_usb(referencia_host) {}


// ===========================================================
// ESTADO Y CONFIGURACIÓN
// ===========================================================

bool ControladorUSB::dispositivoListo() {
    host_usb.checkIntMessage(); 
    bool listo = host_usb.driveReady();

    #if MODO_DESARROLLADOR
    #ifdef DEURAR_CONTROLADOR_USB
        Serial.print(F("[ControladorUSB::dispositivoListo] Estado dispositivo: "));
        Serial.println(listo ? F("LISTO") : F("NO DISPONIBLE"));
    #endif
    #endif

    return listo;
}

bool ControladorUSB::finDeArchivo() {
    if (!archivo_abierto) return true;
    bool eof = host_usb.getEOF();

    #if MODO_DESARROLLADOR
        if (eof) Serial.println(F("[ControladorUSB::dispositivoListo] Fin de archivo alcanzado"));
    #endif

    return eof;
}


// ===========================================================
// GESTIÓN DE ARCHIVOS
// ===========================================================

bool ControladorUSB::listarArchivos(void (*callback)(const char* nombre)) {
    if (!dispositivoListo()) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorUSB::listarArchivos] Dispositivo USB no listo"));
        #endif
        return false;
    }

    if (host_usb.cd("/", false) != ANSW_USB_INT_SUCCESS) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorUSB::listarArchivos] No se pudo acceder al directorio raíz"));
        #endif
        return false;
    }

    host_usb.resetFileList();

    #if MODO_DESARROLLADOR
        Serial.println(F("[ControladorUSB::listarArchivos] Listando archivos del directorio raíz..."));
    #endif

    while (host_usb.listDir() == ANSW_USB_INT_SUCCESS) {
        const char* nombre = host_usb.getFileName();
        if (nombre && callback) {
            callback(nombre);
            #if MODO_DESARROLLADOR
                Serial.print(F("  > "));
                Serial.println(nombre);
            #endif
        }
    }

    #if MODO_DESARROLLADOR
        Serial.println(F("[ControladorUSB::listarArchivos] Fin del listado de archivos"));
    #endif

    return true;
}

bool ControladorUSB::abrirArchivo(const char* nombre) {
    if (!dispositivoListo() || !nombre) return false;

    host_usb.setFileName(nombre);
    if (host_usb.openFile() != ANSW_USB_INT_SUCCESS) {
        #if MODO_DESARROLLADOR
            Serial.print(F("[ControladorUSB::abrirArchivo] No se pudo abrir archivo: "));
            Serial.println(nombre);
        #endif
        return false;
    }

    archivo_abierto = true;

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorUSB::abrirArchivo] Archivo abierto: "));
        Serial.println(nombre);
        Serial.print(F("Tamaño: "));
        Serial.print(host_usb.getFileSize());
        Serial.println(F(" bytes"));
    #endif

    return true;
}

bool ControladorUSB::cerrarArchivo() {
    if (!archivo_abierto) return true;

    host_usb.closeFile();
    archivo_abierto = false;

    #if MODO_DESARROLLADOR
        Serial.println(F("[ControladorUSB::cerrarArchivo] Archivo cerrado correctamente"));
    #endif

    return true;
}


bool ControladorUSB::leerLineaNoBloqueante(char* buffer, uint8_t tamano_buffer) {
    static size_t indice = 0;
    static bool leyendo = false;

    if (!archivo_abierto || !buffer || tamano_buffer == 0)
        return false;

    if (!leyendo) {
        indice = 0;
        leyendo = true;
    }

    const uint8_t MAX_CHARS_POR_CICLO = 8;
    uint8_t leidos = 0;

    while (leidos < MAX_CHARS_POR_CICLO && indice < tamano_buffer - 1) {
        if (host_usb.getEOF()) {
            buffer[indice] = '\0';
            leyendo = false;
            return (indice > 0);
        }

        char c;
        if (!host_usb.readFile(&c, 1)) break; // No hay más datos disponibles

        leidos++;

        if (c == '\r') continue;
        if (c == '\n') {
            buffer[indice] = '\0';
            leyendo = false;

            #if MODO_DESARROLLADOR
            #ifdef DEPURAR_CONTROLADOR_USB
                Serial.print(F("[ControladorUSB::leerLineaNoBloqueante] Línea leída: "));
                Serial.println(buffer);
            #endif
            #endif

            return true;
        }

        buffer[indice++] = c;
    }

    return false;
}

const char* ControladorUSB::obtenerNombreArchivoActual() {
    return archivo_abierto ? host_usb.getFileName() : nullptr;
}

size_t ControladorUSB::obtenerTamanoArchivo() {
    return archivo_abierto ? host_usb.getFileSize() : 0;
}

uint32_t ControladorUSB::obtenerPosicionActual() {
    return archivo_abierto ? host_usb.getCursorPos() : 0;
}
