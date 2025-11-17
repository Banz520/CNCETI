#include "consola.h"
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
Consola::Consola(Ch376msc &miHostUsb) 
    : miDisplay(0,0,0,0,0,0),
      //miDisplay(), el constructor recibe valores cualquiera para la compatibilidad con la clase del constructor del UTFT
      miLista(miDisplay, COLOR_NEGRO, COLOR_BLANCO, COLOR_GRIS_CLARO, COLOR_AZUL, 12, 218, 20),
      miGestorWidgets(miDisplay),
      miMenuInicio(miDisplay, miLista,miGestorWidgets),
      miGestorArchivos(miHostUsb),
      miPantallaEjecucion(miDisplay,miGestorWidgets),
      contexto_actual(MENU_INICIO),
      primer_actualizacion(true)
{
}

void Consola::iniciar() {
    uint16_t display_id = miDisplay.readID();
    if (display_id == 0xD3D3) display_id = 0x9481;
    //miDisplay.begin(display_id);
    miDisplay.InitLCD();
    //Creo que set contrast y set brightness no hacen nada
    //miDisplay.setContrast(64);
    //miDisplay.setBrightness(16);
    //miDisplay.setRotation(DISPLAY_ORIENTACION);
    miDisplay.fillScreen(COLOR_BLANCO);
    contexto_actual = MENU_INICIO;
    contexto_anterior = MENU_INICIO;
    primer_actualizacion = true;
    
    // Forzar dibujado inicial
    mostrarInterfazContexto();
}

void Consola::actualizar(char tecla, const float &origen_x, const float &posicion_x, const float &destino_x,
                        const float &origen_y, const float &posicion_y, const float &destino_y,
                        const float &origen_z, const float &posicion_z, const float &destino_z, 
                        const char* comando_gcode) {

    if (tecla != '\0' ) {  // Carácter nulo indica que no hay tecla
        procesarTecla(tecla);
    }
    //if(tecla)procesarTecla(tecla);
    
   
    
    // Verificar si hubo cambio de contexto o es la primera actualización
    bool hubo_cambio_contexto = (contexto_actual != contexto_anterior) || primer_actualizacion;
    
    if (hubo_cambio_contexto) {
        // Limpiar interfaz anterior si es necesario
        if (!primer_actualizacion) {
            limpiarPantallaContextoAnterior();
        }
        
        // Mostrar nueva interfaz
        mostrarInterfazContexto();
        
        // Actualizar estado
        contexto_anterior = contexto_actual;
        primer_actualizacion = false;

        if(contexto_anterior == EJECUCION){
            //miControladorSD.cerrarArchivoGcode();
        }


        switch (contexto_actual) {
            case MENU_INICIO:
                // Actualizaciones específicas del menú inicio si las hay
                break;
                
            case MENU_ARCHIVOS_SD:
                // Actualizaciones específicas de archivos SD
                break;
                
            case MENU_ARCHIVOS_USB:
                // Actualizaciones específicas de archivos USB
                break;
                
            case EJECUCION:
                // Solo actualizar datos, no redibujar interfaz completa
                if(!miGestorArchivos.abrirArchivoPorNombre("CAKE~1.GCO"))
                //if(!miControladorSD.abrirArchivoGcode("CAKE~1.GCO")){
                    miDisplay.fillScreen(COLOR_GRIS_OSCURO);
                    cambiarContexto(MENU_INICIO);
                }
                break;
                
            case CONFIGURACION:
                // Actualizaciones específicas de configuración
                break;
        }
        #if MODO_DESARROLLADOR
        Serial.print(F("Cambio de contexto a: "));
        Serial.println(contexto_actual);
        #endif
    }
    
    // Actualizar datos dinámicos según contexto (sin redibujar interfaz completa)
    switch (contexto_actual) {
        case MENU_INICIO:
            // Actualizaciones específicas del menú inicio si las hay
            break;
            
        case MENU_ARCHIVOS_SD:
            // Actualizaciones específicas de archivos SD
            break;
            
        case MENU_ARCHIVOS_USB:
            // Actualizaciones específicas de archivos USB
            
            break;
            
        case EJECUCION:
            // Solo actualizar datos, no redibujar interfaz completa
            miPantallaEjecucion.actualizarDatos(origen_x, posicion_x, destino_x,
                                               origen_y, posicion_y, destino_y,
                                               origen_z, posicion_z, destino_z,
                                               comando_gcode);
            break;
            
        case CONFIGURACION:
            // Actualizaciones específicas de configuración
            miDisplay.fillScreen(COLOR_AZUL);
            break;
        default:
                miDisplay.fillScreen(COLOR_ROJO);
        break;
    }
}

void Consola::mostrarInterfazContexto() {
    switch (contexto_actual) {
        case MENU_INICIO:
            miMenuInicio.mostrar();
            #if MODO_DESARROLLADOR
            Serial.println(F("Mostrando MENU INICIO"));
            #endif
            break;
            
        case MENU_ARCHIVOS_SD:
            miDisplay.fillScreen(COLOR_BLANCO);
            // Aquí llamarías a tu método para mostrar interfaz de archivos SD
            // miGestorArchivos.mostrarInterfazSD();
            #if MODO_DESARROLLADOR
            Serial.println(F("Mostrando ARCHIVOS SD"));
            #endif
            break;
            
        case MENU_ARCHIVOS_USB:
            miDisplay.fillScreen(COLOR_VERDE);
            
            // miGestorArchivos.mostrarInterfazUSB();
            #if MODO_DESARROLLADOR
            Serial.println("Mostrando ARCHIVOS USB");
            #endif
            break;
            
        case EJECUCION:
            miPantallaEjecucion.mostrar();
            #if MODO_DESARROLLADOR
            Serial.println(F("Mostrando PANTALLA EJECUCION"));
            #endif
            break;
            
        case CONFIGURACION:
            miDisplay.fillScreen(COLOR_BLANCO);
            // Aquí llamarías a tu método para mostrar interfaz de configuración
            #if MODO_DESARROLLADOR
            Serial.println("Mostrando CONFIGURACION");
            #endif
            break;
    }
}

void Consola::limpiarPantallaContextoAnterior() {
    // Solo limpiar si es necesario (dependiendo de tu implementación)
    // Por ejemplo, si usas diferentes colores de fondo por contexto
    miDisplay.fillScreen(COLOR_BLANCO);
}

void Consola::procesarTecla(char tecla) {
    #if MODO_DESARROLLADOR
    Serial.print(F("[Consola::procesarTecla] Tecla: "));
    Serial.println(tecla);
    #endif
    
    // Teclas globales (funcionan en cualquier contexto)
    switch (tecla) {
        case '*':  // Tecla de menú/atrás
            if (contexto_actual != MENU_INICIO) {
                cambiarContexto(MENU_INICIO);
            }
            return;
        case '#':  // Tecla de función especial
            Serial.println("Funcion especial activada");
            return;
    }
    
    // Teclas específicas por contexto
    switch (contexto_actual) {
        case MENU_INICIO:
            switch (tecla) {
                case '1':
                    cambiarContexto(MENU_ARCHIVOS_SD);
                    break;
                case '2':
                    cambiarContexto(MENU_ARCHIVOS_USB);
                    break;
                case '3':
                    cambiarContexto(EJECUCION);
                    Serial.println(F("[Consola::procesarTecla] CAMBIANDO DE CONTEXTO 'MENUINICIO' A 'EJECUCION' "));
                    break;
                case '4':
                    cambiarContexto(CONFIGURACION);
                    break;
            }
            break;
            
        case MENU_ARCHIVOS_SD:
            switch (tecla) {
                case '1': case '2': case '3': case '4': case '5': 
                case '6': case '7': case '8': case '9':
                    //miGestorArchivos.seleccionarArchivoSD(tecla - '1');
                    break;
                case '0':
                    cambiarContexto(MENU_INICIO);
                    break;
            }
            break;
            
        case MENU_ARCHIVOS_USB:
            switch (tecla) {
                case '1': case '2': case '3': case '4': case '5': 
                case '6': case '7': case '8': case '9':
                    //miGestorArchivos.seleccionarArchivoUSB(tecla - '1');
                    break;
                case '0':
                    cambiarContexto(MENU_INICIO);
                    break;
            }
            break;
            
        case EJECUCION:
            switch (tecla) {
                case '1':  // Iniciar/pausar
                    break;
                case '2':  // Detener
                    break;
                case '0':
                    cambiarContexto(MENU_INICIO);
                    break;
            }
            break;
            
        case CONFIGURACION:
            switch (tecla) {
                case '1': case '2': case '3':
                    break;
                case '0':
                    cambiarContexto(MENU_INICIO);
                    break;
            }
            break;
        default: 
        break;
    }
}

void Consola::cambiarContexto(CONTEXTO_APP nuevo_contexto) {
    // Solo cambiar si es diferente al contexto actual
    if (contexto_actual != nuevo_contexto) {
        contexto_anterior = contexto_actual;
        contexto_actual = nuevo_contexto;
        
        // El dibujado se hará en la siguiente llamada a actualizar()
        // mediante la verificación de cambio de contexto
    }
}


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
    #if MODO_DESARROLLADOR
    miGestorArchivos.obtenerListaArchivosUSBDebug("/",&cantidad_archivos);
    #endif
    cantidad_archivos = 0;
    const char** archivos_gcode = miGestorArchivos.obtenerListaArchivosUSB("/",&cantidad_archivos);//miControladorSD.obtenerListaArchivosGcode();
    
    if(cantidad_archivos <= 0){
        Serial.println(F("[INFO] No hay archivos G-code"));
        return;
    }

    Serial.print(F("[OK] Archivos G-code: "));
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

    Serial.print(F("[OK] Archivos G-code: "));
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
#if MODO_DESARROLLADOR
#endif