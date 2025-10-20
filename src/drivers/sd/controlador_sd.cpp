#include "controlador_sd.h"
#include "constantes.h"

bool ControladorSD::iniciarSD(){
    if(!SD.begin()) 
    {
        #if MODO_DESARROLLADOR
            Serial.println("Error al iniciar la tarjeta SD");
        #endif
        return false;
    }
    #if MODO_DESARROLLADOR
            Serial.println("La tarjeta SD se inicializo con exito");
    #endif

    directorio_raiz = SD.open("/");
    return true; // Agregado: retornar éxito
}
const char** ControladorSD::leerDirectorio(File dir, int numTabs){
    // Nueva funcionalidad: lista para archivos .gcode
    static const int MAX_ARCHIVOS = 50; // Ajusta según tus necesidades
    static char* listaArchivos[MAX_ARCHIVOS];
    static int contadorArchivos = 0;
    
    // Inicializar lista si es la primera llamada
    if (numTabs == 0) {
        contadorArchivos = 0;
        for (int i = 0; i < MAX_ARCHIVOS; i++) {
            listaArchivos[i] = nullptr;
        }
    }

    while (true) {
        File archivo_actual = dir.openNextFile();

        if (!archivo_actual) {
            #if MODO_DESARROLLADOR
                Serial.println("No se encontraron mas archivos en el directorio");
            #endif
            break;
        }

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
            #endif
            leerDirectorio(archivo_actual, numTabs + 1);
        } else {
            #if MODO_DESARROLLADOR
                // files have sizes, directories do not
                Serial.print("\t\t");
                Serial.println(archivo_actual.size(), DEC);
            #endif
            
            // Nueva funcionalidad: verificar extensión .gcode
            const char* nombreArchivo = archivo_actual.name();
            int longitud = strlen(nombreArchivo);
            
            if (longitud > 6 && 
                strcmp(nombreArchivo + longitud - 6, ".gcode") == 0 && 
                contadorArchivos < MAX_ARCHIVOS) {
                
                // Reservar memoria y copiar el nombre del archivo
                listaArchivos[contadorArchivos] = new char[longitud + 1];
                strcpy(listaArchivos[contadorArchivos], nombreArchivo);
                contadorArchivos++;
                
                #if MODO_DESARROLLADOR
                    Serial.print(">>> ARCHIVO GCODE ENCONTRADO: ");
                    Serial.println(nombreArchivo);
                #endif
            }
        }

        archivo_actual.close();
    }

    // Retornar la lista solo cuando se complete el escaneo recursivo
    if (numTabs == 0) {
        // Agregar marcador de fin de lista
        if (contadorArchivos < MAX_ARCHIVOS) {
            listaArchivos[contadorArchivos] = nullptr;
        }
        
        #if MODO_DESARROLLADOR
            Serial.print("Total de archivos .gcode encontrados: ");
            Serial.println(contadorArchivos);
        #endif
        
        return (const char**)listaArchivos;
    }
    
    return nullptr;
}

ControladorSD::~ControladorSD(){
    SD.end();
}