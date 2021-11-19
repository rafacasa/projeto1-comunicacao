// Inclusão das Bibliotecas
#include "serial.h"
#include "crc16.h"
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>

// Configurações da conexão serial
#define PORTA_COM "COM3"
#define BAUDRATE 9600
#define PARIDADE NOPARITY
#define STOPBITS ONESTOPBIT


// Função para apagar os resíduos da leitura do teclado
void flush_in() {
	int ch;
	do {
		ch = fgetc(stdin);
	} while (ch != EOF && ch != '\n');
}


// Função principal
int main(int argc, char *argv[])
{

	char c;
	unsigned short setpoint;
	unsigned short limite;
	unsigned short aux;
	unsigned short saida;
	unsigned short temp;
	unsigned short i;
	unsigned short crc16_result;
	unsigned char buffer[10];
	unsigned char recebido[10];
	SerialPort s;


	// Abrir a porta serial
	if(!serialOpen(&s, PORTA_COM,BAUDRATE,8,PARIDADE,STOPBITS))
	{
		printf("Erro de inicializacao.\n");
		system("PAUSE"); return -1;
	}

	// Fazer uma consulta ao microcontrolador para obter os valores de setpoint e alerta configurados no momento
	buffer[0] = 0x0A;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0xC7;
	buffer[4] = 0xC1;
	serialPutBytes(&s, buffer, 5);

	if(serialGetc(&s) == 0x3F){
		recebido[0] = 0x3F;
		i=1;
		while(serialBytesReceived(&s)){
			recebido[i] = serialGetc(&s);
			i++;
		}
	}

	crc16_result = crc16_ccitt(recebido,5);

	if(crc16_result == (recebido[5]<<8) + recebido[6]){

		setpoint = recebido[1];
		limite = recebido[2];	
	}

	// Loop do menu
	while(c != 'x'){

		system("cls");


		printf("Digite a opcao desejada (x para encerrar):\n");
		printf("1. Definir setpoint\n");
		printf("2. Definir limite\n");
		printf("3. Monitorar variaveis\n");
		printf("Opcao: ");

		c = getchar();
		flush_in();

		// Verificação da escolha do menu
		switch (c){
			// Mudar setpoint
			case '1':
				while(1){
					system("cls");
					printf("Limite: %d", limite);
					printf("\nSetpoint: %d",setpoint);
					printf("\n\n0. Retornar\n\n");
					printf("Novo setpoint: ");
					scanf("%hu", &aux);
					flush_in();

					if(aux==0) {
						system ("cls"); 
						break;
					}
					else if(aux>0 && aux<=limite) {
						setpoint=aux;
						buffer[0] = 0x0A;
						buffer[1] = 0xC4;
						buffer[2] = setpoint;
						crc16_result = crc16_ccitt(buffer,3);
						serialPutBytes(&s, buffer, 3);
						serialPutc(&s, crc16_result>>8);
						serialPutc(&s, crc16_result & 0xFF);
					} else {
						printf("\nSetpoint invalido\n");
						system("PAUSE");
					}

				}
				break;
			// Mudar limite alerta
			case '2':
				while(1){
					system("cls");

					printf("Limite: %d",limite);
					printf("\nSetpoint: %d",setpoint);
					printf("\n\n0. Retornar\n\n");
					printf("Novo limite: ");
					scanf("%hu", &aux);
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
			// Solicitar os dados do arduino
			case '3':
				while(1){

					if(kbhit()) {
						flush_in();
						break;	
					}


					buffer[0] = 0x0A;
					buffer[1] = 0x00;
					buffer[2] = 0x00;
					buffer[3] = 0xC7;
					buffer[4] = 0xC1;
					serialPutBytes(&s, buffer, 5);

					if(serialGetc(&s) == 0x3F){
						recebido[0] = 0x3F;
						i=1;
						while(serialBytesReceived(&s)){
							recebido[i] = serialGetc(&s);
							i++;
						}
					}

					crc16_result = crc16_ccitt(recebido,5);

					if(crc16_result == (recebido[5]<<8) + recebido[6]){

						setpoint = recebido[1];
						limite = recebido[2];	
						temp = recebido[3];
						saida = recebido[4] * 100 / 255;

					}

					system("cls");

					printf("Variaveis: ");
					printf("\nLimite: %d",limite);
					printf("\nSetpoint: %d",setpoint);
					printf("\nTemperatura: %d",temp);
					printf("\nMotor: %d", saida);
					printf("\nPressione Enter para Retornar\n");
					Sleep(1000);
				}
				break;
			// Fechar o programa
			case 'x':
				break;
			default:
				printf("Erro, tente de novo\n");
				system("PAUSE");

		}

	}


	// Fechar a porta serial
	if(!serialClose(&s))
	{
		printf("Erro de finalizacao.\n");
		system("PAUSE"); return -1;
	}

	// 
	system("PAUSE");
	return 0;
}
