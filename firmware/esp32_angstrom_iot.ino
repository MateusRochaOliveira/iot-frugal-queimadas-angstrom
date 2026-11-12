#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// ==========================================
// 1. CONFIGURAÇÕES DE HARDWARE E REDE
// ==========================================
#define DHTPIN 23          // Pino GPIO 23 conforme esquemático do projeto
#define DHTTYPE DHT11      // Sensor DHT11

// Credenciais Wi-Fi (altere na Arduino IDE antes de gravar no ESP32)
const char* ssid = "NOME_DA_SUA_REDE_WIFI";
const char* password = "SENHA_DA_SUA_REDE_WIFI";

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);      // Servidor HTTP na porta 80 para leitura sem fio

// Variáveis globais do estado atual
float temperatura = 0.0;
float umidade = 0.0;
float indiceAngstrom = 0.0;
String classificacaoRisco = "AGUARDANDO";
int codigoRisco = 0; // 1: Baixo, 2: Moderado, 3: Alto, 4: Muito Alto/Extremo

// Controle de intervalo não-bloqueante (leitura a cada 2 segundos)
unsigned long ultimoTempoLeitura = 0;
const unsigned long intervaloLeitura = 2000;

// ==========================================
// 2. CLASSIFICAÇÃO DO ÍNDICE DE ÅNGSTRÖM
// ==========================================
void calcularClassificacao(float I) {
  if (I > 4.0) {
    classificacaoRisco = "IMPROVAVEL / BAIXO";
    codigoRisco = 1;
  } else if (I > 2.5 && I <= 4.0) {
    classificacaoRisco = "MODERADO";
    codigoRisco = 2;
  } else if (I > 2.0 && I <= 2.5) {
    classificacaoRisco = "ALTO";
    codigoRisco = 3;
  } else {
    classificacaoRisco = "MUITO ALTO / EXTREMO";
    codigoRisco = 4;
  }
}

// ==========================================
// 3. PAYLOAD JSON PADRONIZADO (SERIAL E WI-FI)
// ==========================================
String gerarJSON() {
  String json = "{";
  json += "\"local\":\"Santa Barbara - MG\",";
  json += "\"tempo_ms\":" + String(millis()) + ",";
  json += "\"temperatura\":" + String(temperatura, 1) + ",";
  json += "\"umidade\":" + String(umidade, 1) + ",";
  json += "\"angstrom\":" + String(indiceAngstrom, 2) + ",";
  json += "\"risco\":\"" + classificacaoRisco + "\",";
  json += "\"nivel\":" + String(codigoRisco) + ",";
  json += "\"wifi_rssi\":" + String(WiFi.RSSI());
  json += "}";
  return json;
}

// Endpoint HTTP (/dados) para leitura sem fio pelo PWA
void handleDados() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", gerarJSON());
}

// ==========================================
// 4. SETUP INICIAL
// ==========================================
void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.println("{\"status\":\"Iniciando no sensor IoT frugal - Santa Barbara MG\"}");

  // Tenta conectar por até 10s; se não houver rede, segue autônomo offline
  unsigned long inicioTentativa = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicioTentativa < 10000) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("{\"wifi\":\"Conectado\",\"ip\":\"");
    Serial.print(WiFi.localIP());
    Serial.println("\"}");
  } else {
    Serial.println("{\"wifi\":\"Offline - Operando em modo autonomo local\"}");
  }

  server.on("/dados", HTTP_GET, handleDados);
  server.begin();
}

// ==========================================
// 5. LOOP PRINCIPAL (EDGE COMPUTING)
// ==========================================
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }

  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoTempoLeitura >= intervaloLeitura) {
    ultimoTempoLeitura = tempoAtual;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("{\"erro\":\"Falha na leitura do sensor DHT11 no GPIO 23\"}");
      return;
    }

    umidade = h;
    temperatura = t;

    // Cálculo determinístico embarcado: I = (R / 20) + ((27 - T) / 10)
    indiceAngstrom = (umidade / 20.0) + ((27.0 - temperatura) / 10.0);
    calcularClassificacao(indiceAngstrom);

    // Envio pela porta Serial (compatível com Web Serial API do PWA)
    Serial.println(gerarJSON());
  }
}
