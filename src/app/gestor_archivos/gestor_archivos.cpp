#include "gestor_archivos.h"
#include <Arduino.h>

/**
 * @file gestor_archivos.cpp
 * @brief Implementación del GestorArchivos que integra SD y USB.
 */

GestorArchivos::GestorArchivos(ControladorSD &refSD, ControladorUSB &refUSB)
    : sd(refSD), usb(refUSB), origen_actual(TipoDispositivo::NINGUNO),
      total_archivos(0), indice_seleccion(0), archivo_abierto(false) {
    limpiarListaInterna();
    linea_buffer[0] = '\0';
}

bool GestorArchivos::inicializar() {
    bool sd_ok = sd.estaLista();
    bool usb_ok = usb.dispositivoListo();

    if (usb_ok) origen_actual = TipoDispositivo::USB;
    else if (sd_ok) origen_actual = TipoDispositivo::SD;
    else origen_actual = TipoDispositivo::NINGUNO;

#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] inicializar: origen seleccionado = "));
    switch (origen_actual) {
        case TipoDispositivo::SD: Serial.println(F("SD")); break;
        case TipoDispositivo::USB: Serial.println(F("USB")); break;
        default: Serial.println(F("NINGUNO")); break;
    }
#endif

    return origen_actual != TipoDispositivo::NINGUNO;
}

TipoDispositivo GestorArchivos::obtenerOrigen() const {
    return origen_actual;
}

void GestorArchivos::cambiarOrigen(TipoDispositivo nuevo_origen) {
    origen_actual = nuevo_origen;
#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] cambiarOrigen -> "));
    Serial.println(origen_actual == TipoDispositivo::SD ? F("SD") : F("USB"));
#endif
    limpiarListaInterna();
}

bool GestorArchivos::esGcodeNombre(const char* nombre) const {
    if (!nombre) return false;
    const char* ext = strrchr(nombre, '.');
    if (!ext) return false;
    // soporta .GCO, .GC, .GCODE (case-insensitive)
    return (strcasecmp(ext, ".gcode") == 0 ||
            strcasecmp(ext, ".gco") == 0 ||
            strcasecmp(ext, ".gc") == 0);
}

void GestorArchivos::limpiarListaInterna() {
    total_archivos = 0;
    indice_seleccion = 0;
    for (size_t i = 0; i < MAX_ARCHIVOS; ++i) lista_nombres[i][0] = '\0';
}

void GestorArchivos::adicionarNombreLista(const char* nombre) {
    if (!nombre) return;
    if (total_archivos >= MAX_ARCHIVOS) return;
    // Copia segura truncando si es necesario. Nombres FAT cortos ya caben.
    strncpy(lista_nombres[total_archivos], nombre, NOMBRE_LONGITUD - 1);
    lista_nombres[total_archivos][NOMBRE_LONGITUD - 1] = '\0';
    ++total_archivos;
}

bool GestorArchivos::escanearDirectorio(const char* ruta_directorio) {
    limpiarListaInterna();

#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] Escaneando directorio: "));
    Serial.println(ruta_directorio ? ruta_directorio : F("/"));
#endif

    if (origen_actual == TipoDispositivo::USB) {
        return listarArchivosUSB(ruta_directorio ? ruta_directorio : "/");
    } else if (origen_actual == TipoDispositivo::SD) {
        return listarArchivosSD(ruta_directorio ? ruta_directorio : "/");
    }

    return false;
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
    if (idx < 0) idx = (int)total_archivos - 1;
    if ((size_t)idx >= total_archivos) idx = 0;
    indice_seleccion = (size_t)idx;
#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] seleccionar indice: "));
    Serial.println(indice_seleccion);
#endif
}

size_t GestorArchivos::obtenerIndiceSeleccion() const {
    return indice_seleccion;
}

bool GestorArchivos::abrirArchivoPorIndice(size_t indice) {
    if (indice >= total_archivos) return false;
    cerrarArchivo();

    const char* nombre = lista_nombres[indice];
    bool ok = false;

    if (origen_actual == TipoDispositivo::USB) {
        ok = usb.abrirArchivo(nombre);
    } else if (origen_actual == TipoDispositivo::SD) {
        ok = sd.abrirArchivo(nombre);
    }

    archivo_abierto = ok;
    if (ok) indice_seleccion = indice;

#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] abrirArchivoPorIndice: "));
    Serial.print(indice);
    Serial.print(F(" -> "));
    Serial.println(ok ? F("OK") : F("ERROR"));
#endif

    return ok;
}

void GestorArchivos::cerrarArchivo() {
    if (!archivo_abierto) return;

    if (origen_actual == TipoDispositivo::USB) usb.cerrarArchivo();
    else if (origen_actual == TipoDispositivo::SD) sd.cerrarArchivo();

    archivo_abierto = false;

#if MODO_DESARROLLADOR
    Serial.println(F("[GestorArchivos] Archivo cerrado"));
#endif
}

/**
 * @brief Lectura no bloqueante: intenta leer una línea del archivo abierto.
 *
 * Para USB: se usa la función no bloqueante del controlador USB.  
 * Para SD: se usa el método leerLineaArchivo del ControladorSD (puede bloquear).
 */
const char* GestorArchivos::leerLineaNoBloqueante() {
    if (!archivo_abierto) return nullptr;

    // Limpieza previa del buffer
    linea_buffer[0] = '\0';

    bool linea_listo = false;

    if (origen_actual == TipoDispositivo::USB) {
        // USB: no bloqueante
        linea_listo = leerLineaDesdeUSB();
    } else if (origen_actual == TipoDispositivo::SD) {
        // SD: best-effort (puede bloquear dependiendo de la implementacion SD)
        linea_listo = leerLineaDesdeSD();
    }

    if (!linea_listo) return nullptr;

    // Filtrar líneas vacías y comentarios
    // quitar espacios iniciales
    char *p = linea_buffer;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\0' || *p == ';') {
        // ignorar y solicitar siguiente llamada
        return nullptr;
    }

    return linea_buffer;
}

bool GestorArchivos::leerLineaDesdeUSB() {
    // Asume que usb.leerLineaNoBloqueante rellena linea_buffer y devuelve true cuando línea completa
    bool ok = usb.leerLineaNoBloqueante(linea_buffer, sizeof(linea_buffer));
    return ok;
}

bool GestorArchivos::leerLineaDesdeSD() {
    // Nota: ControladorSD::leerLineaArchivo lee hasta \n en una sola llamada.
    // Dependiendo de las implementaciones esto puede tomar algo de tiempo.
    // Aquí la llamamos pero documentamos que puede bloquear.
    bool ok = sd.leerLineaArchivo(linea_buffer, sizeof(linea_buffer));
#if MODO_DESARROLLADOR
    if (ok) {
        Serial.print(F("[GestorArchivos] Línea SD leída: "));
        Serial.println(linea_buffer);
    }
#endif
    return ok;
}

bool GestorArchivos::reiniciarLecturaActual() {
    if (!archivo_abierto) return false;
    if (origen_actual == TipoDispositivo::USB) {
        const char* nombre = lista_nombres[indice_seleccion];
        // reabrir el archivo (CH376 no tiene seek sencillo, reopen)
        usb.cerrarArchivo();
        bool ok = usb.abrirArchivo(nombre);
#if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos] reiniciarLecturaActual USB -> "));
        Serial.println(ok ? F("OK") : F("ERROR"));
#endif
        archivo_abierto = ok;
        return ok;
    } else if (origen_actual == TipoDispositivo::SD) {
        bool ok = sd.reiniciarLectura();
#if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos] reiniciarLecturaActual SD -> "));
        Serial.println(ok ? F("OK") : F("ERROR"));
#endif
        return ok;
    }
    return false;
}

bool GestorArchivos::hayArchivoAbierto() const {
    return archivo_abierto;
}

size_t GestorArchivos::obtenerTamanoArchivoActual() const {
    if (!archivo_abierto) return 0;
    if (origen_actual == TipoDispositivo::USB) return usb.obtenerTamanoArchivo();
    if (origen_actual == TipoDispositivo::SD) return sd.obtenerTamanoArchivo();
    return 0;
}

uint32_t GestorArchivos::obtenerPosicionArchivoActual() const {
    if (!archivo_abierto) return 0;
    if (origen_actual == TipoDispositivo::USB) return usb.obtenerPosicionActual();
    //if (origen_actual == TipoDispositivo::SD) return sd.();
    return 0;
}

/* =========================
   Listado específico USB/SD
   ========================= */

bool GestorArchivos::listarArchivosUSB(const char* ruta) {
    // Callback que copia nombres al buffer local
    struct Ctx { GestorArchivos* self; };
    Ctx ctx{ this };

    // Callback debe ser una función C; definimos una lambda sin captura y usamos un puntero a contexto
    auto callback_copy = [](const char* nombre) {
        // Esta función no tiene contexto directo; para mantener simple
        // usaremos una función estática auxiliar que depende de una variable global
        // Para evitar globals complicados, implementamos la copia manualmente:
#if MODO_DESARROLLADOR
        // En este diseño preferimos usar un callback que directamente llene la lista
#endif
    };

    // Como la firma del controlador USB es void (*)(const char*), usaremos una función intermedia:
    // Implementamos aquí una lambda que llama a un método estático que usa un puntero temporal.
    // SOLUCIÓN práctica: llamamos a usb.listarArchivos y recolectamos nombres consultando host_usb.getFileName()
    // pero el controlador USB ya invoca el callback con el nombre.
    // Para evitar complicaciones con capturas, definimos un callback estático que escribe en una estructura global temporal.
    // Para mantenerlo sencillo en este contexto, invocamos usb.listarArchivos y copiamos a través de una función local:
    
    total_archivos = 0; // reiniciar
    
    // Definimos un callback C-style que copia en la lista del objeto actual.
    // Para ello necesitamos un puntero estático temporal al this — lo liberamos al final.
    static GestorArchivos* s_tmp_this = nullptr;
    s_tmp_this = this;

    auto cb = [](const char* nombre) {
        if (!s_tmp_this) return;
        if (s_tmp_this->total_archivos >= MAX_ARCHIVOS) return;
        s_tmp_this->adicionarNombreLista(nombre);
    };

    // Convertimos la lambda sin captura a función C mediante puntero a función:
    void (*cb_func)(const char*) = +[](const char* nombre) {
        if (!s_tmp_this) return;
        if (s_tmp_this->total_archivos >= MAX_ARCHIVOS) return;
        s_tmp_this->adicionarNombreLista(nombre);
    };

    // Llamada al controlador USB
    bool ok = usb.listarArchivos(cb_func);

    // limpiar puntero temporal
    s_tmp_this = nullptr;

#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] listarArchivosUSB: encontrado "));
    Serial.println(total_archivos);
#endif

    return ok && total_archivos > 0;
}

bool GestorArchivos::listarArchivosSD(const char* ruta) {
    // Abrir el directorio indicado (para SD.h usamos SD.open)
    File dir = SD.open(ruta);
    if (!dir) {
#if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos] listarArchivosSD: no se pudo abrir ruta "));
        Serial.println(ruta);
#endif
        return false;
    }

    total_archivos = 0;

    while (true) {
        File f = dir.openNextFile();
        if (!f) break;
        if (!f.isDirectory()) {
            const char* nombre = f.name();
            if (esGcodeNombre(nombre) && total_archivos < MAX_ARCHIVOS) {
                adicionarNombreLista(nombre);
            }
        }
        f.close();
    }
    dir.close();

#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] listarArchivosSD: encontrado "));
    Serial.println(total_archivos);
#endif

    return total_archivos > 0;
}

bool GestorArchivos::abrirArchivoPorNombre(const char* nombre) {
    if (!nombre || total_archivos == 0) {
#if MODO_DESARROLLADOR
        Serial.println(F("[GestorArchivos] abrirArchivoPorNombre: lista vacía o nombre nulo"));
#endif
        return false;
    }

    // Buscar el nombre (insensible a mayúsculas)
    size_t indice_encontrado = SIZE_MAX;
    for (size_t i = 0; i < total_archivos; ++i) {
        if (strcasecmp(nombre, lista_nombres[i]) == 0) {
            indice_encontrado = i;
            break;
        }
    }

    if (indice_encontrado == SIZE_MAX) {
#if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos] abrirArchivoPorNombre: archivo no encontrado -> "));
        Serial.println(nombre);
#endif
        return false;
    }

    // Validar extensión G-code
    if (!esGcodeNombre(nombre)) {
#if MODO_DESARROLLADOR
        Serial.print(F("[GestorArchivos] abrirArchivoPorNombre: no es archivo G-code -> "));
        Serial.println(nombre);
#endif
        return false;
    }

    cerrarArchivo();  // cerrar si ya hay uno abierto

    bool ok = false;

    if (origen_actual == TipoDispositivo::USB) {
        ok = usb.abrirArchivo(nombre);
    } else if (origen_actual == TipoDispositivo::SD) {
        ok = sd.abrirArchivo(nombre);
    }

    archivo_abierto = ok;
    if (ok) indice_seleccion = indice_encontrado;

#if MODO_DESARROLLADOR
    Serial.print(F("[GestorArchivos] abrirArchivoPorNombre -> "));
    Serial.print(nombre);
    Serial.print(F(" | resultado: "));
    Serial.println(ok ? F("OK") : F("ERROR"));
#endif

    return ok;
}
