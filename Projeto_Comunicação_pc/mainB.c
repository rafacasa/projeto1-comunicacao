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
#include <string.h>

// configuracao de uma porta para 4800 8-N-1
#define PORTA_COM "COM5"         // porta de comunicao
#define BAUDRATE 9600          // taxa em bps
#define PARIDADE NOPARITY       // paridade (NOPARITY, PARITY_EVEN, PARITY_ODD
#define STOPBITS ONESTOPBIT     // stop bits (ONESTOPBIT, TWOSTOPBITS, ONE5STOPBITS)

// metodo principal
int main(int argc, char *argv[])
{  
  // cria uma estrutura para manipular uma porta serial 
  SerialPort s;
  // SerialPort s1;     // cria uma outra porta serial

  // abre a porta de comunicacao com as definicoes
  if(!serialOpen(&s, PORTA_COM,BAUDRATE,8,PARIDADE,STOPBITS))
  {
      printf("Erro de inicializacao.\n");
      system("PAUSE"); return -1;
  }  

  // primeiro envia uns caracteres pela interface
  /*printf("Digite uma tecla para enviar os caracteres abaixo:\n");
  system("PAUSE");
  serialPutc(&s,'a');
  serialPutc(&s,'b');
  serialPutc(&s,'c');
  serialPutc(&s,'d');
  serialPutc(&s,'\r');
  serialPutc(&s,'\n');

  // agora envia uma frase
  printf("Digite uma tecla para enviar uma frase...\n");
  system("PAUSE");
  serialPuts(&s,"Enviei uma frase!");
  serialPutc(&s,'\r');
  serialPutc(&s,'\n');
  */

  // agora espera o outro PC me enviar uma string
  printf("\nEstou esperando o outro pc enviar uma string...\n");        
  char string[200];
  serialGetLine(&s,string);
  printf("String recebida: \n%s\n",string);
  
  // Sleep(1000);
  
  /*
  printf("\nEnviando de volta...\n");  
  serialPutc(&s,'\n');     
  serialPuts(&s,"Voce me enviou: ");
  serialPuts(&s,string);
  serialPutc(&s,'\n');
  
  // agora espera o outro PC me enviar um caractere
  serialPuts(&s,"Tecle um caractere: ");
  char c;
  c = serialGetc(&s);	// esta funcao aguarda um caractere pelo tempo maximo do timeout. se nao receber nada dentro do tempo, ela retorna 0
  printf("Caractere recebido: %c",c);
  
  // agora espera o outro PC me enviar um caractere com a funcao que retorna 1 se houve timeout
  serialPuts(&s,"Tecle um caractere: ");  
  if(serialGetcTimeout(&s,&c)) // esta funcao retorna 1 se ocorreu timeout. salva na variavel c o caractere recebido
  {
  	  printf("Ocorreu timeout.");
  }else{
      printf("Caractere recebido: %c",c);	
  }  
  */
  
  // finaliza a porta de comunicacao
  if(!serialClose(&s))
  {
      printf("Erro de finalizacao.\n");
      system("PAUSE"); return -1;
  }

  // encerra o programa
  system("PAUSE");
  return 0;
}
