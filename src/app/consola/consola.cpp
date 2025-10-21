#include "consola.h"

Consola::Consola() : 
miDisplay(), 
miMenuInicio(miDisplay), 
miBarraAlerta(miDisplay,"Sin alerta"),
miLista(miDisplay,COLOR_NEGRO,COLOR_BLANCO,COLOR_GRIS_CLARO,COLOR_AZUL,12,218,20){

}

void Consola::iniciar(){
    //uint8_t estado_usb = 0;
    //Inicia el display obteniendo el ID especifico del tipo de pantalla (TFT No touch de 3.5 pulgadas)
    miDisplay.begin(miDisplay.readID());
    miDisplay.setRotation(1);
    miDisplay.fillScreen(COLOR_BLANCO);
    //estado_usb = miControladorUSB.inicializar_usb_host();
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