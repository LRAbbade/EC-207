#include "msp430g2553.h"
#include "lcd.h"
#include "timer.h"

typedef unsigned char uchar;

// informacoes que receber do UART
typedef struct Node
{
    uchar luminosidade;
    uchar estado;
} node; 

void Start(void);
node receberInfo(void);
void enviarValorPelaUART(void);
void zerarTimer(void);
void ligarBacklight(void);
void desligarBacklight(void);
void atualizarDisplay(node info);

uchar intensidade = 0;
uchar luminosidade = 0;
uchar estado = 0;
extern unsigned int tempo_limite;

void main(void)
{
    Start();

    for (;;)
    {
        // atualizar info do display de acordo com info recebida
        // se atualizar, ligar o backlight e zerar o timer
        atualizarDisplay(receberInfo());

        if (P1IN & BIT3)
        {
            ligarBacklight();

            intensidade += 20;
            if (intensidade > 100) intensidade = 0;

            enviarValorPelaUART();
            zerarTimer();
        }

        // checar se o timer do botao passou de 5 segundos, desligar o backlight
    }

    return;
}

void Start(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;
    DCOCTL=CALDCO_1MHZ;                       //Calibra para 1MHz
    BCSCTL1=CALBC1_1MHZ;
    //Intervalo=100ms. Se clock=1MHz, T=1us. Entao: contagem=50000 divisor=2
    //50000 x 1us/2 = 50000 x 2us = 100000us = 100ms
    ConfigTimer0(50000,2);                      //Inicia o relogio para funcionar
    _BIS_SR(GIE);                             //Habilita interrupcao global
    StartTimer0();                  // inicia o timer

    InitLCD();                      // inicia o display LCD

    P1REN |= BIT3;                  // liga o resistor interno de P1.3
    P1OUT &= ~BIT3;                 // modo pull-down
}

node receberInfo(void)
{
    // pega o que tiver no UART e devolve em node
}

void enviarValorPelaUART(void)
{
    // enviar a intensidade pelo UART
}

void zerarTimer(void)
{
    // zerar o timer que desliga o backlight
    tempo_limite = 50;
}

void ligarBacklight(void)
{
    // liga o backlight
}

void desligarBacklight(void)
{
    // desliga o backlight
}

// atualiza as informacoes do display
void atualizarDisplay(node info)
{
    if (luminosidade == info.luminosidade && estado == info.estado) return;     // nao houve mudancas

    luminosidade = info.luminosidade;
    estado = info.estado;

    ligarBacklight();
    zerarTimer();
}