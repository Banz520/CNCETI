#include "consola.h"

Consola::Consola() 
    : miDisplay(),
      miLista(miDisplay, COLOR_NEGRO, COLOR_BLANCO, COLOR_GRIS_CLARO, COLOR_AZUL, 12, 218, 20),
      miBarraAlerta(miDisplay, SIN_ALERTA_TEXT),  // Ahora usa FlashStringHelper
      miMenuInicio(miDisplay, miLista)
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

void Consola::pruebaLecturaSD(){
    miControladorSD.iniciarSD();

    miControladorSD.imprimirEstructuraDirectorio(SD.open("/"),0);
    const char** archivos_gcode = miControladorSD.obtenerListaArchivosGcode();
    if(!miControladorSD.listaVacia()){
        miLista.inicializar(archivos_gcode,miControladorSD.obtenerCantidadArchivos());
        miLista.mostrar_lista();
    }
    
}

#endif