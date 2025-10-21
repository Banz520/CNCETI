#include "controlador_sd.h"
#include "constantes.h"
#include <string.h>

// Inicialización de miembros estáticos
const size_t ControladorSD::MAX_ARCHIVOS;
const size_t ControladorSD::MAX_LONGITUD_NOMBRE;
const size_t ControladorSD::TAMANO_BUFFER_LINEA;

/**
 * @brief Constructor de la clase ControladorSD
 * @details Inicializa el contador de archivos y limpia la lista interna
 */
ControladorSD::ControladorSD() : contador_archivos(0) {
    limpiarListaArchivos();
}

/**
 * @brief Inicializa la tarjeta SD y abre el directorio raíz
 * @return true si la inicialización fue exitosa, false en caso contrario
 * @note Este método debe ser llamado antes de cualquier operación con la SD
 */
bool ControladorSD::iniciarSD(){
    if(!SD.begin()) {
        #if MODO_DESARROLLADOR
            Serial.println("[ERROR] Error al iniciar la tarjeta SD");
        #endif
        return false;
    }
    
    #if MODO_DESARROLLADOR
        Serial.println("[SUCCESS] La tarjeta SD se inicializo con exito");
    #endif

    directorio_raiz = SD.open("/");
    return true;
}

#if MODO_DESARROLLADOR
/**
 * @brief Imprime recursivamente la estructura de directorios y archivos
 * @param dir Directorio a imprimir
 * @param num_tabs Nivel de indentación para la visualización
 * @note Método disponible solo en modo desarrollador
 */
void ControladorSD::imprimirEstructuraDirectorio(File dir, int num_tabs) {
    while (true) {
        File archivo_actual = dir.openNextFile();

        if (!archivo_actual) {
            if (num_tabs == 0) {
                Serial.println("[INFO] No se encontraron mas archivos en el directorio");
            }
            break;
        }

        // Usar name() para obtener el nombre real del archivo
        for (uint8_t i = 0; i < num_tabs; i++) {
            Serial.print('\t');
        }

        Serial.print(archivo_actual.name());

        if (archivo_actual.isDirectory()) {
            Serial.println("/");
            imprimirEstructuraDirectorio(archivo_actual, num_tabs + 1);
        } else {
            Serial.print("\t\t");
            // Usar size() para obtener el tamaño real
            Serial.println(archivo_actual.size(), DEC);
        }

        archivo_actual.close();
    }
}
#endif

/**
 * @brief Verifica si un archivo tiene extensión .gcode o .gco
 * @param nombre_archivo Nombre del archivo a verificar
 * @return true si el archivo tiene extensión válida de G-code
 * @return false si el archivo es inválido o no tiene extensión G-code
 * @note Soporta tanto formato largo (.gcode) como corto FAT32 (.gco)
 */
bool ControladorSD::esArchivoGcode(const char* nombre_archivo) {
    if (!nombre_archivo || strlen(nombre_archivo) == 0) {
        #if MODO_DESARROLLADOR
            Serial.println("[ERROR] esArchivoGcode: Nombre de archivo nulo o vacio");
        #endif
        return false;
    }
    
    size_t longitud = strlen(nombre_archivo);
    
    if (longitud < 4) {
        #if MODO_DESARROLLADOR
            Serial.print("[ERROR] esArchivoGcode: Nombre demasiado corto - '");
            Serial.print(nombre_archivo);
            Serial.println("'");
        #endif
        return false;
    }
    
    const char* extension = strrchr(nombre_archivo, '.');
    
    if (!extension) {
        #if MODO_DESARROLLADOR
            Serial.print("[ERROR] esArchivoGcode: Sin extension - '");
            Serial.print(nombre_archivo);
            Serial.println("'");
        #endif
        return false;
    }
    
    #if MODO_DESARROLLADOR
        Serial.print("[DEBUG] Extension detectada: '");
        Serial.print(extension);
        Serial.println("'");
    #endif
    
    bool es_valido = false;
    
    if (strlen(extension) >= 6) {
        es_valido = (strcasecmp(extension, ".gcode") == 0);
    }
    
    if (!es_valido && strlen(extension) >= 4) {
        es_valido = (strcasecmp(extension, ".gco") == 0);
    }
    
    #if MODO_DESARROLLADOR
        if (es_valido) {
            Serial.print("[SUCCESS] '");
            Serial.print(nombre_archivo);
            Serial.println("' ES archivo G-code valido");
        }
    #endif
    
    return es_valido;
}

/**
 * @brief Agrega un archivo a la lista interna de archivos G-code
 * @param nombre_archivo Nombre del archivo a agregar
 * @note El archivo se copia al buffer interno pre-asignado
 */
void ControladorSD::agregarArchivoALista(const char* nombre_archivo) {
    if (contador_archivos >= MAX_ARCHIVOS) {
        #if MODO_DESARROLLADOR
            Serial.println("[WARNING] Lista de archivos llena - No se puede agregar mas");
        #endif
        return;
    }
    
    if (!nombre_archivo || strlen(nombre_archivo) == 0) {
        return;
    }
    
    strncpy(buffer_archivos[contador_archivos], nombre_archivo, MAX_LONGITUD_NOMBRE - 1);
    buffer_archivos[contador_archivos][MAX_LONGITUD_NOMBRE - 1] = '\0';
    contador_archivos++;
}

/**
 * @brief Lee recursivamente un directorio y encuentra archivos G-code
 * @param dir Directorio a escanear
 * @param num_tabs Nivel de indentación para recursión
 * @return Lista de archivos G-code encontrados terminada en nullptr
 * @note La lista es estática y se sobreescribe en cada llamada
 */
const char** ControladorSD::leerDirectorio(File dir, int num_tabs) {
    if (num_tabs == 0) {
        limpiarListaArchivos();
        #if MODO_DESARROLLADOR
            Serial.println("[INFO] Iniciando escaneo de directorio");
        #endif
    }

    while (true) {
        File archivo_actual = dir.openNextFile();

        if (!archivo_actual) {
            #if MODO_DESARROLLADOR
                if (num_tabs == 0) {
                    Serial.println("[INFO] Escaneo de directorio completado");
                }
            #endif
            break;
        }

        const char* nombre_archivo = archivo_actual.name();
        
        #if MODO_DESARROLLADOR
            for (uint8_t i = 0; i < num_tabs; i++) {
                Serial.print('\t');
            }
            Serial.print(nombre_archivo);
        #endif

        if (archivo_actual.isDirectory()) {
            #if MODO_DESARROLLADOR
                Serial.println("/");
            #endif
            leerDirectorio(archivo_actual, num_tabs + 1);
        } else {
            #if MODO_DESARROLLADOR
                Serial.print("\t\t");
                Serial.println(archivo_actual.size(), DEC);
            #endif
            
            if (esArchivoGcode(nombre_archivo)) {
                agregarArchivoALista(nombre_archivo);
            }
        }

        archivo_actual.close();
    }

    if (num_tabs == 0) {
        #if MODO_DESARROLLADOR
            Serial.print("[INFO] Total de archivos .gcode encontrados: ");
            Serial.println(contador_archivos);
        #endif
        
        static const char* lista_punteros[MAX_ARCHIVOS + 1];
        for (size_t i = 0; i < contador_archivos; i++) {
            lista_punteros[i] = buffer_archivos[i];
        }
        lista_punteros[contador_archivos] = nullptr;
        
        return lista_punteros;
    }
    
    return nullptr;
}

/**
 * @brief Obtiene la lista de archivos G-code del directorio raíz
 * @return Lista de archivos G-code terminada en nullptr
 * @note Abre el directorio raíz si no está abierto
 */
const char** ControladorSD::obtenerListaArchivosGcode() {
    if (!directorio_raiz) {
        directorio_raiz = SD.open("/");
    }
    
    if (directorio_raiz) {
        return leerDirectorio(directorio_raiz, 0);
    }
    
    #if MODO_DESARROLLADOR
        Serial.println("[ERROR] No se pudo abrir directorio raiz");
    #endif
    
    return nullptr;
}

/**
 * @brief Obtiene la cantidad de archivos G-code encontrados
 * @return Número de archivos en la lista interna
 */
size_t ControladorSD::obtenerCantidadArchivos() const {
    return contador_archivos;
}

/**
 * @brief Obtiene el nombre de un archivo por índice
 * @param indice Posición del archivo en la lista (0-based)
 * @return Nombre del archivo o nullptr si índice inválido
 */
const char* ControladorSD::obtenerNombreArchivo(size_t indice) const {
    if (indice < contador_archivos) {
        return buffer_archivos[indice];
    }
    return nullptr;
}

/**
 * @brief Verifica si la lista de archivos está vacía
 * @return true si la lista está vacía, false en caso contrario
 */
bool ControladorSD::listaVacia() const {
    return contador_archivos == 0;
}

/**
 * @brief Limpia el buffer interno y resetea el contador de archivos
 */
void ControladorSD::limpiarListaArchivos() {
    contador_archivos = 0;
    for (size_t i = 0; i < MAX_ARCHIVOS; i++) {
        buffer_archivos[i][0] = '\0';
    }
}

/**
 * @brief Verifica si una línea de G-code es un comentario
 * @param linea Cadena de texto con la línea a verificar
 * @return true si la línea es un comentario (comienza con ';')
 * @return false si la línea contiene código ejecutable
 * @note Ignora espacios y tabs al inicio de la línea
 */
bool ControladorSD::esLineaComentario(const char* linea) {
    if (!linea || strlen(linea) == 0) {
        return true;
    }
    
    const char* ptr = linea;
    while (*ptr == ' ' || *ptr == '\t') {
        ptr++;
    }
    
    return (*ptr == ';');
}

/**
 * @brief Lee una línea optimizada desde un archivo usando peek()
 * @param archivo Archivo abierto para lectura
 * @param buffer Buffer para almacenar la línea leída
 * @param tamano_buffer Tamaño máximo del buffer
 * @return true si se leyó una línea correctamente
 * @return false si se alcanzó el fin del archivo o hubo error
 * @note Usa peek() para mejor eficiencia en el manejo de saltos de línea
 */
bool ControladorSD::leerLineaArchivoOptimizada(File& archivo, char* buffer, size_t tamano_buffer) {
    if (!archivo || !buffer || tamano_buffer == 0) {
        return false;
    }
    
    size_t indice = 0;
    
    while (indice < tamano_buffer - 1) {
        // Usar peek() para mirar sin consumir el caracter
        int caracter = archivo.peek();
        
        if (caracter == -1) break; // Fin del archivo
        if (caracter == '\n') {
            archivo.read(); // Consumir el salto de línea
            break;
        }
        if (caracter == '\r') {
            archivo.read(); // Consumir retorno de carro
            continue;
        }
        
        // Leer el caracter actual
        caracter = archivo.read();
        buffer[indice++] = (char)caracter;
    }
    
    buffer[indice] = '\0';
    
    #if MODO_DESARROLLADOR
        if (indice > 0) {
            unsigned long pos_actual = archivo.position();
            Serial.print("[DEBUG] Linea leida (");
            Serial.print(indice);
            Serial.print(" chars) en pos ");
            Serial.print(pos_actual);
            Serial.print(": ");
            Serial.println(buffer);
        }
    #endif
    
    return (indice > 0);
}

/**
 * @brief Abre un archivo G-code para lectura
 * @param nombre_archivo Nombre del archivo a abrir
 * @return true si el archivo se abrió correctamente
 * @return false si el archivo no existe, no es G-code o no pudo abrirse
 * @note Cierra cualquier archivo previamente abierto
 */
bool ControladorSD::abrirArchivoGcode(const char* nombre_archivo) {
    if (!nombre_archivo) {
        #if MODO_DESARROLLADOR
            Serial.println("[ERROR] abrirArchivoGcode: Nombre de archivo nulo");
        #endif
        return false;
    }
    
    if (archivo_actual) {
        #if MODO_DESARROLLADOR
            Serial.print("[INFO] Cerrando archivo anterior: ");
            Serial.println(archivo_actual.name());
        #endif
        archivo_actual.close();
    }
    
    // Usar exists() para verificar existencia
    if (!SD.exists(nombre_archivo)) {
        #if MODO_DESARROLLADOR
            Serial.print("[ERROR] abrirArchivoGcode: Archivo no existe - ");
            Serial.println(nombre_archivo);
        #endif
        return false;
    }
    
    if (!esArchivoGcode(nombre_archivo)) {
        #if MODO_DESARROLLADOR
            Serial.print("[ERROR] abrirArchivoGcode: No es archivo G-code - ");
            Serial.println(nombre_archivo);
        #endif
        return false;
    }
    
    archivo_actual = SD.open(nombre_archivo, FILE_READ);
    
    if (!archivo_actual) {
        #if MODO_DESARROLLADOR
            Serial.print("[ERROR] abrirArchivoGcode: No se pudo abrir - ");
            Serial.println(nombre_archivo);
        #endif
        return false;
    }
    
    #if MODO_DESARROLLADOR
        Serial.print("[SUCCESS] Archivo G-code abierto: ");
        Serial.println(archivo_actual.name());
        Serial.print("[INFO] Tamaño: ");
        Serial.print(archivo_actual.size());
        Serial.print(" bytes, Posicion: ");
        Serial.print(archivo_actual.position());
        Serial.println(" bytes");
    #endif
    
    return true;
}

/**
 * @brief Cierra el archivo G-code actualmente abierto
 * @note No hace nada si no hay archivo abierto
 */
void ControladorSD::cerrarArchivoGcode() {
    if (archivo_actual) {
        #if MODO_DESARROLLADOR
            Serial.println("[INFO] Cerrando archivo G-code actual");
        #endif
        archivo_actual.close();
    }
}

/**
 * @brief Verifica si hay un archivo G-code abierto
 * @return true si hay un archivo abierto, false en caso contrario
 */
bool ControladorSD::archivoAbierto() {
    return (bool)archivo_actual;
}

/**
 * @brief Lee la siguiente línea de G-code ejecutable del archivo
 * @return Línea de G-code o nullptr si se alcanzó el fin del archivo
 * @note Ignora líneas vacías y comentarios (que comienzan con ';')
 */
const char* ControladorSD::leerLineaGcode() {
    static char buffer_linea[TAMANO_BUFFER_LINEA];
    static unsigned long numero_linea = 0;
    
    if (!archivo_actual) {
        #if MODO_DESARROLLADOR
            Serial.println("[ERROR] leerLineaGcode: No hay archivo abierto");
        #endif
        numero_linea = 0;
        return nullptr;
    }
    
    if (!archivo_actual.available()) {
        #if MODO_DESARROLLADOR
            Serial.print("[INFO] Fin del archivo. Total lineas procesadas: ");
            Serial.println(numero_linea);
        #endif
        numero_linea = 0;
        return nullptr;
    }
    
    // Reiniciar contador si estamos al inicio del archivo
    if (archivo_actual.position() == 0) {
        numero_linea = 0;
    }
    
    while (leerLineaArchivoOptimizada(archivo_actual, buffer_linea, TAMANO_BUFFER_LINEA)) {
        if (strlen(buffer_linea) == 0) continue;
        if (esLineaComentario(buffer_linea)) continue;
        
        numero_linea++;
        
        #if MODO_DESARROLLADOR
            unsigned long posicion = archivo_actual.position();
            size_t tamano = archivo_actual.size();
            size_t porcentaje = tamano > 0 ? (posicion * 100) / tamano : 0;
            Serial.print("[SUCCESS] Linea ");
            Serial.print(numero_linea);
            Serial.print(" (");
            Serial.print(porcentaje);
            Serial.print("%): ");
            Serial.println(buffer_linea);
        #endif
        
        return buffer_linea;
    }
    
    return nullptr;
}

/**
 * @brief Busca un archivo por nombre en la lista de archivos G-code
 * @param nombre_archivo Nombre del archivo a buscar
 * @return true si el archivo existe en la lista, false en caso contrario
 * @note La búsqueda es case-insensitive
 */
bool ControladorSD::buscarArchivoPorNombre(const char* nombre_archivo) {
    if (!nombre_archivo || listaVacia()) {
        return false;
    }
    
    for (size_t i = 0; i < contador_archivos; i++) {
        if (strcasecmp(buffer_archivos[i], nombre_archivo) == 0) {
            #if MODO_DESARROLLADOR
                Serial.print("[SUCCESS] Archivo encontrado en indice: ");
                Serial.println(i);
            #endif
            return true;
        }
    }
    
    #if MODO_DESARROLLADOR
        Serial.println("[INFO] Archivo no encontrado en la lista");
    #endif
    
    return false;
}

// =============================================================================
// NUEVOS MÉTODOS OPTIMIZADOS
// =============================================================================

/**
 * @brief Reinicia la lectura del archivo al inicio
 * @return true si se reinició correctamente, false en caso de error
 * @note Utiliza el método seek() de la clase File
 */
bool ControladorSD::reiniciarLecturaArchivo() {
    if (!archivo_actual) {
        #if MODO_DESARROLLADOR
            Serial.println("[ERROR] reiniciarLecturaArchivo: No hay archivo abierto");
        #endif
        return false;
    }
    
    // Usar seek() para volver al inicio
    bool exito = archivo_actual.seek(0);
    
    #if MODO_DESARROLLADOR
        if (exito) {
            Serial.println("[SUCCESS] Lectura de archivo reiniciada al inicio");
        } else {
            Serial.println("[ERROR] No se pudo reiniciar la lectura del archivo");
        }
    #endif
    
    return exito;
}

/**
 * @brief Obtiene el tamaño del archivo actualmente abierto
 * @return Tamaño en bytes del archivo, 0 si no hay archivo abierto
 */
size_t ControladorSD::obtenerTamanoArchivoActual() {
    if (!archivo_actual) {
        return 0;
    }
    return archivo_actual.size();
}

/**
 * @brief Obtiene la posición actual de lectura en el archivo
 * @return Posición actual en bytes desde el inicio, 0 si no hay archivo abierto
 */
unsigned long ControladorSD::obtenerPosicionActual() {
    if (!archivo_actual) {
        return 0;
    }
    //Algo anda raro aca
    File miFile = SD.open("/");
    miFile.position();
    if(!miFile)

    return archivo_actual.position();
}

/**
 * @brief Obtiene el nombre del archivo actualmente abierto
 * @return Nombre del archivo o nullptr si no hay archivo abierto
 * @note Utiliza el método name() de la clase File
 */
const char* ControladorSD::obtenerNombreArchivoActual() {
    if (!archivo_actual) {
        return nullptr;
    }
    return archivo_actual.name();
}

/**
 * @brief Busca una posición específica en el archivo
 * @param posicion Posición a la que buscar (0-tamaño_archivo)
 * @return true si la búsqueda fue exitosa, false en caso contrario
 * @note Utiliza el método seek() de la clase File
 */
bool ControladorSD::buscarEnArchivo(unsigned long posicion) {
    if (!archivo_actual) {
        #if MODO_DESARROLLADOR
            Serial.println("[ERROR] buscarEnArchivo: No hay archivo abierto");
        #endif
        return false;
    }
    
    // Usar seek() para saltar a posición específica
    bool exito = archivo_actual.seek(posicion);
    
    #if MODO_DESARROLLADOR
        Serial.print("[DEBUG] Buscando posicion ");
        Serial.print(posicion);
        Serial.print(": ");
        Serial.println(exito ? "EXITO" : "FALLIDO");
    #endif
    
    return exito;
}

/**
 * @brief Verifica si se alcanzó el fin del archivo
 * @return true si estamos al final del archivo, false en caso contrario
 * @note Utiliza el método available() de la clase File
 */
bool ControladorSD::esFinDeArchivo() {
    if (!archivo_actual) {
        return true;
    }
    return !archivo_actual.available();
}

#if MODO_DESARROLLADOR
/**
 * @brief Modo desarrollador: Permite ingresar nombre de archivo por serial
 * @note Método interactivo para pruebas, solo disponible en modo desarrollador
 */
void ControladorSD::modoDesarrolladorLeerArchivo() {
    Serial.println("\n=== MODO DESARROLLADOR: LECTURA G-CODE ===");
    Serial.println("Ingrese el nombre del archivo G-code:");
    
    while (!Serial.available()) {
        delay(100);
    }
    
    String entrada = Serial.readStringUntil('\n');
    entrada.trim();
    
    if (entrada.length() == 0) {
        Serial.println("[ERROR] Entrada vacia");
        return;
    }
    
    Serial.print("[INFO] Archivo seleccionado: ");
    Serial.println(entrada);
    
    if (!buscarArchivoPorNombre(entrada.c_str())) {
        Serial.println("[ERROR] Archivo no encontrado en la lista");
        Serial.println("[INFO] Archivos disponibles:");
        for (size_t i = 0; i < contador_archivos; i++) {
            Serial.print("  - ");
            Serial.println(buffer_archivos[i]);
        }
        return;
    }
    
    if (!abrirArchivoGcode(entrada.c_str())) {
        Serial.println("[ERROR] No se pudo abrir el archivo");
        return;
    }
    
    Serial.println("\n=== CONTENIDO EJECUTABLE DEL ARCHIVO ===");
    const char* linea;
    int contador_lineas = 0;
    
    while ((linea = leerLineaGcode()) != nullptr) {
        Serial.print("Linea ");
        Serial.print(++contador_lineas);
        Serial.print(" (");
        Serial.print(obtenerPosicionActual());
        Serial.print("/");
        Serial.print(obtenerTamanoArchivoActual());
        Serial.print(" bytes): ");
        Serial.println(linea);
        delay(50); // Reducido para mayor fluidez
    }
    
    Serial.println("=== FIN DEL ARCHIVO ===");
    cerrarArchivoGcode();
}
#endif

/**
 * @brief Destructor de la clase ControladorSD
 * @details Libera todos los recursos: cierra archivos y finaliza la SD
 */
ControladorSD::~ControladorSD(){
    if (directorio_raiz) {
        directorio_raiz.close();
    }
    if (archivo_actual) {
        archivo_actual.close();
    }
    SD.end();
    
    #if MODO_DESARROLLADOR
        Serial.println("[INFO] ControladorSD destruido - recursos liberados");
    #endif
}