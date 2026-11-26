#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "Protocentral_MAX30205.h"
#include "MAX30105.h"
#include "heartRate.h"

#define I2C_SDA 8
#define I2C_SCL 9
#define BUZZER_PIN 10
#define LED_POWER 0
#define LED_ALARM 1

const char* ssid = "HVRC";
const char* password = "h1v2r3c4";

WebServer server(80);
MAX30205 tempSensor;
MAX30105 particleSensor;

long lastBeat = 0;
float beatsPerMinute = 0;
int beatAvg = 0;
long irValue = 0;

const int NUM_LECTURAS = 10;
int lecturasBPM[NUM_LECTURAS];
int indiceLectura = 0;
int totalLecturas = 0;

// --- VARIABLES DE CONFIGURACIÓN (Modificables por el Dashboard) ---
float TEMP_ALERTA = 38.5;   // Ahora es variable, no const
int BPM_ALERTA = 120; 
bool enAlerta = false;
bool buzzerSilenciado = false; // Para la HMI

// --- FUNCIÓN PARA ENVIAR DATOS AL DASHBOARD ---
void handleData() {
  String json = "{";
  json += "\"temp\":" + String(tempSensor.getTemperature(), 2) + ",";
  json += "\"bpm\":" + String(beatAvg) + ",";
  json += "\"ir\":" + String(irValue) + ",";
  json += "\"alerta\":" + String(enAlerta ? "true" : "false") + ",";
  json += "\"umbral_temp\":" + String(TEMP_ALERTA,1) + ",";
  json += "\"umbral_bpm\":" + String(BPM_ALERTA);
  json += "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204); // 204 significa "OK, sin contenido"
}


// --- FUNCIÓN PARA SILENCIAR LA ALARMA (HMI) ---
void handleSilenciar() {
  buzzerSilenciado = true;
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Alarma silenciada");
}

// --- FUNCIÓN PARA ACTUALIZAR EL UMBRAL (HMI) ---
void handleConfig() {
  
  if (server.hasArg("temp")) {
    float nuevoUmbral = server.arg("temp").toFloat();
    if (nuevoUmbral >= 20.0 && nuevoUmbral <= 42.0) {
      TEMP_ALERTA = nuevoUmbral;
    }
  }

  if (server.hasArg("bpm")) {
    int nuevoBpm = server.arg("bpm").toInt();
    if (nuevoBpm >= 20 && nuevoBpm <= 180) { // Rango lógico para alerta
      BPM_ALERTA = nuevoBpm;
    }
  }
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Configuracion actualizada");
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_ALARM, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_ALARM, LOW);
  digitalWrite(LED_POWER, HIGH);

  Wire.begin(I2C_SDA, I2C_SCL);
  tempSensor.scanAvailableSensors();
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  particleSensor.setup(25, 4, 2, 100, 411, 4096);
  particleSensor.setPulseAmplitudeRed(25);
  particleSensor.setPulseAmplitudeGreen(0);

  Serial.print("Conectando a Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_POWER, !digitalRead(LED_POWER));
  }
  Serial.println("\nWi-Fi Conectado. IP: " + WiFi.localIP().toString());
  digitalWrite(LED_POWER, HIGH);

  // Configuración de rutas del servidor
  server.on("/data", HTTP_GET, handleData);
  server.on("/silenciar", HTTP_GET, handleSilenciar);
  server.on("/silenciar", HTTP_OPTIONS, handleOptions);
  server.on("/config", HTTP_GET,handleConfig);
  server.on("/config", HTTP_OPTIONS,handleOptions);
  server.on("/data", HTTP_OPTIONS, handleOptions);
  server.begin();
  
  lastBeat = millis();
}

void loop() {
  server.handleClient(); // Atender al dashboard

  float tempC = tempSensor.getTemperature();
  irValue = particleSensor.getIR();

  if (irValue > 50000) {
    if (checkForBeat(irValue) == true) {
      long delta = millis() - lastBeat;
      lastBeat = millis();
      if (delta > 0) beatsPerMinute = 60.0 / (delta / 1000.0);
      if (beatsPerMinute < 200 && beatsPerMinute > 30) {
        lecturasBPM[indiceLectura] = (int)beatsPerMinute;
        indiceLectura = (indiceLectura + 1) % NUM_LECTURAS;
        if (totalLecturas < NUM_LECTURAS) totalLecturas++;
        int suma = 0;
        for (int i = 0; i < totalLecturas; i++) suma += lecturasBPM[i];
        beatAvg = suma / totalLecturas;
      }
    }
  } else {
    totalLecturas = 0; indiceLectura = 0; beatAvg = 0; beatsPerMinute = 0;
  }

  // --- LÓGICA DE ALERTA ---
  if ((tempC >= TEMP_ALERTA) || (beatAvg >= BPM_ALERTA && beatAvg > 0)) {
    if (!enAlerta) {
      Serial.println("🚨 ¡ALERTA!");
      enAlerta = true;
      buzzerSilenciado = false; // Al entrar en alerta, el silencio se resetea
    }
    digitalWrite(LED_ALARM, HIGH); 
    if (!buzzerSilenciado) {digitalWrite(BUZZER_PIN, HIGH); }
    else {digitalWrite(BUZZER_PIN, LOW);}
  } 
  
  else {
    if (enAlerta) {
      Serial.println("✅ Normal");
      enAlerta = false;
    }
    digitalWrite(BUZZER_PIN, LOW); 
    digitalWrite(LED_ALARM, LOW); 
    buzzerSilenciado = false; // Si vuelve a la normalidad, reseteamos
  }
  delay(10); // Pequeño delay para estabilidad
}