//Arquivo Timer.c 
#include <msp430g2553.h>
#include "timer.h"
//Variaveis globais que podem ser usadas no main.c
unsigned int tempo_limite=50;            // 50 * 100ms = 5 segundos

/**************************** FUNCOES DO TIMER ********************************/
void ConfigTimer0(unsigned int intervalo,unsigned char divisor)
{
  TA0CTL=TA0CTL|TASSEL_2;
  switch(divisor)
  {
  case 1: TA0CTL=TA0CTL|ID_0;
          break;
  case 2: TA0CTL=TA0CTL|ID_1;
          break;
  case 4: TA0CTL=TA0CTL|ID_2;
          break;
  case 8: TA0CTL=TA0CTL|ID_3;
          break;          
  }
  TA0CCR0=intervalo-1;
  TA0CCTL0=CCIE;
}

void StartTimer0(void)
{
  TA0CTL=TA0CTL|MC_1;     //inicia contagem modo crescente
}

void StopTimer0(void)
{
  TA0CTL=TA0CTL&(~MC_3);  //zera o campo MC para parar o timer
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void timer (void)
{
  TA0CCTL0&=~CCIFG;

  if(tempo_limite>0) 
  {
    tempo_limite--;    //decrementa timeout
    return;
  }

  desligarBacklight();
}
