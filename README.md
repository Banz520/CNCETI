# CNCETI
![Version](https://img.shields.io/badge/version-1.0.0-blue)
![License](https://img.shields.io/badge/license-CC--BY--4.0-green)
![Platform](https://img.shields.io/badge/platform-Arduino%20%7C%20PlatformIO-00979D)
![CNC](https://img.shields.io/badge/CNC-3%20Ejes-important)

Sistema de control completo para máquina CNC de 3 ejes con interfaz de usuario integrada, soporte para almacenamiento externo y procesamiento de archivos G-code.

---

## Características Principales

### Control de Movimiento
- **Control preciso de 3 ejes** (X, Y, Z) mediante steppers
- **Interpolación de movimientos** con MultiStepper
- **Procesamiento en tiempo real** de comandos G-code

### Interfaz de Usuario
- **Pantalla TFT 3.5 pulgadas** para visualización de estado
- **Teclado matricial 4x4** para control manual
- **Consola integrada** con información en tiempo real

### Almacenamiento y Archivos
- **Soporte para Micro SD** lectura de archivos G-code
- **Conexión USB** con sistema de archivos FAT
- **Gestión de archivos** G-code extensivos

### Procesamiento G-code
- **Intérprete completo** de comandos G-code
- **Ejecución secuencial** de instrucciones
- **Buffer de comandos** para procesamiento fluido

---

## Diagrama de Arquitectura del Sistema

```mermaid
graph TB
    A[Usuario] --> B[Teclado Matricial 4x4]
    A --> C[Display TFT 3.5]
    
    B --> D[Consola Principal]
    C --> D
    
    D --> E[Controlador CNC]
    D --> F[Interprete G-code]
    D --> G[Gestor Archivos]
    
    E --> H[MultiStepper 3 Ejes]
    H --> I[Motor Eje X]
    H --> J[Motor Eje Y]
    H --> K[Motor Eje Z]
    
    G --> L[Memoria Micro SD]
    G --> M[Dispositivo USB]
    
    style D fill:#e1f5fe
    style E fill:#f3e5f5
    style G fill:#e8f5e8
```

---

## Estructura del Hardware

### Componentes Principales
- **Microcontrolador**: Arduino compatible
- **Display**: TFT 3.5 pulgadas con interfaz SPI
- **Teclado**: Matricial 4x4 personalizado
- **Drivers**: Controladores de stepper motors
- **Almacenamiento**: Lector Micro SD + Controlador USB CH376

### Conexiones de Pines
```cpp
// Configuración en pines.h
/**
 * DISPLAY PINES 22 - 41 
 * Referencia: "http://www.rinkydinkelectronics.com/files/UTFT_Requirements.pdf"
 * COMUNICACION SPI PARA TARJETA SD PINES 49 - 52
 * COMUNICACION SERIAL UART PAR USB PINES 16 - 17
 */
#define PIN_CHIP_SELECT_SD 49

#define PIN_MOTOR_X_PUL 62
#define PIN_MOTOR_X_DIR 63
#define PIN_MOTOR_X_EN 61
#define PIN_MOTOR_Y_PUL 60
#define PIN_MOTOR_Y_DIR 59
#define PIN_MOTOR_Y_EN 58
#define PIN_MOTOR_Z_PUL 57
#define PIN_MOTOR_Z_DIR 56
#define PIN_MOTOR_Z_EN 55


#define PIN_TECLADO_FILA_1 2
#define PIN_TECLADO_FILA_2 3
#define PIN_TECLADO_FILA_3 4
#define PIN_TECLADO_FILA_4 5

#define PIN_TECLADO_COL_1 6
#define PIN_TECLADO_COL_2 7
#define PIN_TECLADO_COL_3 8
#define PIN_TECLADO_COL_4 9

```

---

## Flujo de Operación

```mermaid
sequenceDiagram
    participant U as Usuario
    participant T as Teclado
    participant C as Consola
    participant SD as ControladorSD
    participant I as InterpreteGcode
    participant CNC as ControladorCNC
    participant M as Motores
    
    U->>T: Presiona tecla
    T->>C: Enviar entrada
    C->>C: Actualizar display
    
    U->>C: Selecciona archivo G-code
    C->>SD: Abrir archivo
    SD->>SD: Leer línea G-code
    
    loop Ejecución por línea
        SD->>I: Enviar línea G-code
        I->>I: Interpretar comando
        I->>CNC: Comando procesado
        CNC->>M: Ejecutar movimiento
        M-->>CNC: Movimiento completado
        CNC-->>C: Estado actualizado
    end
```

---

## Instalación y Configuración

### Requisitos del Sistema

**Hardware:**
- Placa Arduino Mega compatible
- Display TFT 3.5"
- Teclado matricial 4x4
- Módulo Micro SD
- Controlador USB CH376
- Drivers de motores a pasos

**Software:**
- Visual Studio Code
- Extension de PlatformIO para VS Code
- Librerías requeridas:
  - `Ch376msc.h` (Controlador USB)
  - `MultiStepperLite.h` (Control de motores)
  - `Keypad.h` (Manejo de teclado)

### Configuración Inicial

1. **Conectar el hardware** según la especificación de pines
2. **Cargar las librerías** necesarias en el IDE Arduino
3. **Compilar y subir** el sketch al microcontrolador
4. **Inicializar el sistema** mediante el teclado

---

## Uso del Sistema

### Navegación por Menú
- **Teclas numéricas**: Navegación y entrada de datos
- **Tecla A**: Función específica 1
- **Tecla B**: Función específica 2  
- **Tecla C**: Función específica 3
- **Tecla D**: Función específica 4
- **Tecla ***: Limpiar buffer/acción especial
- **Tecla #**: Confirmar/acción especial

### Operación con Archivos G-code

1. **Seleccionar medio de almacenamiento** (SD o USB)
2. **Navegar al archivo G-code deseado**
3. **Cargar y validar el archivo**
4. **Iniciar ejecución** desde el menú principal
5. **Monitorear progreso** en display TFT

---

## Estructura de Código

### Módulos Principales

```mermaid
graph LR
    A[main.cpp] --> B[controlador_cnc.h]
    A --> C[consola.h]
    A --> D[interprete_gcode.h]
    A --> E[comando_gcode.h]
    
    B --> F[MultiStepperLite]
    C --> G[ControladorUSB]
    C --> H[ControladorSD]
    D --> I[Procesador G-code]
    
    style A fill:#fff3e0
    style B fill:#e1f5fe
    style C fill:#e8f5e8
```

### Archivos de Cabeza Principales

- **`pines.h`**: Definición de pines y conexiones
- **`constantes.h`**: Constantes y configuraciones del sistema
- **`controlador_usb.h`**: Manejo de dispositivos USB
- **`consola.h`**: Interfaz de usuario y display
- **`interprete_gcode.h`**: Procesamiento de comandos G-code
- **`controlador_cnc.h`**: Control central de la máquina CNC
- **`comando_gcode.h`**: Estructura de datos para comandos

---

## Especificaciones Técnicas

### Control de Movimiento
- **Resolución**: Configurable por steps/mm (Predefinida: 80mm)
- **Velocidad máxima**: Dependiente de drivers y motores
- **Aceleración**: Controlada por software

### Procesamiento G-code
- **Comandos soportados**: G0, G1, G2, G3, etc.
- **Parámetros**: X, Y, Z, F, S, M
- **Buffer de línea**: 256 caracteres

---

## Modo Desarrollador

El sistema incluye un modo de desarrollo para debugging:

```cpp
#define MODO_DESARROLLADOR  // Habilitar modo desarrollo
```

**Características del modo desarrollo:**
- Logging detallado por puerto serial
- Monitoreo de estado interno
- Diagnóstico de errores
- Información de depuración

---

## Solución de Problemas

### Problemas Comunes

1. **Archivos G-code no detectados**
   - Verificar formato FAT en memorias
   - Confirmar conexión de módulos SD/USB

2. **Movimiento errático de ejes**
   - Revisar configuración de pines
   - Verificar alimentación de motores

3. **Display no responde**
   - Confirmar conexiones SPI
   - Verificar inicialización de pantalla

### Logs de Diagnóstico

En modo desarrollador, el sistema proporciona información detallada:
- Estado de teclado
- Procesamiento de líneas G-code
- Ejecución de comandos CNC
- Errores de sistema

---

## Licencia

Este proyecto está bajo licencia MIT. Ver archivo `LICENSE` para más detalles.


---


## Licencia

Este proyecto está bajo la licencia **Creative Commons Attribution 4.0 International (CC BY 4.0)**. 
Eres libre de:
- Compartir — copiar y redistribuir el material en cualquier medio o formato
- Adaptar — remezclar, transformar y construir upon el material para cualquier propósito

Bajo los siguientes términos:
- **Atribución** — Debes dar crédito apropiado, proporcionar un enlace a la licencia, 
  e indicar si se realizaron cambios.

Consulta la licencia completa en: https://creativecommons.org/licenses/by/4.0/

---

**Desarrollado para el control de máquinas CNC de 3 ejes con interfaz de usuario integrada**