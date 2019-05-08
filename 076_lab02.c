/********************************************************************************************/
/*                                                                                          */
/* EA 076 - Laboratório de Sistemas Embarcados                                              */
/* Projeto 2: sadsadasd Modificar tudo                                                              */
/*                                                                                          */
/* Grupo: Victor Perez,148122 e Maria Kersanach, 156571                                     */
/* Entrega em XX de Abril de 2018                                                           */
/*                                                                        
/* Link para video: XX                                                 */
/* Link para roteiro: XX                                               */
/*                                                                                          */
/********************************************************************************************/ 

/* Projeto 2: XXX              */
/* EXPLICACAO BASICA XXX       */

#include <string.h>

#include <SoftwareSerial.h>
SoftwareSerial serial (0, 1); //pinos 0 e 1 para rx e tx

/// #include "SoftwareSerial.h"   // para bluetooth
/// SoftwareSerial bluetooth(2, 3);   // TX e RX usadas para modulo bluetooth

// #include <Adafruit_GFX.h>

//#include <Adafruit_PCD8544.h>
//Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);

#include <Wire.h>   // para I2C

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 13, 4, 5, 6, 7);
//RS - pino 12
//Enable - pino 13
//DB7 - pino 7
//DB6 - pino 6
//DB5 - pino 5
//DB4 - pino 4


String input; // string lida na entrada serial
char last; // ultimo caractere lido na entrada serial
int nchar = 0; // numero de caracteres lidos
int rpm; // valor estimado para velocidade em rpm
int pinH0 = 10;   // pinos 10 PWM
int pinH1 = 11;   // pinos 11 PWM
int pinIn = 9; // pino de input do sensor de luz do motor
int pinPWM;  // enable, Usa pwm sim pcausa do setvel - arranjar porta ?????????
String velo; // string that will receive the new velo



// Configuracao Temporizador 0 (8 bits) para gerar interrupcoes periodicas a cada 2ms no modo Clear Timer on Compare Match (CTC)
void configuracao_Timer0(){
  
  TCCR0A = 0x02;
  OCR0A = 124;
  TIMSK0 = 0x02;
  TCCR0B = 0x04;
}



/***    ***/

void setup(){
  Serial.begin(9600);   // taxa de transmissao serial
  /// bluetooth.begin(9600);

  // Seta como output os pinos de pwm e pinos ponte H
  pinMode(pinPWM, OUTPUT);
  pinMode(pinH1, OUTPUT);
  pinMode(pinH0, OUTPUT);

  // Deixa parado inicialmente
  digitalWrite(pinH0, LOW);
  digitalWrite(pinH1, LOW);

  //// Wire.begin();
  //// Wire.setClock(400000); // standard value is 100000
  //// Wire.beginTransmission(xx);

  init_lcd();
}



// Zera a entrada depois do tratamento para receber a proxima
void newEnter(){
  nchar = 0;  
  input = "";
}


/***    LCD   ***/

// Funcao que configura estado inicial do display LCD 
void init_lcd(){
  lcd.begin(16, 2);
  lcd.clear();   //Apaga o buffer e o display
  lcd.setCursor(0, 0);  //Seta a posição do cursor
  lcd.println("PARADO");
  lcd.setCursor(0,8);
  lcd.println("DUTYCYCLE 000%");
  lcd.setCursor(0,16);
  lcd.println("VEL XXX RPM");
  lcd.display();

}

// Exibe na segunda linha do visor a velocidade atual do motor
void LCDVel (String mostra) {
  lcd.setCursor(0x18, 0x10);
  lcd.println(mostra);
  lcd.println(" aa%");
  lcd.display();
  
}

// Exibe na primeira linha do visor o estado atual do motor
void LCDEstado (String mostra) {
  lcd.setCursor(0, 0);
  lcd.println(mostra);
  lcd.display();

}


// Usa "wire" para exibir velo no de 7 segmentos 
void exibe7seg(int vel){
  Wire.write(vel);
  Wire.endTransmission();
}

/***    Sentido e Velocidade do Motor   ***/

// Configura sentido de movimento do motor (via ponte H) alternando entre
// os pinos da ponte H - pinH0 e pinH1
void setFlow (int flow) {

  if (flow == 1) { // seta vent mode
    digitalWrite(pinH0, HIGH);   // manda sinal pwm para pino que faz sentido vent
    digitalWrite(pinH1, LOW);

  } else if (flow == 2) { // seta exaust mode
      digitalWrite(pinH0, LOW);   
      digitalWrite(pinH1, HIGH); // manda sinal pwm para pino que faz sentido vent

  } else if (!flow) { // pára motor
      digitalWrite(pinH0, LOW);   
      digitalWrite(pinH1, LOW);
  }
}


// Recebe novo valor de velo pelo serial e muda a velo do motor
void setVel (int vel) {
  analogWrite(pinPWM, (int) ((vel * 255)/100));

}



/***  ERROS   ***/

// Indicacao de erros de acordo com seu tipo e preparacao para nova entrada
void erro(int codigo) { 

  if (codigo == 1) Serial.print("ERRO: COMANDO INEXISTENTE\n");
  else if (codigo == 2) Serial.print("ERRO: PARÂMETRO AUSENTE\n");
  else if (codigo == 3) Serial.print("ERRO: PARÂMETRO INCORRETO\n");
  else Serial.print("ERRO NAO IDENTIFICADO\n");

  newEnter();
}




void loop() {

  /// while(bluetooth.available()) {
  while(Serial.available()){ // enquanto ainda tem novos caracteres
    
    /// input = input + char(bluetooth.read()); // comuta string ate o fim do comando
    input = input + char(Serial.read()); // comuta string ate o fim do comando
    char last = input[nchar];

    if (last == '*') { // usar charcmp
      Serial.print("TESTE - fim de linha");

      if (input.substring(0,3) == "VEL") {
        velo = input.substring(5, 8); // create velo as a substring of input starting from 5 and w size of 3
       
        for (int i = 0; i < 2; i++) {
          if (!(isDigit(velo[i]))) erro(2); // caso nao tenha um valor numerico como parametro

          int newVel = velo.toInt();  // conver the string velo into a integer
          if (newVel < 0 || newVel > 100) erro(3); // caso nao seja um valor certo
          else {
            setVel(newVel);   // sendo um valor valido, seta a velo para este novo valor
            LCDVel(velo);
            Serial.print("Nova vel: ");
            Serial.println(velo);
            exibe7seg(newVel);
          } 

        }


      } else if (input == "VENT*") { // se o comando for VENT
          setFlow(1);
          LCDEstado("VENTILADOR");
          Serial.println("OK VENT");

      } else if (input == "EXAUST*") {
          setFlow(2);
          LCDEstado("EXAUSTOR");
          Serial.println("OK EXAUST");

      } else if (input == "PARA*") {
          setFlow(0);
          LCDEstado("PARADO");
          Serial.println("OK PARA");

      } else if (input == "RETVEL*") {
          Serial.print("VEL: ");
          Serial.print(rpm); // valor medido no pwm
          Serial.println(" RPM");  

      } else erro(1); 
    }

    nchar++;

    if (nchar >= 9) erro(1); // Comando nao existe
  }
 
  newEnter();
}
