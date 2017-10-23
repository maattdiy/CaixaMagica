
/********************************************************************
  Caixa Mágica • Módulo Quatro em Linha
*********************************************************************/

#include "Arduino.h"
#include "QuatroLinha.h"

// ==================================================================
// Constantes/Variáveis
// ==================================================================

int idJogador = 0;
int posPeca = 0;
int idVencedor;

const int gridL = 15;
const int gridA = 7;
int grid[gridL][gridA];

// ==================================================================
// Métodos
// ==================================================================

QuatroLinha::QuatroLinha() {}

void QuatroLinha::jogar() {
  idJogador = 0;
  idVencedor = 0;
  posPeca = -1;
}

int QuatroLinha::getResultado() {
  
}

void verificarFim() {
  int seguidas[8];
  int id;
  
  for (int x = 1; x <= gridL; x++) {
    for (int y = 1; y <= gridA; y++) {
    id = grid[x, y];
    if (id != 0) {
        seguidas[id]++;
    if (seguidas[id] = 4) {
      idVencedor = id;
          break;
    };
    }
  }
}
}

void QuatroLinha::processarResposta(int col) {

	// Verifica se a peça pode descer
	for (int y = 1; y <= gridA; y++) {
      if (grid[posPeca, y] == 0) {
        grid[posPeca, y] == idJogador;
		    posPeca = -1;
		if (idJogador == 2) { idJogador = 1; } else { idJogador = 2; }
	  }
	};
	
  verificarFim();
}

