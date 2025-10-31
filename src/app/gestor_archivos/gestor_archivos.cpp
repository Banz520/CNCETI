#include "gestor_archivos.h"
#include <string.h>


GestorArchivos::~GestorArchivos(){
    hostSD.end();
}
/**
 * @brief Este metodo recibe el puntero al nombre de un archivo y verifica si dentro de la cadena
 * @param nombre_archivo El puntero al nombre de un archivo 
 * @return Regresa true si encontro al final de la cadena la extension GCO y false si el nombre del archivo no es valido o no encontro la cadena
 * 
 * */            
bool GestorArchivos::EsArchivoGcode(const char* nombre_archivo) {
    if (!nombre_archivo || !*nombre_archivo) return false;
    const char* fin_cadena = nombre_archivo;
    while (*fin_cadena) fin_cadena++;
    if (fin_cadena - nombre_archivo >= 5) { 
      fin_cadena--;
      if(*fin_cadena == 'O' && *(fin_cadena-1) == 'C' && *(fin_cadena-2) == 'G') return true;
    }
    
    return false;
}

/**
 * @brief Inicializa la tarjeta SD y abre el directorio raíz
 * @return true si la inicialización fue exitosa, false en caso contrario
 * @note Este método debe ser llamado antes de cualquier operación con la SD
 */
bool GestorArchivos::iniciarPuertoSD(){
    if(!hostSD.begin()) {
        #if MODO_DESARROLLADOR
            Serial.println(F("Error al iniciar la tarjeta SD"));
        #endif
        return false;
    }
    
    #if MODO_DESARROLLADOR
        Serial.println(F("La tarjeta SD se inicializo con exito"));
    #endif

    directorio_raiz_sd = hostSD.open("/");
    return true;
}

/**
 * @brief Inicia la comunicacion con el driver del puerto usb
 * @return Regresa false si no se recibe respuesta del chip al consultar su id (getChipVer = 0), regresa true en caso contrario (Se asume que la comunicacion es correcta)
 * 
 */
bool GestorArchivos::iniciarPuertoUSB(){
    hostUsb.init();

    if(hostUsb.getChipVer() == 0){
        #if MODO_DESARROLLADOR
        Serial.println(F("No se encontro conexion con el driver del puerto USB"));
        #endif
        return false;
    }
    return true;
}

const char** GestorArchivos::obtenerListaArchivosSD(const char* directorio, uint8_t* cantidad) {
    
    
    File dir = hostSD.open(directorio);
    if (!dir || !dir.isDirectory()) {
        if (cantidad) *cantidad = 0;
        return nullptr;
    }

    File entrada;
    char* posicion_actual = buffer_nombres_archivos;
    contador_archivos = 0;

    while (contador_archivos < MAX_ARCHIVOS && (entrada = dir.openNextFile())) {
        // Verificar que hay espacio en el buffer
        if (posicion_actual + MAX_LONGITUD_NOMBRE <= 
            buffer_nombres_archivos + (MAX_ARCHIVOS * MAX_LONGITUD_NOMBRE)) {
         
            if (EsArchivoGcode(entrada.name())) {
                // PRIMERO: Copiar el nombre al buffer
                strncpy(posicion_actual, entrada.name(), MAX_LONGITUD_NOMBRE - 1);
                posicion_actual[MAX_LONGITUD_NOMBRE - 1] = '\0'; // Asegurar terminación
                
                // LUEGO: Guardar el puntero y avanzar
                lista_archivos[contador_archivos++] = posicion_actual;
                posicion_actual += MAX_LONGITUD_NOMBRE;
            }
        }
        entrada.close();
    }
    
    dir.close();
    
    if (cantidad) *cantidad = contador_archivos;
    return contador_archivos > 0 ? lista_archivos : nullptr;
}
const char** GestorArchivos::obtenerListaArchivosUSB(const char* directorio, uint8_t* cantidad) {
    
    char* posicion_actual = buffer_nombres_archivos;
    contador_archivos = 0;

    Serial.print("Leyendo directorio USB: ");
    Serial.println(directorio);

    // Listar archivos del directorio actual
    while (contador_archivos < MAX_ARCHIVOS && hostUsb.listDir()) {
        // Verificar que hay espacio en el buffer
        if (posicion_actual + MAX_LONGITUD_NOMBRE <= 
            buffer_nombres_archivos + (MAX_ARCHIVOS * MAX_LONGITUD_NOMBRE)) {
            
            const char* nombre_archivo = hostUsb.getFileName();
            
            // Solo procesar archivos (no directorios) y que sean GCODE
            if (hostUsb.getFileAttrb() != CH376_ATTR_DIRECTORY && 
                EsArchivoGcode(nombre_archivo)) {
                
                // Copiar el nombre al buffer
                strncpy(posicion_actual, nombre_archivo, MAX_LONGITUD_NOMBRE - 1);
                posicion_actual[MAX_LONGITUD_NOMBRE - 1] = '\0'; // Asegurar terminación
                
                // Guardar el puntero en la lista
                lista_archivos[contador_archivos] = posicion_actual;
                contador_archivos++;
                
                // Mover al siguiente espacio disponible
                posicion_actual += MAX_LONGITUD_NOMBRE;
                
                Serial.print("Agregado USB: ");
                Serial.println(nombre_archivo);
            } else {
                if (hostUsb.getFileAttrb() == CH376_ATTR_DIRECTORY) {
                    Serial.print("Omitido (directorio): ");
                } else {
                    Serial.print("Omitido (no GCODE): ");
                }
                Serial.println(nombre_archivo);
            }
        } else {
            Serial.println("Advertencia: Buffer lleno en USB");
            break;
        }
    }
    
    Serial.print("Total archivos GCODE encontrados en USB: ");
    Serial.println(contador_archivos);
    
    if (cantidad) *cantidad = contador_archivos;
    return contador_archivos > 0 ? lista_archivos : nullptr;
}

#if MODO_DESARROLADOR

#endif