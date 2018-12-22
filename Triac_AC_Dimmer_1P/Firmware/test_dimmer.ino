int AC_LOAD = 3;    // Output to Opto Triac pin
int dimming = 128;  // Dimming level (0-128)  0 = ON, 128 = OFF

void setup()
{
  pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
  attachInterrupt(0, zero_crosss_int, RISING);  // Choose the zero cross interrupt # from the table above
}

//the interrupt function must take no parameters and return nothing
void zero_crosss_int()  //function to be fired at the zero crossing to dim the light
{
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms 
  // Every zerocrossing thus: (50Hz)-> 10ms (1/2 Cycle) 
  // For 60Hz => 8.33ms (10.000/120)
  // 10ms=10000us
  // (10000us - 10us) / 128 = 75 (Approx) For 60Hz =>65

  int dimtime = (75*dimming);    // For 60Hz =>65    
  delayMicroseconds(dimtime);    // Wait till firing the TRIAC
  digitalWrite(AC_LOAD, HIGH);   // Fire the TRIAC
  delayMicroseconds(10);         // triac On propogation delay (for 60Hz use 8.33)
  digitalWrite(AC_LOAD, LOW);    // No longer trigger the TRIAC (the next zero crossing will swith it off) TRIAC
}

void loop()  {
  for (int i=5; i <= 128; i++){
    dimming=i;
    delay(10);
   }
}
