#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial serialdobluetooth(8, 9);  // RX, TX do Bluetooth

#define pinESC1 4
#define pinESC2 5
#define pinESC3 6
#define pinESC4 7

#define velMin 34
#define velMax 154

int velAtH = velMin;
int veldisF, veldisT;
int veldirE, veldirD;
bool continuarIncrementando = false;
bool continuarDecrementando = false;
bool conectado = true; // <- estado de conexão Bluetooth

Servo esc1, esc2, esc3, esc4;
char valordobluetooth;

void setup() {
  esc1.attach(pinESC1);
  esc2.attach(pinESC2);
  esc3.attach(pinESC3);
  esc4.attach(pinESC4);
  serialdobluetooth.begin(9600);
  Serial.begin(9600);

  // Calibração inicial
  esc1.write(velMax);
  esc2.write(velMax);
  esc3.write(velMax);
  esc4.write(velMax);
  delay(5000);
  esc1.write(velMin);
  esc2.write(velMin);
  esc3.write(velMin);
  esc4.write(velMin);
  delay(5000);
  Serial.println("Drone Calibrado");
}

void loop() {
  if (serialdobluetooth.available()) {
    valordobluetooth = serialdobluetooth.read();

    if (valordobluetooth == 'Q') {
      // Desacelera até 34 e desativa comandos
      while (velAtH > velMin) {
        velAtH--;
        esc1.write(velAtH); esc2.write(velAtH);
        esc3.write(velAtH); esc4.write(velAtH);
        Serial.println(velAtH);
        delay(50);
      }
      conectado = false; // Desativa controle
      Serial.println("Bluetooth desconectado, comandos desativados.");
    }

    else if (valordobluetooth == 'R') {
      conectado = true; // Reativa controle
      Serial.println("Reconectado. Comandos reativados.");
    }

    // Comandos só funcionam se estiver conectado
    else if (conectado) {
      if (valordobluetooth == 'C') { continuarIncrementando = true; LC(); }
      else if (valordobluetooth == 'B') { continuarDecrementando = true; LB(); }

      else if (valordobluetooth == 'f' || valordobluetooth == 't' ||
               valordobluetooth == 'e' || valordobluetooth == 'd' ||
               valordobluetooth == 'c' || valordobluetooth == 'b') {
        continuarIncrementando = false;
        continuarDecrementando = false;
        SCBEDFT();
      }

      else if (valordobluetooth == 'E') LE();
      else if (valordobluetooth == 'D') LD();
      else if (valordobluetooth == 'F') LF();
      else if (valordobluetooth == 'T') LT();
    }
  }
}

// Incrementar velocidade
void LC() {
  while (velAtH < velMax && continuarIncrementando) {
    velAtH++;
    esc1.write(velAtH); esc2.write(velAtH);
    esc3.write(velAtH); esc4.write(velAtH);
    Serial.println(velAtH);
    delay(50);

    if (serialdobluetooth.available()) {
      char novoComando = serialdobluetooth.read();
      if (ehComandoDirecional(novoComando)) {
        continuarIncrementando = false;
        SCBEDFT();
        break;
      }
    }
  }
}

// Decrementar velocidade
void LB() {
  while (velAtH > velMin && continuarDecrementando) {
    velAtH--;
    esc1.write(velAtH); esc2.write(velAtH);
    esc3.write(velAtH); esc4.write(velAtH);
    Serial.println(velAtH);
    delay(50);

    if (serialdobluetooth.available()) {
      char novoComando = serialdobluetooth.read();
      if (ehComandoDirecional(novoComando)) {
        continuarDecrementando = false;
        SCBEDFT();
        break;
      }
    }
  }
}

// Estabilizar motores na velocidade atual
void SCBEDFT() {
  esc1.write(velAtH); esc2.write(velAtH);
  esc3.write(velAtH); esc4.write(velAtH);
}

// Esquerda
void LE() {
  veldirE = round((float)velAtH / 2);
  veldirD = velAtH;
  esc1.write(veldirE); esc3.write(veldirE);
  esc2.write(veldirD); esc4.write(veldirD);
}

// Direita
void LD() {
  veldirE = velAtH;
  veldirD = round((float)velAtH / 2);
  esc1.write(veldirE); esc3.write(veldirE);
  esc2.write(veldirD); esc4.write(veldirD);
}

// Frente
void LF() {
  veldisF = round((float)velAtH / 2);
  veldisT = velAtH;
  esc1.write(veldisF); esc2.write(veldisF);
  esc3.write(veldisT); esc4.write(veldisT);
}

// Trás
void LT() {
  veldisF = velAtH;
  veldisT = round((float)velAtH / 2);
  esc1.write(veldisF); esc2.write(veldisF);
  esc3.write(veldisT); esc4.write(veldisT);
}

// Verifica se o comando recebido é direcional
bool ehComandoDirecional(char comando) {
  return comando == 'f' || comando == 't' || comando == 'e' ||
         comando == 'd' || comando == 'c' || comando == 'b';
}
