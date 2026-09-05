#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Sensor ultrassônico
const int pin_echo = 15;
const int pin_trig = 2;

// Sensores infravermelhos
const int sensor_direita  = 23;
const int sensor_esquerda = 13;

// Configuração PWM
const int frequenciaPWM = 5000;
const int resolucaoPWM  = 8;
const int velocidade    = 255;
const int parado        = 0;

// PINAGEM DOS MOTORES
const int roda_DirFrente_Go   = 21; 
const intf roda_DirFrente_Back = 22; 

const int roda_DirTras_Go     = 19; 
const int roda_DirTras_Back   = 18; 

const int roda_EsqFrente_Go   = 16;  
const int roda_EsqFrente_Back = 4; 

const int roda_EsqTras_Go     = 17; 
const int roda_EsqTras_Back   = 5;  

bool modoAutonomo = false;
unsigned long tempoUltimaLeitura = 0;
long distanciaAtual = 0;

void parar() {
    ledcWrite(roda_DirFrente_Go, parado);
    ledcWrite(roda_DirFrente_Back, parado);
    ledcWrite(roda_DirTras_Go, parado);
    ledcWrite(roda_DirTras_Back, parado);

    ledcWrite(roda_EsqFrente_Go, parado);
    ledcWrite(roda_EsqFrente_Back, parado);
    ledcWrite(roda_EsqTras_Go, parado);
    ledcWrite(roda_EsqTras_Back, parado);
}

// INVERSÃO CORRIGIDA: Invertido _Go e _Back de todos os motores
void ir_frente() {
    parar();
    ledcWrite(roda_DirFrente_Back, velocidade);
    ledcWrite(roda_DirTras_Back, velocidade);
    ledcWrite(roda_EsqFrente_Back, velocidade);
    ledcWrite(roda_EsqTras_Back, velocidade); // Garante acionamento da roda esquerda traseira
}

void ir_tras() {
    parar();
    ledcWrite(roda_DirFrente_Go, velocidade);
    ledcWrite(roda_DirTras_Go, velocidade);
    ledcWrite(roda_EsqFrente_Go, velocidade);
    ledcWrite(roda_EsqTras_Go, velocidade);
}

// INVERSÃO CORRIGIDA: Troca de lado nas curvas para corrigir rotação
void girar_esquerda() {
    parar();
    // Lado ESQUERDO recua / Lado DIREITO avança
    ledcWrite(roda_EsqFrente_Go, velocidade);
    ledcWrite(roda_EsqTras_Go, velocidade);
    ledcWrite(roda_DirFrente_Back, velocidade);
    ledcWrite(roda_DirTras_Back, velocidade);
}

void girar_direita() {
    parar();
    // Lado DIREITO recua / Lado ESQUERDO avança
    ledcWrite(roda_DirFrente_Go, velocidade);
    ledcWrite(roda_DirTras_Go, velocidade);
    ledcWrite(roda_EsqFrente_Back, velocidade);
    ledcWrite(roda_EsqTras_Back, velocidade);
}

void setup() {
    Serial.begin(115200);

    int pinosMotores[] = {22, 21, 18, 19, 4, 16, 17, 5};
    for (int i = 0; i < 8; i++) {
        pinMode(pinosMotores[i], OUTPUT);
        digitalWrite(pinosMotores[i], LOW);
    }

    ledcAttach(roda_DirFrente_Go, frequenciaPWM, resolucaoPWM);
    ledcAttach(roda_DirFrente_Back, frequenciaPWM, resolucaoPWM);
    ledcAttach(roda_DirTras_Go, frequenciaPWM, resolucaoPWM);
    ledcAttach(roda_DirTras_Back, frequenciaPWM, resolucaoPWM);

    ledcAttach(roda_EsqFrente_Go, frequenciaPWM, resolucaoPWM);
    ledcAttach(roda_EsqFrente_Back, frequenciaPWM, resolucaoPWM);
    ledcAttach(roda_EsqTras_Go, frequenciaPWM, resolucaoPWM);
    ledcAttach(roda_EsqTras_Back, frequenciaPWM, resolucaoPWM);

    parar();

    pinMode(sensor_direita, INPUT);
    pinMode(sensor_esquerda, INPUT);
    pinMode(pin_echo, INPUT);
    pinMode(pin_trig, OUTPUT);

    SerialBT.begin("Megaton_Robo_ESP32");
}

int distancia_inimigo() {
    unsigned long agora = millis();
    if (agora - tempoUltimaLeitura >= 50) {
        tempoUltimaLeitura = agora;
        digitalWrite(pin_trig, LOW);
        delayMicroseconds(2);
        digitalWrite(pin_trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(pin_trig, LOW);

        long duracao = pulseIn(pin_echo, HIGH, 25000);
        distanciaAtual = (duracao == 0) ? 999 : (duracao * 0.034 / 2);
    }
    return distanciaAtual;
}

void rotina_autonoma() {
    int direita = digitalRead(sensor_direita);
    int esquerda = digitalRead(sensor_esquerda);
    long distancia = distancia_inimigo();

    if (esquerda == 0 || direita == 0) {
        ir_tras();
        delay(400);
        girar_direita();
        delay(200);
    } else if (distancia <= 60) {
        ir_frente();
    } else {
        girar_direita();
    }
}

void loop() {
    if (SerialBT.available()) {
        char comando = SerialBT.read();

        if (comando == 'F') { modoAutonomo = false; ir_frente(); }
        else if (comando == 'B') { modoAutonomo = false; ir_tras(); }
        else if (comando == 'L') { modoAutonomo = false; girar_esquerda(); }
        else if (comando == 'R') { modoAutonomo = false; girar_direita(); }
        else if (comando == 'S') { modoAutonomo = false; parar(); }
        else if (comando == 'X' || comando == 'W') { modoAutonomo = true; }
    }

    if (modoAutonomo) {
        rotina_autonoma();
    }
}
