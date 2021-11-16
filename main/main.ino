/*
  Trabalho 1 de Comunicação de Dados
*/

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

// Variáveis Globais
unsigned long timestamp_ultimo_acionamento_liga_desliga = 0;
unsigned long timestamp_ultimo_acionamento_reset_alerta = 0;


bool ligado = false;
bool alerta = false;


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
    }
    timestamp_ultimo_acionamento_reset_alerta = millis();
  }
}


void setup() {
  // Inicializar pinos
  pinMode(BOTAO_LIGA_DESLIGA, INPUT);
  pinMode(BOTAO_RESET_ALERTA, INPUT);
  pinMode(SENSOR_TEMPERATURA, INPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(LED_ALERTA, OUTPUT);

  attachInterrupt(BOTAO_LIGA_DESLIGA, liga_desliga, FALLING);
  attachInterrupt(BOTAO_RESET_ALERTA, reset_alerta, FALLING);

  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  unsigned int leitura_adc;
  int temperatura;

  leitura_adc = analogRead(SENSOR_TEMPERATURA);
  temperatura = map(leitura_adc, 0, 4096, VALOR_MINIMO_SENSOR_TEMPERATURA, VALOR_MAXIMO_SENSOR_TEMPERATURA);

  Serial.println(temperatura);
  
  if(ligado) {
    digitalWrite(MOTOR_PWM, HIGH);
  } else {
    digitalWrite(MOTOR_PWM, LOW);
  }
  if(alerta) {
    digitalWrite(LED_ALERTA, HIGH);
  } else {
    digitalWrite(LED_ALERTA, LOW);
  }
  delay(500);        
}
