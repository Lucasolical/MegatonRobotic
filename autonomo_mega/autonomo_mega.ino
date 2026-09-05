unsigned long tempoUltimaLeitura = 0;
const unsigned long intervaloLeitura = 50; // 200 é como tava antes
long distanciaAtual = 999; // valor padrão distante

//Sensor ultrassônico:
const int pin_echo = 15; // INPUT
const int pin_trig = 2; // OUTPUT

// Sensores infravermelhos:
const int sensor_direita = 23;
const int sensor_esquerda = 13;

// Valores PwM utilizada no motor:
const int velocidade = 255;
const int parado = 0;

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
    // Configuração dos canais PWM
    pinMode(motorRightFront_Go, OUTPUT);
    pinMode(motorLeftFront_Go, OUTPUT);
    pinMode(motorRightBack_Go, OUTPUT);
    pinMode(motorLeftBack_Go, OUTPUT);

    pinMode(motorRightFront_Back, OUTPUT);
    pinMode(motorLeftFront_Back, OUTPUT);
    pinMode(motorRightBack_Back, OUTPUT);
    pinMode(motorLeftBack_Back, OUTPUT);


    // Sensores:
    Serial.begin(9600);
    pinMode(sensor_direita, INPUT);
    pinMode(sensor_esquerda, INPUT);
    pinMode(pin_echo, INPUT);
    pinMode(pin_trig, OUTPUT);
} 


long distancia_inimigo() 
{
    unsigned long agora = millis();

    if (agora - tempoUltimaLeitura >= intervaloLeitura) // Executa a leitura apenas se o tempo de intervalo tiver passado
    {
        tempoUltimaLeitura = agora;

        // Pulso do ultrassônico
        digitalWrite(pin_trig, LOW);
        delayMicroseconds(2);
        digitalWrite(pin_trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(pin_trig, LOW);

        long duracao = pulseIn(pin_echo, HIGH, 30000); // Mede o tempo de resposta

        if (duracao == 0) // Se der timeout, não há nada no alcance
        {
            distanciaAtual = 999; // aq ele manda girar dnv pra procurar
        } 
        else 
        {
            distanciaAtual = duracao * 0.034 / 2; // Calcula a distância em cm
        }
    }

    return distanciaAtual;
}

void ir_frente() {
    analogWrite(motorRightFront_Go, velocidade);
    analogWrite(motorLeftFront_Go, velocidade);
    analogWrite(motorRightBack_Go, velocidade);
    analogWrite(motorLeftBack_Go, velocidade);

    analogWrite(motorRightFront_Back, parado);
    analogWrite(motorLeftFront_Back, parado);
    analogWrite(motorRightBack_Back, parado);
    analogWrite(motorLeftBack_Back, parado);
}

void ir_tras() {
    analogWrite(motorRightFront_Go, parado);
    analogWrite(motorLeftFront_Go, parado);
    analogWrite(motorRightBack_Go, parado);
    analogWrite(motorLeftBack_Go, parado);

    analogWrite(motorRightFront_Back, velocidade);
    analogWrite(motorLeftFront_Back, velocidade);
    analogWrite(motorRightBack_Back, velocidade);
    analogWrite(motorLeftBack_Back, velocidade);
}

void girar_direita() {
    analogWrite(motorRightFront_Go, parado);
    analogWrite(motorLeftFront_Go, velocidade);
    analogWrite(motorRightBack_Go, parado);
    analogWrite(motorLeftBack_Go, velocidade);

    analogWrite(motorRightFront_Back, velocidade);
    analogWrite(motorLeftFront_Back, parado);
    analogWrite(motorRightBack_Back, velocidade);
    analogWrite(motorLeftBack_Back, parado);
}

void girar_esquerda() {
    analogWrite(motorRightFront_Go, velocidade);
    analogWrite(motorLeftFront_Go, parado);
    analogWrite(motorRightBack_Go, velocidade);
    analogWrite(motorLeftBack_Go, parado);

    analogWrite(motorRightFront_Back, parado);
    analogWrite(motorLeftFront_Back, velocidade);
    analogWrite(motorRightBack_Back, parado);
    analogWrite(motorLeftBack_Back, velocidade);
}

void loop() {
    int direita = digitalRead(sensor_direita);
    int esquerda = digitalRead(sensor_esquerda);
    long distancia = distancia_inimigo();

    if (esquerda == 0 && direita == 0) {
        ir_tras();
        delay(400);
    } 

    else if (esquerda == 0) {  // detectou a borda pela esquerda e vai avançar pela direira pra o adversario cair
        girar_direita();
        delay(150);
        
        ir_frente();
        delay(250);
        
        // Verifica se ainda está no limite após o ataque antes de recuar
        if (digitalRead(sensor_esquerda) == 0 || digitalRead(sensor_direita) == 0) { //recua pq chegou na borda
            ir_tras();
            delay(300);
        }
    } 

    else if (direita == 0) {  // detectou a borda pela direita e vai avançar pela esquerda pra o adversario cair
        girar_esquerda();
        delay(150);
        
        ir_frente();
        delay(250);
        
        if (digitalRead(sensor_esquerda) == 0 || digitalRead(sensor_direita) == 0) { //recua pq chegou na borda
            ir_tras();
            delay(300);
        }
    } 
    
    else { // busca e ataque normal
        if (distancia > 0 && distancia <= 60) {
            ir_frente(); // ataca
        } else {
            girar_direita(); // procura
        }
    }
}
