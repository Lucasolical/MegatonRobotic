#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Configuração PWM
const int frequenciaPWM = 5000;
const int resolucaoPWM  = 8;
const int velocidade    = 255;
const int parado        = 0;

// PINAGEM DOS MOTORES
const int roda_DirFrente_Go   = 21;
const int roda_DirFrente_Back = 22;

const int roda_DirTras_Go     = 19;
const int roda_DirTras_Back   = 18;

const int roda_EsqFrente_Go   = 16;
const int roda_EsqFrente_Back = 4;

const int roda_EsqTras_Go     = 17;
const int roda_EsqTras_Back   = 5;

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

// Frente
void ir_frente() {
    parar();

    ledcWrite(roda_DirFrente_Back, velocidade);
    ledcWrite(roda_DirTras_Back, velocidade);
    ledcWrite(roda_EsqFrente_Back, velocidade);
    ledcWrite(roda_EsqTras_Back, velocidade);
}

// Trás
void ir_tras() {
    parar();

    ledcWrite(roda_DirFrente_Go, velocidade);
    ledcWrite(roda_DirTras_Go, velocidade);
    ledcWrite(roda_EsqFrente_Go, velocidade);
    ledcWrite(roda_EsqTras_Go, velocidade);
}

// Girar para esquerda
void girar_esquerda() {
    parar();

    ledcWrite(roda_EsqFrente_Go, velocidade);
    ledcWrite(roda_EsqTras_Go, velocidade);

    ledcWrite(roda_DirFrente_Back, velocidade);
    ledcWrite(roda_DirTras_Back, velocidade);
}

// Girar para direita
void girar_direita() {
    parar();

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

    SerialBT.begin("Megaton_Robo_ESP32");
}

void loop() {
    if (SerialBT.available()) {
        char comando = SerialBT.read();

        if (comando == 'F') {
            ir_frente();
        }

        else if (comando == 'B') {
            ir_tras();
        }

        else if (comando == 'L') {
            girar_esquerda();
        }

        else if (comando == 'R') {
            girar_direita();
        }

        else if (comando == 'S') {
            parar();
        }
    }
}
