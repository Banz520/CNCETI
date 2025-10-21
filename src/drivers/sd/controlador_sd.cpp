#include "controlador_sd.h"
#include "constantes.h"
#include <string.h>

// Inicialización de miembros estáticos (solo necesarios si son constexpr)
const size_t ControladorSD::MAX_ARCHIVOS;
const size_t ControladorSD::MAX_LONGITUD_NOMBRE;

ControladorSD::ControladorSD() : contadorArchivos(0) {
    // Inicializar buffer en constructor
    limpiarListaArchivos();
}

/**
 * @brief Inicia la SD y asigna el directorio raiz
 * @return Regresa falso si no se logra iniciar la SD y verdadero si lo logra
 */
bool ControladorSD::iniciarSD(){
    if(!SD.begin()) {
        #if MODO_DESARROLLADOR
            Serial.println("Error al iniciar la tarjeta SD");
        #endif
        return false;
    }
    
    #if MODO_DESARROLLADOR
        Serial.println("La tarjeta SD se inicializo con exito");
    #endif

    directorio_raiz = SD.open("/");
    return true;
}

/**
 * @brief Este metodo se utiliza para el modo desarrollador para mostrar la estructura el directorio especificado
 * @param dir El directorio que se quiere imprimir
 */
#if MODO_DESARROLLADOR
void ControladorSD::imprimirEstructuraDirectorio(File dir, int numTabs) {
    while (true) {
        File archivo_actual = dir.openNextFile();

        if (!archivo_actual) {
            #if MODO_DESARROLLADOR
                if (numTabs == 0) {
                    Serial.println("No se encontraron mas archivos en el directorio");
                }
            #endif
            break;
        }

        // Imprimir indentación
        for (uint8_t i = 0; i < numTabs; i++) {
            #if MODO_DESARROLLADOR
                Serial.print('\t');
            #endif
        }

        #if MODO_DESARROLLADOR
            Serial.print(archivo_actual.name());
        #endif

        if (archivo_actual.isDirectory()) {
            #if MODO_DESARROLLADOR
                Serial.println("/");
                imprimirEstructuraDirectorio(archivo_actual, numTabs + 1);
            #endif
            
        } else {
            #if MODO_DESARROLLADOR
                Serial.print("\t\t");
                Serial.println(archivo_actual.size(), DEC);
            #endif
        }

        archivo_actual.close();
    }
}
#endif

/**
 * @brief Verifica si un archivo tiene extension .gcode o .gc (formatos cortos FAT32)
 * 
 * Esta funcion detecta archivos G-code en ambos formatos:
 * - Formato largo: ".gcode", ".GCODE" (6 caracteres)
 * - Formato corto FAT32: ".GCO", ".gco" (4 caracteres)
 * 
 * @param nombreArchivo Puntero al nombre del archivo a verificar
 * @return true Si el archivo tiene extension valida de G-code
 * @return false Si el archivo es invalido o no tiene extension G-code
 * 
 * @note Compatible con formato 8.3 de FAT32 que trunca extensiones largas
 * @note Case-insensitive para mayor flexibilidad
 */
bool ControladorSD::esArchivoGcode(const char* nombreArchivo) {
    // Validacion de entrada segura
    if (!nombreArchivo || strlen(nombreArchivo) == 0) {
        #if MODO_DESARROLLADOR
            Serial.println("[ERROR] esArchivoGcode: Nombre de archivo nulo o vacio");
        #endif
        return false;
    }
    
    size_t longitud = strlen(nombreArchivo);
    
    // Verificar longitud minima para contener cualquier extension valida
    if (longitud < 4) { // Minimo: "a.gco"
        #if MODO_DESARROLLADOR
            Serial.print("[ERROR] esArchivoGcode: Nombre demasiado corto - '");
            Serial.print(nombreArchivo);
            Serial.println("'");
        #endif
        return false;
    }
    
    #if MODO_DESARROLLADOR
        Serial.print("[DEBUG] esArchivoGcode: Analizando '");
        Serial.print(nombreArchivo);
        Serial.print("' - Longitud: ");
        Serial.println(longitud);
    #endif
    
    // Obtener la extension (parte despues del ultimo punto)
    const char* extension = strrchr(nombreArchivo, '.');
    
    if (!extension) {
        #if MODO_DESARROLLADOR
            Serial.print("[ERROR] esArchivoGcode: Sin extension - '");
            Serial.print(nombreArchivo);
            Serial.println("'");
        #endif
        return false;
    }
    
    #if MODO_DESARROLLADOR
        Serial.print("[DEBUG] Extension detectada: '");
        Serial.print(extension);
        Serial.println("'");
    #endif
    
    // Verificar extensiones validas (case-insensitive)
    bool esValido = false;
    
    // Formato largo: .gcode (6 caracteres incluido el punto)
    if (strlen(extension) >= 6) {
        esValido = (strcasecmp(extension, ".gcode") == 0);
        #if MODO_DESARROLLADOR
            if (esValido) Serial.println("[OK] Formato largo .gcode detectado");
        #endif
    }
    
    // Formato corto FAT32: .gco (4 caracteres incluido el punto)
    if (!esValido && strlen(extension) >= 4) {
        esValido = (strcasecmp(extension, ".gco") == 0);
        #if MODO_DESARROLLADOR
            if (esValido) Serial.println("[OK] Formato corto .gco detectado (FAT32 8.3)");
        #endif
    }
    
    // Resultado final
    #if MODO_DESARROLLADOR
        if (esValido) {
            Serial.print("[SUCCESS] '");
            Serial.print(nombreArchivo);
            Serial.println("' ES archivo G-code valido");
        } else {
            Serial.print("[INFO] '");
            Serial.print(nombreArchivo);
            Serial.println("' NO es archivo G-code");
        }
    #endif
    
    return esValido;
}

void ControladorSD::agregarArchivoALista(const char* nombreArchivo) {
    if (contadorArchivos >= MAX_ARCHIVOS) {
        #if MODO_DESARROLLADOR
            Serial.println(">>> LISTA DE ARCHIVOS LLENA - No se puede agregar más");
        #endif
        return;
    }
    
    if (!nombreArchivo || strlen(nombreArchivo) == 0) {
        return;
    }
    
    // Copiar nombre al buffer pre-asignado (más seguro)
    strncpy(bufferArchivos[contadorArchivos], nombreArchivo, MAX_LONGITUD_NOMBRE - 1);
    bufferArchivos[contadorArchivos][MAX_LONGITUD_NOMBRE - 1] = '\0'; // Null-terminate seguro
    
    contadorArchivos++;
    
    #if MODO_DESARROLLADOR
        Serial.print(">>> ARCHIVO GCODE AGREGADO: ");
        Serial.println(nombreArchivo);
    #endif
}

const char** ControladorSD::leerDirectorio(File dir, int numTabs) {
    // Reiniciar contador al inicio de nuevo escaneo
    if (numTabs == 0) {
        limpiarListaArchivos();
        
        #if MODO_DESARROLLADOR
            Serial.println("=== INICIANDO ESCANEO DE DIRECTORIO ===");
        #endif
    }

    while (true) {
        File archivo_actual = dir.openNextFile();

        if (!archivo_actual) {
            #if MODO_DESARROLLADOR
                if (numTabs == 0) {
                    Serial.println("=== ESCANEO DE DIRECTORIO COMPLETADO ===");
                }
            #endif
            break;
        }

        const char* nombreArchivo = archivo_actual.name();
        
        #if MODO_DESARROLLADOR
            // Imprimir estructura completa del directorio
            for (uint8_t i = 0; i < numTabs; i++) {
                Serial.print('\t');
            }
            Serial.print(nombreArchivo);
        #endif

        if (archivo_actual.isDirectory()) {
            #if MODO_DESARROLLADOR
                Serial.println("/");
            #endif
            leerDirectorio(archivo_actual, numTabs + 1);
        } else {
            #if MODO_DESARROLLADOR
                Serial.print("\t\t");
                Serial.println(archivo_actual.size(), DEC);
            #endif
            
            // Verificar si es archivo .gcode y agregar a lista
            if (esArchivoGcode(nombreArchivo)) {
                agregarArchivoALista(nombreArchivo);
            }
        }

        archivo_actual.close();
    }

    // Retornar lista solo cuando se complete el escaneo recursivo
    if (numTabs == 0) {
        #if MODO_DESARROLLADOR
            Serial.print("Total de archivos .gcode encontrados: ");
            Serial.println(contadorArchivos);
        #endif
        
        // Crear array de punteros para retornar (terminado en nullptr)
        static const char* listaPunteros[MAX_ARCHIVOS + 1];
        for (size_t i = 0; i < contadorArchivos; i++) {
            listaPunteros[i] = bufferArchivos[i];
        }
        listaPunteros[contadorArchivos] = nullptr; // Marcador fin de lista
        
        return listaPunteros;
    }
    
    return nullptr;
}

const char** ControladorSD::obtenerListaArchivosGcode() {
    // Método conveniente que combina abrir directorio y leer
    if (!directorio_raiz) {
        directorio_raiz = SD.open("/");
    }
    
    if (directorio_raiz) {
        return leerDirectorio(directorio_raiz, 0);
    }
    
    #if MODO_DESARROLLADOR
        Serial.println("Error: No se pudo abrir directorio raiz");
    #endif
    
    return nullptr;
}

//no sabia que vs code tambien dejaba hacer eso como en netbeans
/**
 * @brief regresa el contador de archivos
 */
size_t ControladorSD::obtenerCantidadArchivos() const {
    return contadorArchivos;
}

const char* ControladorSD::obtenerNombreArchivo(size_t indice) const {
    if (indice < contadorArchivos) {
        return bufferArchivos[indice];
    }
    return nullptr;
}

bool ControladorSD::listaVacia() const {
    return contadorArchivos == 0;
}

/**
 * @brief limpia el buffer y resetea el contador de archivos a 0
 */
void ControladorSD::limpiarListaArchivos() {
    contadorArchivos = 0;
    // Limpiar buffers (opcional, pero buena práctica)
    for (size_t i = 0; i < MAX_ARCHIVOS; i++) {
        bufferArchivos[i][0] = '\0';
    }
    
    #if MODO_DESARROLLADOR
        Serial.println("Lista de archivos limpiada");
    #endif
}

ControladorSD::~ControladorSD(){
    if (directorio_raiz) {
        directorio_raiz.close();
    }
    SD.end();
    
    #if MODO_DESARROLLADOR
        Serial.println("ControladorSD destruido - recursos liberados");
    #endif
}