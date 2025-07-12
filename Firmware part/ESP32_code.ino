#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <DHT.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ==== Wi-Fi credentials ====
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "************"

// ==== Firebase credentials ====
#define API_KEY "API_Key"
#define DATABASE_URL "your_url"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ==== DHT sensor configuration ====
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ==== Capteurs ====
#define LDR1 39
#define LDR2 36
const int potPin = 34;

// ==== Variables partagées ====
int r1 = 0, r2 = 0, potValue = 0;
float temperature = 0.0, humidity = 0.0, lux_moyenne = 0.0;

unsigned long startTime;
SemaphoreHandle_t dataMutex;
bool signupOK = false;

void TaskLectureCapteurs(void * parameter) {
  for(;;) {
    int ldr1 = analogRead(LDR1);
    int ldr2 = analogRead(LDR2);
    int pot = analogRead(potPin);

    float lux = (500.0 / (((3.3 - (ldr1 * 3.3 / 4095.0)) * 10000.0 / (ldr1 * 3.3 / 4095.0)) / 1000.0)
              + 500.0 / (((3.3 - (ldr2 * 3.3 / 4095.0)) * 10000.0 / (ldr2 * 3.3 / 4095.0)) / 1000.0)) / 2.0;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    xSemaphoreTake(dataMutex, portMAX_DELAY);
    r1 = ldr1;
    r2 = ldr2;
    potValue = pot;
    lux_moyenne = lux;
    if (!isnan(temp) && !isnan(hum)) {
      temperature = temp;
      humidity = hum;
    }
    xSemaphoreGive(dataMutex);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskUART(void * parameter) {
  for(;;) {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    int ldr1 = r1, ldr2 = r2;
    xSemaphoreGive(dataMutex);

    Serial2.print(ldr1);
    Serial2.print(",");
    Serial2.println(ldr2);

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskFirebase(void * parameter) {
  for(;;) {
    if (Firebase.ready() && signupOK) {
      unsigned long duration = (millis() - startTime) / 1000;

      xSemaphoreTake(dataMutex, portMAX_DELAY);
      int pot = potValue;
      float lux = lux_moyenne, temp = temperature, hum = humidity;
      xSemaphoreGive(dataMutex);

      Firebase.RTDB.setInt(&fbdo, "Duree/Duree_connexion", duration);
      Firebase.RTDB.setFloat(&fbdo, "Luminosite/Moyenne", lux);
      Firebase.RTDB.setInt(&fbdo, "Charge/Puissance", pot);

      if (!isnan(temp) && !isnan(hum)) {
        Firebase.RTDB.setFloat(&fbdo, "DHT11/Temperature", temp);
        Firebase.RTDB.setFloat(&fbdo, "DHT11/Humidite", hum);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, -1, 17);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase SignUp OK");
    signupOK = true;
  } else {
    Serial.printf("Firebase SignUp Error: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
  startTime = millis();
  dataMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskLectureCapteurs, "LectureCapteurs", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskUART, "UART", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskFirebase, "Firebase", 8192, NULL, 1, NULL, 1);
}

void loop() {
  
}
