#ifndef CONSOLA_H
#define CONSOLA_H

#include "menu_inicio.h"
#include "MCUFRIEND_kbv.h"
#include <UTFTGLUE.h>
#include "lista.h"
#include "constantes.h"
#include "gestor_widgets.h"
#include "gestor_archivos.h"
#include "pantalla_ejecucion.h"


enum CONTEXTO_APP : uint8_t {
    MENU_INICIO,
    MENU_ARCHIVOS_SD,
    MENU_ARCHIVOS_USB,
    EJECUCION,
    CONFIGURACION
};

class Consola {
private:
    //MCUFRIEND_kbv miDisplay;
    UTFTGLUE miDisplay;
    Lista miLista; // Única instancia de Lista
    GestorWidgets miGestorWidgets;
    MenuInicio miMenuInicio;
    
    GestorArchivos &miGestorArchivos;
    PantallaEjecucion miPantallaEjecucion;

    CONTEXTO_APP contexto_actual;
    CONTEXTO_APP contexto_anterior;
    bool primer_actualizacion;
    
    void procesarTecla(char tecla);
    void cambiarContexto(CONTEXTO_APP nuevo_contexto);
    void mostrarInterfazContexto();
    void limpiarPantallaContextoAnterior();
    

public:
    Consola(GestorArchivos &miGestorArchivos_ref);

    void iniciar();
    void actualizar(char tecla,const float &origen_x,const float &posicion_x,const float &destino_x,const float &origen_y,const float &posicion_y,const float &destino_y,const float &origen_z,const float &posicion_z,const float &destino_z, const char* comando_gcode);
    
    CONTEXTO_APP obtenerContextoActual() const { return contexto_actual; }
    
    void pruebaLecturaSD();
    void pruebaLecturaUSB();
    void pruebaLecturaGcode();
    void pruebaAbrirGcodeSD();
    #if MODO_DESARROLLADOR
    
    #endif
};

#endif