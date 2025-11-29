#define pinIN1 4
#define pinIN2 8
#define pinPWM1 5
#define pinPWM2 9

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
}


////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  while (mySerial.available() > 0) {
    strData += (char)mySerial.read();
    recievedFlag = true;
    delay(2); // без этого не успеет уловить и записать все символы в строку
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
    //if (strData == "LFU") boolCentr = 0;
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
  digitalWrite(pinPWM1, 1);
  digitalWrite(pinIN1, 0);
  digitalWrite(pinPWM2, 1);
  digitalWrite(pinIN2, 0);
}

void right() // движение вправо
{
  digitalWrite(pinPWM1, 1);
  digitalWrite(pinIN1, 0);
  digitalWrite(pinPWM2, 0);
  digitalWrite(pinIN2, 1);
}

void backward() // движение назад
{
  digitalWrite(pinPWM1, 0);
  digitalWrite(pinIN1, 1);
  digitalWrite(pinPWM2, 0);
  digitalWrite(pinIN2, 1);
}

void left() // движение влево
{
  digitalWrite(pinPWM1, 0);
  digitalWrite(pinIN1, 1);
  digitalWrite(pinPWM2, 1);
  digitalWrite(pinIN2, 0);
}

void stoping() // остановка
{
  digitalWrite(pinPWM1, 0);
  digitalWrite(pinIN1, 0);
  digitalWrite(pinPWM2, 0);
  digitalWrite(pinIN2, 0);
}

void ledForward() // светодиоды спереди
{
  digitalWrite(A0, light);
  digitalWrite(A2, light);
}
