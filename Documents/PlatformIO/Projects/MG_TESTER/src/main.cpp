#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long start, finished, elapsed;

Servo myservo;  
int val;    
int max = 180;
int min = 125;

// Signals from JB-118N
int ACT_SIG = 13;
int TMP_SIG = 12;
int LOSS_SIG = 10;

// Signals on Arduino mega
int TMP_ACT = 2;
int LOSS_ACT = 3;

//Počty akcí
int numOfAct = 0;
int numOfOk = 0;
int numOfTmp = 0;
int numOfTmpOk = 0;
int numOfLoss = 0;
int numOfLossOk = 0;

//Číslo testu
int test_number = 0;

bool ACT_FLAG = false;
bool OK_FLAG = false;
bool TMP_FLAG = false;
bool TMPOK_FLAG = false;

//Počet opakování testu
int numOfActTest = 1;
int numOfTmpTest = 1;
int numOfLossTest = 4;

//Procentuální kritérium pro splnění testu
int test_pass_crit = 90;
//Test results
float result_act = 0;
float result_tmp = 0;
float result_ok = 0;
float result_tmp_ok = 0;
int loss_result = 0;

int i;
float elapsedTime[5];
float timeCrit = 20;

void setup() {
  Serial.begin(9600);
  pinMode(ACT_SIG, INPUT_PULLUP);
  pinMode(TMP_SIG, INPUT_PULLUP);
  pinMode(LOSS_SIG, INPUT_PULLUP);

  pinMode(TMP_ACT, OUTPUT);
  pinMode(LOSS_ACT, OUTPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(125);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MG_TESTER");
  lcd.setCursor(0, 1);
  lcd.print("CERNYM");
  delay(1000);
  lcd.clear();
}

void loop() {
  switch(test_number){
    case 0:
      Serial.println(" ");
      Serial.println(" ");
      Serial.println("Activation test started");
      for (i = 0; i < numOfActTest; i++) {
        for (int pos = min; pos <= max; pos += 1) 
        { 
          myservo.write(pos);             
          delay(100);
        }
        delay(2000);            
        if(digitalRead(ACT_SIG) == 0 && !ACT_FLAG){
          ACT_FLAG = true;
          numOfAct += 1;
          Serial.print("Number of ACT: ");
          Serial.println(numOfAct);

          lcd.setCursor(0, 0);
          lcd.print("ACT: ");
          lcd.setCursor(4, 0);
          lcd.print(numOfAct);
        }
        for (int pos = max; pos >= min; pos -= 1) 
        { 
          myservo.write(pos);             
          delay(100);
        }
        delay(2000);
        if(digitalRead(ACT_SIG) == 1 && ACT_FLAG){
          ACT_FLAG = false;
          numOfOk += 1;
          Serial.print("Number of OK: ");
          Serial.println(numOfOk);

          lcd.setCursor(0, 1);
          lcd.print("OK: ");
          lcd.setCursor(4, 1);
          lcd.print(numOfOk);
        }
      }
      test_number = 1;
      i = 0;
      break;
    case 1:
      //Tamper test - ON/OFF tamper with relay?
      Serial.println("Tamper test started");
      for (i = 0; i < numOfTmpTest; i++){
        //Tamper OFF
        Serial.println("Tamper off");
        digitalWrite(TMP_ACT, 1);
        delay(2000);
        //Serial.println(digitalRead(TMP_SIG));
        if(digitalRead(TMP_SIG) == 0){
          numOfTmp += 1;
          Serial.print("Number of TMP: ");
          Serial.println(numOfTmp);
        }
        Serial.println("Tamper on");
        digitalWrite(TMP_ACT, 0);
        delay(2000);
        //Serial.println(digitalRead(TMP_SIG));
        if(digitalRead(TMP_SIG) == 1){
          //TMP_FLAG = false;
          numOfTmpOk += 1;
          Serial.print("Number of TMP OK: ");
          Serial.println(numOfTmpOk);
        }
      }
      test_number = 2;
      break;
    case 2:
      /*Device loss and load test. Switch off radio module with relay. Read if device Y_LED is ON and count it. 
      Then switch on radio module and read if periphery is OK and count it. After finished test write result PASS, FAIL.
      */
      Serial.println("Loss test started");
      for (i = 0; i <= numOfLossTest; i++)
      {
        //Radio off
        digitalWrite(LOSS_ACT, 1);
        delay(10000);
        while (digitalRead(LOSS_SIG) == HIGH)
        {
          //Serial.println("Waiting for loss");
          //Serial.println(digitalRead(LOSS_SIG));
          delay(200);
        }
        Serial.println("Ztrata OK");
        if(digitalRead(LOSS_SIG == LOW))
        {
          numOfLoss += 1;
        }
        start = millis();
        digitalWrite(LOSS_ACT, 0);
        while (digitalRead(LOSS_SIG) == LOW) {
          //Serial.println("Waiting for calm");
          //Serial.println(digitalRead(LOSS_SIG));
          delay(500);
        }
        Serial.println("Obnova OK");
        if(digitalRead(LOSS_SIG == HIGH))
        {
          numOfLossOk += 1;
        }
        finished = millis();
        elapsed = finished - start;
        elapsedTime[i] = elapsed;
      }
      
      //unsigned long StartTime = millis();
      /*
      delay(1200000);
      if(digitalRead(LOSS_SIG) == 0)
      {
        numOfLoss += 1;
        Serial.print("Number of LOSS: ");
        Serial.println(numOfLoss);
      }
      //Radio off
      digitalWrite(LOSS_ACT, 0);
      delay(1200000);
      if(digitalRead(LOSS_SIG) == 1)
      {
        numOfLossOk += 1;
        Serial.print("Number of LOSS OK: ");
        Serial.println(numOfLossOk);
      } 
      */
      test_number = 3;
      break;
    default:
      //ACTIVATION TEST RESULTS
      Serial.println(" ");
      Serial.println(" ");
      Serial.println("ACTIVATION TEST RESULTS!!!");
      Serial.println("--------------------------------");
      Serial.print("Number of successfull activation: ");
      Serial.println(numOfAct);
      Serial.print("Number of successfull calm: ");
      Serial.println(numOfOk);
      result_act = ((float)numOfAct / numOfActTest) * 100;
      Serial.print("Percentage of successful activation: ");
      Serial.println(result_act);
      result_ok = ((float)numOfOk / numOfActTest) * 100;
      Serial.print("Percentage of successful calm: ");
      Serial.println(result_ok);

      //TAMPER TEST RESULTS
      Serial.println(" ");
      Serial.println(" ");
      Serial.println("TAMPER TEST RESULTS!!!");
      Serial.println("--------------------------------");
      Serial.print("Number of successfull TMP: ");
      Serial.println(numOfTmp);
      Serial.print("Number of successfull TMP calm: ");
      Serial.println(numOfTmpOk);
      result_tmp = ((float)numOfTmp / numOfTmpTest) * 100;
      Serial.print("Percentage of successful TMP: ");
      Serial.println(result_tmp);
      result_tmp_ok = ((float)numOfTmpOk / numOfTmpTest) * 100;
      Serial.print("Percentage of successful TMP calm: ");
      Serial.println(result_tmp_ok);

      //LOSS TEST RESULTS
      Serial.println(" ");
      Serial.println(" ");
      Serial.println("LOSS TEST RESULTS!!!");
      Serial.println("--------------------------------");
      Serial.print("Number of successfull LOSS: ");
      Serial.println(numOfLoss);
      Serial.print("Number of successfull LOSS calm: ");
      Serial.println(numOfLossOk);

      for(int i = 0; i < numOfLossTest; i++)
      {
        Serial.print((elapsedTime[i]/1000)/60);
        Serial.println(" min");
      }
      
      //PASS FAIL RESULTS OF ALL TEST
      Serial.println();
      Serial.println();
      Serial.println("ALL TEST RESULTS");
      Serial.println("--------------------------------");
      if(result_act >= test_pass_crit && result_ok >= test_pass_crit){
      Serial.println("Test of activation PASSED");
      }
      else {
        Serial.println("Test of activation FAILED");
      }
      if(result_tmp >= test_pass_crit && result_tmp_ok >= test_pass_crit){
        Serial.println("Test of TMP PASSED");
      }
      else{
        Serial.println("Test of TMP FAILED");
      }
      if(numOfLoss == numOfLossOk)
      {
        for(int i = 0; i < numOfLossTest; i++)
        {
          if(elapsedTime[i] <= timeCrit)
          {
            loss_result += 1;
          }
        }
        if(loss_result == numOfLossTest)
        {
          Serial.println("Test of LOSS PASSED");
        }
        else
        {
          Serial.println();
          Serial.println("Test of LOSS FAILED");
        }
      }
      while(true){

      }
  } 
}
