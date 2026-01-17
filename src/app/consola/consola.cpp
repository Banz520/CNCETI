#include "consola.h"

/**
 * @file consola.cpp
 * @brief Implementación de la interfaz de usuario con browser de archivos integrado
 */

Consola::Consola(GestorArchivos &miGestorArchivos_ref) 
    : miDisplay(0,0,0,0,0,0),
      miLista(miDisplay, COLOR_NEGRO, COLOR_BLANCO, COLOR_GRIS_CLARO, COLOR_AZUL, 12, 218, 20),
      miGestorWidgets(miDisplay),
      miMenuInicio(miDisplay, miLista, miGestorWidgets),
      miGestorArchivos(miGestorArchivos_ref),
      miPantallaEjecucion(miDisplay, miGestorWidgets),
      contexto_actual(MENU_INICIO),
      contexto_anterior(MENU_INICIO),
      primer_actualizacion(true),
      archivos_cargados(false),
      array_nombres_archivos(nullptr),
      cantidad_archivos_actual(0)
{
}

void Consola::iniciar() {
    uint16_t display_id = miDisplay.readID();
    if (display_id == 0xD3D3) display_id = 0x9481;
    
    miDisplay.InitLCD();
    miDisplay.fillScreen(COLOR_BLANCO);
    
    contexto_actual = MENU_INICIO;
    contexto_anterior = MENU_INICIO;
    primer_actualizacion = true;
    
    // Inicializar variables de browser
    archivos_cargados = false;
    array_nombres_archivos = nullptr;
    cantidad_archivos_actual = 0;
    
    mostrarInterfazContexto();
    
    #if MODO_DESARROLLADOR
        Serial.println(F("[Consola::iniciar] Consola inicializada"));
    #endif
}

void Consola::actualizar(char tecla, 
                        const float &origen_x, const float &posicion_x, const float &destino_x,
                        const float &origen_y, const float &posicion_y, const float &destino_y,
                        const float &origen_z, const float &posicion_z, const float &destino_z, 
                        const char* comando_gcode) {

    // Procesar entrada de teclado
    if (tecla != '\0') {
        procesarTecla(tecla);
    }
    miDisplay.print(String(tecla),300,200);
    // Verificar cambio de contexto
    bool hubo_cambio_contexto = (contexto_actual != contexto_anterior) || primer_actualizacion;
    
    if (hubo_cambio_contexto) {
        // Limpiar pantalla si no es primera actualización
        if (!primer_actualizacion) {
            limpiarPantallaContextoAnterior();
        }
        
        // Liberar recursos del contexto anterior
        if (contexto_anterior == MENU_ARCHIVOS_SD || contexto_anterior == MENU_ARCHIVOS_USB) {
            liberarListaArchivos();
        }
        
        if (contexto_anterior == EJECUCION) {
            miGestorArchivos.cerrarArchivo();
        }
        
        // Mostrar nueva interfaz
        mostrarInterfazContexto();
        
        // Actualizar estado
        contexto_anterior = contexto_actual;
        primer_actualizacion = false;
        
        #if MODO_DESARROLLADOR
            Serial.print(F("[Consola::actualizar] Cambio a contexto: "));
            Serial.println(contexto_actual);
        #endif
    }
    
    // Actualizar datos dinámicos según contexto
    switch (contexto_actual) {
        case MENU_INICIO:
            // Sin actualizaciones dinámicas
            break;
            
        case MENU_ARCHIVOS_SD:
        case MENU_ARCHIVOS_USB:
            // La navegación se maneja en procesarTecla
            break;
            
        case EJECUCION:
            miPantallaEjecucion.actualizarDatos(
                origen_x, posicion_x, destino_x,
                origen_y, posicion_y, destino_y,
                origen_z, posicion_z, destino_z,
                comando_gcode
            );
            break;
            
        case CONFIGURACION:
            // Sin actualizaciones dinámicas
            break;
    }
}

// ========================================
// MÉTODOS DE GESTIÓN DE CONTEXTO
// ========================================

void Consola::mostrarInterfazContexto() {
    switch (contexto_actual) {
        case MENU_INICIO:
            miMenuInicio.mostrar();
            #if MODO_DESARROLLADOR
                Serial.println(F("[Consola] Mostrando MENU_INICIO"));
            #endif
            break;
            
        case MENU_ARCHIVOS_SD:
            if (cargarListaArchivos()) {
                mostrarBrowserArchivos();
                #if MODO_DESARROLLADOR
                    Serial.println(F("[Consola] Mostrando MENU_ARCHIVOS_SD"));
                #endif
            } else {
                #if MODO_DESARROLLADOR
                    Serial.println(F("[Consola] ERROR: No hay archivos en SD"));
                #endif
                // Volver al menú inicio si no hay archivos
                cambiarContexto(MENU_INICIO);
            }
            break;
            
        case MENU_ARCHIVOS_USB:
            if (cargarListaArchivos()) {
                mostrarBrowserArchivos();
                #if MODO_DESARROLLADOR
                    Serial.println(F("[Consola] Mostrando MENU_ARCHIVOS_USB"));
                #endif
            } else {
                #if MODO_DESARROLLADOR
                    Serial.println(F("[Consola] ERROR: No hay archivos en USB"));
                #endif
                cambiarContexto(MENU_INICIO);
            }
            break;
            
        case EJECUCION:
            miPantallaEjecucion.mostrar();
            #if MODO_DESARROLLADOR
                Serial.println(F("[Consola] Mostrando EJECUCION"));
            #endif
            break;
            
        case CONFIGURACION:
            miDisplay.fillScreen(COLOR_BLANCO);
            // TODO: Implementar pantalla de configuración
            #if MODO_DESARROLLADOR
                Serial.println(F("[Consola] Mostrando CONFIGURACION"));
            #endif
            break;
    }
}

void Consola::limpiarPantallaContextoAnterior() {
    miDisplay.fillScreen(COLOR_BLANCO);
}

void Consola::cambiarContexto(CONTEXTO_APP nuevo_contexto) {
    if (contexto_actual != nuevo_contexto) {
        contexto_anterior = contexto_actual;
        contexto_actual = nuevo_contexto;
    }
}

// ========================================
// MÉTODOS DE BROWSER DE ARCHIVOS
// ========================================

bool Consola::cargarListaArchivos() {
    // Liberar lista anterior si existe
    liberarListaArchivos();
    
    // Escanear directorio según contexto
    const char* ruta = "/";
    
    #if MODO_DESARROLLADOR
        Serial.print(F("[Consola::cargarListaArchivos] Escaneando: "));
        Serial.println(ruta);
    #endif
    
    if (!miGestorArchivos.escanearDirectorio(ruta)) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[Consola::cargarListaArchivos] ERROR: No se pudo escanear"));
        #endif
        return false;
    }
    
    cantidad_archivos_actual = miGestorArchivos.obtenerCantidadArchivos();
    
    if (cantidad_archivos_actual == 0) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[Consola::cargarListaArchivos] No hay archivos G-code"));
        #endif
        return false;
    }
    
    // Crear array dinámico para punteros a nombres
    array_nombres_archivos = new const char*[cantidad_archivos_actual];
    
    if (!array_nombres_archivos) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[Consola::cargarListaArchivos] ERROR: Memoria insuficiente"));
        #endif
        return false;
    }
    
    // Copiar punteros a nombres de archivo
    for (size_t i = 0; i < cantidad_archivos_actual; ++i) {
        array_nombres_archivos[i] = miGestorArchivos.obtenerNombreArchivo(i);
    }
    
    archivos_cargados = true;
    
    #if MODO_DESARROLLADOR
        Serial.print(F("[Consola::cargarListaArchivos] Cargados "));
        Serial.print(cantidad_archivos_actual);
        Serial.println(F(" archivos"));
    #endif
    
    return true;
}

void Consola::liberarListaArchivos() {
    if (array_nombres_archivos != nullptr) {
        delete[] array_nombres_archivos;
        array_nombres_archivos = nullptr;
    }
    
    archivos_cargados = false;
    cantidad_archivos_actual = 0;
    
    #if MODO_DESARROLLADOR
        Serial.println(F("[Consola::liberarListaArchivos] Memoria liberada"));
    #endif
}

void Consola::mostrarBrowserArchivos() {
    if (!archivos_cargados || cantidad_archivos_actual == 0) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[Consola::mostrarBrowserArchivos] ERROR: No hay archivos"));
        #endif
        return;
    }
    
    // Limpiar pantalla
    miDisplay.fillScreen(COLOR_BLANCO);
    
    // Inicializar lista con los archivos
    miLista.inicializar(array_nombres_archivos, cantidad_archivos_actual);
    
    // Mostrar lista
    miLista.mostrar_lista();
    
    #if MODO_DESARROLLADOR
        Serial.println(F("[Consola::mostrarBrowserArchivos] Browser mostrado"));
    #endif
}

void Consola::actualizarSeleccionArchivo() {
    if (!archivos_cargados) return;
    
    // Obtener índice seleccionado del GestorArchivos
    size_t indice_gestor = miGestorArchivos.obtenerIndiceSeleccion();
    
    // Actualizar índice en Lista (si tiene método para ello)
    // miLista.setIndiceSeleccionado(indice_gestor);
    
    // Redibujar lista con nueva selección
    miLista.mostrar_lista();
    
    #if MODO_DESARROLLADOR
        Serial.print(F("[Consola::actualizarSeleccionArchivo] Índice: "));
        Serial.println(indice_gestor);
    #endif
}

bool Consola::abrirArchivoSeleccionado() {
    if (!archivos_cargados) {
        #if MODO_DESARROLLADOR
            Serial.println(F("[Consola::abrirArchivoSeleccionado] ERROR: No hay archivos"));
        #endif
        return false;
    }
    
    size_t indice = miGestorArchivos.obtenerIndiceSeleccion();
    
    #if MODO_DESARROLLADOR
        Serial.print(F("[Consola::abrirArchivoSeleccionado] Abriendo índice: "));
        Serial.println(indice);
    #endif
    
    if (miGestorArchivos.abrirArchivoPorIndice(indice)) {
        #if MODO_DESARROLLADOR
            Serial.print(F("[Consola::abrirArchivoSeleccionado] Archivo abierto: "));
            Serial.println(miGestorArchivos.obtenerNombreArchivo(indice));
        #endif
        
        // Cambiar a contexto de ejecución
        cambiarContexto(EJECUCION);
        return true;
    }
    
    #if MODO_DESARROLLADOR
        Serial.println(F("[Consola::abrirArchivoSeleccionado] ERROR al abrir"));
    #endif
    
    return false;
}

void Consola::navegarArribaLista() {
    if (!archivos_cargados) return;
    
    // Navegar hacia arriba en GestorArchivos
    miGestorArchivos.navegarLista(-1);
    miLista.navegar_siguiente();
    
    // Actualizar visualización
    actualizarSeleccionArchivo();
    
    #if MODO_DESARROLLADOR
        Serial.println(F("[Consola::navegarArribaLista] Navegando arriba"));
    #endif
}

void Consola::navegarAbajoLista() {
    if (!archivos_cargados) return;
    
    // Navegar hacia abajo en GestorArchivos
    miGestorArchivos.navegarLista(+1);
    miLista.navegar_previo();
    // Actualizar visualización
    actualizarSeleccionArchivo();
    
    #if MODO_DESARROLLADOR
        Serial.println(F("[Consola::navegarAbajoLista] Navegando abajo"));
    #endif
}

// ========================================
// PROCESAMIENTO DE ENTRADA DE TECLADO
// ========================================

void Consola::procesarTecla(char tecla) {
    #if MODO_DESARROLLADOR
        Serial.print(F("[Consola::procesarTecla] Tecla: "));
        Serial.println(tecla);
    #endif
    
    // Teclas globales (funcionan en cualquier contexto)
    switch (tecla) {
        case '*':  // Volver al menú inicio
            if (contexto_actual != MENU_INICIO) {
                cambiarContexto(MENU_INICIO);
            }
            return;
            
        case '#':  // Función especial (definir según necesidad)
            #if MODO_DESARROLLADOR
                Serial.println(F("[Consola::procesarTecla] Función especial"));
            #endif
            return;
    }
    
    // Procesamiento según contexto
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
                    // Acceso directo a ejecución (solo para pruebas)
                    cambiarContexto(EJECUCION);
                    break;
                case '4':
                    cambiarContexto(CONFIGURACION);
                    break;
            }
            break;
            
        case MENU_ARCHIVOS_SD:
        case MENU_ARCHIVOS_USB:
            procesarTeclaBrowserArchivos(tecla);
            break;
            
        case EJECUCION:
            switch (tecla) {
                case '1':  // Pausar/Reanudar
                    // TODO: Implementar control de pausa
                    break;
                case '2':  // Detener
                    // TODO: Implementar detención
                    break;
                case '0':
                case '*':
                    cambiarContexto(MENU_INICIO);
                    break;
            }
            break;
            
        case CONFIGURACION:
            switch (tecla) {
                case '0':
                case '*':
                    cambiarContexto(MENU_INICIO);
                    break;
            }
            break;
    }
}

void Consola::procesarTeclaBrowserArchivos(char tecla) {
    switch (tecla) {
        case '2':  // Navegar abajo
            navegarAbajoLista();
            break;
            
        case '8':  // Navegar arriba
            navegarArribaLista();
            break;
            
        case '5':  // Seleccionar archivo (Enter)
        case '#':
            abrirArchivoSeleccionado();
            break;
            
        case '0':  // Volver
        case '*':
            cambiarContexto(MENU_INICIO);
            break;
            
        case '4':  // Página anterior (opcional)
            // TODO: Implementar si Lista soporta paginación
            break;
            
        case '6':  // Página siguiente (opcional)
            // TODO: Implementar si Lista soporta paginación
            break;
            
        default:
            #if MODO_DESARROLLADOR
                Serial.print(F("[Consola::procesarTeclaBrowserArchivos] Tecla no asignada: "));
                Serial.println(tecla);
            #endif
            break;
    }
}

// ========================================
// MÉTODOS DE PRUEBA
// ========================================

void Consola::pruebaLecturaUSB() {
    if (!miGestorArchivos.inicializar()) {
        Serial.println(F("[pruebaLecturaUSB] ERROR: Falló inicialización"));
        return;
    }
    
    Serial.println(F("[pruebaLecturaUSB] USB inicializado"));
    
    if (!miGestorArchivos.escanearDirectorio("/")) {
        Serial.println(F("[pruebaLecturaUSB] ERROR: No se pudo escanear"));
        return;
    }
    
    size_t cantidad = miGestorArchivos.obtenerCantidadArchivos();
    
    if (cantidad == 0) {
        Serial.println(F("[pruebaLecturaUSB] No hay archivos G-code"));
        return;
    }
    
    Serial.print(F("[pruebaLecturaUSB] Archivos encontrados: "));
    Serial.println(cantidad);
    
    // Crear array temporal para mostrar en lista
    const char** archivos = new const char*[cantidad];
    
    for (size_t i = 0; i < cantidad; ++i) {
        archivos[i] = miGestorArchivos.obtenerNombreArchivo(i);
        Serial.print(F("  ["));
        Serial.print(i);
        Serial.print(F("] "));
        Serial.println(archivos[i]);
    }
    
    miLista.inicializar(archivos, cantidad);
    miLista.mostrar_lista();
    
    delete[] archivos;
}

void Consola::pruebaLecturaSD() {
    Serial.println(F("\n=== PRUEBA LECTURA SD ==="));
    
    if (!miGestorArchivos.inicializar()) {
        Serial.println(F("[pruebaLecturaSD] ERROR: Falló inicialización"));
        return;
    }
    
    if (!miGestorArchivos.escanearDirectorio("/")) {
        Serial.println(F("[pruebaLecturaSD] ERROR: No se pudo escanear"));
        return;
    }
    
    size_t cantidad = miGestorArchivos.obtenerCantidadArchivos();
    
    if (cantidad == 0) {
        Serial.println(F("[pruebaLecturaSD] No hay archivos G-code"));
        return;
    }
    
    Serial.print(F("[pruebaLecturaSD] Archivos encontrados: "));
    Serial.println(cantidad);
    
    const char** archivos = new const char*[cantidad];
    
    for (size_t i = 0; i < cantidad; ++i) {
        archivos[i] = miGestorArchivos.obtenerNombreArchivo(i);
        Serial.print(F("  ["));
        Serial.print(i);
        Serial.print(F("] "));
        Serial.println(archivos[i]);
    }
    
    miLista.inicializar(archivos, cantidad);
    miLista.mostrar_lista();
    
    delete[] archivos;
}

void Consola::pruebaAbrirGcodeSD() {
    if (!miGestorArchivos.inicializar()) {
        Serial.println(F("[pruebaAbrirGcodeSD] ERROR: No se pudo iniciar SD"));
        return;
    }
    
    if (miGestorArchivos.abrirArchivoPorNombre("FRUITC~1.GCO")) {
        Serial.println(F("[pruebaAbrirGcodeSD] Archivo abierto correctamente"));
    } else {
        Serial.println(F("[pruebaAbrirGcodeSD] ERROR al abrir archivo"));
    }
}

void Consola::pruebaLecturaGcode() {
    if (!miGestorArchivos.inicializar()) {
        Serial.println(F("[pruebaLecturaGcode] ERROR: No se pudo iniciar"));
        return;
    }
    
    if (!miGestorArchivos.abrirArchivoPorNombre("FRUITC~1.GCO")) {
        Serial.println(F("[pruebaLecturaGcode] ERROR: No se pudo abrir archivo"));
        return;
    }
    
    Serial.println(F("[pruebaLecturaGcode] Leyendo archivo..."));
    
    // Leer primeras 10 líneas como ejemplo
    for (int i = 0; i < 10; ++i) {
        const char* linea = miGestorArchivos.leerLineaNoBloqueante();
        if (linea) {
            Serial.print(F("Línea "));
            Serial.print(i + 1);
            Serial.print(F(": "));
            Serial.println(linea);
        } else {
            Serial.println(F("Fin de archivo o línea no lista"));
            break;
        }
        delay(10);  // Pequeño delay para dar tiempo a lectura no bloqueante
    }
}