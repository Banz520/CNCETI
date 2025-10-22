#include "menu_inicio.h"


MenuInicio::MenuInicio(MCUFRIEND_kbv &display_ref, Lista &lista_ref, GestorWidgets &gestor_ref) 
    : display(display_ref),
      miGestorWidgets(gestor_ref),  // CORREGIDO: inicializa referencia
      listaOpciones(lista_ref)
{
    listaOpciones.inicializar(opciones_menu, 5);
}

void MenuInicio::mostrar() {
    ConfigBarra config_superior = {0, 0, 480, 40, COLOR_GRIS_CLARO, COLOR_NEGRO, MENU_PRINCIPAL_TEXT};
    ConfigBarra config_inferior = {0, 280, 480, 40, COLOR_GRIS_OSCURO, COLOR_BLANCO, BARRA_INF_TEXT};

    miGestorWidgets.dibujarBarra(config_superior);
    listaOpciones.mostrar_lista();
    miGestorWidgets.dibujarBarra(config_inferior);
}