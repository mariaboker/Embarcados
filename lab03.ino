/******************************************************************************************/
/*                                                                                        */
/* EA 076 - Laboratório de Sistemas Embarcados                                            */
/* Projeto 3:                                           */
/*                                                                                        */
/* Grupo: Victor Perez,148122 e Maria Kersanach, 156571                                   */
/* Entrega em 03 de Julho de 2019                                                         */
/*                                                                                        */
/* Link para video:                                               */
/* Link para roteiro:                                            */
/*                                                                                        */
/******************************************************************************************/

/* Projeto 2: CONTROLADOR DE VENTILADOR DE TETO   */


// codigo exemplo https://codebender.cc/sketch:275439#EEPROM_SWITCH.ino


#include <Wire.h>

int a = 1; // digito mais significativo do display
int b = 2;
int c = 3;
int d = 4;

void setup() {
  Wire.begin();
  Wire.setClock(400000);

  Wire.beginTransmission(63); // argumento é o endereco do dispositivo no barramento
  Wire.write(240); // arg é o byte a ser transmitido
  Wire.endTransmission(63); 

}

void loop() {
  
  // range de memoria 80 a 87
  Wire.beginTransmission(80); // endereco de memoria
  Wire.write(0); // adress
  Wire.write(114); // data
  Wire.endTransmission(80);
  delay(5);
  Wire.beginTransmission(80); // endereco de memoria
  Wire.write(0);
  Wire.endTransmission(80);
  Wire.requestFrom(80,1);
  byte a = Wire.read();

  delay(5);

  //Serial.print(a); CONFIGURAR SERIAL
  Wire.beginTransmission(63); // argumento é o endereco do dispositivo no barramento
  Wire.write(a); // arg é o byte a ser transmitido
  Wire.endTransmission(63); 


}
