#include "controlador_usb.h"
#include <string.h>

/**
 * @file controlador_usb.cpp
 * @brief Implementación del controlador USB para CH376
 */

ControladorUSB::ControladorUSB(Ch376msc& referencia_host)
    : host_usb(referencia_host), archivo_abierto(false), 
      indice_buffer(0), linea_en_progreso(false) {
    buffer_lectura[0] = '\0';
}

// ========================================
// ESTADO Y VALIDACIÓN
// ========================================

bool ControladorUSB::dispositivoListo() {
    host_usb.checkIntMessage(); 
    bool listo = host_usb.driveReady();

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorUSB::dispositivoListo] Estado: "));
        Serial.println(listo ? F("LISTO") : F("NO DISPONIBLE"));
    #endif

    return listo;
}

bool ControladorUSB::shieldConectado() {
    bool conectado = host_usb.getCHpresence();
    
    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorUSB::shieldConectado] Shield CH376: "));
        Serial.println(conectado ? F("OK") : F("NO DETECTADO"));
    #endif
    
    return conectado;
}

bool ControladorUSB::finDeArchivo() {
    if (!archivo_abierto) return true;
    bool eof = host_usb.getEOF();

    #if MODO_DESARROLLADOR
        if (eof) {
            Serial.println(F("[ControladorUSB::finDeArchivo] EOF alcanzado"));
        }
    #endif

    return eof;
}

bool ControladorUSB::verificarCambioEstado() {
    return host_usb.checkIntMessage();
}

// ========================================
// GESTIÓN DE ARCHIVOS
// ========================================

bool ControladorUSB::listarArchivos(void (*callback)(const char* nombre)) {
    if (!dispositivoListo()) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorUSB::listarArchivos] Dispositivo no listo"));
        #endif
        return false;
    }

    if (host_usb.cd("/", false) != ANSW_USB_INT_SUCCESS) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorUSB::listarArchivos] Error accediendo raíz"));
        #endif
        return false;
    }

    host_usb.resetFileList();

    #if MODO_DESARROLLADOR
        Serial.println(F("[ControladorUSB::listarArchivos] Iniciando listado..."));
        uint8_t contador = 0;
    #endif

    while (host_usb.listDir() == ANSW_USB_INT_SUCCESS) {
        const char* nombre = host_usb.getFileName();
        if (nombre && callback) {
            callback(nombre);
            #if MODO_DESARROLLADOR
                Serial.print(F("  ["));
                Serial.print(++contador);
                Serial.print(F("] "));
                Serial.println(nombre);
            #endif
        }
    }

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorUSB::listarArchivos] Total: "));
        Serial.println(contador);
    #endif

    return true;
}

bool ControladorUSB::abrirArchivo(const char* nombre) {
    if (!nombre) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorUSB::abrirArchivo] ERROR: nombre nulo"));
        #endif
        return false;
    }

    if (!dispositivoListo()) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorUSB::abrirArchivo] Dispositivo no listo"));
        #endif
        return false;
    }

    // Cerrar archivo previo si existe
    if (archivo_abierto) {
        cerrarArchivo();
    }

    host_usb.setFileName(nombre);
    uint8_t resultado = host_usb.openFile();
    
    if (resultado != ANSW_USB_INT_SUCCESS) {
        #if MODO_DESARROLLADOR
            Serial.print(F("[ControladorUSB::abrirArchivo] ERROR al abrir: "));
            Serial.print(nombre);
            Serial.print(F(" | Código: 0x"));
            Serial.println(resultado, HEX);
        #endif
        return false;
    }

    archivo_abierto = true;
    indice_buffer = 0;
    linea_en_progreso = false;

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorUSB::abrirArchivo] Abierto: "));
        Serial.print(nombre);
        Serial.print(F(" | Tamaño: "));
        Serial.print(host_usb.getFileSize());
        Serial.println(F(" bytes"));
    #endif

    return true;
}

bool ControladorUSB::cerrarArchivo() {
    if (!archivo_abierto) return true;

    uint8_t resultado = host_usb.closeFile();
    archivo_abierto = false;
    indice_buffer = 0;
    linea_en_progreso = false;

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorUSB::cerrarArchivo] "));
        Serial.println(resultado == ANSW_USB_INT_SUCCESS ? F("OK") : F("ERROR"));
    #endif

    return resultado == ANSW_USB_INT_SUCCESS;
}

// ========================================
// LECTURA NO BLOQUEANTE
// ========================================

bool ControladorUSB::leerLineaNoBloqueante(char* buffer, uint8_t tamano_buffer) {
    if (!archivo_abierto || !buffer || tamano_buffer == 0) {
        return false;
    }

    // Inicializar nueva lectura de línea
    if (!linea_en_progreso) {
        indice_buffer = 0;
        linea_en_progreso = true;
    }

    // Leer hasta 8 caracteres por llamada (no bloqueante)
    const uint8_t MAX_CHARS_POR_CICLO = 8;
    uint8_t leidos = 0;

    while (leidos < MAX_CHARS_POR_CICLO && indice_buffer < tamano_buffer - 1) {
        // Verificar EOF
        if (host_usb.getEOF()) {
            buffer[indice_buffer] = '\0';
            linea_en_progreso = false;
            
            #if MODO_DESARROLLADOR
                if (indice_buffer > 0) {
                    Serial.print(F("[ControladorUSB::leerLineaNoBloqueante] EOF | Línea parcial: "));
                    Serial.println(buffer);
                }
            #endif
            
            return (indice_buffer > 0);
        }

        char c;
        if (host_usb.readFile(&c, 1) != 1) {
            // No hay datos disponibles en este ciclo
            break;
        }

        leidos++;

        // Filtrar caracteres de control
        if (c == '\r') continue;  // Ignorar CR
        
        if (c == '\n') {
            // Línea completa encontrada
            buffer[indice_buffer] = '\0';
            linea_en_progreso = false;

            #if MODO_DESARROLLADOR
                Serial.print(F("[ControladorUSB::leerLineaNoBloqueante] Línea completa ("));
                Serial.print(indice_buffer);
                Serial.println(F(" chars)"));
            #endif

            return true;
        }

        // Acumular carácter
        buffer[indice_buffer++] = c;
    }

    // Línea aún no completa
    return false;
}

uint8_t ControladorUSB::leerBloque(uint8_t* buffer, uint8_t cantidad) {
    if (!archivo_abierto || !buffer || cantidad == 0) {
        return 0;
    }

    uint8_t leidos = host_usb.readFile((char*)buffer, cantidad);

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorUSB::leerBloque] Leídos: "));
        Serial.print(leidos);
        Serial.print(F(" / "));
        Serial.println(cantidad);
    #endif

    return leidos;
}

// ========================================
// INFORMACIÓN Y NAVEGACIÓN
// ========================================

const char* ControladorUSB::obtenerNombreArchivoActual() {
    return archivo_abierto ? host_usb.getFileName() : nullptr;
}

size_t ControladorUSB::obtenerTamanoArchivo() {
    return archivo_abierto ? host_usb.getFileSize() : 0;
}

uint32_t ControladorUSB::obtenerPosicionActual() {
    return archivo_abierto ? host_usb.getCursorPos() : 0;
}

bool ControladorUSB::reiniciarArchivo() {
    if (!archivo_abierto) return false;

    const char* nombre = host_usb.getFileName();
    if (!nombre) return false;

    #if MODO_DESARROLLADOR
        Serial.println(F("[ControladorUSB::reiniciarArchivo] Reabriendo archivo..."));
    #endif

    // CH376 no tiene seek directo, hay que reabrir
    cerrarArchivo();
    return abrirArchivo(nombre);
}