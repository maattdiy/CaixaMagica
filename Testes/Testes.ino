
/********************************************************************
  Caixa Mágica Testes

  Links de suporte:
  http://playground.arduino.cc/Learning/MFRC522
  http://www.arduinoecia.com.br/2014/12/controle-de-acesso-modulo-rfid-rc522.html
  http://www.arduinoecia.com.br/2015/07/sensor-de-som-ky-038-microfone-arduino.html

  https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library
  https://learn.adafruit.com/neopixels-and-servos
  https://learn.sparkfun.com/tutorials/ws2812-breakout-hookup-guide/adding-firmware
  http://www.tweaking4all.com/hardware/arduino/arduino-ws2812-led/

*********************************************************************/

// ==================================================================
// Includes
// ==================================================================

#include <Wire.h>
#include <SoftwareSerial.h>
#include <IRLib.h>               // https://github.com/cyborg5/IRLib
#include <Streaming.h>           // http://arduiniana.org/libraries/streaming

#include <SPI.h>
#include <MFRC522.h>             // http://playground.arduino.cc/Learning/MFRC522
#include <Adafruit_NeoPixel.h>   // https://github.com/adafruit/Adafruit_NeoPixel
#include <avr/power.h>
#include <Adafruit_TiCoServo.h>  // https://learn.adafruit.com/neopixels-and-servos
#include <NewPing.h>             // https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home
#include <NewTone.h>             // http://forum.arduino.cc/index.php?topic=143940.0

/* Alternaivas
  #include <ServoTimer2.h>         // https://github.com/nabontra/ServoTimer2
  #include <IRremote.h>            // https://github.com/z3t0/Arduino-IRremote/releases
  #include <NewTone.h>             // http://forum.arduino.cc/index.php?topic=143940.0
  #include <Servo.h>               // http://www.arduino.cc/en/Tutorial/Knob
  #include <Ultrasonic.h>
*/

// ==================================================================
// Constantes/Variáveis
// ==================================================================

const bool DEV = true;
const bool DEBUG_DIST = false;
const bool DEBUG_ANALOGICO = true;
const int DELAY_DEFAULT = 100;

const int pinIR = 4;
const int pinLaser = 5;
const int pinMic = A4;
const int pinLdr = A5;
const int pinServoPremio1 = 9;
const int pinServoPremio2 = 10;
const int pinFitaLed = 3; //6
const int pinLedInfo = 13;
const int pinBuz = 10;

const int pinRfidRST = A0;   // 9 padrão, alterado por causa do Servo que precisa pin fixo por causa do NeoPixels
const int pinRfidSS = 2;     // 10 padrão, alterado por causa do Servo que precisa pin fixo por causa do NeoPixels

const int pinSonarTrig = 8;  // Ao contrário pois o sensor está de cabeça para baixo
const int pinSonarEcho = 7;

const int pinJSX = A2;
const int pinJSY = A3;

const int servoPremioPosIni = 93;
const int servoPremioOffset = 55;
const int servoPremioDelay = 700;

const int fitaLedPixels = 64;
const int fitaLedPixelsIni = 1;

IRrecv irRec(pinIR); IRdecode irCod;
Adafruit_NeoPixel fitaLED = Adafruit_NeoPixel(fitaLedPixels, pinFitaLed);
Adafruit_NeoPixel fitaLED2 = Adafruit_NeoPixel(16, 6);
Adafruit_TiCoServo servoPremio1;
Adafruit_TiCoServo servoPremio2;
MFRC522 rfid(pinRfidSS, pinRfidRST);
NewPing sonar(pinSonarTrig, pinSonarEcho, 400);

/* Alternaivas
  Servo servoPremio;
  Ultrasonic ultrasonic(pinSonarTrig, pinSonarEcho);
*/

unsigned long ultScanRfid = 0;
unsigned long ultDistCM = 0;
int ultMic = 0;
int ultLDR = 0;
unsigned long ultTempoEscuro = 0;
char ultModo = '?';
int contLedStandby = 0;
int contLedStandbyOffset = 10;
int ultJSX = 0;
int ultJSY = 0;
char ultJSCmd = ' ';
char ultJSCmd2 = ' ';

// ==================================================================
// Métodos
// ==================================================================

void setup() {
  Serial.begin(9600);
  configurarPins();
  imprimirInfosSetup();
}

void loop() {
  verificarSerial();
  verificarJoystick();
  //verificarIR();
  verificarRfid();
  //verificarSensoresAnalogicos();
  //verificarModo();
  
  imprimirInfosLoop();

  // TESTES
  //verificarSonar();
  testarFitaLED();
  //beep();
  //testarLaser();

  delay(DELAY_DEFAULT);
}

// ==================================================================
// Métodos auxiliares
// ==================================================================

void configurarPins() {
  // Pinos simples
  //pinMode(pinLdr, INPUT);
  //pinMode(pinBuz, OUTPUT);
  pinMode(pinLaser, OUTPUT);
  //pinMode(pinLedInfo, OUTPUT);

  // RFID
  SPI.begin();
  rfid.PCD_Init();

  // IR
  //irRec.No_Output();
  //irRec.enableIRIn();

  // Servos
  //servoPremio1.attach(pinServoPremio1, 1000, 2000);
  //servoPremio1.write(servoPremioPosIni);

  // NeoPixels
  fitaLED = Adafruit_NeoPixel(fitaLedPixels, pinFitaLed);
  fitaLED.begin();
  fitaLED.show();

  fitaLED2 = Adafruit_NeoPixel(16, 6);
  fitaLED2.begin();
  fitaLED2.show();
}

void verificarSerial() {
  int num;
  String op;

  if (Serial.available() <= 0) return;

  op = Serial.readString();
  
  Serial << "Op: " << op << endl;
  ultModo = ' ';
  
  if (op == "leds") {
    executarLed_Rfid();
    testarFitaLED();
  }

  if (op == "teste") {
    executarLed_Rfid();
    testarFitaLED();
  }  
}

void verificarIR() {
  char tecla;

  if (!irRec.GetResults(&irCod)) return;
  irCod.decode();

  if (irCod.value != 0) {
    Serial << "IR " << _HEX(irCod.value) << endl;
    //tecla = traduzirTeclaIR(irCod.value);

    if (tecla != 0) {
      Serial << "IR " << tecla << endl;
      //executarIR(tecla);
    }
  }

  irRec.resume();
}

void verificarRfid() {
  if ((millis() - ultScanRfid) <= 1000) return;

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
  Serial << "RFID: " << conteudo << endl;

  rfid.PICC_HaltA();
  ultScanRfid = millis();

  executarRfid();
}

void executarRfid() {
  //beep();

  executarLed_Rfid();
}

void verificarSensoresAnalogicos() {
  verificarSonar();
  ultLDR = lerAnalogico(pinLdr);
  ultMic = lerAnalogico(pinMic);

  // Detecta quando mudou de escuro para claro
  if (ultLDR <= 20) {
    if (ultTempoEscuro == 0) ultTempoEscuro = millis();

    if ((millis() - ultTempoEscuro) >= 3000) {
      ultTempoEscuro = millis();
      executarLed_Inicio();
    }
  }

  if (ultLDR >= 40) ultTempoEscuro = 0;
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

  int i = sonar.ping_cm();

  if (i >= 10) {
    Serial.println(i);
    ultDistCM = i;
    //executarLed_Cor(i, 0, 0);
  }
}

void beep() {
  // Não utiliza bibliotecas para evitar conflito de timer
  NewTone(pinBuz, 1000, 50);
  
  //digitalWrite(pinBuz, HIGH);
  //delay(50);
  //digitalWrite(pinBuz, LOW);

  //tone(pinBuz, 400, 200);
}

void executarLed_Rfid() {
  fitaLED.clear();

  for (int i = fitaLedPixelsIni; i < fitaLED.numPixels(); i++) {
    fitaLED.setPixelColor(i, 255, 0, 0);
    delay(20);
    fitaLED.show();
  }

  delay(70);

  for (int c = 255; c > 0; c -= 15) {
    for (int i = fitaLedPixelsIni; i < fitaLED.numPixels(); i++) {
      fitaLED.setPixelColor(i, c, 0, 0);
    }
    fitaLED.show();
    delay(50);
  }

  fitaLED.clear();
  fitaLED.show();
}

void executarLed_Inicio() {
  for (int i = 0; i <= 6; i++) {
    executarLed_Cor(255, 0, 0);
    delay(150);
    executarLed_Cor(50, 0, 0);
    delay(150);
  }

  fitaLED.clear();
  fitaLED.show();
}

void executarLed_Standby() {
  if (contLedStandby >= 250) contLedStandbyOffset = -10;
  if (contLedStandby <= 0) contLedStandbyOffset = 10;
  contLedStandby = contLedStandby + contLedStandbyOffset;

  Serial << "contLedStandby: " << contLedStandby << endl;
  executarLed_Cor(0, 0, contLedStandby);
}

void executarLed_Cor(int r, int g, int b) {
  fitaLED.clear();

  for (int i = fitaLedPixelsIni; i < fitaLED.numPixels(); i++) {
    fitaLED.setPixelColor(i, r, g, b);
  }

  fitaLED.show();
}

void executarLed_Premio(uint8_t wait) {
  for (int j = 0; j < 256; j++) { // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < fitaLED.numPixels(); i = i + 3) {
        fitaLED.setPixelColor(i + q, Wheel( (i + j) % 255)); // turn every third pixel on
      }
      fitaLED.show();

      delay(wait);

      for (int i = 0; i < fitaLED.numPixels(); i = i + 3) {
        fitaLED.setPixelColor(i + q, 0); // turn every third pixel off
      }
    }
  }
}

void executarLed_Rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < fitaLED.numPixels(); i++) {
      fitaLED.setPixelColor(i, Wheel((i + j) & 255));
    }
    fitaLED.show();
    delay(wait);
  }
}

void executarPremio() {
  servoPremio1.write(servoPremioPosIni);
  delay(servoPremioDelay);
  servoPremio1.write(servoPremioPosIni - servoPremioOffset); // Vai para trás
  delay(servoPremioDelay * 1.5);
  servoPremio1.write(servoPremioPosIni + (servoPremioOffset * 1.5)); // Vai para frente
  delay(servoPremioDelay);
  servoPremio1.write(servoPremioPosIni);
  delay(servoPremioDelay);

  executarLed_Premio(30);
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

void testarServo() {
  beep();

  for (int i = 0; i <= 3; i++) {
    executarPremio();
  }
}

void testarLaser() {
  //beep();

  for (int i = 0; i <= 1; i++) {
    digitalWrite(pinLaser, HIGH);
    delay(100);
    digitalWrite(pinLaser, LOW);    
    delay(100);
  }
}

void testarFitaLED() {
  //beep();

  int r = random(0, 255);
  int g = random(0, 255);
  int b = random(0, 255);

  fitaLED.clear();
  fitaLED2.clear();

  for (int i = 0; i < fitaLED.numPixels(); i++) {
    fitaLED.setPixelColor(i, r, g, b);
  }
  fitaLED.show();

  for (int i = 0; i < fitaLED2.numPixels(); i++) {
    fitaLED2.setPixelColor(i, r, g, b);
  }
  fitaLED2.show();
}

void imprimirInfosSetup() {
  Serial << "Caixa Magica!" << endl << endl;

  // Info Servos
  Serial << "Servo premio: " << servoPremio1.read() << endl;

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
  
  Serial << "Iniciado." << endl << endl;
}

void imprimirInfosLoop() {
  if ((ultDistCM > 0) && (DEBUG_DIST)) {
    Serial << "Distancia CM: " << ultDistCM << endl;
  }

  //Serial << "Volume: " << ultMic << endl;
}

void verificarJoystick() {
  ultJSCmd2 = ultJSCmd;
  ultJSCmd = ' ';
  ultJSX = lerAnalogico(pinJSX);
  ultJSY = lerAnalogico(pinJSY);

  if (ultJSX >= 0 & ultJSX <= 200) { ultJSCmd = '<'; }
  if (ultJSX >= 700 & ultJSX <= 1024) { ultJSCmd = '>'; }

  if (ultJSY >= 0 & ultJSY <= 200) { ultJSCmd = '^'; }
  if (ultJSY >= 700 & ultJSY <= 1024) { ultJSCmd = 'v'; }

  if (ultJSCmd != ultJSCmd2) { 
      Serial << "Joystick:  Cmd: " << ultJSCmd << ", X: " << ultJSX << ", Y: " << ultJSY << endl;
      if (ultJSCmd != ' ') { beep(); }
  }
}

