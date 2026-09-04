
unsigned long tempoUltimaLeitura = 0;
const unsigned long intervaloLeitura = 200; // se precisar testa com 500 ms
long distanciaAtual = 0;

//Sensor ultrassônico:
const int pin_echo = 15; // INPUT
const int pin_trig = 2; // OUTPUT

// Sensores infravermelhos:
const int sensor_direita = 23;
const int sensor_esquerda = 13;

// Valores PwM utilizada no motor:
const int velocidade = 255;
const int parado = 0;
// const int semivelocidade = 100;

// Motores que vão para frente:
const int motorRightFront_Go = 22;
const int motorLeftFront_Go = 4;
const int motorRightBack_Go = 18;
const int motorLeftBack_Go = 5;

// Motores que vão para trás:
const int motorRightFront_Back = 21;
const int motorLeftFront_Back = 16;
const int motorRightBack_Back = 19;
const int motorLeftBack_Back = 17;


void setup() {
// Criação dos canais em seus respectivos pinos:
    ledcAttach(motorRightFront_Go, 5000, 8);
    ledcAttach(motorLeftFront_Go, 5000, 8);
    ledcAttach(motorRightBack_Go, 5000, 8);
    ledcAttach(motorLeftBack_Go, 5000, 8);

    ledcAttach(motorRightFront_Back, 5000, 8);
    ledcAttach(motorLeftFront_Back, 5000, 8);
    ledcAttach(motorRightBack_Back, 5000, 8);
    ledcAttach(motorLeftBack_Back, 5000, 8);


    // Sensores:
    Serial.begin(9600);
    pinMode(sensor_direita, INPUT);
    pinMode(sensor_esquerda, INPUT);
    pinMode(pin_echo, INPUT);
    pinMode(pin_trig, OUTPUT);
} 

int distancia_inimigo() 
{
    unsigned long agora = millis();
    if (agora - tempoUltimaLeitura >= intervaloLeitura){
        tempoUltimaLeitura = agora;
        digitalWrite(pin_trig, LOW);
        delayMicroseconds(2);
        digitalWrite(pin_trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(pin_trig, LOW);
        long duracao = pulseIn(pin_echo, HIGH, 30000); // timeout de 30ms
        distanciaAtual = duracao * 0.034 / 2;
        return distanciaAtual;
    }
}
 
void ir_frente() {
    ledcWrite(0, velocidade);
    ledcWrite(1, velocidade);
    ledcWrite(2, velocidade);
    ledcWrite(3, velocidade);

    ledcWrite(4, parado);
    ledcWrite(5, parado);
    ledcWrite(6, parado);
    ledcWrite(7, parado);
}

void ir_tras() {
    ledcWrite(0, parado);
    ledcWrite(1, parado);
    ledcWrite(2, parado);
    ledcWrite(3, parado);

    ledcWrite(4, velocidade);
    ledcWrite(5, velocidade);
    ledcWrite(6, velocidade);
    ledcWrite(7, velocidade);
}

void girar_direita() {
    ledcWrite(0, parado);
    ledcWrite(1, velocidade);
    ledcWrite(2, parado);
    ledcWrite(3, velocidade);

    ledcWrite(4, velocidade);
    ledcWrite(5, parado);
    ledcWrite(6, velocidade);
    ledcWrite(7, parado);
}

void girar_esquerda() {
    ledcWrite(0, velocidade);
    ledcWrite(1, parado);
    ledcWrite(2, velocidade);
    ledcWrite(3, parado);

    ledcWrite(4, parado);
    ledcWrite(5, velocidade);
    ledcWrite(6, parado);
    ledcWrite(7, velocidade);
}

void loop()
{
    int direita = digitalRead(sensor_direita);
    int esquerda = digitalRead(sensor_esquerda);
    long tempo = 0;
    long distancia = distancia_inimigo();
    
    if (esquerda == 0) // detectou a borda pela esquerda e vai avançar pela direira pra o adversario cair
    { 
        girar_direita();
        ir_frente();
        
    }
    else if (direita == 0) // detectou a borda pela direita e vai avançar pela esquerda pra o adversario cair
    { 
        girar_esquerda();
        ir_frente();
        
    }
    else if (esquerda == 0 && direita == 0) // detectou ambas as bordas e vai recuar pra nao sair
    {
        ir_tras();
    }
    
    
    if (distancia <= 60) // (distancia >0 && distancia <= 60) // sugeriram pra gnt se com esse limite no 0 ia funcionar bem tbm
    {
        ir_frente();
    }
    girar_direita();
}
