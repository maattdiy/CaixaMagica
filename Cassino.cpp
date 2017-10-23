
/********************************************************************
  Caixa Mágica •  Módulo Cassino
*********************************************************************/

#include "Arduino.h"
#include "Cassino.h"

// ==================================================================
// Constantes/Variáveis
// ==================================================================

#define maxGiros 15
int giros = 0;
int _resultadoCassino;
char colunas[3];

#define maxPecas 3
char pecas[] = {'X', 'O', '7', '?'};

// ==================================================================
// Métodos
// ==================================================================

Cassino::Cassino() {}

void sortearLinha() {
  int y;
  
  for (int i = 0; i < 3; i++) {
    y = random(0, maxPecas);
    colunas[i] = pecas[y];
  }
}

void Cassino::jogar() {
  randomSeed(millis());
  
  giros = 0;
  _resultadoCassino = 0;
}

// ==================================================================
// Propriedades
// ==================================================================

bool Cassino::processarResultado() {
  if ( _resultadoCassino != 0) { return false; }
  
  if (giros == maxGiros) { 
    _resultadoCassino = (colunas[0] == colunas[1] & colunas[0] == colunas[2]) ? 1 : -1;
    return false;
  }
  
  sortearLinha();
  //delay(((int)pow(2, giros) * 50));
  delay(200);
  
  giros++;
  return true;
}

char Cassino::getColuna(int i) {
  return colunas[i];
}

int Cassino::getResultado() {
  return _resultadoCassino;
}

