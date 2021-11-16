/*
  Trabalho 1 de Comunicação de Dados
*/

// Pinagem das funções
#define BOTAO_LIGA_DESLIGA 32
#define BOTAO_RESET_ALERTA 22
#define MOTOR_PWM 12
#define LED_ALERTA 13

//Configuraçao de tempo entre acionamentos de botões para o debounce (em ms)
#define TEMPO_DEBOUNCE 100 


// Variáveis Globais
unsigned long timestamp_ultimo_acionamento_liga_desliga = 0;


bool ligado = false;


// Função chamada pela interrupção acionada pelo botão liga desliga
void IRAM_ATTR liga_desliga() {
  // Debounce por software
  if ( (millis() - timestamp_ultimo_acionamento_liga_desliga) >= TEMPO_DEBOUNCE ) {
    ligado = !ligado;
    timestamp_ultimo_acionamento_liga_desliga = millis();
  }
}


void setup() {
  // Inicializar pinos
  pinMode(BOTAO_LIGA_DESLIGA, INPUT);
  pinMode(BOTAO_RESET_ALERTA, INPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(LED_ALERTA, OUTPUT);

  attachInterrupt(BOTAO_LIGA_DESLIGA, liga_desliga, FALLING);
}

// the loop function runs over and over again forever
void loop() {
  if(ligado) {
    digitalWrite(MOTOR_PWM, HIGH);
  } else {
    digitalWrite(MOTOR_PWM, LOW);
  }
  delay(1000);        
}
