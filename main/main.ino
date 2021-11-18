/*
  Trabalho 1 de Comunicação de Dados
*/

#include <Crc16.h>    // biblioteca de funcoes CRC-16

// Pinagem das funções
#define BOTAO_LIGA_DESLIGA 32
#define BOTAO_RESET_ALERTA 22
#define MOTOR_PWM 12
#define LED_ALERTA 13
#define SENSOR_TEMPERATURA 36

//Configuraçao de tempo entre acionamentos de botões para o debounce (em ms)
#define TEMPO_DEBOUNCE 500 

// Configuração dos valores mínimo e máximo do sensor de temperatura para a conversão de volts para graus celsius
#define VALOR_MINIMO_SENSOR_TEMPERATURA 2
#define VALOR_MAXIMO_SENSOR_TEMPERATURA 150

#define FREQUENCIA_PWM 1000
#define CANAL_PWM 0

// Variáveis Globais
unsigned long timestamp_ultimo_acionamento_liga_desliga = 0;
unsigned long timestamp_ultimo_acionamento_reset_alerta = 0;

uint8_t setpoint = 40;
uint8_t limite_alerta = 115;
uint8_t temperatura_lida;
uint8_t pwm_motor;

volatile unsigned int leitura_adc;
volatile bool leitura_temperatura_feita;

bool ligado = false;
bool alerta = false;

uint8_t buffer_leitura[5]; 

hw_timer_t * timer = NULL;

Crc16 crc;

// Função chamada pela interrupção acionada pelo botão liga desliga
void IRAM_ATTR liga_desliga() {
  // Debounce por software
  if ( (millis() - timestamp_ultimo_acionamento_liga_desliga) >= TEMPO_DEBOUNCE ) {
    if(!alerta) {
    ligado = !ligado;
    }
    timestamp_ultimo_acionamento_liga_desliga = millis();
  }
}

// Função chamada pela interrupção acionada pelo botão de reset do alerta
void IRAM_ATTR reset_alerta() {
  // Debounce por software
  if ( (millis() - timestamp_ultimo_acionamento_reset_alerta) >= TEMPO_DEBOUNCE ) {
    if(alerta) {
     alerta = false;
     digitalWrite(LED_ALERTA, LOW);
    }
    timestamp_ultimo_acionamento_reset_alerta = millis();
  }
}

void IRAM_ATTR leitura_temperatura_timer() {
  leitura_adc = analogRead(SENSOR_TEMPERATURA);
  leitura_temperatura_feita = true;
}


void enviar_dados() {
  uint8_t buffer_envio[7];
  uint16_t valor_crc;
  
  buffer_envio[0] = 0x3F;
  buffer_envio[1] = setpoint;
  buffer_envio[2] = limite_alerta;
  buffer_envio[3] = temperatura_lida;
  buffer_envio[4] = pwm_motor;
  
  valor_crc = crc.XModemCrc(buffer_envio,0,5);
  buffer_envio[5] = highByte(valor_crc);
  buffer_envio[6] = lowByte(valor_crc);

  Serial.write(buffer_envio, 7);
}


void setup() {
  // Inicializar pinos
  pinMode(BOTAO_LIGA_DESLIGA, INPUT);
  pinMode(BOTAO_RESET_ALERTA, INPUT);
  pinMode(SENSOR_TEMPERATURA, INPUT);
  pinMode(LED_ALERTA, OUTPUT);

  ledcSetup(CANAL_PWM, FREQUENCIA_PWM, 8);
  ledcAttachPin(MOTOR_PWM, CANAL_PWM);

  attachInterrupt(BOTAO_LIGA_DESLIGA, liga_desliga, FALLING);
  attachInterrupt(BOTAO_RESET_ALERTA, reset_alerta, FALLING);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &leitura_temperatura_timer, true);
  timerAlarmWrite(timer, 500000, true);
  timerAlarmEnable(timer);
  
  Serial.begin(115200);
}


void loop() {
  
  if (leitura_temperatura_feita) {
    temperatura_lida = map(leitura_adc, 0, 4096, VALOR_MINIMO_SENSOR_TEMPERATURA, VALOR_MAXIMO_SENSOR_TEMPERATURA);
  
    if(ligado) {
      if(temperatura_lida > limite_alerta) {
        alerta = true;
        ligado = false;
        pwm_motor = 0;
        digitalWrite(LED_ALERTA, HIGH);
      } else {
        if(temperatura_lida < setpoint) {
          pwm_motor = 255;
        } else {
          pwm_motor = map(temperatura_lida, setpoint, limite_alerta, 255, 0);
        }
      }
    } else {
      pwm_motor = 0;
    }
  
    ledcWrite(CANAL_PWM, pwm_motor);
  }


  if(Serial.available() >= 5) {
    Serial.readBytes(buffer_leitura, 5);
    if(buffer_leitura[0] == 0x0A) {
      if(crc.XModemCrc(buffer_leitura,0,5) == 0) {
        switch(buffer_leitura[1]) {
          case 0x00:
            enviar_dados();
            break;
          case 0xC4:
            Serial.write(0xC4);
            break;
          case 0xF1:
            Serial.write(0xF1);
            break;
          default:
            Serial.print("DEFAULT SWITCH");
            break;
        }
      }
    }
  }
}
