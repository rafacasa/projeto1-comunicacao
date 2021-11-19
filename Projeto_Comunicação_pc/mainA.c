//****************************************************************************
// Comunicacao de Dados
// Professor: Gustavo Kunzel
// Codigo de exemplo para acesso a porta serial
// main.c - contem o metodo principal
// serial.h - biblioteca com as funcoes serial
// serial.c - declaracao das funcoes
//****************************************************************************

// bibliotecas de funcoes
#include "serial.h"
#include "crc16.h"
#include <string.h>
#include <stdio.h>
#include <conio.h>

// configuracao de uma porta para 4800 8-N-1
#define PORTA_COM "COM3"         // porta de comunicao
#define BAUDRATE 9600          // taxa em bps
#define PARIDADE NOPARITY       // paridade (NOPARITY, PARITY_EVEN, PARITY_ODD
#define STOPBITS ONESTOPBIT     // stop bits (ONESTOPBIT, TWOSTOPBITS, ONE5STOPBITS)

void flush_in() {
    int ch;
    do {
        ch = fgetc(stdin);
    } while (ch != EOF && ch != '\n');
}

// metodo principal
int main(int argc, char *argv[])
{

  char c;
  unsigned short setpoint=50, limite=80, aux, saida, temp, i;
  unsigned short crc16_result;
  unsigned char buffer[10];
  unsigned char recebido[10];
  // cria uma estrutura para manipular uma porta serial
  SerialPort s;
  // SerialPort s1;     // cria uma outra porta serial

   //abre a porta de comunicacao com as definicoes
  if(!serialOpen(&s, PORTA_COM,BAUDRATE,8,PARIDADE,STOPBITS))
  {
      printf("Erro de inicializacao.\n");
      system("PAUSE"); return -1;
  }

  while(c != 'x'){
  	
  	system("cls");
  

    printf("Digite a opcao desejada (x para encerrar):\n");
    printf("1. Definir setpoint\n");
    printf("2. Definir limite\n");
    printf("3. Monitorar variaveis\n");
    printf("Opcao: ");

    c = getchar();
    flush_in();

    switch (c){
    case '1':
    	while(1){
			system("cls");
			printf("Limite: %d",limite);
	        printf("\nSetpoint: %d",setpoint);
	        printf("\n\n0. Retornar\n\n");
	        printf("Novo setpoint: ");
	        scanf("%d", &aux);
	        flush_in();
	        
	        if(aux==0) {
	        	system ("cls"); 
				break;
			}
	        else if(aux>0 && aux<=80) {
				setpoint=aux;
				
				buffer[0] = 0x0A;
				buffer[1] = 0xC4;
				buffer[2] = setpoint;
				
				crc16_result = crc16_ccitt(buffer,3);
				
				serialPutBytes(&s, buffer, 3);
				serialPutc(&s, crc16_result>>8);
				serialPutc(&s, crc16_result & 0xFF);
			}
				
	        else {
	        	printf("\nSetpoint invalido\n");
	        	system("PAUSE");
			}
        
    	}
        break;
    case '2':
        while(1){
	        system("cls");
			
	        printf("Limite: %d",limite);
	        printf("\nSetpoint: %d",setpoint);
	        printf("\n\n0. Retornar\n\n");
	        printf("Novo limite: ");
	        scanf("%d", &aux);
	        flush_in();
	        
	        if(aux==0) {
	        	system ("cls"); 
				break;
			}
	        else if(aux>setpoint && aux<=150) {
				limite=aux;
				
				buffer[0] = 0x0A;
				buffer[1] = 0xF1;
				buffer[2] = limite;
				
				crc16_result = crc16_ccitt(buffer,3);
				
				serialPutBytes(&s, buffer, 3);
				serialPutc(&s, crc16_result>>8);
				serialPutc(&s, crc16_result & 0xFF);
				
			}
	        else {
	        	printf("\nLimite invalido\n");
	        	system("PAUSE");
			}
	        
	    	}
        break;
    case '3':
    	while(1){
    		
    		if(kbhit())
				break;
    		
    		if(serialGetc(&s) == 0x3F){
				recebido[0] = 0x3F;
				i=1;
	    		while(serialBytesReceived(&s) < 7){
	    			recebido[i] = serialGetc(&s);
	    			i++;
				}
			}
			
			crc16_result = crc16_ccitt(recebido,5);
			
			if(crc16_result == (recebido[5]<<8) + recebido[6]){
			
    	
			setpoint = recebido[1];
			limite = recebido[2];	
			temp = recebido[3];
			saida = recebido[4];
			
			}
			
			system("cls");
			
	        printf("Variaveis: ");
	        printf("\nLimite: %d",limite);
	        printf("\nSetpoint: %d",setpoint);
	        printf("\nTemperatura: %d",temp);
	        printf("\nMotor: %d", saida);
	        printf("\nQualquer tecla para Retornar\n");
	        }
        break;
    case 'x':
    	break;
    default:
    	printf("Erro, tente de novo\n");
    	system("PAUSE");
    
    }
    
}


   //finaliza a porta de comunicacao
  if(!serialClose(&s))
  {
      printf("Erro de finalizacao.\n");
      system("PAUSE"); return -1;
  }

  // encerra o programa
  system("PAUSE");
  return 0;
}
