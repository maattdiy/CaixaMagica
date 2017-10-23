#ifndef Matematica_h
#define Matematica_h

#include "Arduino.h"

class Matematica
{
  public:
  Matematica();
  void jogar();
  void sortear();
  bool processarSonar(int ultDistCM);
  int processarResultado();
  int getUltDigito();
  void aumentarUltDigito(int qtd);
  void setUltDigito(int num);
  int getResultado();
  int getNivel();
  void setNivel(int nivel);
  void aumentarNivel();
  int getNumero1();
  int getNumero2();
  char getSinal1();
  String getRespostaAtual();
  String getRespostaAtualTela();
  String getRespostaCerta();
  private:
};

#endif

