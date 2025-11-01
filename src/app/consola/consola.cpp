#include "consola.h"

Consola::Consola(Ch376msc &miHostUsb) 
    : miDisplay(),
      miLista(miDisplay, COLOR_NEGRO, COLOR_BLANCO, COLOR_GRIS_CLARO, COLOR_AZUL, 12, 218, 20),
      miGestorWidgets(miDisplay),
      miMenuInicio(miDisplay, miLista,miGestorWidgets),
      miGestorArchivos(miHostUsb)
{
}

void Consola::iniciar() {
    miDisplay.begin(miDisplay.readID());
    miDisplay.setRotation(1);
    miDisplay.fillScreen(COLOR_BLANCO);
    miMenuInicio.mostrar();

    /*
    switch (estado_usb)
    {
    case 0:
        miBarraAlerta.set_mensaje("ERROR: No hay conexion al puerto USB");
        miBarraAlerta.mostrar();
        
        break;

    case 1:
        miBarraAlerta.set_mensaje("ERROR: No se pudo iniciar el puerto USB");
        miBarraAlerta.mostrar();
    
    case 2: 
        miBarraAlerta.set_mensaje("El puerto USB se inicio con exito");
        miBarraAlerta.mostrar();
        break;
    
    default:
        miBarraAlerta.set_mensaje("ERROR: Ocurrio un error desconocido");
        miBarraAlerta.mostrar();

        break;
    }
        */
}


void Consola::bucleDeEjecucion(){
    //miControladorUSB.tarea();
    
    
}

//Metodos de pruebas de desarrollo

#if MODO_DESARROLLADOR

void Consola::pruebaLecturaUSB(){
    if(!miGestorArchivos.iniciarPuertoUSB()){
        Serial.println(F("[Consola::pruebaLecturaUSB]ERROR! - Falló inicialización USB"));
        return;
    }
    else{
        Serial.println(F("[Consola::pruebaLecturaUSB]EXITO! - Se inicio el puerto USB"));
    }

    // Obtener archivos G-code
    uint8_t cantidad_archivos;
    miGestorArchivos.obtenerListaArchivosUSBDebug("/",&cantidad_archivos);
    cantidad_archivos = 0;
    const char** archivos_gcode = miGestorArchivos.obtenerListaArchivosUSB("/",&cantidad_archivos);//miControladorSD.obtenerListaArchivosGcode();
    
    if(cantidad_archivos <= 0){
        Serial.println("[INFO] No hay archivos G-code");
        return;
    }

    Serial.print("[OK] Archivos G-code: ");
    //Serial.println(miControladorSD.obtenerCantidadArchivos());

    // Mostrar lista
    miLista.inicializar(archivos_gcode, cantidad_archivos);
    miLista.mostrar_lista();
}

void Consola::pruebaLecturaSD(){
    
    /*miControladorSD.iniciarSD();

    miControladorSD.imprimirEstructuraDirectorio(SD.open("/"),0);
    const char** archivos_gcode = miControladorSD.obtenerListaArchivosGcode();
    if(!miControladorSD.listaVacia()){
        miLista.inicializar(archivos_gcode,miControladorSD.obtenerCantidadArchivos());
        miLista.mostrar_lista();
    }
    */

     Serial.println("\n=== PRUEBA RÁPIDA LECTURA SD ===");
    
    // Inicializar SD
    if(!miGestorArchivos.iniciarPuertoSD()){
        Serial.println("[ERROR] Falló inicialización SD");
        return;
    }

    // Mostrar estructura
    //miControladorSD.imprimirEstructuraDirectorio(SD.open("/"),0);

    // Obtener archivos G-code
    uint8_t cantidad_archivos;
    const char** archivos_gcode = miGestorArchivos.obtenerListaArchivosSD("/",&cantidad_archivos);//miControladorSD.obtenerListaArchivosGcode();
    
    if(cantidad_archivos <= 0){
        Serial.println("[INFO] No hay archivos G-code");
        return;
    }

    Serial.print("[OK] Archivos G-code: ");
    //Serial.println(miControladorSD.obtenerCantidadArchivos());

    // Mostrar lista
    miLista.inicializar(archivos_gcode, cantidad_archivos);
    miLista.mostrar_lista();

    /**
     * 
   
    // Probar lectura del primer archivo
    const char* primer_archivo = miControladorSD.obtenerNombreArchivo(0);
    if(primer_archivo && miControladorSD.abrirArchivoGcode(primer_archivo)){
        Serial.print("\n[TEST] Leyendo: ");
        Serial.println(primer_archivo);
        
        int lineas = 0;
        const char* linea;
        while((linea = miControladorSD.leerLineaGcode()) != nullptr && lineas < 30){
            Serial.print("L");
            Serial.print(++lineas);
            Serial.print(": ");
            Serial.println(linea);
        }
        
        Serial.print("[OK] Leídas ");
        Serial.print(lineas);
        Serial.println(" líneas (primeras 5 o menos)");
        
        miControladorSD.cerrarArchivoGcode();
        
    }
    
    Serial.println("=== FIN PRUEBA ===");
      */
}

void Consola::pruebaAbrirGcodeSD(){
    miControladorSD.abrirArchivoGcode("FRUITC~1.GCO");
    return;

}
void Consola::pruebaLecturaGcode(){
    if(!miControladorSD.iniciarSD()){
        Serial.println(F("[Consola::pruebaLecturaGcode]ERROR! - No se logro iniciar la SD"));
        return;
    }
    miControladorSD.abrirArchivoGcode("FRUITC~1.GCO");
    miControladorSD.leerLineaGcode();
}
#endif