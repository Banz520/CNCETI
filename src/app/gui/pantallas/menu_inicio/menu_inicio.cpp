#include "menu_inicio.h"

MenuInicio::MenuInicio(MCUFRIEND_kbv &display_ref, Lista &lista_ref) 
    : display(display_ref),
      miBarraSup(display_ref, MENU_PRINCIPAL_TEXT),
      miBarraInf(display_ref, BARRA_INF_TEXT),
      listaOpciones(lista_ref)
{
    // Usar el nuevo método para PROGMEM
    listaOpciones.inicializar(opciones_menu, NUM_OPCIONES_MENU);
    // DEBUG temporal
    
}

void MenuInicio::mostrar() {
    miBarraSup.mostrar();
    listaOpciones.mostrar_lista();
    miBarraInf.mostrar();
}