#include <stdio.h>
#include <stdlib.h>
#include "msp430g2553.h"
#include "lcd.h"
#include "uart.h"

void Start(void);
void receberInfo(void);
void enviarValorPelaUART(void);
void zerarTimer(void);
void ligarBacklight(void);
void desligarBacklight(void);
void atualizarDisplay(void);
void setIntensidade(void);
void displayClear(void);

char intensidade = 0;
char luminosidade = 0;
char leds = 0;
unsigned int tempo_limite = 50;
char rx[10];                            //armazena recepcao do pc (máximo 10chars)
char mensagem_tx[5];                   //armazena mensagem de texto para tx

void main(void)
{ 
  WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer
  
  Start();

    for (;;)
    {
        atualizarDisplay();
      
        if (!(P1IN & BIT3))             // se apertar o botao, arruma a intensidade
        {
            setIntensidade();
            __delay_cycles(281300);
        }
    }
}

void Start(void)
{
    DCOCTL=CALDCO_1MHZ;                       //Calibra para 1MHz
    BCSCTL1=CALBC1_1MHZ;
    
    //Intervalo=100ms. Se clock=1MHz, T=1us. Entao: contagem=50000 divisor=2
    //50000 x 1us/2 = 50000 x 2us = 100000us = 100ms
    
    TA0CTL |= TASSEL_2|ID_1|MC_1;            // inicia o timer
    TA0CCR0=50000-1;
    TA0CCTL0=CCIE;
    
    _BIS_SR(GIE);                     //Habilita interrupcao global

    InitLCD();                      // inicia o display LCD

    P1REN |= BIT3;                  // liga o resistor interno de P1.3 para o botao
    P1OUT |= BIT3;                 // modo pull-up
    
    P1DIR |= BIT4;                  // coloca P1.4 como saida para o backlight do display (base do transistor)
    
    ConfigUART(9600);    // 9600 = baudrate
}

void receberInfo(void)
{
    // pega o que tiver no UART e salva em leds e luminosidade
   UARTReceive(&rx[0],0);     //Salva em rx dado recebido. Se nao recebeu, rx=0
   
   if (rx[0] >= '0' && rx[0] <= '1') leds = rx[0] - '0';
   if (rx[1] >= '0' && rx[1] <= '5') luminosidade = (rx[1] - '0') * 20;
}

void enviarValorPelaUART(void)
{
    // enviar a intensidade pelo UART

    char toSend = intensidade / 20;
    toSend += '0';
  
    sprintf(mensagem_tx,"%c\n\r", toSend);
    UARTSend(mensagem_tx);
}

void zerarTimer(void)
{
    // zerar o timer que desliga o backlight
    tempo_limite = 50;
}

void ligarBacklight(void)
{
    // liga o backlight
    P1OUT |= BIT4;
}

void desligarBacklight(void)
{
    // desliga o backlight
    P1OUT &= ~BIT4;
}

// atualiza as informacoes do display
void atualizarDisplay(void)
{
    char lastLuminosidade = luminosidade;
    char lastLeds = leds;
    
    receberInfo();
  
    if (luminosidade == lastLuminosidade && leds == lastLeds) return;     // nao houve mudancas

    displayClear();
    ligarBacklight();
    
    LCDPrintXYStr(1, 1, "Luminosidade:");
    LCDPrintXYVal(14, 1, luminosidade);
    LCDPrintXYStr(1, 2, (leds ? (unsigned char*)"Leds ON" : (unsigned char*)"Leds OFF"));
    
    zerarTimer();
}

void setIntensidade(void)
{
  displayClear();
  ligarBacklight();
  
  intensidade += 20;
  if (intensidade > 100) intensidade = 0;
  
  LCDPrintXYStr(1, 1, "Intensidade:");
  LCDPrintXYVal(1, 2, (int)intensidade);
  
  enviarValorPelaUART();
  zerarTimer();
}

void displayClear(void)
{
  LCDCmd(0x01);
  __delay_cycles(52500);
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

  desligarBacklight();         // se passou 5 segundos sem acao
}