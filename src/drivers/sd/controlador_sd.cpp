#include "controlador_sd.h"
#include <Arduino.h>

ControladorSD::ControladorSD() : inicializada(false) {}

bool ControladorSD::iniciar(uint8_t pin_cs) {
    inicializada = SD.begin(pin_cs);

#if MODO_DESARROLLADOR
    if (inicializada)
        Serial.println(F("[ControladorSD] SD inicializada correctamente"));
    else
        Serial.println(F("[ControladorSD] ERROR: No se pudo inicializar la tarjeta SD"));
#endif

    return inicializada;
}

bool ControladorSD::estaLista() const {
    return inicializada;
}

bool ControladorSD::existeArchivo(const char* nombre) {
    if (!inicializada || !nombre) return false;
    bool existe = SD.exists(nombre);

#if MODO_DESARROLLADOR
    Serial.print(F("[ControladorSD] Verificando existencia de archivo '"));
    Serial.print(nombre);
    Serial.print(F("': "));
    Serial.println(existe ? F("SI") : F("NO"));
#endif

    return existe;
}

bool ControladorSD::abrirArchivo(const char* nombre) {
    if (!inicializada || !nombre) return false;

    if (archivo_actual) {
        archivo_actual.close();
    }

    archivo_actual = SD.open(nombre, FILE_READ);

#if MODO_DESARROLLADOR
    if (archivo_actual) {
        Serial.print(F("[ControladorSD] Archivo abierto: "));
        Serial.println(nombre);
        Serial.print(F("Tamaño: "));
        Serial.print(archivo_actual.size());
        Serial.println(F(" bytes"));
    } else {
        Serial.print(F("[ControladorSD] ERROR al abrir archivo: "));
        Serial.println(nombre);
    }
#endif

    return (bool)archivo_actual;
}

void ControladorSD::cerrarArchivo() {
    if (archivo_actual) {
#if MODO_DESARROLLADOR
        Serial.println(F("[ControladorSD] Cerrando archivo actual"));
#endif
        archivo_actual.close();
    }
}

bool ControladorSD::leerLineaArchivo(char* buffer, size_t tamano) {
    if (!archivo_actual || !buffer || tamano == 0) return false;

    size_t indice = 0;
    while (indice < tamano - 1) {
        int c = archivo_actual.read();
        if (c == -1) break;       // EOF
        if (c == '\r') continue;  // Ignorar CR
        if (c == '\n') break;     // Fin de línea
        buffer[indice++] = (char)c;
    }

    buffer[indice] = '\0';

#if MODO_DESARROLLADOR
    if (indice > 0) {
        Serial.print(F("[ControladorSD] Línea leída ("));
        Serial.print(indice);
        Serial.println(F(" chars)"));
    }
#endif

    return (indice > 0);
}

bool ControladorSD::reiniciarLectura() {
    if (!archivo_actual) return false;
    bool ok = archivo_actual.seek(0);

#if MODO_DESARROLLADOR
    Serial.println(ok ? F("[ControladorSD] Lectura reiniciada") : F("[ControladorSD] ERROR al reiniciar lectura"));
#endif

    return ok;
}

size_t ControladorSD::obtenerTamanoArchivo() {
    if (!archivo_actual) {
        return 0;
    }
    return archivo_actual.size();
}
bool ControladorSD::esFinDeArchivo() {
    if (!archivo_actual) return true;
    return archivo_actual.available() == 0;
}

size_t ControladorSD::listarArchivosRaiz(size_t max_archivos) {
    if (!inicializada) return 0;

    File dir = SD.open("/");
    if (!dir) {
#if MODO_DESARROLLADOR
        Serial.println(F("[ControladorSD] ERROR al abrir directorio raíz"));
#endif
        return 0;
    }

    size_t contador = 0;

#if MODO_DESARROLLADOR
    Serial.println(F("[ControladorSD] === Archivos en raíz ==="));
#endif

    while (true) {
        File archivo = dir.openNextFile();
        if (!archivo) break;
        if (!archivo.isDirectory()) {
#if MODO_DESARROLLADOR
            Serial.print(F(" - "));
            Serial.print(archivo.name());
            Serial.print(F(" ("));
            Serial.print(archivo.size());
            Serial.println(F(" bytes)"));
#endif
            contador++;
            if (contador >= max_archivos) break;
        }
        archivo.close();
    }

    dir.close();

#if MODO_DESARROLLADOR
    Serial.print(F("[ControladorSD] Total archivos listados: "));
    Serial.println(contador);
#endif

    return contador;
}

ControladorSD::~ControladorSD() {
    cerrarArchivo();
    SD.end();

#if MODO_DESARROLLADOR
    Serial.println(F("[ControladorSD] Recursos liberados y SD finalizada"));
#endif
}
