#include "gestor_archivos.h"
#include <Arduino.h>

/**
 * @file gestor_archivos.cpp
 * @brief Implementación del gestor unificado de archivos G-code
 */

GestorArchivos::GestorArchivos(ControladorSD &refSD, ControladorUSB &refUSB)
    : sd(refSD), usb(refUSB), origen_actual(TipoDispositivo::NINGUNO),
      total_archivos(0), indice_seleccion(0), archivo_abierto(false) {
    limpiarListaInterna();
    linea_buffer[0] = '\0';
}

// ========================================
// INICIALIZACIÓN Y CONFIGURACIÓN
// ========================================

bool GestorArchivos::inicializar() {
    bool sd_ok = sd.estaLista();
    bool usb_ok = usb.dispositivoListo();

    // Prioridad: USB > SD > NINGUNO
    if (usb_ok) {
        origen_actual = TipoDispositivo::USB;
    } else if (sd_ok) {
        origen_actual = TipoDispositivo::SD;
    } else {
        origen_actual = TipoDispositivo::NINGUNO;
    }

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::inicializar] Origen seleccionado: "));
        switch (origen_actual) {
            case TipoDispositivo::SD:
                Serial.println(F("SD"));
                break;
            case TipoDispositivo::USB:
                Serial.println(F("USB"));
                break;
            default:
                Serial.println(F("NINGUNO"));
                break;
        }
    #endif

    return origen_actual != TipoDispositivo::NINGUNO;
}

TipoDispositivo GestorArchivos::obtenerOrigen() const {
    return origen_actual;
}

void GestorArchivos::cambiarOrigen(TipoDispositivo nuevo_origen) {
    if (nuevo_origen == origen_actual) return;

    origen_actual = nuevo_origen;
    limpiarListaInterna();

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::cambiarOrigen] Nuevo origen: "));
        Serial.println(origen_actual == TipoDispositivo::SD ? F("SD") : F("USB"));
    #endif
}

// ========================================
// LISTADO Y NAVEGACIÓN
// ========================================

bool GestorArchivos::escanearDirectorio(const char* ruta_directorio) {
    limpiarListaInterna();

    const char* ruta = ruta_directorio ? ruta_directorio : "/";

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::escanearDirectorio] Escaneando: "));
        Serial.println(ruta);
    #endif

    bool resultado = false;

    if (origen_actual == TipoDispositivo::USB) {
        resultado = listarArchivosUSB(ruta);
    } else if (origen_actual == TipoDispositivo::SD) {
        resultado = listarArchivosSD(ruta);
    }

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::escanearDirectorio] Archivos G-code encontrados: "));
        Serial.println(total_archivos);
    #endif

    return resultado;
}

size_t GestorArchivos::obtenerCantidadArchivos() const {
    return total_archivos;
}

const char* GestorArchivos::obtenerNombreArchivo(size_t indice) const {
    if (indice >= total_archivos) return nullptr;
    return lista_nombres[indice];
}

void GestorArchivos::navegarLista(int direccion) {
    if (total_archivos == 0) return;

    int idx = (int)indice_seleccion + direccion;

    // Navegación circular
    if (idx < 0) {
        idx = (int)total_archivos - 1;
    } else if ((size_t)idx >= total_archivos) {
        idx = 0;
    }

    indice_seleccion = (size_t)idx;

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::navegarLista] Índice seleccionado: "));
        Serial.print(indice_seleccion);
        Serial.print(F(" ("));
        Serial.print(lista_nombres[indice_seleccion]);
        Serial.println(F(")"));
    #endif
}

size_t GestorArchivos::obtenerIndiceSeleccion() const {
    return indice_seleccion;
}

// ========================================
// GESTIÓN DE ARCHIVOS
// ========================================

bool GestorArchivos::abrirArchivoPorIndice(size_t indice) {
    if (indice >= total_archivos) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[GestorArchivos::abrirArchivoPorIndice] ERROR: índice fuera de rango"));
        #endif
        return false;
    }

    cerrarArchivo();  // Cerrar archivo previo

    const char* nombre = lista_nombres[indice];
    bool ok = false;

    if (origen_actual == TipoDispositivo::USB) {
        ok = usb.abrirArchivo(nombre);
    } else if (origen_actual == TipoDispositivo::SD) {
        ok = sd.abrirArchivo(nombre);
    }

    archivo_abierto = ok;
    if (ok) {
        indice_seleccion = indice;
    }

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::abrirArchivoPorIndice] Índice: "));
        Serial.print(indice);
        Serial.print(F(" | Archivo: "));
        Serial.print(nombre);
        Serial.print(F(" | Estado: "));
        Serial.println(ok ? F("OK") : F("ERROR"));
    #endif

    return ok;
}

bool GestorArchivos::abrirArchivoPorNombre(const char* nombre) {
    if (!nombre || total_archivos == 0) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[GestorArchivos::abrirArchivoPorNombre] ERROR: nombre nulo o lista vacía"));
        #endif
        return false;
    }

    // Validar extensión G-code
    if (!esGcodeNombre(nombre)) {
        #if MODO_DESARROLLADOR
            Serial.print(F("[GestorArchivos::abrirArchivoPorNombre] ERROR: no es archivo G-code: "));
            Serial.println(nombre);
        #endif
        return false;
    }

    // Buscar en la lista (case-insensitive)
    size_t indice_encontrado = SIZE_MAX;
    for (size_t i = 0; i < total_archivos; ++i) {
        if (strcasecmp(nombre, lista_nombres[i]) == 0) {
            indice_encontrado = i;
            break;
        }
    }

    if (indice_encontrado == SIZE_MAX) {
        #if MODO_DESARROLLADOR
            Serial.print(F("[GestorArchivos::abrirArchivoPorNombre] ERROR: archivo no encontrado en lista: "));
            Serial.println(nombre);
        #endif
        return false;
    }

    // Usar abrirArchivoPorIndice para mantener consistencia
    return abrirArchivoPorIndice(indice_encontrado);
}

void GestorArchivos::cerrarArchivo() {
    if (!archivo_abierto) return;

    if (origen_actual == TipoDispositivo::USB) {
        usb.cerrarArchivo();
    } else if (origen_actual == TipoDispositivo::SD) {
        sd.cerrarArchivo();
    }

    archivo_abierto = false;

    #if MODO_DESARROLLADOR
        Serial.println(F("[GestorArchivos::cerrarArchivo] Archivo cerrado"));
    #endif
}

bool GestorArchivos::hayArchivoAbierto() const {
    return archivo_abierto;
}

// ========================================
// LECTURA NO BLOQUEANTE
// ========================================

const char* GestorArchivos::leerLineaNoBloqueante() {
    if (!archivo_abierto) return nullptr;

    linea_buffer[0] = '\0';
    bool linea_lista = false;

    if (origen_actual == TipoDispositivo::USB) {
        linea_lista = leerLineaDesdeUSB();
    } else if (origen_actual == TipoDispositivo::SD) {
        linea_lista = leerLineaDesdeSD();
    }

    if (!linea_lista) return nullptr;

    // Filtrar espacios iniciales
    char *p = linea_buffer;
    while (*p == ' ' || *p == '\t') p++;

    // Ignorar líneas vacías y comentarios
    if (*p == '\0' || *p == ';') {
        return nullptr;
    }

    // Retornar la línea desde el primer carácter no-espacio
    return p;
}

bool GestorArchivos::leerLineaDesdeUSB() {
    return usb.leerLineaNoBloqueante(linea_buffer, sizeof(linea_buffer));
}

bool GestorArchivos::leerLineaDesdeSD() {
    bool ok = sd.leerLineaArchivo(linea_buffer, sizeof(linea_buffer));

    #if MODO_DESARROLLADOR
        if (ok) {
            Serial.print(F("[GestorArchivos::leerLineaDesdeSD] Línea leída: "));
            Serial.println(linea_buffer);
        }
    #endif

    return ok;
}

bool GestorArchivos::reiniciarLecturaActual() {
    if (!archivo_abierto) return false;

    bool ok = false;

    if (origen_actual == TipoDispositivo::USB) {
        ok = usb.reiniciarArchivo();
    } else if (origen_actual == TipoDispositivo::SD) {
        ok = sd.reiniciarLectura();
    }

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::reiniciarLecturaActual] "));
        Serial.println(ok ? F("OK") : F("ERROR"));
    #endif

    return ok;
}

// ========================================
// INFORMACIÓN
// ========================================

size_t GestorArchivos::obtenerTamanoArchivoActual() const {
    if (!archivo_abierto) return 0;

    if (origen_actual == TipoDispositivo::USB) {
        return usb.obtenerTamanoArchivo();
    } else if (origen_actual == TipoDispositivo::SD) {
        return sd.obtenerTamanoArchivo();
    }

    return 0;
}

uint32_t GestorArchivos::obtenerPosicionArchivoActual() const {
    if (!archivo_abierto) return 0;

    if (origen_actual == TipoDispositivo::USB) {
        return usb.obtenerPosicionActual();
    } else if (origen_actual == TipoDispositivo::SD) {
        return sd.obtenerPosicionActual();
    }

    return 0;
}

uint8_t GestorArchivos::calcularPorcentajeProgreso() const {
    if (!archivo_abierto) return 0;

    size_t tamano = obtenerTamanoArchivoActual();
    if (tamano == 0) return 0;

    uint32_t posicion = obtenerPosicionArchivoActual();
    return (uint8_t)((posicion * 100UL) / tamano);
}

// ========================================
// MÉTODOS AUXILIARES PRIVADOS
// ========================================

bool GestorArchivos::esGcodeNombre(const char* nombre) const {
    if (!nombre) return false;

    const char* ext = strrchr(nombre, '.');
    if (!ext) return false;

    // Soporta .gcode, .gco, .gc (case-insensitive)
    return (strcasecmp(ext, ".gcode") == 0 ||
            strcasecmp(ext, ".gco") == 0 ||
            strcasecmp(ext, ".gc") == 0);
}

void GestorArchivos::limpiarListaInterna() {
    total_archivos = 0;
    indice_seleccion = 0;
    for (size_t i = 0; i < MAX_ARCHIVOS; ++i) {
        lista_nombres[i][0] = '\0';
    }
}

void GestorArchivos::adicionarNombreLista(const char* nombre) {
    if (!nombre || total_archivos >= MAX_ARCHIVOS) return;

    strncpy(lista_nombres[total_archivos], nombre, NOMBRE_LONGITUD - 1);
    lista_nombres[total_archivos][NOMBRE_LONGITUD - 1] = '\0';
    ++total_archivos;
}

bool GestorArchivos::listarArchivosUSB(const char* ruta) {
    total_archivos = 0;

    // Variable estática para pasar this al callback
    static GestorArchivos* instancia_temp = nullptr;
    instancia_temp = this;

    // Callback C-style usando lambda sin captura
    auto callback_wrapper = +[](const char* nombre) {
        if (!instancia_temp || !nombre) return;
        
        // Filtrar solo archivos G-code
        if (instancia_temp->esGcodeNombre(nombre)) {
            instancia_temp->adicionarNombreLista(nombre);
        }
    };

    bool ok = usb.listarArchivos(callback_wrapper);

    instancia_temp = nullptr;  // Limpiar puntero temporal

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::listarArchivosUSB] Archivos G-code encontrados: "));
        Serial.println(total_archivos);
    #endif

    return ok && (total_archivos > 0);
}

bool GestorArchivos::listarArchivosSD(const char* ruta) {
    total_archivos = 0;

    // Variable estática para pasar this al callback
    static GestorArchivos* instancia_temp = nullptr;
    instancia_temp = this;

    // Callback C-style usando lambda sin captura
    auto callback_wrapper = +[](const char* nombre) {
        if (!instancia_temp || !nombre) return;
        
        // Filtrar solo archivos G-code
        if (instancia_temp->esGcodeNombre(nombre)) {
            instancia_temp->adicionarNombreLista(nombre);
        }
    };

    bool ok = sd.listarDirectorio(ruta, callback_wrapper);

    instancia_temp = nullptr;  // Limpiar puntero temporal

    #if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos::listarArchivosSD] Archivos G-code encontrados: "));
        Serial.println(total_archivos);
    #endif

    return ok && (total_archivos > 0);
}