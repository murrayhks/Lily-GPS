#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Modulos
#define GPS_RX 4
#define GPS_TX 5

// Pinos para vibrar
#define MOTOR_ESQUERDO 6
#define MOTOR_DIREITO 7
#define LED_ESQUERDO 8
#define LED_DIREITO 9

SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;
const double DESTINO_LAT = -23.5505;  // Lat.
const double DESTINO_LON = -46.6333;  // Long.

// Distância mínima para considerar a chegada ao destino (em metros)
const double DISTANCIA_MINIMA = 10.0;

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(9600);
  gpsSerial.begin(9600);

  // Configura os pinos dos motores e LEDs como saída
  pinMode(MOTOR_ESQUERDO, OUTPUT);
  pinMode(MOTOR_DIREITO, OUTPUT);
  pinMode(LED_ESQUERDO, OUTPUT);
  pinMode(LED_DIREITO, OUTPUT);

  Serial.println("O SISTEMA DE NAVEGAÇÃO POR GPS FOR ATIVADO ! ");
}

void loop() {
  // Processa os dados
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // Verifica se o GPS tem uma localização validada
  if (gps.location.isValid()) {
    double latitudeAtual = gps.location.lat();
    double longitudeAtual = gps.location.lng();

    // Calcula a distância
    double distancia = calcularDistancia(latitudeAtual, longitudeAtual, DESTINO_LAT, DESTINO_LON);

    // Verifica a chegada no desino
    if (distancia <= DISTANCIA_MINIMA) {
      indicarChegada();
    } else {
      // Calcula a direção para o destino
      double direcao = calcularDirecao(latitudeAtual, longitudeAtual, DESTINO_LAT, DESTINO_LON);

      // Indica a direção com os sensores de led.
      indicarDirecao(direcao);
    }
  } else {
    Serial.println("Aguardando sinal do GPS...");
  }

  delay(1000);  //Re-verifica após 1 segund.
}

// Função para calcular a distância até o destino (em metros)
double calcularDistancia(double lat1, double lon1, double lat2, double lon2) {
  const double RAIO_TERRA = 6371000;  // Raio da terra

  // Converte as coordenadas de graus para radianos
  double lat1Rad = radians(lat1);
  double lon1Rad = radians(lon1);
  double lat2Rad = radians(lat2);
  double lon2Rad = radians(lon2);

  // Diferença entre as coordenadas
  double deltaLat = lat2Rad - lat1Rad;
  double deltaLon = lon2Rad - lon1Rad;

  // haversine
  double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
             cos(lat1Rad) * cos(lat2Rad) *
             sin(deltaLon / 2) * sin(deltaLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  // Distância em metros
  double distancia = RAIO_TERRA * c;
  return distancia;
}

// Função para calcular a direção para o destino (em graus)
double calcularDirecao(double lat1, double lon1, double lat2, double lon2) {
  double deltaLon = radians(lon2 - lon1);
  double lat1Rad = radians(lat1);
  double lat2Rad = radians(lat2);

  // Fórmula para calcular o ângulo
  double y = sin(deltaLon) * cos(lat2Rad);
  double x = cos(lat1Rad) * sin(lat2Rad) -
             sin(lat1Rad) * cos(lat2Rad) * cos(deltaLon);
  double angulo = atan2(y, x);

  // Converte o ângulo para graus e ajusta o intervalo [0, 360]
  double direcao = degrees(angulo);
  if (direcao < 0) {
    direcao += 360;
  }

  return direcao;
}

// Função para indicar a direção com os motores
void indicarDirecao(double direcao) {
  // Limites
  if (direcao >= 45 && direcao < 135) {
    // Direita
    digitalWrite(MOTOR_DIREITO, HIGH);
    digitalWrite(MOTOR_ESQUERDO, LOW);
    digitalWrite(LED_DIREITO, HIGH);
    digitalWrite(LED_ESQUERDO, LOW);
    Serial.println("VIRE PARA A DIREITA");
  } else if (direcao >= 225 && direcao < 315) {
    //Esquerda
    digitalWrite(MOTOR_ESQUERDO, HIGH);
    digitalWrite(MOTOR_DIREITO, LOW);
    digitalWrite(LED_ESQUERDO, HIGH);
    digitalWrite(LED_DIREITO, LOW);
    Serial.println("Vire à esquerda");
  } else {
    // Seguir em frente
    digitalWrite(MOTOR_ESQUERDO, LOW);
    digitalWrite(MOTOR_DIREITO, LOW);
    digitalWrite(LED_ESQUERDO, LOW);
    digitalWrite(LED_DIREITO, LOW);
    Serial.println("Siga em frente");
  }
}

// Função para indicar que chegou ao destino
void indicarChegada() {
  // AtivaÇão dos motores e LEDs
  digitalWrite(MOTOR_ESQUERDO, HIGH);
  digitalWrite(MOTOR_DIREITO, HIGH);
  digitalWrite(LED_ESQUERDO, HIGH);
  digitalWrite(LED_DIREITO, HIGH);
  Serial.println("Você chegou ao destino!");
}
