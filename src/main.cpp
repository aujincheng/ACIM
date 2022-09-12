#include <Arduino.h>
#include <HardwareSerial.h>
#include <Stepper.h>

const int iR1 = 36, iR2 = 39, iR3 = 34, iR4 = 35;
const int rS8_blower = 15, rS7_bPunch = 32, rS6_uExtend = 33,rS5_rCy = 25,rS4_rSCap = 26,rS3_conveyor = 27;
const int Emm42_En_Pin=14,Emm42_Stp_Pin=12,Emm42_Dir_Pin=13;

const int stepsPerRevolution = 600;

long i = 0, j = 0;  bool cntDir = false;
int val1 =LOW,val2 =LOW,val3 =LOW, val4 = LOW;
int incomingStuff=0,remainingStuff=0,trigProgram = 0;
int x = 0;
String incomingThing;

int stage1 = 1,stage2 =1,stage3 =1,stage4 =1,stage5 = 1,stage6 = 1,stage8 =1,stage7 =0;

void irReader();
void sSStepper();
void requestNumberOfFabrication();
void onePunchMan();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //SerialPort.begin(15200, SERIAL_8N1, 9, 10); 
  //Serial2.begin(9600,SERIAL_8N1, 9, 10);
  
  pinMode(iR1,INPUT);
  pinMode(iR2,INPUT);
  pinMode(iR3,INPUT);
  pinMode(iR4,INPUT);

  pinMode(rS8_blower,OUTPUT);   digitalWrite(rS8_blower,HIGH); //
  pinMode(rS7_bPunch,OUTPUT);   digitalWrite(rS7_bPunch,HIGH);
  pinMode(rS6_uExtend,OUTPUT);  digitalWrite(rS6_uExtend,HIGH);
  pinMode(rS5_rCy,OUTPUT);      digitalWrite(rS5_rCy,HIGH);
  pinMode(rS4_rSCap,OUTPUT);    digitalWrite(rS4_rSCap,HIGH);
  pinMode(rS3_conveyor,OUTPUT); digitalWrite(rS3_conveyor,HIGH);

  pinMode(Emm42_En_Pin, OUTPUT);  digitalWrite(Emm42_En_Pin , LOW);  // initialize the Emm42_En_Pin as an output
  pinMode(Emm42_Stp_Pin, OUTPUT);  digitalWrite(Emm42_Stp_Pin, LOW);  // initialize the Emm42_Stp_Pin as an output
  pinMode(Emm42_Dir_Pin, OUTPUT);  digitalWrite(Emm42_Dir_Pin, LOW);  // initialize the Emm42_Dir_Pin as an output

  delay(5000);

  Serial.println("The code start.");
  
}

void loop() 
{
  
  irReader();

  

  if (val1 == LOW && stage2 == 1) // Housing detected at location 1
  {
    Serial.println("Detected the silicon housing.");
    Serial.flush();
    delay(2000);
    sSStepper();
    delay(1000);
    stage2 = 0;
  }
  else if (val1 == HIGH && stage1 == 1) // No silicon housing
  {
    Serial.println("Please insert the silicon housing.");
    Serial.flush();
    stage1 = 0;
  }
  else if (val2 == LOW && stage1 == 0 && stage2 == 0 && stage3 == 1) 
  {
    Serial.println("Enter the procedure to punch the bibi.");
    Serial.flush();
    if(val3 == HIGH && stage1 == 0 && stage2 == 0 && stage4 == 1) // Dont have silicon cap.
    {
      Serial.println("Dont have silicon caps. Start refill module.");
      Serial.flush();
      delay(4000);
      digitalWrite(rS4_rSCap,LOW);
      digitalWrite(rS3_conveyor,LOW);
      stage4 = 0;
    }
    else if(val3 == LOW && stage4 ==0 && stage5 == 1) // Detected silicon cap.
    {
      
      Serial.println("Detected silicon cap.");
      Serial.flush();
      digitalWrite(rS4_rSCap,HIGH);
      digitalWrite(rS3_conveyor,HIGH);
      delay(10000);
      digitalWrite(rS5_rCy,LOW);
      delay(10000);
      digitalWrite(rS5_rCy,HIGH);
      delay(10000);
      Serial.println("Turned off the refill module.");
      Serial.flush();
      delay(4000);
      stage5 = 0;
      stage3 = 0;
    }
  }
  else if(val2 == LOW && stage1 == 0 && stage2 == 0 && stage3 == 0 && stage8 == 1)
  {
    // Punching module
    Serial.println("Punching module starts in 5 seconds.");
    Serial.flush();
    delay(5000);
    onePunchMan();
    Serial.println("Punching module completed.");
    Serial.flush();
    delay(5000);
    stage8 = 0;
  }
  
  else if (val4 == LOW && stage1 == 0 && stage2 == 0 && stage3 == 0 && stage6 == 1)
  {
    digitalWrite(rS8_blower,LOW); //Turn on blower
    Serial.println("Turn on the blower.");
    Serial.flush();
    delay(5000);
    digitalWrite(rS8_blower,HIGH); //Turn on blower
    Serial.println("Turn off the blower.");
    Serial.flush();
    delay(5000);
    stage6 = 0;
    stage7 = 1;
  }
  else if (val1 == HIGH && val2 == HIGH && val4 == HIGH && stage7 == 1)
  {
    stage1 = 1;
    stage2 = 1;
    stage3 = 1;
    stage4 = 1;
    stage5 = 1;
    stage6 = 1;
    stage7 = 0;
    stage8 = 1;
    Serial.println("One cycle completed.");
    Serial.flush();
  }

  
  
  
}


/**************************************************************Function********************************************************/

void onePunchMan()
{
  digitalWrite(rS6_uExtend,LOW);
  delay(15000);
  digitalWrite(rS7_bPunch,LOW);
  delay(15000);
  digitalWrite(rS6_uExtend,HIGH);
  delay(15000);
  digitalWrite(rS7_bPunch,HIGH);
  delay(15000);
  sSStepper();
  delay(1000);
  sSStepper();
}

void requestNumberOfFabrication()
{
  incomingThing = Serial.readString();
  remainingStuff = incomingThing.toInt();
  Serial.print("Needs: ");
  Serial.println(remainingStuff);
  
}

void sSStepper()
{
  for (i =0; i < 4800 ; i++) {
  // Full 360 degree = 19200 pulses
  // Half 180 degree =  9600 pulses
  // Qua   90 degree =  4800 pulses
  delayMicroseconds(600); //500us
  digitalWrite(Emm42_Stp_Pin, !digitalRead(Emm42_Stp_Pin)); // Instant ON-OFF the step pin

  }
}

void irReader()
{
  val1 = digitalRead(iR1);
  val2 = digitalRead(iR2);
  val3 = digitalRead(iR3);
  val4 = digitalRead(iR4);
  // IR pin original is high.
}

