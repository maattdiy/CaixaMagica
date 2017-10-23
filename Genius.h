#ifndef Genius_h
#define Genius_h

#include "Arduino.h"

class Genius
{
  public:
    Genius();
    void jogar();
    void verificarResposta(int idx);
    void sortear();
    int getResultado();
    int getNivel();
    int getSequencia(int idx);
  private:
};

#endif

