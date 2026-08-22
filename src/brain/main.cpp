#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>  
#include <Preferences.h>  
#include "Audio.h" // Bibliothèque Schreibfaul1 (v2.0.6 requise selon configuration PIO)

Preferences preferences;
String apiKey = "";     
String witAiToken = ""; 

// --- BROCHES ESP32-S3 ---
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC   13
#define TFT_RST  14
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// BROCHES AUDIO
#define I2S_SCK_MIC 4
#define I2S_WS_MIC  5
#define I2S_SD_MIC  6
#define I2S_BCLK_AMP 15
#define I2S_LRC_AMP  16
#define I2S_DIN_AMP  17

// Objet Audio pour le haut-parleur
Audio audio;

// --- CAPTEURS ---
Adafruit_BME280 bme; 
Adafruit_MPU6050 mpu;
bool bmeActif = false, mpuActif = false;
unsigned long lastBMEReadTime = 0;
const int bmeCheckInterval = 5000; 
float currentTemp = 20.0;

// --- VARIABLES GLOBALES ET GESTION MÉMOIRE ---
volatile int etatEmotion = 4; // 4 = GLITCH AU DEMARRAGE
TaskHandle_t TacheAffichageHandle; 
const int SEUIL_BRUIT_REVEIL = 2500; 
unsigned long lastTriggerTime = 0; 
const int DEBOUNCE_AUDIO_MS = 4000; // Délai anti-rebond pour le micro

// Buffer Audio (Allocation PSRAM pour préserver la RAM interne)
const int recordTimeMs = 3000;
const int sampleRate = 16000;
const int numSamples = (recordTimeMs * sampleRate) / 1000;
int16_t* audioBuffer = nullptr; 

// Certificats Root CA (À remplir pour la production, remplace setInsecure)
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
"... Remplacer par le vrai certificat racine GlobalSign ...\n" \
"-----END CERTIFICATE-----\n";

// Géométrie des yeux
const int baseEyeWidth = 70, baseEyeHeight = 100, irisOffset = 22;    
const int leftEyeBaseX = 50, rightEyeBaseX = 160, baseY = 70;          
float currentXOffset = 0, currentYOffset = 0, targetXOffset = 0, targetYOffset = 0;
unsigned long lastMoveTime = 0, lastBlinkTime = 0;
int nextMoveDelay = 2000, nextBlinkDelay = 3000;
bool isBlinking = false;
float eyeHeightScale = 1.0; 

// ==========================================
// DESSIN PETITE ICÔNE WIFI DISCRÈTE
// ==========================================
void dessinerIconeWifi(int x, int y, uint16_t color) {
  tft.fillCircle(x + 6, y + 10, 1, color);
  tft.drawCircleHelper(x + 6, y + 9, 4, 1 | 2, color);
  tft.drawCircleHelper(x + 6, y + 9, 8, 1 | 2, color);
}

// ==========================================
// CŒUR 0 : AFFICHAGE & YEUX GLITCH (inchangé)
// ==========================================
void CodeTacheAffichage(void * pvParameters) {
  for(;;) { 
    unsigned long currentTime = millis();
    if (etatEmotion == 4) {
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextSize(4);
      int x = 70, y = 120;
      // Couleurs Cyan (0x07FF) et Magenta (0xF81F) de l'identité visuelle
      tft.setTextColor(0x07FF); tft.setCursor(x + random(-4, 5), y + random(-1, 2)); tft.print("LUMI");
      tft.setTextColor(0xF81F); tft.setCursor(x + random(-4, 5), y + random(-1, 2)); tft.print("LUMI");
      tft.setTextColor(ST77XX_WHITE); tft.setCursor(x, y); tft.print("LUMI");
      for(int i = 0; i < 8; i++) {
        tft.fillRect(random(0, 240), random(0, 280), random(20, 80), random(2, 6), ST77XX_BLACK);
        if(random(100) > 85) tft.fillRect(random(0, 240), random(0, 280), random(5, 30), random(1, 3), ST77XX_WHITE);
      }
      delay(120); 
      continue;
    }
    if (currentTime - lastMoveTime > nextMoveDelay) {
      targetXOffset = random(-30, 31); targetYOffset = random(-15, 16); 
      lastMoveTime = currentTime; nextMoveDelay = random(1500, 4000); 
    }
    if (!isBlinking && (currentTime - lastBlinkTime > nextBlinkDelay)) { isBlinking = true; }
    if (isBlinking) {
      eyeHeightScale -= 0.25; 
      if (eyeHeightScale <= 0.1) { eyeHeightScale = 0.1; isBlinking = false; lastBlinkTime = currentTime; nextBlinkDelay = random(2000, 6000); }
    } else {
      if (eyeHeightScale < 1.0) eyeHeightScale += 0.15; if (eyeHeightScale > 1.0) eyeHeightScale = 1.0; 
    }
    currentXOffset += (targetXOffset - currentXOffset) * 0.2; currentYOffset += (targetYOffset - currentYOffset) * 0.2;
    tft.fillScreen(ST77XX_BLACK);
    if (WiFi.status() == WL_CONNECTED) {
      dessinerIconeWifi(218, 8, 0x07FF);
    }
    int currentHeight = baseEyeHeight * eyeHeightScale;
    int currentY = baseY + currentYOffset + ((baseEyeHeight - currentHeight) / 2); 
    int lX = leftEyeBaseX + currentXOffset, rX = rightEyeBaseX + currentXOffset;
    int glitchOffset = 4;
    tft.fillRoundRect(lX - glitchOffset, currentY, baseEyeWidth, currentHeight, 20, 0x07FF); 
    tft.fillRoundRect(lX + glitchOffset, currentY, baseEyeWidth, currentHeight, 20, 0xF81F); 
    tft.fillRoundRect(lX, currentY, baseEyeWidth, currentHeight, 20, ST77XX_WHITE);          
    tft.fillRoundRect(rX - glitchOffset, currentY, baseEyeWidth, currentHeight, 20, 0x07FF);
    tft.fillRoundRect(rX + glitchOffset, currentY, baseEyeWidth, currentHeight, 20, 0xF81F);
    tft.fillRoundRect(rX, currentY, baseEyeWidth, currentHeight, 20, ST77XX_WHITE);
    int lIrisH = currentHeight - (irisOffset * 2), rIrisH = currentHeight - (irisOffset * 2), irisW = baseEyeWidth - (irisOffset * 2);
    if (lIrisH > 0) tft.fillRoundRect(lX + irisOffset, currentY + irisOffset, irisW, lIrisH, 8, ST77XX_BLACK); 
    if (rIrisH > 0) tft.fillRoundRect(rX + irisOffset, currentY + irisOffset, irisW, rIrisH, 8, ST77XX_BLACK); 
    
    if (etatEmotion == 10) {
       tft.fillCircle(120, 240, 8, 0x07FF); // Point cyan d'écoute active
    }
    
    if (etatEmotion == 5) { // Animation bouche
      int mouthY = 210, blockSize = 12, spacing = 3, rows = random(2, 4); 
      for (int r = 0; r < rows; r++) {
        int cols = 7 - (r * 2); 
        int startX = 120 - ((cols * (blockSize + spacing)) / 2); 
        for (int c = 0; c < cols; c++) {
          tft.fillRect(startX + c * (blockSize + spacing) - 2, mouthY + r * (blockSize + spacing), blockSize, blockSize, 0x07FF);
          tft.fillRect(startX + c * (blockSize + spacing), mouthY + r * (blockSize + spacing), blockSize, blockSize, ST77XX_WHITE);
        }
      }
    }
    delay(30); 
  }
}

// ==========================================
// CONFIGURATION I2S (MICROPHONE - CANAL 1)
// ==========================================
void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK_MIC,
    .ws_io_num = I2S_WS_MIC,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD_MIC
  };
  i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_1, &pin_config);
}

// ==========================================
// RECONNAISSANCE VOCALE (WIT.AI)
// ==========================================
String enregistrerEtEnvoyerAWitAi() {
  Serial.println("Enregistrement en cours (3 sec)...");
  
  if (!audioBuffer) { 
      Serial.println("Erreur: Buffer PSRAM non alloué !"); 
      return ""; 
  }

  size_t bytesRead = 0;
  i2s_read(I2S_NUM_1, audioBuffer, numSamples * sizeof(int16_t), &bytesRead, portMAX_DELAY);
  Serial.println("Envoi à Wit.ai...");
  
  WiFiClientSecure client;
  client.setInsecure(); // TODO (Prod) : Remplacer par client.setCACert(rootCACertificate);
  client.setTimeout(10000); 
  HTTPClient http;
  
  if (!http.begin(client, "https://api.wit.ai/speech?v=20230215")) {
    Serial.println("Erreur: Impossible de démarrer la connexion HTTPS");
    return "";
  }
  
  http.addHeader("Authorization", "Bearer " + witAiToken);
  http.addHeader("Content-Type", "audio/raw;encoding=signed-integer;bits=16;rate=16000;endian=little");
  
  int httpResponseCode = http.POST((uint8_t*)audioBuffer, bytesRead);
  String response = "";
  
  if (httpResponseCode == 200) { 
    response = http.getString(); 
    Serial.println("Réponse Wit.ai reçue !");
  } else { 
    Serial.println("Erreur Wit.ai : Code HTTP " + String(httpResponseCode)); 
    if (httpResponseCode < 0) {
       Serial.println(http.errorToString(httpResponseCode).c_str());
    }
  }
  
  http.end();

  String texteExtrait = "";
  if (response.length() > 0) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
        texteExtrait = doc["text"].as<String>();
        texteExtrait.toLowerCase();
    } else {
        Serial.println("Erreur de lecture du JSON Wit.ai");
    }
  }
  return texteExtrait;
}

// ==========================================
// INTELLIGENCE ARTIFICIELLE (GEMINI) + VOIX
// ==========================================
void poserQuestionAGemini(String question) {
  Serial.println("Demande à Gemini : " + question);
  
  WiFiClientSecure client;
  client.setInsecure(); // TODO (Prod) : client.setCACert(rootCACertificate);
  client.setTimeout(10000); 
  HTTPClient http;
  
  apiKey.trim(); 
  // Remplacement du modèle preview par une version stable (1.5-flash)
  String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + apiKey;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  String contexte = "Tu es Lumi, un robot de bureau avec un style visuel 'Glitch'. "
                    "Il fait actuellement " + String((int)currentTemp) + " degres dans la piece. "
                    "Tu dois repondre de maniere ultra-courte (1 a 2 phrases maximum). "
                    "L'humain te dit : ";

  // Utilisation sécurisée d'ArduinoJson pour la construction du Payload
  JsonDocument docPayload;
  docPayload["contents"][0]["parts"][0]["text"] = contexte + question;
  String payload;
  serializeJson(docPayload, payload);
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    JsonDocument doc;
    deserializeJson(doc, response);
    String answer = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
    
    answer.replace("*", "");
    Serial.println("Lumi répond : " + answer);
    
    etatEmotion = 5; 
    audio.connecttospeech(answer.c_str(), "fr"); 
    
  } else {
    Serial.println("Erreur Gemini : " + String(httpResponseCode));
    Serial.println("Raison du refus : " + http.getString()); 
    etatEmotion = 0;
  }
  http.end();
}

// ==========================================
// WIFI & SETUP
// ==========================================
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Portail activé !");
  etatEmotion = 7; 
}

void setup() {
  Serial.begin(115200);
  
  // Allocation du buffer audio dans la PSRAM au démarrage
  audioBuffer = (int16_t*)ps_malloc(numSamples * sizeof(int16_t));
  
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(240, 280); tft.setSPISpeed(40000000); tft.setRotation(1); 
  
  xTaskCreatePinnedToCore(CodeTacheAffichage, "Affichage", 10000, NULL, 1, &TacheAffichageHandle, 0);
  
  audio.setPinout(I2S_BCLK_AMP, I2S_LRC_AMP, I2S_DIN_AMP);
  audio.setVolume(15); 
  
  preferences.begin("lumi_config", false);
  apiKey = preferences.getString("gemini_key", "");
  witAiToken = preferences.getString("wit_key", "");
  pinMode(0, INPUT_PULLUP);
  
  WiFiManagerParameter custom_gemini_key("gemini", "Cle API Gemini", apiKey.c_str(), 100);
  WiFiManagerParameter custom_wit_key("wit", "Token Wit.ai", witAiToken.c_str(), 60);
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  wm.addParameter(&custom_gemini_key);
  wm.addParameter(&custom_wit_key);
  
  bool connected;
  if (digitalRead(0) == LOW) {
    Serial.println("Bouton BOOT maintenu -> Ouverture forcée du portail");
    connected = wm.startConfigPortal("LUMI_CONFIGURATION");
  } else if (apiKey == "" || witAiToken == "") {
    connected = wm.startConfigPortal("LUMI_CONFIGURATION");
  } else {
    connected = wm.autoConnect("LUMI_CONFIGURATION");
  }
  if (connected) {
    etatEmotion = 0; 
  }
  
  String newGeminiKey = custom_gemini_key.getValue();
  String newWitKey = custom_wit_key.getValue();
  if (newGeminiKey != "") {
    apiKey = newGeminiKey;
    preferences.putString("gemini_key", apiKey);
  }
  if (newWitKey != "") {
    witAiToken = newWitKey;
    preferences.putString("wit_key", witAiToken);
  }
  
  // Initialisation I2C pour BME280 ET MPU6050
  Wire.begin(8, 9);
  if (bme.begin(0x76)) bmeActif = true;
  if (mpu.begin()) {
    mpuActif = true;
    Serial.println("MPU6050 Initialisé avec succès");
  }
  
  setupI2S();
}

// ==========================================
// CŒUR 1 : BOUCLE PRINCIPALE
// ==========================================
void loop() {
  audio.loop();
  
  if (etatEmotion == 5 && !audio.isRunning()) {
    etatEmotion = 0;
  }
  
  if (!audio.isRunning()) {
    unsigned long currentMillis = millis();
    
    // Mise à jour des capteurs
    if (bmeActif && (currentMillis - lastBMEReadTime > bmeCheckInterval)) {
      currentTemp = bme.readTemperature();
      lastBMEReadTime = currentMillis;
    }
    
    // Détection de mouvement basique si MPU actif
    if (mpuActif) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        // La logique d'impact/secousse pourra être ajoutée ici
    }
    
    size_t bytesRead = 0;
    int16_t sampleBuffer[256];
    i2s_read(I2S_NUM_1, &sampleBuffer, sizeof(sampleBuffer), &bytesRead, portMAX_DELAY);
    
    const size_t sampleCount = bytesRead / sizeof(sampleBuffer[0]);
    int average = 0;
    if (sampleCount > 0) {
      int32_t sum = 0;
      for (size_t i = 0; i < sampleCount; i++) {
        sum += abs(sampleBuffer[i]);
      }
      average = sum / sampleCount;
    }
    
    // Déclenchement sonore avec filtre anti-rebond
    if (average > SEUIL_BRUIT_REVEIL && (currentMillis - lastTriggerTime > DEBOUNCE_AUDIO_MS)) {
      lastTriggerTime = currentMillis;
      Serial.println("Bruit fort détecté !");
      etatEmotion = 10; 
      
      String texteEntendu = enregistrerEtEnvoyerAWitAi();
      Serial.println("Tu as dit : " + texteEntendu);
      
      // Règle de conception : Mot de réveil strict "lumi" exigé (pas de préfixes)
      if (texteEntendu == "lumi") {
        poserQuestionAGemini(texteEntendu);
      } else {
        etatEmotion = 0;
      }
    }
  }
  delay(1);
}