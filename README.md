# ThermoGuard - Wearable para la Prevención de Golpes de Calor

**ThermoGuard** es un sistema embebido tipo *wearable* (banda para muñeca) diseñado para monitorear signos vitales en tiempo real y alertar al usuario antes de que sufra un golpe de calor. El sistema integra sensores de grado médico con un microcontrolador ESP32-C3 y se conecta a un dashboard web para la visualización remota de datos.

**Proyecto Integrador - Sistemas Programables (SCC1023)**  
*Ingeniería en Sistemas Computacionales - Grupo S7A*

---

## Tabla de Contenidos
1. [Características Principales](#-características-principales)
2. [Arquitectura del Sistema](#-arquitectura-del-sistema)
3. [Hardware Requerido](#-hardware-requerido)
4. [Estructura del Repositorio](#-estructura-del-repositorio)
5. [Instalación y Uso (Arduino)](#-instalación-y-uso-arduino)
6. [Instalación y Uso (Angular)](#-instalación-y-uso-angular)
7. [Lógica de Alerta](#-lógica-de-alerta)
8. [Autores](#-autores)

---

## Características Principales
- **Monitoreo Continuo:** Lectura en tiempo real de temperatura corporal y frecuencia cardíaca.
- **Alertas Locales:** Alarma sonora (buzzer) y visual (LEDs) en el dispositivo cuando se detecta riesgo.
- **Dashboard Remoto:** Interfaz web (HMI) con gráficas en tiempo real y capacidad de configurar los umbrales de alerta.
- **Diseño Wearable:** Montaje en banda de neopreno alimentado por batería LiPo recargable.

---

## Arquitectura del Sistema
El flujo de datos funciona de la siguiente manera:
1. Los sensores **MAX30205** (Temperatura) y **MAX30102** (Pulso) envían datos vía protocolo **I2C** al **ESP32-C3**.
2. El ESP32 procesa la información, calcula promedios y evalúa si los valores superan los umbrales de seguridad.
3. El ESP32 levanta un **Servidor Web** y transmite los datos en formato JSON vía **Wi-Fi/HTTP**.
4. El **Dashboard en Angular** consume los datos mediante peticiones HTTP y los renderiza en tiempo real.

---

## Hardware Requerido
- 1x Microcontrolador ESP32-C3 SuperMini.
- 1x Sensor de Temperatura MAX30205 (Módulo AM-057).
- 1x Sensor de Ritmo Cardíaco MAX30102.
- 1x Buzzer Activo.
- 2x LEDs (Verde y Rojo) + Resistencias (220Ω).
- 1x Módulo de carga LiPo (TP4056 o similar).
- 1x Batería LiPo 3.7V 1000mAh.
- 1x Interruptor deslizable (ON/OFF).

---

## Estructura del Repositorio
El código está dividido en dos partes principales:
```text
ThermoGuard
 ┣ wear-dashboard/    # Código fuente del Dashboard Web (Angular)
 ┃ ┣ src/
 ┃ ┃ ┣ 📂 app/
 ┃ ┃ ┃ ┣ dashboard/   # Componente visual del dashboard
 ┃ ┃ ┃ ┣ services/    # Servicio HTTP para conectar con el ESP32
 ┃ ┃ ┃ ┗ app.config.ts
 ┃ ┣ angular.json
 ┃ ┗ package.json
 ┗ final/             # Código fuente del Microcontrolador (C++)
   ┗ final.ino        # Código principal (Sensores, Lógica y Servidor Web)
```
## Instalación y Uso (Arduino)

1. Abre el archivo `final.ino` que se encuentra en la carpeta `final/`.
2. Asegúrate de tener instalado el soporte para placas **ESP32** en el Arduino IDE.
3. Instala las siguientes librerías desde el Gestor de Librerías:
   - `Protocentral_MAX30205`
   - `SparkFun MAX3010x Pulse and Proximity Sensor Library`
   - `Wire` (incluida por defecto)
4. En el código, modifica las variables `ssid` y `password` con los datos de tu red Wi-Fi local:
   ```cpp
   const char* ssid = "TU_WIFI";
   const char* password = "TU_CONTRASEÑA";
   ```
---

## Instalación y Uso (Angular)

1. Abre una terminal y navega a la carpeta del dashboard:
   ```bash
   cd wear-dashboard
   ```
2. Instala las dependencias necesarias (Asegúrate de tener Node.js instalado):
   ```bash
   npm install
   ```
3. Abre el archivo `src/app/services/esp32.ts` y `src/app/dashboard/dashboard.ts` y cambia la IP por la que te dio el Monitor Serie del ESP32:
   ```typescript
   private apiUrl = 'http://TU_IP_DEL_ESP32';
   // y en el servicio:
   private apiUrl = 'http://TU_IP_DEL_ESP32/data';
   ```
4. Ejecuta el servidor de desarrollo local:
   ```bash
   ng serve -o
   ```
5. El navegador se abrirá automáticamente en `http://localhost:4200` mostrando el dashboard en vivo con los datos del wearable.

---

## Lógica de Alerta
El sistema evalúa constantemente dos variables vitales. Si **cualquiera** de las dos condiciones se cumple, se activa la alarma local (Buzzer + LED Rojo) y la interfaz web cambia a color rojo para notificar al usuario:

- **Temperatura Corporal:** Supera el umbral configurado (por defecto `38.5 °C`).
- **Pulsaciones (BPM):** Supera el umbral configurado (por defecto `120 BPM`).

*Los umbrales pueden ser monitoreados y modificados en tiempo real desde el Panel de Control (HMI) del dashboard web.*

---

## 👥 Autores
- **Pedro Arturo Matrecitos Leyva**
- **Diego Fernando Mejia Villarreal**
- **Hannah Valeria Rivas Chay**
- **Sebastian Tapia Cano**

*Docente: M.C. Gelacio Germán Gandarilla*  
*Periodo: Agosto - Diciembre 2026*
