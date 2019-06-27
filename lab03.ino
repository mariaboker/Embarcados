/*RASCUNHO*******************************************************************************************/
/*                                                                                      */
/* EA 076 - Laboratório de Sistemas Embarcados                                          */
/* Projeto 3: sadsadasd Modificar tudo                                                          */
/*                                                                                      */
/* Grupo: Victor Perez,148122 e Maria Kersanach, 156571                                 */
/* Entrega em XX de Abril de 2018                                                       */
/*                                                                   
/* Link para video: XX                                            */
/* Link para roteiro: XX                                          */
/*                                                                                      */
/********************************************************************************************/


//https://circuitdigest.com/microcontroller-projects/keypad-interfacing-with-8051-microcontroller
/* Projeto 3: XXX         */
/* EXPLICACAO BASICA XXX  */

#include <Wire.h>   // para I2C
#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 13, 4, 5, 6, 7);
//RS - pino 12
//Enable - pino 13
//DB7 - pino 7
//DB6 - pino 6
//DB5 - pino 5
//DB4 - pino 4

int uni=0;
int dez=0;
int cent=0;
int mil=0;
int x = 0;
int y = 0;
int z = 0;
int w = 0;
int j=0;
int k=0;
int valor=0;
int lido[5];
int timeBase = 0; // variavel para delimitar tempo na contagem de rpm
int pulseCount = 0; // flag que indica que a contagem de rpm deve ser realizada (baseia-se no timeBase)

int aux=0;
int pressed=0;;
int aux2=0;

char tecla;
char comando;
char anterior = 0;
char atual = 0;
char guardanum=0;

int apertou=0;
int count=0;

int colunalido;
int colunalido1;
int colunalido2;
int colunalido3;
int mapcolun=0;
int mapline=0;

int analogPin = A3;
int colun1=3;
int colun2=2;
int colun3=15;
int linhas[4] = {11,10,9,8};

int inicia = 0;
int salvaTemp = 0;
int reset = 0;
int finaliza = 0;
int countMed = 0;
int statuss = 0;
int freeSpace = 0;
int transfere = 0;


// Configuracao Temporizador 0 (8 bits) para gerar interrupcoes periodicas a cada 2ms no modo Clear Timer on Compare Match (CTC)
void configuracao_Timer0() {
  TCCR0A = 0x02;
  OCR0A = 124;
  TIMSK0 = 0x02;
  TCCR0B = 0x04;
}

// interrupcao chamada a cada 2ms
ISR(TIMER0_COMPA_vect) {

  aux++; // alterna entre displays (0, 1, 2, 3)
  if (aux > 3) {
    aux = 0;

    if (inicia == 1) {    //conta tempo de 2s para cada coleta e seta salvaTemp para escrever na memoria
      timeBase++;
      if (timeBase > 1000) { // base de tempo de 2s
        timeBase = 0;
        salvaTemp = 1; // flag that enables pulse counting
      }
      if (apertou == 1) count++;
    }
  }
}



void displayMode (int a,int b,int c,int d){
  
  if (aux==0) {    //algarismo unidade
    Wire.beginTransmission(63);
    Wire.write(a+224);
    Wire.endTransmission(63);
  } else if (aux == 1) {  //algarismo dezena
    Wire.beginTransmission(63);
    Wire.write(b+208);
    Wire.endTransmission(63);
  } else if (aux == 2) {  //algarismo centena
    Wire.beginTransmission(63);
    Wire.write(c+176);
    Wire.endTransmission(63);
  } else if (aux == 3) {  //algarismo milhar
    Wire.beginTransmission(63);
    Wire.write(d+112);
    Wire.endTransmission(63);
  }
}

/***  LCD   ***/
// Funcao que configura estado inicial do display LCD
void init_lcd() {
  lcd.begin(16, 2);
  lcd.clear();   //Apaga o buffer e o display
  lcd.setCursor(0,0);  //Seta a posição do cursor
  lcd.display();
  Serial.println("fim init lcd");
}


void scanKeypad() {
  if (aux2 == 0) {
    for (int i = 0; i < 4; i++){
      digitalWrite(linhas[i], LOW);

      if(digitalRead(colun1)==0) {
        //colunalido = colun1;
        mapcolun = 1;
        mapline = i+1;
        aux2 = 1;
        apertou = 1;
        break;
      }

      if(digitalRead(colun2)==0){
        //colunalido = colun2;
        mapcolun = 2;
        mapline = i+1;
        aux2 = 1;
        apertou = 1;
        break;
      }

      if(digitalRead(colun3)==0){
        //colunalido = colun3;
        mapcolun = 3;
        mapline = i+1;
        aux2 = 1;
        apertou = 1;
        break;
      }

      digitalWrite(linhas[i], HIGH);

      if (pressed==1||(count >= 25)&&((digitalRead(colun1)==0)||(digitalRead(colun2)==0)||(digitalRead(colun3)==0))){
        count = 0;
        pressed = 1;
        apertou = 0;

        digitalWrite(linhas[0], LOW);
        digitalWrite(linhas[1], LOW);
        digitalWrite(linhas[2], LOW);
        digitalWrite(linhas[3], LOW);

        colunalido1 = digitalRead(colun1);
        colunalido2 = digitalRead(colun2);
        colunalido3 = digitalRead(colun3);
        Serial.println("pressionou algo");

        if(colunalido1==1&&colunalido2==1&&colunalido3==1){
          digitalWrite(linhas[0], HIGH);
          digitalWrite(linhas[1], HIGH);
          digitalWrite(linhas[2], HIGH);
          digitalWrite(linhas[3], HIGH);
          Serial.println("soltou algo");

          apertou = 0;
          aux2=0;
          pressed=0;
          colunalido==0;

          if (mapline == 1) {
            if (mapcolun == 1) tecla = '1';
            else if (mapcolun == 2) tecla = '2';
            else if (mapcolun == 3) tecla = '3';

          } else if (mapline == 2) {
            if (mapcolun == 1) tecla = '4';
            else if (mapcolun == 2) tecla = '5';
            else if (mapcolun == 3) tecla = '6';

          } else if (mapline == 3) {
            if (mapcolun == 1) tecla = '7';
            else if (mapcolun == 2) tecla = '8';
            else if (mapcolun == 3) tecla = '9';
          
          } else if (mapline == 4){
            if (mapcolun == 1) tecla = '#';
            else if (mapcolun == 2) tecla = '0';
            else if (mapcolun == 3) tecla = '*';
            
            Serial.println(tecla);
          }
        }
      }
    }
  }
}

void coletaNumero(){    //coleta numero de teclado para função transferee
valor = 0;
k=0;
j=0;
tecla=0;
do { // enquanto nao eh o asterisco, le teclado
scanKeypad();
  if(tecla != 0){
    lido[j]=tecla-48;
    Serial.println(tecla); // debug
    Serial.println(lido[j]); // debug
    if (tecla!='#'){
      tecla=0;
      j++;
    }
//Serial.println("tecla: "); // debug  
  }
} while(tecla!='#');

if (lido[1]==-13){    //numero de 1 digito
valor = lido[0];
}else if (lido[2]==-13){  //numero de 2 digitos
  for(j=1;j>=0;j--){
  valor = valor + lido[j]*pow(10,k);
  k++;
  }
}else if (lido[3]==-13){  //numero de 3
  for(j=2;j>=0;j--){
  valor = valor + lido[j]*pow(10,k);
  k++;
  }
}else if (lido[4]==-13){
  for(j=3;j>=0;j--){
  valor = valor + lido[j]*pow(10,k);
  k++;
  }

lcd.setCursor(0,1);  //Seta a posição do cursor
Serial.print("cabou ja");

}
}

void serialEvent(){

}

double getTmedido() {               //get temp sensor
  float temp;
  float ldr;
  ldr = analogRead(0);
  //temp = ldr*(1.1/1023*100);      //medindo errado
  temp=((((ldr/1024)*1.1)-0.5)*100);  //medindo certo
  lcd.setCursor(11,2);
  lcd.print(temp);

  return temp;
}

void executeCommand() {
  if (reset == 1) {
    //apaga a memoria;
    countMed = 0;
    reset = 0;
    inicia = 0;
    //salvaTemp = 0;

    if (inicia == 1){
      if (salvaTemp == 1) {
        //salva valor na posição da memória
        countMed++;
        salvaTemp = 0;
      }

      if (finaliza == 1) {
        inicia == 0;
        finaliza == 0;    

        if (statuss == 1) {        //mostra statuss da memoria
          lcd.clear();        //sobreescreve tudo oq que estava antes no lcd
          lcd.setCursor(0,0);
          lcd.print(countMed);
          lcd.setCursor(0,1);
          lcd.print(freeSpace);

          if (transfere == 1 ) {
          //transfereir da memoria para monitor serial
          transfere = 0;
          }
        }
      }
    }
  }
}


void scanCommand() {
  scanKeypad();
  anterior = tecla;

  if (anterior == '1' || guardanum == '1'){
    statuss=0;
    guardanum = '1';
    lcd.setCursor(0,0);
    lcd.print("reset?          ");
    lcd.setCursor(0,1);
    lcd.print("#-si *-no");
    scanKeypad();
    atual = tecla;

    if (atual == '#'){  //confirma comando reset (executar função aqui ou fora?)
      lcd.clear();
      lcd.print("resetando"); //teste - transformar em coment
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;

    } else if (atual == '*') {  //cancela
      lcd.clear();
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;
      //break;
    }

  } else if (anterior == '2'|| guardanum == '2'){
    statuss=0;
    guardanum = '2';
    lcd.setCursor(0,0);
    lcd.print("ver statuss?     ");
    lcd.setCursor(0,1);
    lcd.print("#-si *-no");
    scanKeypad();
    atual = tecla;

    if (atual == '#'){  //confirma comando statuss (executar função aqui ou fora?)
      lcd.clear();
      lcd.print("mostrei"); //teste - transformar em coment
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;

    } else if (atual == '*'){  //cancela
      lcd.clear();
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;
      //break;
    }

  } else if (anterior == '3'|| guardanum == '3') {
    statuss=0;
    guardanum = '3';
    lcd.setCursor(0,0);
    lcd.print("inicia coleta?  ");
    lcd.setCursor(0,1);
    lcd.print("#-si *-no");
    scanKeypad();
    atual = tecla;

    if (atual == '#'){              //confirma comando inicia (executar função aqui ou fora?)
      lcd.clear();
      lcd.print("iniciou");         //teste - transformar em coment
      inicia = 1;             //flag para coleta
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;

    } else if (atual == '*') {          //cancela
      lcd.clear();
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;
      //break;
    }

  } else if (anterior == '4'|| guardanum == '4'){
    statuss=0;
    guardanum = '4';
    lcd.setCursor(0,0);
    lcd.print("finaliza coleta?");
    lcd.setCursor(0,1);
    lcd.print("#-si *-no");
    scanKeypad();
    atual = tecla;

    if (atual == '#'){              //cstatuss=0;onfirma comando finaliza(executar função aqui ou fora?)
      lcd.clear();
      lcd.print("finalizou");         //teste - transformar em coment
      finaliza = 1;
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;
    } else if (atual == '*'){          //cancela
      lcd.clear();
      anterior = 0;
      atual = 0;
      tecla = 0;
      guardanum=0;
    //break;

    } else if (anterior == '5'|| guardanum == '5'){
      inicia=0;
      statuss=0;
      guardanum = '5';

      lcd.setCursor(0,0);
      lcd.print("transfereir?     ");
      lcd.setCursor(0,1);
      lcd.print("#-si *-no");
      scanKeypad();
      atual = tecla;

      if (atual == '#'){  //confirma comando tranferir (executar função aqui ou fora?)
        lcd.clear();
        lcd.print("num btn 1 e 1022"); //teste - transformar em coment
        tecla = 0;
        coletaNumero();
        Serial.println("pegou um numero");
        lcd.print(valor);
        anterior = 0;
        atual = 0;
        tecla = 0;
        guardanum=0;
      } else if (atual == '*'){  //cancela
        lcd.clear();
        anterior = 0;
        atual = 0;
        tecla = 0;
        guardanum=0;
        //break;

        // else {
        //   anterior = 0;
        //   atual = 0;
        //   tecla = 0;
        // }
      }
    }
  }
}



void setup(){
//init serial e displays 7 seg
Serial.begin(9600);       // taxa de transmissao serial
Wire.begin();
Wire.setClock(400000);    // standard value is 100000
Wire.beginTransmission(63);
Wire.write(240);          //Desliga displays (b11110000)
Wire.endTransmission(63);

analogReference(INTERNAL);  //referencia de 1,1V para o sensor de temperatura

init_lcd();

cli();                // Desabilita interrupções globais
configuracao_Timer0();
sei();                // Habilita interrupcoes globais

//teclado matricial
pinMode(3, INPUT);  //coluna 1
pinMode(2, INPUT);  //coluna 2
pinMode(15, INPUT); //coluna 3
pinMode(11, OUTPUT);  //linha 1
pinMode(10, OUTPUT);  //linha 2
pinMode(9, OUTPUT); //linha 3
pinMode(8, OUTPUT); //linha 4
digitalWrite (11,HIGH);
digitalWrite (10,HIGH);
digitalWrite (9,HIGH);
digitalWrite (8,HIGH);

}


void loop(){

  scanCommand();
  executeCommand();

  if (salvaTemp == 1){
    x=getTmedido()*100;
    uni = x % 10;
    y = x / 10;
    dez = y % 10;
    z = y / 10;
    cent = z % 10;
    w = z / 10;
    mil = w % 10;
    
  displayMode(uni,dez,cent,mil);
  }
}


