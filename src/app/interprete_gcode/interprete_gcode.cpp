/**
 * @file interprete_gcode.cpp
 * @brief Implementacion del interprete de codigos G
 * @author Tu Nombre
 * @date 2024
 */

#include "interprete_gcode.h"
#include "cnc.h"

InterpreteGcode::InterpreteGcode() {
    reiniciarValores();
}

float InterpreteGcode::extraerValor(const String& cadena, const String& prefijo) {
    int indice = cadena.indexOf(prefijo);
    if (indice == -1) {
        return 0.0f;
    }
    
    // Buscar el inicio del numero
    int inicio_numero = indice + prefijo.length();
    int fin_numero = cadena.length();
    
    // Encontrar el final del numero
    for (int i = inicio_numero; i < cadena.length(); i++) {
        char c = cadena[i];
        if ((c < '0' || c > '9') && c != '.' && c != '-') {
            fin_numero = i;
            break;
        }
    }
    
    String valor_str = cadena.substring(inicio_numero, fin_numero);
    return valor_str.toFloat();
}

void InterpreteGcode::procesarInterpolacionLineal() {
#if MODO_DESARROLLADOR
    Serial.print("Ejecutando interpolacion lineal G");
    Serial.print(comando_actual_.comando);
    Serial.print(" - X:");
    Serial.print(comando_actual_.x);
    Serial.print(" Y:");
    Serial.print(comando_actual_.y);
    Serial.print(" Z:");
    Serial.print(comando_actual_.z);
    Serial.print(" Velocidad:");
    Serial.println(comando_actual_.velocidad);
#endif
}

void InterpreteGcode::procesarInterpolacionCircular() {
#if MODO_DESARROLLADOR
    Serial.print("Ejecutando interpolacion circular G");
    Serial.println(comando_actual_.comando);
#endif
}

void InterpreteGcode::procesarParadaProgramada() {
#if MODO_DESARROLLADOR
    Serial.println("Ejecutando parada programada G04");
#endif
}

void InterpreteGcode::procesarSeleccionUnidades() {
#if MODO_DESARROLLADOR
    Serial.print("Seleccionando unidades G");
    Serial.println(comando_actual_.comando);
#endif
}

bool InterpreteGcode::procesarComando(const String& comando) {
    // Reiniciar valores para nuevo comando
    reiniciarValores();
    
    // Convertir a mayusculas para procesamiento case-insensitive
    String comando_upper = comando;
    comando_upper.toUpperCase();
    
    // Eliminar espacios y tabulaciones
    comando_upper.trim();
    
    // Ignorar lineas vacias o comentarios
    if (comando_upper.length() == 0 || comando_upper[0] == ';' || comando_upper[0] == '(') {
        return true;
    }

#if MODO_DESARROLLADOR
    Serial.print("Procesando comando: ");
    Serial.println(comando_upper);
#endif

    // Extraer codigo G
    int indice_g = comando_upper.indexOf('G');
    if (indice_g != -1) {
        String codigo_str = "";
        for (int i = indice_g + 1; i < comando_upper.length(); i++) {
            char c = comando_upper[i];
            if (c >= '0' && c <= '9') {
                codigo_str += c;
            } else {
                break;
            }
        }
        comando_actual_.comando = codigo_str.toInt();
    }

    // Extraer valores de ejes y parametros
    comando_actual_.x = extraerValor(comando_upper, "X");
    comando_actual_.y = extraerValor(comando_upper, "Y"); 
    comando_actual_.z = extraerValor(comando_upper, "Z");
    comando_actual_.velocidad = extraerValor(comando_upper, "F");

#if MODO_DESARROLLADOR
    Serial.print("Parametros extraidos - G");
    Serial.print(comando_actual_.comando);
    Serial.print(" X:");
    Serial.print(comando_actual_.x);
    Serial.print(" Y:");
    Serial.print(comando_actual_.y);
    Serial.print(" Z:");
    Serial.print(comando_actual_.z);
    Serial.print(" F:");
    Serial.println(comando_actual_.velocidad);
#endif

    // Procesar comando segun codigo G para validacion
    switch (comando_actual_.comando) {
        case 0:  // Movimiento rapido
        case 1:  // Interpolacion lineal
            procesarInterpolacionLineal();
            break;
            
        case 2:  // Interpolacion circular horaria
        case 3:  // Interpolacion circular antihoraria
            procesarInterpolacionCircular();
            break;
            
        case 4:  // Parada programada
            procesarParadaProgramada();
            break;
            
        case 20: // Unidades en pulgadas
        case 21: // Unidades en milimetros
            procesarSeleccionUnidades();
            break;
            
        case 90: // Posicionamiento absoluto
#if MODO_DESARROLLADOR
            Serial.println("Estableciendo posicionamiento absoluto G90");
#endif
            break;
            
        case 91: // Posicionamiento relativo
#if MODO_DESARROLLADOR
            Serial.println("Estableciendo posicionamiento relativo G91");
#endif
            break;
            
        default:
#if MODO_DESARROLLADOR
            Serial.print("Codigo G no reconocido: G");
            Serial.println(comando_actual_.comando);
#endif
            return false;
    }
    
    return true;
}

ComandoGcode InterpreteGcode::obtenerComandoActual() const {
    return comando_actual_;
}

void InterpreteGcode::establecerComandoActual(const ComandoGcode& comando) {
    comando_actual_ = comando;
}

void InterpreteGcode::reiniciarValores() {
    comando_actual_.x = 0.0f;
    comando_actual_.y = 0.0f; 
    comando_actual_.z = 0.0f;
    comando_actual_.velocidad = 0.0f;
    comando_actual_.comando = 0;
}

bool InterpreteGcode::hayComandoValido() const {
    return comando_actual_.comando != 0;
}