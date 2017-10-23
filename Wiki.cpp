/********************************************************************
  Caixa Mágica • Módulo Wiki
*********************************************************************/

#include "Arduino.h"
#include "Wiki.h"

// ==================================================================
// Constantes/Variáveis
// ==================================================================

#define maxJogadores 6
#define maxCartas 16

byte _numJogadores;

byte jogadores[maxJogadores][2];
String carta;

// ==================================================================
// Métodos
// ==================================================================

Wiki::Wiki() {}

void Wiki::jogar() {
  // Zera variáveis
  _numJogadores = 0;
  
  for (byte i = 0; i <= maxJogadores-1; i++) {
    jogadores[i][0] = 0;
    jogadores[i][1] = 0;
  }
  
  randomSeed(millis());
  //sortear();
}

void setCartaCore(byte idx) {
  /*
  switch (idx) {
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
  default: carta = ""; break; 
  }
  //case 13: carta = F("Escada"); break;
  //case 14: carta = F("Agua"); break;
  //case 15: carta = F("Ponte"); break;  
  //case 16: carta = F("Estrela"); break;
  //case 12: carta = F("Bicicleta"); break;
  //case 9:  carta = F("Papai Noel"); break;
  //case 16: carta = F("Churrasco"); break;
  */
}

void Wiki::sortear() {
  byte sorteio = random(0, maxCartas);
  setCartaCore(sorteio);
}

void Wiki::sortear(byte idx) {
  setCartaCore(idx);
}

bool Wiki::adicionarJogador(byte idx) {
  byte pos = 255;
  
  for (byte i = 0; i <= _numJogadores-1; i++) {
    if (jogadores[i][0] == idx) {
      pos = i;
      break;
    }
  }
  
  // Não encontrou a peça, novo jogador
  if (pos == 255) {
    //if (pos > maxJogadores) { return false; }
    pos = _numJogadores;
    _numJogadores++;
    jogadores[pos][0] = idx;
    jogadores[pos][1] = 0;
    return true;
  } else {    
    //jogadores[pos][1]++;
    return false;
  }  
}

// ==================================================================
// Propriedades
// ==================================================================

void Wiki::adicionarPonto(byte idx) {
  for (byte i = 0; i <= _numJogadores-1; i++) {
    if (jogadores[i][0] == idx) {
      jogadores[i][1]++;
      break;
    }
  }
}

byte Wiki::getJogadorPontos(byte pos) {
  return jogadores[pos][1];
}

byte Wiki::getJogadorIdx(byte pos) {
  return jogadores[pos][0];
}

String Wiki::getCarta() {
  return carta;
}

byte Wiki::getJogadoresCount() {
  return _numJogadores;
}

byte Wiki::getMaxCartas() {
  return maxCartas;
}

