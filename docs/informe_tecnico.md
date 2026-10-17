# Informe Técnico: Sistema de monitoreo de temperatura con indicación visual y representación angular mediante servomotor

**Diplomatura Universitaria en Internet de las Cosas (IoT)**  
**Facultad de Tecnología y Ciencias Aplicadas - UNCA**  
**Nodo Tecnológico - Municipalidad de SFV de Catamarca**  

* **Estudiante:** Alejo Leonel Maidana
* **Fecha:** Octubre de 2026
* **Plataforma de Desarrollo y Simulación:** Wokwi / PlatformIO (VS Code)
* **Placa:** ESP32 DevKit v4

---

## 1. Descripción del Funcionamiento del Sistema

El proyecto consiste en el diseño e implementación de un sistema embebido de monitoreo ambiental basado en el microcontrolador **ESP32**. 

El sistema realiza las siguientes tareas de forma continua:
1. **Adquisición de datos:** Mide la temperatura ambiental mediante un sensor digital **DHT22** conectado al pin **GPIO 4**, realizando lecturas periódicas cada 2 segundos.
2. **Representación angular (Servomotor):** Mapea linealmente el rango de temperaturas definido entre **-20 °C y 60 °C** a un ángulo mecánico de **0° a 180°** en un servomotor estándar tipo **SG90** conectado al pin **GPIO 13**.
3. **Alarma visual crítica (LED Rojo):** Evalúa el valor de temperatura obtenido contra un umbral de seguridad de **40.0 °C**. Si la temperatura es igual o superior a 40 °C, activa un **LED rojo** en el pin **GPIO 15** (estado ON). Si desciende por debajo de 40 °C, el LED se apaga (estado OFF).
4. **Tolerancia a fallos:** Implementa la validación isnan() sobre la lectura del sensor. Si ocurre un fallo en la comunicación con el DHT22, el sistema mantiene el último estado seguro conocido y no actualiza erróneamente ni el servomotor ni la alarma visual.
5. **Telemetría serie:** Transmite por el puerto serie a **115200 baudios** el reporte de cada medición con el formato obligatorio:  
   Temperatura: XX.X °C | Ángulo: ZZZ° | LED: ON/OFF.

---

## 2. Diagrama de Conexiones y Hardware Simulado

### Tabla de Pines y Conexiones (ESP32)

| Componente | Pin del Periférico | Pin de la ESP32 | Notas de conexión |
| :--- | :--- | :--- | :--- |
| **DHT22** | VCC | 3V3 | Alimentación lógica (3.3V) |
| **DHT22** | SDA (Datos) | GPIO 4 | Señal de datos digital |
| **DHT22** | GND | GND | Tierra común |
| **LED Rojo** | Ánodo (+) | GPIO 15 | Salida digital |
| **LED Rojo** | Cátodo (-) | GND | En serie con resistencia de 220 $\Omega$ |
| **Servomotor SG90** | V+ (Rojo) | 5V | Alimentación de potencia |
| **Servomotor SG90** | GND (Marrón/Negro) | GND | Tierra común |
| **Servomotor SG90** | PWM (Naranja/Amarillo)| GPIO 13 | Señal de control PWM |

> *(Despues tengo que poner una captura de pantalla del circuito armado y funcionando en el simulador Wokwi)*

---

## 3. Explicación del Código Principal

El código fuente fue desarrollado de manera modular y estructurada:

* **Inclusión de Librerías:**  
  Se utilizaron las bibliotecas oficiales requeridas: DHT.h (Adafruit) para gestionar el protocolo de lectura del sensor DHT22 y ESP32Servo.h para generar los pulsos PWM de alta precisión requeridos por el servomotor SG90 en la arquitectura ESP32.

* **Función mapearTemperatura(float temp):**  
  Aplica la fórmula matemática de interpolación lineal:
  \text{Ángulo} = (\text{Temp} - (-20)) \times \frac{180 - 0}{60 - (-20)} + 0
  Se utiliza la función constrain() para garantizar que ninguna lectura anómala fuera del rango $[-20, 60]$ supere los límites mecánicos del servomotor ($[0^{\circ}, 180^{\circ}]$).

* **Función controlarAlarmaVisual(float temp, String &estadoLed):**  
  Compara la temperatura actual con la constante UMBRAL_ALERTA = 40.0. Si la condición se cumple, aplica nivel lógico alto (HIGH) al pin 15 y define la etiqueta "ON"; de lo contrario, aplica nivel bajo (LOW) y define "OFF".

* **Función setup():**  
  Configura la velocidad del puerto serie a 115200 baudios, inicializa el pin del LED como salida, enciende el bus del DHT22 y asigna los temporizadores de hardware del ESP32 mediante ESP32PWM::allocateTimer() para asegurar una señal PWM estable en el servo sin fluctuaciones.

* **Función loop():**  
  Ejecuta una pausa no bloqueante de 2 segundos, lee la temperatura con dht.readTemperature(), verifica la integridad de la lectura y actualiza simultáneamente el servomotor, el LED y la impresión por el Monitor Serie.

---

## 4. Salida por Monitor Serie

Ejemplo de captura del registro de eventos en tiempo real:

`	ext
--- Sistema de Monitoreo de Temperatura Iniciado ---
Temperatura: 25.3 °C | Ángulo: 102° | LED: OFF
Temperatura: 38.7 °C | Ángulo: 132° | LED: OFF
Temperatura: 40.1 °C | Ángulo: 135° | LED: ON
Temperatura: 42.5 °C | Ángulo: 141° | LED: ON
Temperatura: 39.2 °C | Ángulo: 133° | LED: OFF
`

> *(Despues tengo que poner la captura de pantalla de la consola del Monitor Serie de Wokwi)*

---

## 5. Conclusión Personal sobre el Aprendizaje Logrado

El desarrollo de este trabajo práctico permitió afianzar los conceptos fundamentales de los sistemas embebidos aplicados al Internet de las Cosas. Se comprendió la importancia de transformar variables físicas ambientales (temperatura) en señales mecánicas proporcionales (ángulo mediante control PWM) y señales visuales de advertencia (LED). 

Asimismo, se adquirió experiencia práctica en el manejo de librerías en microcontroladores de 32 bits (ESP32), la gestión de temporizadores por hardware y la simulación integral mediante plataformas modernas como Wokwi y PlatformIO, facilitando la validación del sistema sin necesidad inmediata de hardware físico.