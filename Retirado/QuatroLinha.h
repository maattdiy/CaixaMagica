#ifndef QuatroLinha_h
#define QuatroLinha_h

#include "Arduino.h"

class QuatroLinha
{
  public:
    QuatroLinha();
    void jogar();
    void processarResposta(int col);
    int getResultado();
  private:
};

#endif

