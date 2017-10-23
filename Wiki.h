#ifndef Wiki_h
#define Wiki_h

#include "Arduino.h"

class Wiki
{
  public:
  Wiki();
  void jogar();
  void sortear();
  void sortear(byte idx);
  byte getJogadoresCount();
  byte getJogadorIdx(byte pos);
  byte getJogadorPontos(byte idx);
  byte getMaxCartas();
  bool adicionarJogador(byte idx);
  void adicionarPonto(byte idx);
  String getCarta();
  private:
};

#endif

