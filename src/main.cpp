#include "mbed.h"

InterruptIn btt(p10);
DigitalOut led_vermelho(p11);
DigitalOut led_amarelo(p12);
DigitalOut led_azivis(p13);

Timeout timeout;
Ticker ticker;

int estado;
int estado2;
int cont;
int estado_btt;

enum STATE{ INICIO, DESLIGADO, AZIVIS, AMARELO,  VERMELHO, ALERTA, ON };

void MudarEstado();
void EstadoPadrivis();


void MudarAzivis(){
    printf("Azivis\n");
    led_azivis = 1;
    estado2 = estado;
    estado = AZIVIS;
    timeout.attach(&MudarEstado, 20);
}
void MudarVermelho(){
    printf("Vermelho\n");
    led_vermelho = 1;
    estado2 = estado;
    estado = VERMELHO;
    timeout.attach(&MudarEstado, 10);
}
void MudarAmarelo(){
    printf("Amarelo\n");
    led_amarelo = 1;
    estado2 = estado;
    estado = AMARELO;
    timeout.attach(&MudarEstado, 4);
}
void MudarAmarelo1Hz(){
    led_amarelo = !led_amarelo;
    ticker.attach(MudarAmarelo1Hz, 0.5);
}

void MudarAlerta(){
    printf("Alerta\n");
    estado2 = estado;
    estado = ALERTA;
    MudarAmarelo1Hz();
}
void Desliga(){
    printf("Desligado\n");
    estado2 = estado;
    estado = DESLIGADO;
    EstadoPadrivis();
}

void Contar(){ cont++; }

void IniciarCont(){
    printf("Inicio cont\n");
    timeout.detach();
    cont = 0;
    ticker.attach(Contar, 1);
}
void FinalizarContador(){
    printf("End cont\n");
    ticker.detach();
    estado_btt = ON;
    MudarEstado();
}

int main() {
    estado = INICIO;
    estado2 = INICIO;
    MudarEstado();
    
    btt.rise(&IniciarCont);
    btt.fall(&FinalizarContador);
    
    while(1) {
        wait(1);
        printf("btt Cont %d\n", cont);
    }
}

void MudarEstado(){
    EstadoPadrivis();
    if(estado == INICIO && estado2 == INICIO){
        printf("Vermelho init\n");
        led_vermelho = 1;
        estado2 = VERMELHO;
        estado = VERMELHO;
        timeout.attach(&MudarEstado, 10);
    }
    else if(estado_btt == DESLIGADO){
      estado == VERMELHO ? MudarAzivis() : estado == AZIVIS ? MudarAmarelo() : MudarVermelho();
    }
    else if(estado_btt == ON){
        estado_btt = DESLIGADO;
        if(estado == VERMELHO){
          cont>=3 && cont<=10 ?  MudarAlerta() : cont > 10 ? Desliga() : MudarVermelho();
        }
        else if(estado == AMARELO){
          cont>=3 && cont<=10 ?  MudarAlerta() : cont > 10 ? Desliga() : MudarAmarelo();
        }
        else if(estado == AZIVIS){
           cont>=3 && cont<=10 ?  MudarAlerta() : cont==20 || cont<=3 ? MudarAmarelo() : cont > 10 ?  Desliga() : MudarAzivis();
        }
        else if(estado == ALERTA){
            if(cont>=3 && cont<=10){
              estado2 == VERMELHO ?  MudarVermelho() : estado2 == AMARELO ? MudarAmarelo() : MudarAzivis();   
            }
            else if(cont > 10){
                Desliga();
            }
            else{
                estado = estado2;
                MudarAlerta();
            }
        }
        else if(estado == DESLIGADO){
          cont > 10 ? MudarVermelho() : Desliga();
        }
    }
}

void EstadoPadrivis(){
  led_vermelho = 0;
  led_amarelo = 0;
  led_azivis = 0;
  
  timeout.detach();
  ticker.detach();
}