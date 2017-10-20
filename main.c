#include "msp430g2553.h"
#include "lcd.h"

typedef unsigned char uchar;

// informacoes que receber do UART
typedef struct Node
{
    uchar luminosidade;
    uchar estado;
} node; 

node receberInfo(void);
void enviarValorPelaUART(void);
void zerarTimer(void);
void ligarBacklight(void);
void desligarBacklight(void);
void atualizarDisplay(node info);

uchar intensidade = 0;
uchar luminosidade = 0;
uchar estado = 0;

void main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    InitLCD();

    P1REN |= BIT3;
    P1OUT &= ~BIT3;

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