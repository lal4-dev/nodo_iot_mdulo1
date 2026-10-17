#include <Arduino.h>
// LIBRERIAS QUE PIDEN QUE USEMOS(esto lo saque del punto 3.3)
#include <DHT.h>
#include <ESP32Servo.h>

// ==================================================
// DEFINICIÓN DE PINES (esto lo saque del punto 3.1)
// ==================================================
#define DHT_PIN 4 // Pin de datos del sensor DHT22 (GPIO 4)
#define DHT_TYPE DHT22
#define LED_PIN 15   // Pin del ánodo del LED rojo (GPIO 15)
#define SERVO_PIN 13 // Pin de señal PWM del servomotor (GPIO 13)

// ==================================================
// CONSTANTES Y PARÁMETROS DEL SISTEMA (esto lo saque del punto 3.2)
// ==================================================
const float TEMP_MIN = -20.0; // Temperatura mínima para mapeo (0 grados)
const float TEMP_MAX = 60.0;  // Temperatura máxima para mapeo (180 grados)
const int ANGULO_MIN = 0;     // Ángulo mínimo del servo
const int ANGULO_MAX = 180;   // Ángulo máximo del servo
const float UMBRAL_ALERTA =
    40.0; // Umbral crítico para encendido de alarma visual

// ==================================================
// INSTANCIAS DE OBJETOS
// ==================================================
DHT dht(DHT_PIN, DHT_TYPE);
Servo miServo;

// ==================================================
// DECLARACIÓN DE FUNCIONES AUXILIARES
// ==================================================
int mapearTemperatura(float temp);
void controlarAlarmaVisual(float temp, String &estadoLed);
void actualizarServomotor(int angulo);
void mostrarEstadoSerie(float temp, int angulo, const String &estadoLed);

// ==================================================
// FUNCIÓN AUXILIAR: Mapeo lineal temperatura -> ángulo
// Mapea linealmente: -20 °C -> 0° | 60 °C -> 180°
// ==================================================
int mapearTemperatura(float temp) {
  // Aseguramos que la temperatura se mantenga dentro del rango permitido
  float tempAjustada = constrain(temp, TEMP_MIN, TEMP_MAX);

  // Fórmula de interpolación lineal:
  // angulo = (temp - temp_min) * (ang_max - ang_min) / (temp_max - temp_min) +
  // ang_min
  float angulo = (tempAjustada - TEMP_MIN) * (ANGULO_MAX - ANGULO_MIN) /
                     (TEMP_MAX - TEMP_MIN) +
                 ANGULO_MIN;

  return (int)round(angulo);
}

// ==================================================
// FUNCIÓN AUXILIAR: Control de alarma visual (LED Rojo)
// Si temperatura >= 40 °C -> Encendido (ON)
// Si temperatura < 40 °C  -> Apagado (OFF)
// ==================================================
void controlarAlarmaVisual(float temp, String &estadoLed) {
  if (temp >= UMBRAL_ALERTA) {
    digitalWrite(LED_PIN, HIGH);
    estadoLed = "ON";
  } else {
    digitalWrite(LED_PIN, LOW);
    estadoLed = "OFF";
  }
}

// ==================================================
// FUNCIÓN AUXILIAR: Posicionamiento del servomotor
// ==================================================
void actualizarServomotor(int angulo) { miServo.write(angulo); }

// ==================================================
// FUNCIÓN AUXILIAR: Formato en Monitor Serie
// ==================================================
void mostrarEstadoSerie(float temp, int angulo, const String &estadoLed) {
  Serial.print("Temperatura: ");
  Serial.print(temp, 1);
  Serial.print(" °C | Ángulo: ");
  Serial.print(angulo);
  Serial.print("° | LED: ");
  Serial.println(estadoLed);
}

// ==================================================
// SETUP
// ==================================================
void setup() {
  // Configuración del Monitor Serie a 115200 baudios
  Serial.begin(115200);
  delay(500);
  Serial.println("--- Sistema de Monitoreo de Temperatura Iniciado ---");

  // Configuración del pin del LED como salida
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Inicialmente apagado

  // Inicialización del sensor DHT22
  dht.begin();

  // Configuración y conexión del servomotor
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  miServo.setPeriodHertz(50); // Servos estándar SG90 a 50Hz
  miServo.attach(SERVO_PIN, 500,
                 2400); // Rango de pulso típico para SG90 (0° a 180°)

  // Posición inicial del servomotor en 0° (-20 °C)
  miServo.write(0);
}

// ==================================================
// LOOP
// ==================================================
void loop() {
  // Lectura periódica de la temperatura cada 2 segundos (lo saque del
  // punto 3.2.1)
  delay(2000);

  float temperatura = dht.readTemperature();

  // Si falla la lectura, NO se actualizan el motor ni el LED
  if (isnan(temperatura)) {
    Serial.println(
        "Error: No se pudo leer el sensor DHT22. Manteniendo estado previo.");
    return;
  }

  // Con todo esto nos queda:
  //  1. Mapeo de temperatura a ángulo
  int angulo = mapearTemperatura(temperatura);

  //  2. Actualización de posición del servomotor
  actualizarServomotor(angulo);

  //  3. Control del LED indicador de alarma
  String estadoLed = "OFF";
  controlarAlarmaVisual(temperatura, estadoLed);

  //  4. Salida formateada por el Monitor Serie
  mostrarEstadoSerie(temperatura, angulo, estadoLed);
}
