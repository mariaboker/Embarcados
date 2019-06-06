/******************************************************************************************/
/*                                                                                        */
/* EA 076 - Laboratório de Sistemas Embarcados                                            */
/* Projeto 2:  CONTROLADOR DE VENTILADOR DE TETO                                          */
/*                                                                                        */
/* Grupo: Victor Perez,148122 e Maria Kersanach, 156571                                   */
/* Entrega em 05 de Junho de 2019                                                         */
/*                                                                                        */
/* Link para video: encurtador.com.br/MP248                                               */
/* Link para roteiro: encurtador.com.br/MP248                                             */
/*                                                                                        */
/******************************************************************************************/

/* Projeto 2: CONTROLADOR DE VENTILADOR DE TETO   */


#include <string.h>
#include <Wire.h>   // para I2C
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 13, 4, 5, 6, 7);
//RS - pino 12
//Enable - pino 13
//DB7 - pino 7
//DB6 - pino 6
//DB5 - pino 5
//DB4 - pino 4


char last; // ultimo caractere lido na entrada serial
float rpm; // valor estimado para velocidade em rpm
int pinH0 = 10;   // pinos 10 PWM
int pinH1 = 11;   // pinos 11 PWM
int pinIn = 2; // pino de input do sensor de luz do motor
int pinPWM = 9;  // enable, Usa pwm sim pcausa do setvel
String velo; // string that will receive the new velo
int aux = 0; //variavel para alternar entre displays
int atual = 0; // indica sentido atual de rotacao do motor
int Testvel=0;
int countRotation = 0;  //variável para contar numero de pulsos do encoder
int timeBase = 0; // variavel para delimitar tempo na contagem de rpm
int pulseCount = 0; // flag que indica que a contagem de rpm deve ser realizada (baseia-se no timeBase)

char buffer[9];   // input serial
int vel[3];       // valor de velocidade lida no input
int k = 0;
int i = 0;
int j = 2;
int fim = 0;
int erroCod = 0;

int dutyCycle = 0;

int uni=0;
int dez=0;
int cent=0;
int mil=0;
int x = 0;
int y = 0;
int z = 0;
int w = 0;

char c;
int parado = 1;
int setvel = 0;


// Configuracao Temporizador 0 (8 bits) para gerar interrupcoes periodicas a cada 2ms no modo Clear Timer on Compare Match (CTC)
void configuracao_Timer0(){
  TCCR0A = 0x02;
  OCR0A = 124;
  TIMSK0 = 0x02;
  TCCR0B = 0x04;
}

ISR(TIMER0_COMPA_vect) {
  // I2C
  aux++;      // alterna entre displays (0, 1, 2, 3)
  if (aux > 3){
    aux = 0;
  }
  // interrupcao chamada a cada 2ms
  timeBase++;

  if (timeBase > 1000){ // base de tempo de 2s
    timeBase = 0;
    pulseCount = 1; // flag that enables pulse counting
  }
}

// Valores medidos com Osciloscopio para o Motor do Laboratorio
// FREQUENCIA MOTOR - 75Hz
// 2 PULSOS POR CICLO
// PERIODO DE 1 CICLO = 13,333ms

// Lê a velocidade do motor e exibe o valor de rpm calculado
void readVel() {
  attachInterrupt(digitalPinToInterrupt(pinIn), measureVel, RISING);
  
  if (pulseCount) {
    rpm = countRotation * 15;   // relacao linear de 400 ms = 60 pulsos, entao para 2s = 300 pulsos
    countRotation = 0;
    pulseCount = 0;

    if (rpm==0){
      parado=1;
    }
  }
}

void measureVel(){
  countRotation++;
}

/***  Sentido e Velocidade do Motor   ***/
// Configura sentido de movimento do motor (via ponte H) alternando entre
// os pinos da ponte H - pinH0 e pinH1
void setFlow (int flow) {
  //readVel();
  if (flow == 1) {                  // seta vent mode
    if (atual == 0 || atual == 1){
      digitalWrite(pinH0, HIGH);   
      digitalWrite(pinH1, LOW);   // manda sinal pwm para pino que faz sentido vent
      atual = 1;
      parado = 0;

    } else if (atual == 2){
      digitalWrite(pinH0, LOW);   
      digitalWrite(pinH1, LOW);

      if (parado == 1){
        atual = 0;                  //atualiza apenas quando o motor parar
      }
    }
  } else if (flow == 2) {           // seta exaust mode
    if (atual == 0 || atual == 2){
      digitalWrite(pinH0, LOW);   
      digitalWrite(pinH1, HIGH);  // manda sinal pwm para pino que faz sentido vent
      atual = 2;
      parado = 0;
    } else if (atual == 1){
      digitalWrite(pinH0, LOW);   
      digitalWrite(pinH1, LOW);
      if (parado == 1){
        atual = 0;                  //atualiza apenas quando o motor parar
      }
    }
  } else if (flow == 0) {           // pára motor
    digitalWrite(pinH0, LOW);   
    digitalWrite(pinH1, LOW);
    if (parado == 1){
      atual = 0;                  //atualiza apenas quando o motor parar
    }
  }
}


/***  LCD   ***/
// Funcao que configura estado inicial do display LCD
void init_lcd(){
  lcd.begin(16, 2);
  lcd.clear();   //Apaga o buffer e o display
  lcd.setCursor(0,0);  //Seta a posição do cursor
  lcd.print("PARADO");
  lcd.setCursor(0,1); //LINHA 1 COLUNA 0
  lcd.print("DC 000%");   //colunas de DUTYCYCLE - 3,4 e 5
  lcd.setCursor(8,1);
  lcd.print("RPM");   //colunas para rpm - 12,13,14,15
  lcd.display();
  Serial.println("fim init lcd");

}

// Exibe na segunda linha do visor o Dutycycle do motor
void LCDDuty (int mostra) {   //DUTY
  if (mostra == 100){
    lcd.setCursor(3, 1);   //posição de mostra DUTY
    lcd.print(mostra);

  } else if (mostra < 100 && mostra > 9){
    lcd.setCursor(3, 1);   //posição de mostra DUTY
    lcd.print(0);
    lcd.print(mostra);

  } else if (mostra <= 9) {
    lcd.setCursor(3, 1);   //posição de mostra DUTY
    lcd.print(0);
    lcd.print(0);
    lcd.print(mostra);

  } else {
    lcd.setCursor(6, 1);
    lcd.print("%");
  }
}

// exibe valor rpm no lcd
void LCDRPM(int mostra){
  if (mostra < 1000){
    lcd.setCursor(12, 1);  //posição de mostra RPM
    lcd.print(mostra);
    lcd.print("   ");
  } else {
    lcd.setCursor(12, 1);  //posição de mostra RPM
    lcd.print(mostra);
  }
}

// Exibe na primeira linha do visor o estado atual do motor
void LCDEstado (String mostra) {
  lcd.setCursor(0, 0);
  lcd.print(mostra);
  lcd.display();
  Serial.println("exibiu estado no lcd");
}


// trata entradas via serial
void serialEvent() {
  while (Serial.available()) { // Enquanto houverem bytes disponíveis;
    c = Serial.read(); // Lê byte do buffer serial;
    switch(c) {
      //case '\r':            // Marca o fim de um comando.
      //case '\n':
      case '*':
        fim = 1;
        if (i == 0) return;
        buffer[i] = 0;
        i = 0;
        j = 2;
        k = 0;

          if (fim == 1){
            if (buffer[0] == 'V' && buffer[1] == 'E' && buffer [2] == 'L'){  //  comando VEL
              Testvel = 0;

              while (k < 3){
                Testvel = Testvel + pow(10, k) * vel[j];
                j--;
                k++;
              }

              dutyCycle = Testvel;
              Testvel = Testvel * 255 / 100;
              analogWrite(pinPWM,Testvel);
              fim = 0;

            } else if (buffer[0] == 'V' && buffer[1] == 'E' && buffer [2] == 'N' && buffer [3] == 'T'){ // comando VENT
              setvel = 1;
              LCDEstado("VENTILADOR");
              Serial.println("OK VENT");

            } else if (buffer[0] == 'E' && buffer[1] == 'X' && buffer [2] == 'A' && buffer [3] == 'U' && buffer [4] == 'S' && buffer [5] == 'T'){ // comando EXAUST
              setvel = 2;
              LCDEstado("EXAUSTOR  ");
              Serial.println("OK EXAUST");    

            } else if (buffer[0] == 'P' && buffer[1] == 'A' && buffer [2] == 'R' && buffer [3] == 'A'){ // comando PARA
              setvel = 0;
              LCDEstado("PARADO   ");
              Serial.println("OK PARA");  

            } else if (buffer[0] == 'R' && buffer[1] == 'E' && buffer [2] == 'T' && buffer [3] == 'V' && buffer [4] == 'E' && buffer [5] == 'L'){ // comando RETVEL
              Serial.print("VEL: ");
              Serial.print(rpm); // valor medido no pwm
              Serial.println(" RPM");   

            } else Serial.print("ERRO: COMANDO INEXISTENTE\n"); // erro de comando nao identificado com *

        }
        

      break;

    // Adiciona caracter ao buffer se não estiver cheio.
      
      default:
        if (i < 9) {
          buffer[i] = c;
          if (i > 3) {
            vel[i - 4] = buffer[i] - 48;  // pega os digitos referentes ao valor de velocidade
            //if (*vel > 1 && *(vel+1) > 0) Serial.print("ERRO: PARAMETRO INVALIDO\n");
            //Serial.println(*vel);
            //Serial.println(*(vel+1));
          }

          if (buffer[i] == '\n' && buffer[i - 1] != '*') {
            //if (buffer[0] != 'V' || buffer[1] != 'E' || buffer [2] != 'N' || buffer [3] != 'T') Serial.print("ERRO: COMANDO INEXISTENTE\n"); // erro de comando nao identificado sem *
            //if ((buffer != "VENT*") && (buffer != "EXAUST") && (buffer != "")) Serial.print("ERRO: COMANDO INEXISTENTE\n"); // erro de comando nao identificado sem *

          }


          ++i;
        }
    }

  //buffer[0] = " ";
  //buffer[1] = " ";
  //buffer[2] = " ";
  //buffer[3] = " ";
  //buffer[4] = " ";
  }
}


void displayMode (int a,int b,int c,int d){
  if (aux == 0){      //algarismo unidade
      Wire.beginTransmission(63);
      Wire.write(a + 224);
      Wire.endTransmission(63);

  } else if (aux == 1){  // algarismo dezena
      Wire.beginTransmission(63);
      Wire.write(b + 208);
      Wire.endTransmission(63);

  } else if (aux == 2){  // algarismo centena
      Wire.beginTransmission(63);
      Wire.write(c + 176);
      Wire.endTransmission(63);

  } else if (aux == 3){  //algarismo milhar
      Wire.beginTransmission(63);
      Wire.write(d + 112);
      Wire.endTransmission(63);
  }
}


/***  ERROS   ***/
// Indicacao de erros de acordo com seu tipo e preparacao para nova entrada
void erro(int codigo) {
  if (codigo == 1) erroCod = 1;
  else if (codigo == 2) erroCod = 2;
  else if (codigo == 3) erroCod = 3;
}



void setup(){
  Serial.begin(9600);   // taxa de transmissao serial

  // Seta como output os pinos de pwm e pinos ponte H
  pinMode(pinPWM, OUTPUT);
  pinMode(pinH1, OUTPUT);
  pinMode(pinH0, OUTPUT);
  pinMode(pinIn, INPUT);

  // Deixa parado inicialmente
  digitalWrite(pinH0, LOW);
  digitalWrite(pinH1, LOW);

  Wire.begin();
  Wire.setClock(400000); // standard value is 100000
  Wire.beginTransmission(63);
  Wire.write(240);      //Desliga displays (b11110000)
  Wire.endTransmission(63);

  init_lcd();
  Serial.println("fim setup");

  cli();                  // Desabilita interrupções globais
  configuracao_Timer0();
  sei();                  // Habilita interrupcoes globais

  //TESTE MOTOR SETAR COMO 0 DEPOIS
  digitalWrite(pinH0, LOW);   
  digitalWrite(pinH1, LOW);
  analogWrite(pinPWM,0);
 
}



void loop() {
  readVel();
  x = (int)rpm;

  if (setvel==1){
    setFlow (1);
  }else if (setvel == 2){
    setFlow (2);
  }else if(setvel == 0){
    setFlow (0);
  }

  // Exibe valores no LCD
  LCDDuty(dutyCycle);
  LCDRPM(rpm);

//  if (c!='*'&& i!=0){
//    Serial.print(buffer[i-1]);
//  }

  // distribui os digitos para display
  uni = x % 10;
  y = x / 10;
  dez = y % 10;
  z = y / 10;
  cent = z % 10;
  w = z / 10;
  mil = w % 10;
  displayMode(uni, dez, cent, mil);
 
}
