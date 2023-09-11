int LOSS_SIG = 8;
int test = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(LOSS_SIG, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  Serial.println("START");
  unsigned long StartTime = millis();
  delay(10000); //10 sec
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  Serial.println(ElapsedTime);
  */
  switch(test)
  {
    case 0:
      while(digitalRead(LOSS_SIG) != 0)
      {

      }
      Serial.println("Radio loss successful");      
      unsigned long StartTime = millis();
      while(digitalRead(LOSS_SIG) == 0)
      {

      }
      Serial.println("Radio renew successful");
      unsigned long CurrentTime = millis();
      Serial.println((CurrentTime/1000)/60);
      test = 1;
      break;
    default:
      while(true)
      {

      }
  }
}
