/******************************************************************************
*
* Programa                : Alimentador de peixes Relogio com modulo DS1307 e LCD 20x4
* Autor                   : Jose Adolfo de castro Neto
* Data Criação            : 02/07/2017
* Data ultima Modificação : 06/05/2018
*/
//******************************************************************************




//******************************************************************************
// --- Inicializa o LCD ---
#include "Wire.h"
#include <LiquidCrystal.h>
//******************************************************************************
// --- Defines ---
#define DS1307_ADDRESS 0x68


//******************************************************************************
// --- Inicializa o LCD ---
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//******************************************************************************
// --- Pinos ---
int const comida =8; 
int const button_programar = 10;

//******************************************************************************
// --- Variaveis Globais ---
byte zero = 0x00;
char control = 0x01;
char counter = 0x00;




//******************************************************************************

void setup()
{
  //--- configuração do timer0 ---
/*
 *    TEMPO_INTERRUPÇÃO = (1 / FREQUENCIA DO CRISTAL) * Prescaler * OVERFLOW DO TIMER
 *    TEMPO_INTERRUPÇÃO = (1/16MHz)                   * 1024      * 256
 *    TEMPO_INTERRUPÇÃO = 62,5ns                      * 1024      * 256 
 *    TEMPO_INTERRUPÇÃO = 16,384ms  ou 61,03Hz
 */
   TCCR2A = 0x00; // Desabilita o OC0A e o OC0B, modo de operação normal com overflow em 0xff
   TCCR2B = 0x06; // prescaler 1:1024
   TCNT2  = 0; // contador = 0
   TIMSK2 = 0x01; // Habilita a interrupção por overflow do timer0
   SREG   = 0b10000000; // Habilita a interrupção global 0b'1000 0000' 



  
  pinMode(comida,OUTPUT);
  pinMode(button_programar,INPUT);


  
 // Define o LCD com 20 colunas e 4 linhas
 lcd.begin(20, 4);
 Wire.begin();
 Serial.begin(9600);
 // A linha abaixo pode ser retirada apos setar a data e hora
 // SelecionaDataeHora(); 
 
}


//******************************************************************************
// --- Interrupção ---
ISR(TIMER2_OVF_vect)
{
  boolean stado;

  TCNT2 = 0;  // contador
  counter++;


  stado = digitalRead(button_programar); 
 while(stado == true)
  {
    SREG   = 0b00000000;
    programar();
    stado = digitalRead(button_programar); 
    delay(50);
  }
  

  SREG   = 0b10000000;
  //loop();
}

void loop()
{
   int segundos;
    int minutos;
 while(1)
 {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  segundos = ConverteparaDecimal(Wire.read());
  minutos  = ConverteparaDecimal(Wire.read());

//***************************  Programação do tempo de funcionamento  **************************************** 

   if(minutos == 0)
   {
       if(segundos < 3)
         digitalWrite(comida,HIGH);
       else
         digitalWrite(comida,LOW);
   }
   else
   digitalWrite(comida,LOW);

//***************************************************************************************************************
   minutos =(60-minutos);
   if(minutos > 60)
      minutos /= 10;
   
   Mostrarelogio();
   lcd.setCursor(0,1);
   lcd.print("Falta:");
   lcd.setCursor(7,1);
   lcd.print(minutos);
   lcd.setCursor(12,1);
   lcd.print("minutos");
  }
}

void SelecionaDataeHora() // Seta a data e a hora do DS1307
{
 byte segundos = 01; // Valores de 0 a 59
 byte minutos = 16; // Valores de 0 a 59
 byte horas = 16; // Valores de 0 a 23
 byte diadasemana = 0; // Valores de 0 a 6 (0=Domingo, 1 = Segunda...)
 byte diadomes = 9; // Valores de 1 a 31
 byte mes = 7; // Valores de 1 a 12
 byte ano = 17; // Valores de 0 a 99
 Wire.beginTransmission(DS1307_ADDRESS);
 // Stop no CI para que o mesmo possa receber os dados
 Wire.write(zero); 

 // As linhas abaixo escrevem no CI os valores de 
 // data e hora que foram colocados nas variaveis acima
 Wire.write(ConverteParaBCD(segundos));
 Wire.write(ConverteParaBCD(minutos));
 Wire.write(ConverteParaBCD(horas));
 Wire.write(ConverteParaBCD(diadasemana));
 Wire.write(ConverteParaBCD(diadomes));
 Wire.write(ConverteParaBCD(mes));
 Wire.write(ConverteParaBCD(ano));
 Wire.write(zero); //Start no CI
 Wire.endTransmission(); 
}

byte ConverteParaBCD(byte val)
{ 
 // Converte o número de decimal para BCD
 return ( (val/10*16) + (val%10) );
}

byte ConverteparaDecimal(byte val) 
{ 
 // Converte de BCD para decimal
 return ( (val/16*10) + (val%16) );
}

void Mostrarelogio()
{
 Wire.beginTransmission(DS1307_ADDRESS);
 Wire.write(zero);
 Wire.endTransmission();
 Wire.requestFrom(DS1307_ADDRESS, 7);
 int segundos = ConverteparaDecimal(Wire.read());
 int minutos = ConverteparaDecimal(Wire.read());
 int horas = ConverteparaDecimal(Wire.read() & 0b111111); 
 int diadasemana = ConverteparaDecimal(Wire.read()); 
 int diadomes = ConverteparaDecimal(Wire.read());
 int mes = ConverteparaDecimal(Wire.read());
 int ano = ConverteparaDecimal(Wire.read());
 // Imprime mensagem na primeira linha do display
 lcd.setCursor(0,0);
 lcd.print("* ADOLFO AUTOMACAO *");
 lcd.setCursor(0,2);
 lcd.print("Hora :");
 // Mostra a hora atual no display
 lcd.setCursor(7, 2);
 if (horas < 10)
 {lcd.print("0");}
 lcd.print(horas);
 lcd.print(":");
 if (minutos < 10)
 {lcd.print("0");}
 lcd.print(minutos);
 lcd.print(":");
 if (segundos < 10)
 {lcd.print("0");}
 lcd.print(segundos);
 
 // Mostra a data atual no display
 lcd.setCursor(0, 3);
 lcd.print("Data :");
 lcd.setCursor(7,3);
 if (diadomes < 10)
 {lcd.print("0");}
 lcd.print(diadomes);
 lcd.print("/");
 if (mes < 10)
 {lcd.print("0");}
 lcd.print(mes);
 lcd.print("/");
 lcd.print(ano);
 
 //Mostra o dia da semana no display
 lcd.setCursor(16, 3);
 switch(diadasemana)
 {
 case 0:lcd.print("Dom");
 break;
 case 1:lcd.print("Seg");
 break;
 case 2:lcd.print("Ter");
 break;
 case 3:lcd.print("Qua");
 break;
 case 4:lcd.print("Qui");
 break;
 case 5:lcd.print("Sex");
 break;
 case 6:lcd.print("Sab");
 }
}


void programar(void)
{
  //lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("* Programacao *");
   lcd.setCursor(0,2);
   lcd.print("Hora :           ");
   lcd.setCursor(0,3);
   lcd.print("                 ");
   lcd.setCursor(0,4);
   lcd.print("                 ");
   delay(100);
}





