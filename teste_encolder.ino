/* Rotary encoder read example */
#define ENC_A 8
#define ENC_B 9
#define ENC_PORT PINB
int8_t read_encoder();
int aux;
void setup()
{
  /* Setup encoder pins as inputs */
  pinMode(ENC_A, INPUT);
  digitalWrite(ENC_A, HIGH);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_B, HIGH);
  Serial.begin (9600);
  Serial.println("Start");

}

void loop()
{
  static uint8_t counter = 0; //this variable will be changed by encoder input
  int8_t tmpdata;
/**/
  tmpdata = read_encoder();
 
  if(tmpdata)
  {
    counter += tmpdata;
  }
  if(counter == 7)
   {
    aux++;
    counter = 4;
    Serial.print("Counter value: ");
    Serial.println(aux, DEC);
   }

  if(counter == 1)
   {
    aux--;
    counter = 4;
    Serial.print("Counter value: ");
    Serial.println(aux, DEC);
   }



}

/* returns change in encoder state (-1,0,1) */
 int8_t read_encoder()
{
  static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  static uint8_t old_AB = 0;
  /**/
  old_AB <<= 2; 
  old_AB |= ( ENC_PORT & 0x03 ); //add current state
  return ( enc_states[( old_AB & 0x0f )]);
}








