#include "controlador_sd.h"
#include <Arduino.h>

/**
 * @file controlador_sd.cpp
 * @brief Implementación del controlador de tarjeta SD
 */

ControladorSD::ControladorSD() : inicializada(false) {}

ControladorSD::~ControladorSD() {
    cerrarArchivo();
    if (inicializada) {
        SD.end();
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorSD] Recursos liberados"));
        #endif
    }
}

// ========================================
// INICIALIZACIÓN Y ESTADO
// ========================================

bool ControladorSD::iniciar(uint8_t pin_cs) {
    inicializada = SD.begin(pin_cs);

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::iniciar] Pin CS: "));
        Serial.print(pin_cs);
        Serial.print(F(" | Estado: "));
        Serial.println(inicializada ? F("OK") : F("ERROR"));
    #endif

    return inicializada;
}

bool ControladorSD::estaLista() const {
    return inicializada;
}

bool ControladorSD::esFinDeArchivo() {
    if (!archivo_actual) return true;
    return archivo_actual.available() == 0;
}

// ========================================
// GESTIÓN DE ARCHIVOS
// ========================================

bool ControladorSD::existeArchivo(const char* nombre) {
    if (!inicializada || !nombre) return false;
    
    bool existe = SD.exists(nombre);

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::existeArchivo] '"));
        Serial.print(nombre);
        Serial.print(F("': "));
        Serial.println(existe ? F("SI") : F("NO"));
    #endif

    return existe;
}

bool ControladorSD::abrirArchivo(const char* nombre) {
    if (!inicializada || !nombre) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorSD::abrirArchivo] ERROR: SD no inicializada o nombre nulo"));
        #endif
        return false;
    }

    // Cerrar archivo previo
    if (archivo_actual) {
        archivo_actual.close();
    }

    archivo_actual = SD.open(nombre, FILE_READ);

    #if MODO_DESARROLLADOR
        if (archivo_actual) {
            Serial.print(F("[ControladorSD::abrirArchivo] Abierto: "));
            Serial.print(nombre);
            Serial.print(F(" | Tamaño: "));
            Serial.print(archivo_actual.size());
            Serial.println(F(" bytes"));
        } else {
            Serial.print(F("[ControladorSD::abrirArchivo] ERROR al abrir: "));
            Serial.println(nombre);
        }
    #endif

    return (bool)archivo_actual;
}

void ControladorSD::cerrarArchivo() {
    if (archivo_actual) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorSD::cerrarArchivo] Cerrando archivo"));
        #endif
        archivo_actual.close();
    }
}

bool ControladorSD::listarDirectorio(const char* ruta, void (*callback)(const char* nombre)) {
    if (!inicializada || !ruta || !callback) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorSD::listarDirectorio] ERROR: parámetros inválidos"));
        #endif
        return false;
    }

    File dir = SD.open(ruta);
    if (!dir) {
        #if MODO_DESARROLLADOR
            Serial.print(F("[ControladorSD::listarDirectorio] ERROR al abrir: "));
            Serial.println(ruta);
        #endif
        return false;
    }

    if (!dir.isDirectory()) {
        #if MODO_DESARROLLADOR
            Serial.print(F("[ControladorSD::listarDirectorio] No es directorio: "));
            Serial.println(ruta);
        #endif
        dir.close();
        return false;
    }

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::listarDirectorio] Listando: "));
        Serial.println(ruta);
        uint8_t contador = 0;
    #endif

    while (true) {
        File archivo = dir.openNextFile();
        if (!archivo) break;
        
        if (!archivo.isDirectory()) {
            callback(archivo.name());
            #if MODO_DESARROLLADOR
                Serial.print(F("  ["));
                Serial.print(++contador);
                Serial.print(F("] "));
                Serial.print(archivo.name());
                Serial.print(F(" ("));
                Serial.print(archivo.size());
                Serial.println(F(" bytes)"));
            #endif
        }
        
        archivo.close();
    }

    dir.close();

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::listarDirectorio] Total: "));
        Serial.println(contador);
    #endif

    return true;
}

size_t ControladorSD::listarArchivosRaiz(size_t max_archivos) {
    if (!inicializada) return 0;

    File dir = SD.open("/");
    if (!dir) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[ControladorSD::listarArchivosRaiz] ERROR al abrir raíz"));
        #endif
        return 0;
    }

    size_t contador = 0;

    #if MODO_DESARROLLADOR
        Serial.println(F("[ControladorSD::listarArchivosRaiz] === Archivos en raíz ==="));
    #endif

    while (true) {
        File archivo = dir.openNextFile();
        if (!archivo) break;
        
        if (!archivo.isDirectory()) {
            #if MODO_DESARROLLADOR
                Serial.print(F(" ["));
                Serial.print(contador + 1);
                Serial.print(F("] "));
                Serial.print(archivo.name());
                Serial.print(F(" ("));
                Serial.print(archivo.size());
                Serial.println(F(" bytes)"));
            #endif
            
            contador++;
            if (contador >= max_archivos) {
                archivo.close();
                break;
            }
        }
        
        archivo.close();
    }

    dir.close();

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::listarArchivosRaiz] Total: "));
        Serial.println(contador);
    #endif

    return contador;
}

// ========================================
// LECTURA
// ========================================

bool ControladorSD::leerLineaArchivo(char* buffer, size_t tamano) {
    if (!archivo_actual || !buffer || tamano == 0) {
        return false;
    }

    size_t indice = 0;
    
    while (indice < tamano - 1) {
        int c = archivo_actual.read();
        
        if (c == -1) {
            // EOF
            break;
        }
        
        if (c == '\r') {
            // Ignorar CR
            continue;
        }
        
        if (c == '\n') {
            // Fin de línea
            break;
        }
        
        buffer[indice++] = (char)c;
    }

    buffer[indice] = '\0';

    #if MODO_DESARROLLADOR
        if (indice > 0) {
            Serial.print(F("[ControladorSD::leerLineaArchivo] Leída ("));
            Serial.print(indice);
            Serial.println(F(" chars)"));
        }
    #endif

    return (indice > 0);
}

int ControladorSD::leerBloque(void* buffer, uint16_t cantidad) {
    if (!archivo_actual || !buffer) {
        return 0;
    }

    int leidos = archivo_actual.read(buffer, cantidad);

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::leerBloque] Leídos: "));
        Serial.print(leidos);
        Serial.print(F(" / "));
        Serial.println(cantidad);
    #endif

    return leidos;
}

// ========================================
// NAVEGACIÓN
// ========================================

bool ControladorSD::reiniciarLectura() {
    if (!archivo_actual) {
        return false;
    }

    bool ok = archivo_actual.seek(0);

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::reiniciarLectura] "));
        Serial.println(ok ? F("OK") : F("ERROR"));
    #endif

    return ok;
}

bool ControladorSD::moverCursor(uint32_t posicion) {
    if (!archivo_actual) {
        return false;
    }

    bool ok = archivo_actual.seek(posicion);

    #if MODO_DESARROLLADOR
        Serial.print(F("[ControladorSD::moverCursor] Posición: "));
        Serial.print(posicion);
        Serial.print(F(" | "));
        Serial.println(ok ? F("OK") : F("ERROR"));
    #endif

    return ok;
}

// ========================================
// INFORMACIÓN
// ========================================

size_t ControladorSD::obtenerTamanoArchivo() {
    if (!archivo_actual) {
        return 0;
    }
    return archivo_actual.size();
}

uint32_t ControladorSD::obtenerPosicionActual() {
    if (!archivo_actual) {
        return 0;
    }
    return archivo_actual.position();
}

const char* ControladorSD::obtenerNombreArchivoActual() {
    if (!archivo_actual) {
        return nullptr;
    }
    return archivo_actual.name();
}

int ControladorSD::obtenerBytesDisponibles() {
    if (!archivo_actual) {
        return 0;
    }
    return archivo_actual.available();
}