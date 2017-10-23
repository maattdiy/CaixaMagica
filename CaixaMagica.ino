
/********************************************************************
  Caixa Mágica
  0.1 • 03/11/2015 • Markus e Morgana - Conceito inicial
  0.2 • 22/02/2017 • Markus - Simplificação
  1.0 • 21/04/2017 • Markus - Versão funcional
  1.1 • 08/09/2017 • Markus - Reformulação para versão final
  2.0 • 06/10/2017 • Markus - Versão final para evento
  2.1 • 19/10/2017 • Markus - Removido piano, mais palavras wiki, melhoria jogos

  Links de suporte:
  http://playground.arduino.cc/Learning/MFRC522
  http://www.arduinoecia.com.br/2014/12/controle-de-acesso-modulo-rfid-rc522.html

  https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library
  https://learn.adafruit.com/neopixels-and-servos
  https://learn.sparkfun.com/tutorials/ws2812-breakout-hookup-guide/adding-firmware
  http://www.tweaking4all.com/hardware/arduino/arduino-ws2812-led/

  http://playground.arduino.cc/Portugues/LearningSmoothing
  https://learn.adafruit.com/memories-of-an-arduino/optimizing-program-memory
  
  Ligações:
  ______________________________________________________________________
  Laser      | 5           | Vermelho/Amarelo
  Sonar      | 8,7         | Amarelo
  Servos     | 9           | Verde
  RFID       | 11-13, A0-2 | Pinos numerados
  Fita LED   | 6           | Verde
  Tela LED   | 3           | Verde (GND), Marrom (5V), Lilás/Laranja (Data)
  Buzzer     | 10          | Amarelo
  Botão      | A1          | Amarelo
  Joystick   | A2, A3      | Branco

*********************************************************************/

// ==================================================================
// Includes
// ==================================================================

#include <Streaming.h>           // http://arduiniana.org/libraries/streaming

#include <SPI.h>
#include <MFRC522.h>             // http://playground.arduino.cc/Learning/MFRC522
#include <Adafruit_GFX.h>        // https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library
#include <Adafruit_NeoMatrix.h>  // https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library
#include <Adafruit_NeoPixel.h>   // https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_TiCoServo.h>  // https://learn.adafruit.com/neopixels-and-servos
#include <NewPing.h>             // https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home
#include <EEPROM.h>              // https://playground.arduino.cc/Portugues/LearningEEPROMWrite

#include "musical_notes.h"
#include "Cassino.h"
#include "Genius.h"
#include "Matematica.h"
#include "Wiki.h"
//#include "QuatroLinha.h"

// ==================================================================
// Constantes/Variáveis
// ==================================================================

#define DEV true
#define MUDO false
#define DEBUG_DIST false
#define DEBUG_ANALOGICO false
#define DELAY_DEFAULT 50

#define pinLaser 5
#define pinServoPremio 9
#define pinLedInfo 13
#define pinBuz 10
#define pinBotao A1

#define pinRfidRST A0      // 9 padrão, alterado por causa do Servo que precisa pin fixo por causa do NeoPixels
#define pinRfidSS 2        // 10 padrão, alterado por causa do Servo que precisa pin fixo por causa do NeoPixels

#define pinSonarTrig 8     // Ao contrário pois o sensor está de cabeça para baixo
#define pinSonarEcho 7

#define pinJSX A2
#define pinJSY A3

#define servoPremioPosIni 1450 //1500
#define servoPremioPosFim 1900 //2000
#define servoPremioDelay 180

#define pinFitaLed 6
#define fitaLedPixels 41
#define fitaLedPixelsIni 6
#define pinTelaLed 3

// As cores foram calculadas 2x pois há um diferença entre do número gerado pela fita e pela tela
#define corVermelho 63488
#define corVerde 2016
#define corAzul 31
#define corAmarelo 65504
#define corLilas 63519
#define corLaranja 64512
#define corRosa 63503
#define corBranco 65535
const int cores[] = {corVermelho, corVerde, corAzul, corAmarelo, corLilas, corLaranja, corRosa, corBranco};

Adafruit_NeoPixel fitaLED = Adafruit_NeoPixel(fitaLedPixels, pinFitaLed);
Adafruit_NeoMatrix telaLED = Adafruit_NeoMatrix(8, 8, 2, 1, pinTelaLed, 
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_ROWS +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800);

Adafruit_TiCoServo servoPremio;
MFRC522 rfid(pinRfidSS, pinRfidRST);
NewPing sonar(pinSonarTrig, pinSonarEcho, 300);

bool cancelado = false;
char ultModo = '?';
char ultModoIni = '?';
byte pontos = 0;
byte pontosPremio = 4;

unsigned long ultDistCM = 0;
unsigned long ultRfidMillis = 0;
String ultRfid = "";

int ultBotaoCmd = LOW;
int ultJSX = 0;
int ultJSY = 0;
char ultJSCmd = ' ';
char ultJSCmd2 = ' ';
bool auxBotaoPressionado = false;

byte configIdade = 3;
bool configMudo = false;
byte configWikiCarta = 0;
#define idConfigIdade 1
#define idConfigMudo 2
#define idConfigPremio 3
#define idConfigCount 3
#define idConfigWikiCarta 4

// ==================================================================
// Classes dos módulos
// ==================================================================

Cassino cassino = Cassino();
Genius genius = Genius();
Matematica matematica = Matematica();
Wiki wiki = Wiki();
//QuatroLinha quatroLinha = QuatroLinha();

// ==================================================================
// Métodos
// ==================================================================

void setup() {
  Serial.begin(9600);
  configurarPins();
  carregarEPROM();
  imprimirInfosSetup();
  moduloFim();
  beep_Venceu();
}

void loop() {
  verificarSerial();
  verificarRfid();
  //verificarSensoresAnalogicos(0);
  verificarControles();
  
  // O botão repete a última ação
  if (botaoPressionado()) { executarAcao(ultModoIni); }
  
  imprimirInfosLoop();
  delay(DELAY_DEFAULT);
}

// ==================================================================
// Métodos auxiliares
// ==================================================================

void carregarEPROM() {
  int valor;
  
  valor = EEPROM.read(idConfigIdade);
  if (valor > 0 && valor < 255) { configIdade = valor; }
  
  valor = EEPROM.read(idConfigMudo);
  if (valor == 1) { configMudo = true; }

  valor = EEPROM.read(idConfigWikiCarta);
  if (valor < 255) { configWikiCarta = valor+1; }
}

void salvarEPROM() {
  EEPROM.write(idConfigIdade, configIdade);
  EEPROM.write(idConfigMudo, (configMudo ? 1 : 0));
}

void configurarPins() {
  // Pinos simples
  pinMode(pinLaser, OUTPUT);
  pinMode(pinBotao, INPUT);
  
  // RFID
  SPI.begin();
  rfid.PCD_Init();

  // Servo
  servoPremio.attach(pinServoPremio, 1000, 2000);
  servoPremio.write(servoPremioPosIni);

  // NeoPixels
  fitaLED.begin();
  fitaLED.show();
  
  telaLED.begin();
  telaLED.setTextWrap(false);
  telaLED.setBrightness(50);
  telaLED.show();
  
  // Sorteios
  randomSeed(analogRead(A0));
}

void verificarSerial() {
  if (Serial.available() <= 0) return;
  
  String op = Serial.readString();
  Serial << F("Op: ") << op << endl; 

  // Comandos especiais
  if (op == "testar") { testar(); }
  
  char acao = op.c_str()[0];
  //Serial << F("Acao: ") << acao << endl; 
  executarAcao(acao);  
}

char traduzirTagRfid(String value) {
  if (ultRfid == "61 31 81 66") { return '$'; }
  if (ultRfid == "71 4C 82 66") { return '$'; }
  if (ultRfid == "61 90 82 66") { return '$'; }
  if (ultRfid == "71 4D 83 66") { return 'g'; }
  if (ultRfid == "21 DC 81 66") { return 'q'; }
  if (ultRfid == "21 1C 82 66") { return 'b'; }
  if (ultRfid == "71 52 15 24") { return 'p'; }
  if (ultRfid == "51 B9 81 66") { return 'm'; }
  if (ultRfid == "B1 EC 81 66") { return 'w'; }
  if (ultRfid == "91 92 82 66") { return '!'; }
  if (ultRfid == "01 18 82 66") { return '*'; }
  if (ultRfid == "21 E3 81 66") { return '1'; }
  if (ultRfid == "81 74 82 66") { return '2'; }
  if (ultRfid == "A1 E7 85 66") { return '3'; }
  if (ultRfid == "A1 F5 84 66") { return '4'; }
  if (ultRfid == "41 97 80 66") { return '5'; }
  if (ultRfid == "21 FE 82 66") { return '6'; }
  if (ultRfid == "41 F7 81 66") { return '7'; }
  if (ultRfid == "x") { return '8'; }
  if (ultRfid == "x") { return '9'; }
  if (ultRfid == "x") { return '0'; }
  
  return ' ';
}

void executarRfid() {
  char acao;
    
  acao = traduzirTagRfid(ultRfid);
  executarAcao(acao);
}

void executarAcao(char acao) {
  bool mudouModo = true;
  
  ultModo = acao;
  
  // Evita recursão
  if (ultModoIni == 'w') { return; }
  
  switch (acao) {
  case '$':
    cassino_Jogar();
    break;
  case 'g':
    genius_Jogar();
    break;
  case 'q':
    quatroLinha_Jogar();
    break;
  case '&':
    musica_Jogar();
    break;
  case 'b':
    bolaCristal_Jogar();
    break;
  case 'p':
    piano_Jogar();
    break;
  case 'm':
    matematica_Jogar();
    break;
  case 'w':
    wiki_Jogar();
    break;
  case '*':
    configurar_Jogar();
    break;
  case '!':
    executarPremio();
    break;
  default:;
    mudouModo = false;
  }
  
  //if (mudouModo) { ultModo = acao; }
  if ((ultModoIni != ultModo) && mudouModo) { cancelado = true; }
}

void verificarRfid() {
  if ((millis() - ultRfidMillis) <= 1000) return;
  
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent()) return;
  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) return;
  
  //Forma o UID
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < rfid.uid.size; i++) {
    conteudo.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(rfid.uid.uidByte[i], HEX));
  }
  
  conteudo.trim(); conteudo.toUpperCase();
  ultRfid = conteudo;
  Serial << F("RFID: ") << ultRfid << endl;
  
  rfid.PICC_HaltA();
  ultRfidMillis = millis();
  
  executarRfid();
}

void verificarSensoresAnalogicos(int tempo) {
  verificarSonar();
  if (tempo > 0) { delay(tempo); }
}

int lerAnalogico(int pin) {
  const int count = 10;
  int total = 0;

  for (int i = 0; i < count; i++) {
    total = total + analogRead(pin);
  }

  return (int)(total / count);
}

void verificarSonar() {
  // Utiliza a NewPing que é mais precisa
  // long microsec = ultrasonic.timing();
  // ultDistCM = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  unsigned long i = sonar.ping_cm();
  if (i >= 2) {
    ultDistCM = i;
  }
}

void beep() {
  beep_Tone(1000, 50);
}

void beep_Botao() {
  beep_Tone(150, 300);
}

void beep_Moeda() {
  for (int i = 1; i <= 4; i++) {
    beep_Tone(i * 200, 80);  // Notas altas, velocidade rapida
    delay(80);
  }
}

void beep_Venceu() {
  for (int i = 1; i <= 2; i++) {
    beep_Tone(800, 200);  // Notas baixas, velocidade lenta
    delay(200);
  }
}

void beep_Perdeu() {
  for (int i = 1; i <= 2; i++) {
    beep_Tone(150, 500);  // Notas baixas, velocidade lenta
    delay(500);
  }
}

void beep_Tone(int t, int d) {
  if (MUDO || configMudo) return;
  // Não utiliza bibliotecas para evitar conflito de timer
  tone(pinBuz, t, d);
}

void ledCor(int r, int g, int b) {
  fitaLED.clear();

  for (int i = fitaLedPixelsIni; i < fitaLED.numPixels(); i++) {
    fitaLED.setPixelColor(i, r, g, b);
  }

  fitaLED.show();
}

void ledCor(uint32_t c) {
  fitaLED.clear();

  for (int i = fitaLedPixelsIni; i < fitaLED.numPixels(); i++) {
    fitaLED.setPixelColor(i, c);
  }

  fitaLED.show();
}

void ledApagar() {
  fitaLED.clear();
  fitaLED.show();
}

void telaApagar() {
  telaLED.clear();
  telaLED.show();
}

void ledPremio(uint8_t wait) {
  for (int j = 0; j < 256; j = j + 10) { // cycle all 256 colors in the wheel
    beep_Tone(j+200, 100);
    
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < fitaLED.numPixels(); i = i + 3) {
        fitaLED.setPixelColor(i + q, Wheel( (i + j) % 255)); // turn every third pixel on
      }
      fitaLED.show();

      delay(wait);

      for (int i = 0; i < fitaLED.numPixels(); i = i + 3) {
        fitaLED.setPixelColor(i + q, 0); //turn every third pixel off
      }
    }
  }
  
  delay(100);
  ledApagar();
}

void ledPeca(uint8_t wait, int steps) {
  for (int j = 0; j < 256; j = j + steps) { // cycle all 256 colors in the wheel
    beep_Tone(j+200, 100);
    
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < fitaLED.numPixels(); i = i + 3) {
        fitaLED.setPixelColor(i + q, Wheel( (i + j) % 255)); // turn every third pixel on
      }
      fitaLED.show();

      delay(wait);

      for (int i = 0; i < fitaLED.numPixels(); i = i + 3) {
        fitaLED.setPixelColor(i + q, 0); //turn every third pixel off
      }
    }
  }
  
  delay(80);
  ledApagar();
}

void telaEscrever(String texto, int cor, int x) {
  telaLED.clear();
  telaLED.setCursor(x, 1);
  telaLED.setTextColor(cor);
  telaLED.print(texto);
  telaLED.show();
}

void telaEscrever(String texto, int cor) {
  telaEscrever(texto, cor, 1);
}

void telaSmile() {
  telaLED.clear();
  
  telaLED.drawRoundRect(3, 0, 9, 8, 3, corAmarelo);
  telaLED.writePixel(6, 2, corBranco);
  telaLED.writePixel(8, 2, corBranco);

  telaLED.writePixel(5, 4, corLaranja);
  telaLED.writePixel(6, 5, corLaranja);
  telaLED.writePixel(7, 5, corLaranja);
  telaLED.writePixel(8, 5, corLaranja);
  telaLED.writePixel(9, 4, corLaranja);
  
  telaLED.show();
}

void executarPremio() {
  ledPremio(10);

  servoPremio.write(servoPremioPosIni);
  delay(130);
  servoPremio.write(servoPremioPosFim);  // Vai para trás
  delay(120);
  servoPremio.write(servoPremioPosIni);
  
  /*
  servoPremio.write(servoPremioPosIni);
  delay(200);
  servoPremio.write(servoPremioPosFim);  // Vai para trás
  delay(190);
  servoPremio.write(servoPremioPosIni);
  */
}

void verificarControles() {
  verificarBotao();
  verificarJoystick();  
}

void verificarBotao() {
  int valor = digitalRead(pinBotao);
  
  if (valor == ultBotaoCmd) return;
  ultBotaoCmd = valor;
  
  if (valor == HIGH) {
    //executarPremio();
    //ultBotaoMillis = millis();
    auxBotaoPressionado = false;    
    beep();
  } else {
    //if ((millis() - ultBotaoMillis) <= 400 ) {
      auxBotaoPressionado = true;
    //}
  }
}

bool botaoPressionado() {
  if (auxBotaoPressionado) {
    auxBotaoPressionado = false;
    return true;
  } else {
    return false;
  }
  //return (ultBotaoCmd == HIGH && ((millis() - ultBotaoMillis) <= 300));
}

void verificarJoystick() {
  ultJSCmd2 = ultJSCmd;
  ultJSCmd = ' ';
  ultJSX = lerAnalogico(pinJSX);
  ultJSY = lerAnalogico(pinJSY);

  if (ultJSX >= 0   & ultJSX <=  200) { ultJSCmd = '<'; }
  if (ultJSX >= 700 & ultJSX <= 1024) { ultJSCmd = '>'; }

  if (ultJSY >= 0   & ultJSY <=  200) { ultJSCmd = '^'; }
  if (ultJSY >= 700 & ultJSY <= 1024) { ultJSCmd = 'v'; }

  if (ultJSCmd != ultJSCmd2) { 
      //Serial << "Joystick:  Cmd: " << ultJSCmd << ", X: " << ultJSX << ", Y: " << ultJSY << endl;
      if (ultJSCmd != ' ') { 
        if (ultModoIni != 'p') {
          beep(); 
        }
      }
  }
}

void processarPonto(bool verificarPremio = true) {
  pontos++;
  telaDesenharPlacar();
  if ((pontos == pontosPremio) && (verificarPremio)) { 
    delay(500);
    telaSmile();
    executarPremio();
    telaDesenharPlacar();
    delay(1000);
  }
}

byte numCiclo(byte n, int offset, byte iMin, byte iMax) {
  n = n + offset;
  if (n < iMin) { n = iMax; }
  if (n > iMax) { n = iMin; }
  return n;
}

// ==================================================================
// Métodos importados
// ==================================================================

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  
  if (WheelPos < 85) {
    return fitaLED.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  
  if (WheelPos < 170) {
    WheelPos -= 85;
    return fitaLED.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  
  WheelPos -= 170;
  return fitaLED.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// ==================================================================
// Métodos para testes
// ==================================================================

void testar() {
  testarServo();
}

void testarServo() {  
  for (int i = 0; i <= 4; i++) {
    executarPremio();
    delay(1000);
  }
}

void imprimirInfosSetup() {
  Serial.println(F("Caixa Magica!"));

  // Info Servos
  //Serial << "Servo premio: " << servoPremio.read() << endl;

  // Info RFID
  byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print(F("MFRC522 Software Version: "));
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));

  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }

  Serial.println("");
  Serial.println(F("Configurações:"));
  Serial << F("  Idade: ") << configIdade << endl;
  Serial << F("  Mudo: ") << configMudo << endl;
  Serial << F("  Carta: ") << configWikiCarta << endl;
  Serial.println("");
  
  Serial.println(F("Tudo pronto"));
  Serial.println(F("Comandos: configurar (*), cassino ($), genius (g), matematica (m), wiki (w), bola de cristal (b), piano (p), 4 em linha (q)"));
}

void imprimirInfosLoop() {
  if ((ultDistCM > 0) && (DEBUG_DIST)) {
    Serial << F("Distancia CM: ") << ultDistCM << endl;
  }
  
  //Serial << "Volume: " << ultMic << endl;
}

// ==================================================================
// Geral para os módulos
// ==================================================================

void moduloInicio() {
  cancelado = false;
  pontos = 0;
  ultModoIni = ultModo;
  ledApagar();
  telaApagar();
}

void moduloFim() {
  ledApagar();
  telaApagar();  
  delay(300);
  
  // Efeito para o led central
  for (byte i = 0; i <= 100; i=i+20) {
    ledCor(0, 0, i);
    delay(50);
  }
}

int getCorIdx(int idx, bool a) {
  int cor = 0;
  int valor;
  
  if (a) { valor = 70; } else { valor = 255; }
  
  switch (idx) {
  case 1:
    cor = telaLED.Color(valor, 0, 0);
    break;
  case 2:
    cor = telaLED.Color(0, valor, 0);
    break;
  case 3:
    cor = telaLED.Color(0, 0, valor);
    break;
  case 4:
    cor = telaLED.Color(valor, valor, 0);
    break;
  default:;
  }
  
  return cor;
}

int getCorStr(String s) {
  if (s == "Sim" || s == "Aha") { return corVerde; }
  if (s == "Nao" || s == "Xiii") { return corVermelho; }
  if (s == "??") { return corAmarelo; }
  
  if (s == "X") { return corLilas; }
  if (s == "O") { return corAmarelo; }
  if (s == "7") { return corAzul; }
  
  return telaLED.Color(200, 200, 200);
}

void telaDesenharPlacar() {
  telaDesenharPlacar(pontos, pontosPremio);
}

void telaDesenharPlacar(int atual, int total) {
  int n = fitaLedPixels - (int)(total / 2);
  int fim = max(atual, total);
  
  ledApagar();
  
  for (int i = 1; i <= fim; i++) {        
    if (i <= atual) {
      if (i > total) {
        fitaLED.setPixelColor(n, 255, 0, 0);
      } else {
        fitaLED.setPixelColor(n, 0, 255, 0);
      }
    } else { 
      fitaLED.setPixelColor(n, 20, 20, 20);
    }

    n++;
    if (n >= fitaLedPixels) { n = fitaLedPixelsIni + 1 ; }
  }
  
  fitaLED.show();
}

// ==================================================================
// Configurador
// ==================================================================

void configurar_Jogar() {
  bool alterou = false;
  byte idAtual = idConfigIdade;
  
  moduloInicio();
  configurar_DesnharInfo(idAtual);
  beep();
  
  while (true) {
    verificarControles();
    delay(200); // Tempo para o joystick
    
    if (botaoPressionado() || cancelado) {
      if (alterou) { salvarEPROM(); }
      break;
    }
    
    switch (ultJSCmd) {
    case '^':
      switch (idAtual) {
      case idConfigIdade:
        configIdade = numCiclo(configIdade, 1, 3, 10);
        break;
      case idConfigMudo:
        configMudo = !configMudo;
        break;
      }      
      alterou = true;
      configurar_DesnharInfo(idAtual);
      break;
    case 'v':
      switch (idAtual) {
      case idConfigIdade:
        configIdade = numCiclo(configIdade, -1, 3, 10);
        alterou = true;
        break;
      case idConfigMudo:
        configMudo = !configMudo;
        alterou = true;
        break;
      case idConfigPremio:
        cancelado = true;
        executarPremio();  
        break;
      }
      
      configurar_DesnharInfo(idAtual);
      break;
    case '<':
      idAtual = numCiclo(idAtual, -1, 1, idConfigCount);
      configurar_DesnharInfo(idAtual);
      break;
    case '>':
      idAtual = numCiclo(idAtual, 1, 1, idConfigCount);
      configurar_DesnharInfo(idAtual);
      break;
    default:;
    }
  }
  
  moduloFim();  
}

void configurar_DesnharInfo(byte id) {
  switch (id) {
  // Configuração da idade
  case idConfigIdade:
    //telaEscrever(String((int)configIdade) + ((configIdade>=10) ? F("") : F("a")), corAzul);
    telaEscrever(String((int)configIdade), corAzul);
    break;
  // Configuração do som
  case idConfigMudo:
    telaLED.clear(); telaLED.setCursor(1, 1);
    telaLED.setTextColor(corBranco);
    telaLED.print("S");
    if (configMudo) {
      telaLED.setTextColor(corVermelho);
      telaLED.print("0");
    } else {
      telaLED.setTextColor(corAzul);
      telaLED.print("1");
    }
    telaLED.show();
    break;
  // Teste do prêmio
  case idConfigPremio:
    telaSmile();
    break;
  }
}

// ==================================================================
// Cassino
// ==================================================================

void cassino_Jogar() {
  byte giro;
  byte posIni;
  
  moduloInicio();
  cassino_EfeitoMoeda();
  cassino.jogar();
  
  while (cassino.processarResultado()) {
    giro++;   
    if ((giro % 2) == 0) {posIni = fitaLedPixelsIni; } else { posIni = fitaLedPixelsIni + 1; }
    
    fitaLED.clear();
    
    for (byte i = posIni; i <= fitaLedPixels; i = i+3) {
      fitaLED.setPixelColor(i, 255, 255, 0);
    }
    
    fitaLED.show();
    telaApagar();
    delay(80);
    cassino_Desenhar();
    beep();    
  }
  
  delay(500);
  
  // Resultado
  if (cassino.getResultado() == 1) {
    ledCor(0, 255, 0);
    musica_AxlF();
    executarPremio();
  } else {
    ledCor(255, 0, 0);
    beep_Perdeu();
  }
  
  delay(1000);
  moduloFim();
}

void cassino_EfeitoMoeda() {
  ledCor(fitaLED.Color(255, 255, 0));
  beep_Moeda();
  delay(500);
  ledApagar();
  cassino_DesenharMoeda();
  delay(400);
}

void cassino_Desenhar() {
  byte x = 0;  
  
  telaLED.clear();
  
  for (byte i = 0; i < 3; i++) {   
    telaLED.setCursor(x, 1);
    telaLED.setTextColor(getCorStr(String(cassino.getColuna(i))));
    telaLED.print(cassino.getColuna(i));
    telaLED.show();

    x = x + 5;
  }
}

void cassino_DesenharMoeda() {
  int p = 150;
  
  for (int y = 10; y >= 4; y--) {
    telaLED.clear();
    telaLED.fillCircle(4, y, 3, corAmarelo);
    telaLED.show();
    
    p = p + 10;
    beep_Tone(p, 50);
    delay(90);
  }
}

// ==================================================================
// Genius
// ==================================================================

void genius_Jogar() {
  int idxSel = -1;
  bool verificar = false;
  
  moduloInicio();
  ledPeca(30, 30);
  
  genius_ConfigNivel();
  genius.jogar();
  telaDesenharPlacar();
  genius_DesenharSequencia();
  
  while(genius.getResultado() >= 0) {
    verificarControles();
    verificar = false;
    
    if (botaoPressionado()) { 
      if (idxSel >= 0) { verificar = true; }
    }
    
    switch (ultJSCmd) {
    case '>':
      idxSel++;
      if (idxSel > 3) {idxSel = 0; }
      genius_Desenhar(idxSel, false);
      delay(250);
      break;
    case '<':
      idxSel--;
      if (idxSel < 0) {idxSel = 3; }
      genius_Desenhar(idxSel, false);
      delay(250);
      break;
    case 'v':
    case '^':
      if (idxSel < 0) { break; }
      verificar = true;
      break;
    default:;
    }
    
    if (verificar) {
      genius_Desenhar(idxSel, true);
      delay(200);
      genius_Desenhar(idxSel, false);
      
      genius.verificarResposta(idxSel);
      delay(300);
      
      if (genius.getResultado() == 1) {
        beep_Venceu();
        processarPonto(false);
        genius.sortear();

        delay(300);
        genius_DesenharSequencia();
        idxSel = -1;
      }      
    }
  }
  
  genius_DesenharFim();  
  moduloFim();
}

void genius_ConfigNivel() {
  switch (configIdade) {
  case 1: case 2: case 3: case 4:
    pontosPremio = 3;
    break;
  case 5: case 6:
    pontosPremio = 4;
    break;
  case 7: case 8: case 9:
    pontosPremio = 6;
    break;
  default:
    pontosPremio = 10;
  }
}

void genius_Desenhar() {
  genius_Desenhar(-1, false);
}

void genius_Desenhar(int sel, bool pres) {
  genius_Desenhar(sel, pres, 1);
}

void genius_Desenhar(int sel, bool pres, int y) {
  int x = 1;
  int yReal;
  bool a;
  
  telaLED.clear();
  
  for (int i = 0; i < 4; i++) {
    yReal = y;
    
    if (sel == i) {
      a = false;
      if (pres) { yReal++; }
      telaLED.fillRect(x, 7, 3, 1, corBranco);
    } else {
      a = true;
    }    
    
    telaLED.fillRect(x, yReal, 3, 5, getCorIdx(i+1, a));
    x = x + 4;
  }

  telaLED.show();
}

void genius_DesenharSequencia() {
  int t = 0;
  genius_Desenhar();
  delay(500);
  
  t = 400 - (genius.getNivel() * 30);
  if (t < 50) { t = 50; }
  
  for (int i = 0; i <= genius.getNivel(); i++) {
    genius_Desenhar();
    delay(t);
    genius_Desenhar(genius.getSequencia(i), false);
    beep();
    delay(t + 200);
  }
  
  genius_Desenhar();
}

void genius_DesenharFim() {
  telaLED.clear();
  telaLED.setCursor(2, 1);
  telaLED.setTextColor(telaLED.Color(255, 0, 0));
  telaLED.print(genius.getNivel());
  telaLED.show();
  beep_Perdeu();
  delay(2000);
}

// ==================================================================
// Quatro em linha
// ==================================================================

void quatroLinha_Jogar() {
  /*
  int idxSel = -1;
  bool verificar = false;
  
  moduloInicio();
  ledPeca(30, 30);
  
  quatroLinha.jogar();
  quatroLinha_Desenhar(7);
  
  while(quatroLinha.getResultado() == 0) {
    verificarControles();
    verificar = false;
    
    if (botaoPressionado()) { 
      if (idxSel >= 0) {
        verificar = true;
      }
    };
    
    switch (ultJSCmd) {
    case '>':
      idxSel = numCiclo(idxSel, 1, 1, 16);
      //if (idxSel > quatroLinha.getLargura()) {idxSel = 0; }
      quatroLinha_Desenhar(idxSel);
      delay(250);
      break;
    case '<':
      idxSel = numCiclo(idxSel, -1, 1, 16);
      //if (idxSel < 0) {idxSel = quatroLinha.getLargura(); }
      quatroLinha_Desenhar(idxSel);
      delay(250);
      break;
    case 'v':
    case '^':
      verificar = true;
      break;
    default:;
    }
    
    if (verificar) {
      //quatroLinha_Desenhar(idxSel, true);
      delay(200);
      //quatroLinha_Desenhar(idxSel);
      
      quatroLinha.processarResposta(idxSel);
      delay(300);
      
      if (quatroLinha.getResultado() == 1) {
        //quatroLinha.sortear();
        idxSel = -1;
      }      
    }
  }
  
  moduloFim();
  */
}

void quatroLinha_Desenhar() {
  quatroLinha_Desenhar(-1);
}

void quatroLinha_Desenhar(int sel) {
  int corBorda = telaLED.Color(100, 100, 100);

  telaLED.clear();
  /*
  // Borda  
  telaLED.writeLine(0, 0, 0, 7, corBorda);
  telaLED.writeLine(0, 7, 15, 7, corBorda);
  telaLED.writeLine(15, 0, 15, 7, corBorda);
  telaLED.show();
  */
  
  // Seleção
  if (sel > 0) {
    telaLED.setPixelColor(sel-1, 255, 0, 0);    
  }

  telaLED.show();
}

// ==================================================================
// Música
// ==================================================================

void musica_Jogar() {
  // Não utilizado mais por falta de memória
  // http://dimescu.com/2011/09/arduino-twinkle-twinkle-little-star/
  musica_AxlF();  
}

void musica_AxlF() {
  // https://github.com/nseidle/AxelF_DoorBell/blob/master/AxelF/AxelF.ino
  
  int line1[] = {
    NOTE_D4, 0, NOTE_F4, NOTE_D4, 0, NOTE_D4, NOTE_G4, NOTE_D4, NOTE_C4,
    NOTE_D4, 0, NOTE_A4, NOTE_D4, 0, NOTE_D4, NOTE_AS4, NOTE_A4, NOTE_F4,
    NOTE_D4, NOTE_A4, NOTE_D5, NOTE_D4, NOTE_C4, 0, NOTE_C4, NOTE_A3, NOTE_E4, NOTE_D4,
    0};
  
  int line1_durations[] = {
    8, 8, 6, 16, 16, 16, 8, 8, 8, 
    8, 8, 6, 16, 16, 16, 8, 8, 8,
    8, 8, 8, 16, 16, 16, 16, 8, 8, 2,
    2};    
    //9, 9, 10, 2 = 29 total things
  
  for (int thisNote = 0 ; thisNote < 29 ; thisNote++) {
    int noteDuration = 1000/line1_durations[thisNote];
    beep_Tone(line1[thisNote], noteDuration * 3);
    delay(100);
  }
}

// ==================================================================
// Bola de cristal
// ==================================================================

void bolaCristal_Jogar() {
  int maxRespostas = 3;
  String respostas[maxRespostas] = { "Sim", "Nao", "??" };
  String resposta;
  
  moduloInicio();
  ledPeca(20, 5);
  resposta = respostas[random(0, maxRespostas)];
  
  telaLED.clear();
  telaLED.setCursor(0, 1);
  telaLED.setTextColor(getCorStr(resposta));
  telaLED.print(resposta);
  telaLED.show();
  
  beep();
  delay(3000);
  moduloFim();
}

// ==================================================================
// Piano virtual
// ==================================================================

void piano_Jogar() {
  /*
  const int distIni = 4;
  const int distFim = 60;
  int dist = 0;
  int notasIdx = -1;
  int ultNotaIdx = -1;
  
  const int notasCount = 8;
  //int notas[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5}; // C-Major scale
  //int notas[] = {NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5}; // A-Minor scale
  const int notas[] = {NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_AS4, NOTE_C5, NOTE_DS5}; // C Blues scale
  
  moduloInicio();
  ledPeca(30, 30);
  telaEscrever(F("<-"), corAzul);
  digitalWrite(pinLaser, HIGH);
  delay(1000);
  
  while(true) {    
    verificarControles();
    verificarSensoresAnalogicos(100);
    //verificarRfid();
    
    if (botaoPressionado() || cancelado) { 
      beep_Perdeu();
      break;
    }
	  
    if (ultDistCM >= distIni && ultDistCM <= distFim) {
      if (abs(dist - ultDistCM) >= 2) {
        dist = ultDistCM;
        notasIdx = map(dist, distIni, distFim, 0, notasCount-1);
      }
    } else {
      dist = 0;
      notasIdx = -1;
    }
    
    if (notasIdx != ultNotaIdx) {
      if (notasIdx >=0) {
        piano_TocarNota(notas[notasIdx], notasIdx, 250, cores[notasIdx]);
      } else {
        telaApagar();
        ledApagar();
      }
      
      //Serial << "Nota idx: " << notasIdx << endl;
      ultNotaIdx = notasIdx;
    }
    
    switch (ultJSCmd) {
    case '>':
    case '<':
    case '^':
    case 'v':
      telaApagar();
      ledApagar();
      musica_AxlF();
      break;
    default:;
    }    
  }
  
  digitalWrite(pinLaser, LOW);
  moduloFim();
  */
}

/*
void piano_TocarNota(int nota, int idx, int tempo, int cor) { 
  int h = idx; //map(idx, 0, 7, 0, 7);
  int y = 7-h;
  int alpha = (idx + 1) * 15;
  int cor2 = telaLED.Color(40, 40, 40);
  
  beep_Tone(nota, tempo);

  // Linha na altura da nota
  telaLED.clear();
  telaLED.fillRect(0, y-1, 16, 1, cor2);
  telaLED.fillRect(0, y, 16, 1, cor);
  telaLED.fillRect(0, y+1, 16, 1, cor2);
  telaLED.show();
  
  // Efeito de arco
  fitaLED.clear();
  for (int i = fitaLedPixelsIni; i < fitaLED.numPixels(); i++) {
    fitaLED.setPixelColor(i, cor);
  }
  fitaLED.show();
}
*/

// ==================================================================
// Matemática
// ==================================================================

void matematica_Jogar() {
  bool sonarAtivo = false;
  bool processar;
  byte offset = 0;
  int idx;
  
  moduloInicio();
  ledPeca(30, 30);
  
  matematica_ConfigNivel();
  matematica_Inicio();
  
  while (true) {
    processar = false;
    verificarControles();
    verificarSensoresAnalogicos(120);    
    //verificarRfid();
    if (cancelado) { return; }
    
    if (sonarAtivo) {
      if (matematica.processarSonar(ultDistCM)) {
        if (matematica.getUltDigito() >= 0) {
          //Serial << "Dígito: " << matematica.getUltDigito() << endl;
          //Serial << matematica.getRespostaAtualTela() << endl;
          telaEscrever(matematica.getRespostaAtualTela(), corAmarelo);
          beep();
        } else {
          //matematica_DesenharTexto("?", corAmarelo);
        }
      }
    } else {
      //delay(50);
    }
    
    // Controle pelo joystick
    switch (ultJSCmd) {
    case '>':
      //processar = true;
      break;
    case '<':
      //offset = (offset == 0 ? 1 : 0);
      //matematica_DesenharExpressao(-offset, 1);
      break;
    case '^':
      sonarAtivo = false;
      matematica.aumentarUltDigito(1);
      matematica_DesenharRespostaAtual();
      break;
    case 'v':
      if (sonarAtivo) {
        processar = true;
        sonarAtivo = true;
      } else {
        matematica.aumentarUltDigito(-1);
        matematica_DesenharRespostaAtual();
      }
      break;
    default:;
    }
    
    // Escaneou os números
    if ((ultModo >= '0') && (ultModo <= '9')) {
      idx = ultModo-48; // Mapea o char para o número
      ultModo = ' ';
      
      matematica.setUltDigito(idx);
      matematica_DesenharRespostaAtual(); delay(500);
      processar = true;
    }
    
    if (botaoPressionado()) { processar = true; } //sonarAtivo = true;
    if (processar) {
      if (matematica_processarResultado() != 0) { break; }
    }
  }
  
  moduloFim();
}

void matematica_Inicio() {
  //ledApagar();
  matematica.jogar();
  matematica_DesenharExpressaoEfeito();
  telaDesenharPlacar();
  delay(1000);
}

void matematica_ConfigNivel() {
  switch (configIdade) {
  case 1: case 2: case 3: case 4:
    pontosPremio = 2;
    matematica.setNivel(1);
    break;
  case 5: case 6:
    pontosPremio = 4;
    matematica.setNivel(2);
    break;
  case 7: case 8: case 9:
    pontosPremio = 6;
    matematica.setNivel(3);
    break;
  default:
    pontosPremio = 10;
    matematica.setNivel(4);
  }
}

int matematica_processarResultado() {
  matematica.processarResultado();
  matematica_DesenharRespostaAtual();
  
  // Continua processando, ainda não terminou a resposta
  if (matematica.getResultado() == 0) {
    delay(700);
    return 0;
  } else {      
    // Venceu
    if (matematica.getResultado() == 1) {
      telaEscrever(matematica.getRespostaAtualTela(), corVerde);      
      musica_AxlF();
      processarPonto();
      delay(500);
      if (pontos == pontosPremio) {
        return 1;
      } else {
        matematica_Inicio();
        return 0;
      }
    // Perdeu
    } else {
      telaEscrever(matematica.getRespostaAtualTela(), corVermelho);
      ledCor(255, 0, 0);
      beep_Perdeu();
      delay(1000);
      telaEscrever(String(matematica.getRespostaCerta()), corVerde, 3);
      delay(2000);
      return -1;
    }
  }
}

void matematica_DesenharRespostaAtual() {
  telaEscrever(matematica.getRespostaAtualTela(), corAmarelo);
}

void matematica_DesenharNivel() {
  int h = map(matematica.getNivel(), 1, 4, 0, 7);
  
  telaLED.clear();
  telaLED.setCursor(1, 1);
  telaLED.setTextColor(corBranco);
  telaLED.print("N");
  telaLED.fillRect(9, 8-h, 4, h, corVermelho);
  telaLED.show();
}

void matematica_DesenharExpressaoEfeito() {
  for (int y = 7; y >= 1; y--) {
    matematica_DesenharExpressao(0, y);
    beep_Tone((8 - y) * 200, 80);
    delay(50);
  }
}

void matematica_DesenharExpressao() {
  matematica_DesenharExpressao(0, 1);
}

void matematica_DesenharExpressao(int x, int y) {
  telaLED.clear();
  
  telaLED.setCursor(x, y);
  telaLED.setTextColor(corAzul);
  telaLED.print(matematica.getNumero1());
  
  telaLED.setCursor(x+5, y);
  telaLED.setTextColor(corLilas);
  telaLED.print(matematica.getSinal1());
  
  telaLED.setCursor(x+11, y);
  telaLED.setTextColor(corAzul);
  telaLED.print(matematica.getNumero2());
  
  telaLED.show();
}

// ==================================================================
// Wiki
// ==================================================================

void wiki_Jogar() {
  const byte idMenuSair = 2;
  byte idxMenu = 1;
  bool jogando = false;
  byte tempo = 0;
  byte idx;
  
  moduloInicio();
  ledPeca(30, 30);
  wiki_EfeitoPeca(-1);
  wiki.jogar();
  telaEscrever("?", corBranco);
  
  while (true) {
    //verificarSensoresAnalogicos(100);
    delay(100);
    verificarControles();
    verificarRfid();
    
    if (botaoPressionado()) {
      if (idxMenu == idMenuSair) {
        cancelado = true;
        break;
      } else {
        if (jogando) {
          wiki_DesenharCarta();
        } else {
          wiki_Sortear();
          jogando = true;
          telaEscrever("Foi", corAzul);
        }
      }
    }
    
    if (cancelado) {
      //beep_Perdeu();
      break; 
    }
    
    // Escaneou alguma peça do jogo
    if ((ultModo >= '1') && (ultModo <= '7')) {
      idx = ultModo-49; // Mapea o char '1'..'6' para o índice 0..5
      ultModo = ' ';
      
      if (jogando) {        
        wiki.adicionarPonto(idx);
        wiki_DesenharPlacar();
        wiki_EfeitoPeca(idx);
        if (wiki_ProcessarResultado()) { cancelado = true; break; }
        wiki_Sortear();
      } else {
        if (wiki.adicionarJogador(idx)) {
          wiki_DesenharPlacar();
          wiki_EfeitoPeca(idx);
        }
      }
    }
    
    switch (ultJSCmd) {
    case '>':
      wiki_Sortear();
      wiki_DesenharCarta();
      break;
    case '^':
    case 'v':
      idxMenu = numCiclo(idxMenu, 1, 1, idMenuSair);
      switch (idxMenu) {
      case 1: wiki_DesenharPlacar(); break;
      case idMenuSair: telaEscrever("Sair", corVermelho); break;
      }
    }
  }
  
  moduloFim();
  ultModo = ' ';
  ultModoIni = ' ';
}

void wiki_DesenharPlacar() {
  byte x = 1;
  byte h = 0;
  byte p;
  bool temPontos = false;
  
  telaLED.clear();
  
  // Desenha o placar de cada jogador
  for (byte i = 0; i <= wiki.getJogadoresCount()-1; i++) {
    if (wiki.getJogadorPontos(i) > 0) { temPontos = true; }
	  p = wiki.getJogadorPontos(i) + 1;
	  h = telaLED.height() - p - 1;
    telaLED.fillRect(x, h, 1, p, cores[wiki.getJogadorIdx(i)]);
    x = x + 2;
  }
  
  if (temPontos) { telaLED.fillRect(1, 1, x-2, 1, corBranco); }
  telaLED.show();
}

void wiki_EfeitoPeca(int idx) {
  if (idx >= 0) {
    ledCor(255, 255, 255);
    beep_Venceu();
    delay(500);
    ledApagar();
    delay(500);
  }
  
  // Efeito para o led central
  for (byte i = 0; i <= 20; i=i+5) {
    ledCor(i, i, i);
    delay(50);;
  }
}

void wiki_DesenharCarta() {
  String carta = ""; // wiki.getCarta();

  // Inserido aqui por restrições de memória. Função original: wiki.setCartaCore
  switch (configWikiCarta) {
  case 0:  carta = F("Casa"); break;
  case 1:  carta = F("Sapo"); break;
  case 2:  carta = F("Abelha"); break;
  case 3:  carta = F("Monstro"); break;
  case 4:  carta = F("Navio"); break;
  case 5:  carta = F("Cama"); break;
  case 6:  carta = F("Rato"); break;
  case 7:  carta = F("Livro"); break;
  case 8:  carta = F("Trem"); break;  
  case 9:  carta = F("Flor"); break;
  case 10: carta = F("Bola"); break;  
  case 11: carta = F("Toalha"); break;
  case 12: carta = F("Chave"); break; 
  case 13: carta = F("Escada"); break;
  case 14: carta = F("Agua"); break;
  case 15: carta = F("Ponte"); break;  
  case 16: carta = F("Estrela"); break;
  default: carta = ""; break; 
  }
  
  int offset = (carta.length()*6) - telaLED.width();
  
  if (offset < 0) { offset = 0; }
  
  telaEscrever(carta, corAzul);
  delay(1000);
  
  for (byte i = 0; i <= offset; i++) {
    telaEscrever(carta, corAzul, -i);
    delay(200);
  }
  
  delay(1000);
  wiki_DesenharPlacar();
}

void wiki_Sortear() {
  configWikiCarta = numCiclo(configWikiCarta, 1, 0, wiki.getMaxCartas()-1);
  EEPROM.write(idConfigWikiCarta, configWikiCarta);
  //wiki.sortear(configWikiCarta);
}

bool wiki_ProcessarResultado() {
  for (byte i = 0; i <= wiki.getJogadoresCount()-1; i++) {
    if (wiki.getJogadorPontos(i) >= 4) {      
      wiki_ProcessarGanhador(i);
      return true;
      break;
    }
  }
  
  return false;
}

void wiki_ProcessarGanhador(int pos) {
  ledApagar();
  
  telaLED.clear();
  telaLED.fillRect(3, 3, 10, 4, cores[wiki.getJogadorIdx(pos)]);
  telaLED.show();
  
  executarPremio();
  musica_AxlF();
  delay(3000);  
}

