#ifndef Cassino_h
#define Cassino_h

#include "Arduino.h"

class Cassino
{
  public:
    Cassino();
    void jogar();
    char getColuna(int i);
    int getResultado();
    bool processarResultado();
  private:
};

#endif

