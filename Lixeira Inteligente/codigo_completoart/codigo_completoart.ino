#include <MQTT.h>
#include <MQTTClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ultrasonic.h>

// Definição dos pinos
#define trigPin 33      // Pino TRIG do HC-SR04
#define echoPin 35      // Pino ECHO do HC-SR04
#define buzzerPin 25     // Pino do buzzer
#define ledPositivo 26   // LED positivo (distante)
#define ledNegativo 32   // LED negativo (perto)
#define chaveMestre 15  // Pino da chave mestre

// Configurações do WiFi
const char* ssid = "Teste";  // Substitua pelo seu SSID
const char* password = "eduw8101";      // Substitua pela sua senha

// Configurações do MQTT
const char* mqttServer = 192.168.114.167;  // Substitua pelo IP do seu broker
const int mqttPort = 1883;
const char* mqttTopic = "lixeira/status";

WiFiClient wifiClient;              // Cria um cliente WiFi
PubSubClient mqttClient(wifiClient);  // Cria um cliente MQTT

// Variáveis
long duracao;
float distancia;
float capacidadeCheia = 20.0;            // Distância em cm quando o lixo está cheio
float limite80 = capacidadeCheia * 0.8;  // 80% da capacidade cheia
float limiteSilencio = 30.0;             // Limite para não emitir som

void setup() {
  // Inicializa comunicação serial
  Serial.begin(115200);

  // Conexão com o WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  // Configura o cliente MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  

  // Configura os pinos
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPositivo, OUTPUT);
  pinMode(ledNegativo, OUTPUT);
}

void reconnect() {
  // Loop até se conectar
  while (!mqttClient.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    // Tente conectar
    if (mqttClient.connect("ESP32Client")) { // Use um ID de cliente exclusivo
      Serial.println("conectado");
    } else {
      Serial.print("falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando novamente em 2 segundos");
      delay(2000);
    }
  }
}

// Função para calcular a distância usando o sensor ultrassônico
void distancias() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Calcula o tempo do pulso ECHO
    duracao = pulseIn(echoPin, HIGH);
    
    // Calcula a distância (em cm)
    distancia = (duracao * 0.0343) / 2;

    // Exibe a distância no monitor serial
    Serial.print("Distância: ");
    Serial.println(distancia);
}

void loop() {
  // Certifique-se de que o cliente MQTT esteja conectado
  if (!mqttClient.connected()) {
    reconnect(); // Conecta se não estiver
  }
  mqttClient.loop(); // Mantenha o cliente MQTT ativo

  // Chama a função para calcular a distância
  distancias();

  // Se a distância for maior que 15 cm, desliga o som e os LEDs
  if (distancia > limiteSilencio) {
    digitalWrite(ledPositivo, LOW);  // Apaga LED positivo
    digitalWrite(ledNegativo, LOW);  // Apaga LED negativo
    noTone(buzzerPin);               // Não emite som
    Serial.println("Distância maior que 15 cm. Nenhum LED aceso.");
  }
  // Se a distância for maior que 80% da capacidade cheia, mas menor que 15 cm
  else if (distancia > limite80 && distancia <= limiteSilencio) {
    digitalWrite(ledPositivo, HIGH);  // Acende LED positivo (objeto distante)
    digitalWrite(ledNegativo, LOW);   // Apaga LED negativo
    tone(buzzerPin, 1000);            // Som fraco (1000 Hz)
    Serial.println("LED Positivo aceso (distante).");
  }
  // Se o objeto estiver perto (menor ou igual a 80% da capacidade cheia)
  else if (distancia <= limite80) {
    digitalWrite(ledNegativo, HIGH);  // Acende LED negativo (objeto perto)
    digitalWrite(ledPositivo, LOW);   // Apaga LED positivo
    tone(buzzerPin, 500);             // Som forte (500 Hz)
    Serial.println("LED Negativo aceso (perto).");
  }

  // Publica a distância no tópico MQTT
  String mensagem = "Distância: " + String(distancia);
  mqttClient.publish(mqttTopic, mensagem.c_str());

  delay(1000);        // Mantém o som por 1 segundo
  noTone(buzzerPin);  // Para o buzzer

  delay(500);  // Delay para estabilidade
}
