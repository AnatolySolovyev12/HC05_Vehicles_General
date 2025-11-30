#define pinIN1 5
#define pinIN2 9
#define pinPWM1 6
#define pinPWM2 10

#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

String strData = "";
boolean recievedFlag;

bool boolUp = 0;
bool boolDown = 0;
bool boolLeft = 0;
bool boolRight = 0;
bool boolCentr = 0;
bool light = 0;

long long oldMillis = 0;
byte interval = 60;

byte pwmValueMotor1 = 255;
byte pwmValueMotor2 = 255;

//////////////////////////////////////////////////////////////////////////////////////////
void setup()  {

  pinMode (pinPWM1, OUTPUT);
  pinMode (pinIN1, OUTPUT);
  pinMode (pinPWM2, OUTPUT);
  pinMode (pinIN2, OUTPUT);

  pinMode (A0, OUTPUT);
  pinMode (A2, OUTPUT);

  Serial.begin(9600);
  mySerial.begin(9600);

  // Timer0 (пины 5, 6)
TCCR0A = _BV(WGM00) | _BV(WGM01) | _BV(COM0A1) | _BV(COM0B1);
TCCR0B = _BV(CS00); // предделитель 1

// Timer1 (пины 9, 10)
TCCR1A = _BV(WGM10) | _BV(WGM12) | _BV(COM1A1) | _BV(COM1B1);
TCCR1B = _BV(CS10);
}


////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  while (mySerial.available() > 0) {
    strData += (char)mySerial.read();
    recievedFlag = true;
    delay(4); // без этого не успеет уловить и записать все символы в строку
  }


  if (strData != "") {
    Serial.println(strData);
    if (strData == "UD") boolUp = 1;
    if (strData == "UU") boolUp = 0;
    if (strData == "DD") boolDown = 1;
    if (strData == "DU") boolDown = 0;
    if (strData == "LD") boolLeft = 1;
    if (strData == "LU") boolLeft = 0;
    if (strData == "RD") boolRight = 1;
    if (strData == "RU") boolRight = 0;
    if (strData == "CD") boolCentr = 1;
    if (strData == "CU") boolCentr = 0;
    if (strData == "LFD") light = !light;
    //if (strData == "LFU") mySerial.write("TEST");
  }


  if (recievedFlag && (millis() - oldMillis >= interval)) // защита от дерганья если быстро переключать
  {
    oldMillis = millis();

    if (boolUp) forward();
    else if (boolRight) right();
    else if (boolDown) backward();
    else if (boolLeft) left();
    else if (boolCentr) stoping();
    else stoping();

    ledForward();
  }
  recievedFlag = false;
  strData = "";
}





//////////////////////////////////////////////////////////////////////////////
void forward() // движение вперед
{
  analogWrite(pinPWM1, pwmValueMotor1);
  analogWrite(pinIN1, 0);
  analogWrite(pinPWM2, pwmValueMotor2);
  analogWrite(pinIN2, 0);
}

void right() // движение вправо
{
  analogWrite(pinPWM1, pwmValueMotor1);
  analogWrite(pinIN1, 0);
  analogWrite(pinPWM2, 0);
  analogWrite(pinIN2, pwmValueMotor2);
}

void backward() // движение назад
{
  analogWrite(pinPWM1, 0);
  analogWrite(pinIN1, pwmValueMotor1);
  analogWrite(pinPWM2, 0);
  analogWrite(pinIN2, pwmValueMotor2);
}

void left() // движение влево
{
  analogWrite(pinPWM1, 0);
  analogWrite(pinIN1, pwmValueMotor1);
  analogWrite(pinPWM2, pwmValueMotor2);
  analogWrite(pinIN2, 0);
}

void stoping() // остановка
{
  analogWrite(pinPWM1, 0);
  analogWrite(pinIN1, 0);
  analogWrite(pinPWM2, 0);
  analogWrite(pinIN2, 0);
}

void ledForward() // светодиоды спереди
{
  digitalWrite(A0, light);
  digitalWrite(A2, light);
}
