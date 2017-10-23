
/********************************************************************
  Módulo Genius
*********************************************************************/

#include "Arduino.h"
#include "Genius.h"

// ==================================================================
// Constantes/Variáveis
// ==================================================================

int const maxCores = 4;

int sequencia[100] = {};
int nivel = -1;
int nivelResposta = -1;
int resultado = 0;

// ==================================================================
// Métodos
// ==================================================================

Genius::Genius() {}

void Genius::sortear() {
  int sorteio;
  
  sorteio = random(0, maxCores);
  sequencia[nivel] = sorteio;
  
  resultado = 0;
  nivelResposta = -1;
}

void Genius::jogar() {
  randomSeed(millis());
  
  nivel = 0;
  nivelResposta = -1;
  resultado = 0;
  
  sortear();
}

void executarVenceu() {
  nivel++;
  resultado = 1;
}

void executarPerdeu() {
  //nivel = 0;
  resultado = -1;
}

void Genius::verificarResposta(int idx) {
  nivelResposta++;
  
  // Se a resposta atual esta correta
  if (sequencia[nivelResposta] == idx) {
    // Se acertou tudo passa para o proximo
    if (nivelResposta == nivel) {
      executarVenceu();
    }
  } else {
    // Perdeu, errou a sequencia
    executarPerdeu();
  }
}

// ==================================================================
// Propriedades
// ==================================================================

int Genius::getResultado() {
  return resultado;
}

int Genius::getNivel() {
  return nivel;
}

int Genius::getSequencia(int idx) {
  return sequencia[idx];
}

