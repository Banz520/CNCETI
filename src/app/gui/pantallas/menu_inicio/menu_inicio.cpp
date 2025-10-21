#include "menu_inicio.h"

MenuInicio::MenuInicio(MCUFRIEND_kbv &display_ref) : 
display(display_ref), 
miBarraSup(display_ref,"Menu principal - Seleccione una opcion"), 
listaOpciones(display_ref,COLOR_NEGRO,COLOR_BLANCO,COLOR_GRIS_CLARO,COLOR_AZUL,12,218,20), 
miBarraInf(display_ref,"Utilice las teclas <,> para navegar, ENTER para selec.")
{
    this->display = display_ref;
    listaOpciones.inicializar(opciones_menu,5);
    
}

void MenuInicio::mostrar(){
    miBarraSup.mostrar();
    listaOpciones.mostrar_lista();
    miBarraInf.mostrar();
}

