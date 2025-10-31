#include "menu_inicio.h"

/**
 * @brief Constructor de la clase MenuInicio
 * 
 * Inicializa el menú de inicio con las referencias necesarias para el display,
 * gestión de widgets y lista de opciones. También inicializa la lista de opciones
 * del menú con los valores predefinidos.
 * 
 * @param display_ref Referencia al objeto de display MCUFRIEND_kbv
 * @param lista_ref Referencia al objeto Lista para gestionar las opciones del menú
 * @param gestor_ref Referencia al objeto GestorWidgets para manejar los elementos de la interfaz
 * 
 * @post La lista de opciones queda inicializada con las opciones del menú principal
 */
MenuInicio::MenuInicio(MCUFRIEND_kbv &display_ref, Lista &lista_ref, GestorWidgets &gestor_ref) 
    : display(display_ref),
      miGestorWidgets(gestor_ref),
      listaOpciones(lista_ref)
{
    listaOpciones.inicializar(opciones_menu, 5);
}

/**
 * @brief Muestra el menú de inicio en pantalla
 * 
 * Dibuja la interfaz completa del menú principal que consiste en:
 * - Una barra superior con el título del menú
 * - La lista de opciones centrada en la pantalla
 * - Una barra inferior con información adicional
 * 
 * Utiliza colores predefinidos para los diferentes elementos y
 * coordina con el gestor de widgets para el renderizado.
 * 
 * @see ConfigBarra
 * @see GestorWidgets::dibujarBarra()
 * @see Lista::mostrar_lista()
 */
void MenuInicio::mostrar() {
    ConfigBarra config_superior = {0, 0, 480, 40, COLOR_GRIS_CLARO, COLOR_NEGRO, MENU_PRINCIPAL_TEXT};
    ConfigBarra config_inferior = {0, 280, 480, 40, COLOR_GRIS_OSCURO, COLOR_BLANCO, BARRA_INF_TEXT};

    miGestorWidgets.dibujarBarra(config_superior);
    listaOpciones.mostrar_lista();
    miGestorWidgets.dibujarBarra(config_inferior);
}