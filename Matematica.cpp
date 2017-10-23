
/********************************************************************
  Caixa Mágica • Módulo Matemática
*********************************************************************/

#include "Arduino.h"
#include "Matematica.h"

// ==================================================================
// Constantes/Variáveis
// ==================================================================

const int distIni = 3;
const int distFim = 70;
const int nivelMax = 4;

int _digito;
int _ultDigito;

int _nivel = 1;
int _pontos;
int _pontosPremio = 5;
int _num1;
int _num2;
char _sinal;
int _resultado;
String _respostaCerta;
String _respostaAtual;
String _respostaAtualTela;

// ==================================================================
// Métodos
// ==================================================================

Matematica::Matematica() {}

void sortearExpressao() {
  char sinais[] = {'+', '-', 'x', '/'};
  int maxSinais;
  int maxNum = 9;
  float r = 0;
  
  // Ajusta variáveis de parâmetros conforme o nível
  switch (_nivel) {
    case 1:
      maxNum = 5;
	    maxSinais = 1;
      break;
    case 2:
      maxNum = 6;
	    maxSinais = 2;
      break;
    case 3:
      maxNum = 9;
	    maxSinais = 3;
      break;
    case 4:
      maxNum = 9;
	    maxSinais = 4;
      break;
    default:;  
  }  
  
  while (true) {
    _num1 = random(1, maxNum+1);
    _num2 = random(1, maxNum+1);
    _sinal = sinais[random(0, maxSinais)];
    
    switch (_sinal) {
    case '+':
      r = _num1 + _num2;
      break;
    case '-':
      r = _num1 - _num2;
      break;
    case 'x':
      r = _num1 * _num2;
      break;
    case '/':
      r = _num1 / _num2;
      break;
    default:;  
    }
    
	  // Deve estar entre 0 e 99 e não ter fração
    if ((r > 0) && (r <= 99) && (r - (int)r == 0)) break;
  }
  
  _respostaCerta = String((int)r);
}

void formatarRespostaAtual() {
  _respostaAtualTela = (_respostaAtual + String(_ultDigito) + F("___")).substring(0, _respostaCerta.length());
}

void Matematica::jogar() {
  randomSeed(millis());
  sortear();
}

void Matematica::sortear() {
  // Reseta variáveis
  _digito = -1;
  _ultDigito = -1;
  _resultado = 0;  
  _respostaAtual = "";
  _respostaAtualTela = "";
  
  // Faz a expressão
  sortearExpressao();
}

bool Matematica::processarSonar(int ultDistCM) {
  bool mudou = false;
  int dist = 0;
  
  // Verifica se mudou a distância e mapeia para um número de 0 a 9
  if (ultDistCM >= distIni && ultDistCM <= distFim) { 
    if (abs(dist - ultDistCM) >= 1) {
	    dist = ultDistCM;
	    _digito = map(dist, distIni, distFim, 0, 10);
      if (_digito > 9) { _digito = 9; }
    }
  } else {
    //dist = 0;
    //_digito = -1;
  }
  
  if (_digito != _ultDigito) {
	  mudou = true;
    _ultDigito = _digito;
    formatarRespostaAtual();
  }
  
  return mudou;
}

int Matematica::processarResultado() {
  int dif;
  
  if (_ultDigito >= 0) { _respostaAtual = _respostaAtual + String(_ultDigito); }
  dif = _respostaCerta.length() - _respostaAtual.length();
  
  // Se terminou de responder
  if (dif == 0) {
      if (_respostaAtual == _respostaCerta) {
         _resultado = 1;
      } else {
         _resultado = -1;
      }
  } else {
    // Espera pelo próximo dígito da resposta
    _ultDigito = 0;
  }
  
  formatarRespostaAtual();  
  return _resultado;
}

// ==================================================================
// Propriedades
// ==================================================================

int Matematica::getResultado() {
  return _resultado;
}

int Matematica::getNivel() {
  return _nivel;
}

void Matematica::setNivel(int nivel) {
  _nivel = nivel;
}

void Matematica::aumentarNivel() {
  _nivel++;
  if (_nivel > nivelMax) { _nivel = 1; }
}

int Matematica::getNumero1() {
  return _num1;
}

int Matematica::getNumero2() {
  return _num2;
}

char Matematica::getSinal1() {
  return _sinal;
}

int Matematica::getUltDigito() {
  return _ultDigito;
}

void Matematica::aumentarUltDigito(int qtd) {
  _ultDigito = _ultDigito + qtd;
  if (_ultDigito < 0) { _ultDigito = 9; }
  if (_ultDigito > 9) { _ultDigito = 0; }
  formatarRespostaAtual();
}

void Matematica::setUltDigito(int num) {
  _ultDigito = num;
  formatarRespostaAtual();
}

String Matematica::getRespostaCerta() {
  return _respostaCerta;
}

String Matematica::getRespostaAtual() {
  return _respostaAtual;
}

String Matematica::getRespostaAtualTela() {
  return _respostaAtualTela;
}

