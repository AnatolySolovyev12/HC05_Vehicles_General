#define pinIN1 5
#define pinIN2 9
#define pinPWM1 6
#define pinPWM2 10

#define INIT_KEY 99
#define INIT_ADDR 0

#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial mySerial(2, 3);

String strData = "";

bool noSpeedUpFlag = 0;

bool boolUp = 0;
bool boolDown = 0;
bool boolLeft = 0;
bool boolRight = 0;
bool boolCentr = 0;
bool light = 0;

long long oldMillis = 0;
byte interval = 500;

byte maxSpeedMotor1 = 255;
byte maxSpeedMotor2 = 255;
byte tempSpeedMotor1 = 0;
byte tempSpeedMotor2 = 0;
byte correctValueStartSpeedMotor1 = 0;
byte correctValueStartSpeedMotor2 = 0;

byte directionMotion = 0; // 0 - stopring / 1 - forward / 2 - bakward / 3 - left / 4 - right


struct GeneralStruct
{
  byte STRUCT_maxSpeedMotor1 = 255;
  byte STRUCT_maxSpeedMotor2 = 255;
  byte STRUCT_correctValueStartSpeedMotor1 = 0;
  byte STRUCT_correctValueStartSpeedMotor2 = 0;
};

GeneralStruct currentGenStruct;


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

  if (EEPROM.read(INIT_ADDR) != (byte)INIT_KEY) // проверка на первый запуск
  {
    EEPROM.write(INIT_ADDR, (byte)INIT_KEY);      // записали ключ
    EEPROM.put(1, currentGenStruct);
  }
  else
  {
    EEPROM.get(1, currentGenStruct); // инициализируем стандартные переменные
    maxSpeedMotor1 = currentGenStruct.STRUCT_maxSpeedMotor1;
    maxSpeedMotor2 = currentGenStruct.STRUCT_maxSpeedMotor2;
    correctValueStartSpeedMotor1 = currentGenStruct.STRUCT_correctValueStartSpeedMotor1;
    correctValueStartSpeedMotor2 = currentGenStruct.STRUCT_correctValueStartSpeedMotor2;
  }
}



////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  //******************************************** // Получаем сообщение если есть что получать. Считываем его посимвольно в цикле и прибавляем к строке.
  while (mySerial.available() > 0) {
    strData += (char)mySerial.read();
    delay(4); // без этого не успеет уловить и записать все символы в строку
  }


  //******************************************* // Обрабатываем полученную строку
  if (strData != "") {
    // Serial.println(strData);

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
    if (strData == "LFD")
    {
      light = !light;
      mySerial.print("Light = ");
      mySerial.println(light ? "ON" : "OFF");
    }
    //if (strData == "LFU") mySerial.write("TEST");

    if (strData == "L+" && maxSpeedMotor1 < 255)
    {
      maxSpeedMotor1 += 5;
      mySerial.print(maxSpeedMotor1);
    }

    if (strData == "R+" && maxSpeedMotor2 < 255)
    {
      maxSpeedMotor2 += 5;
      mySerial.print(maxSpeedMotor2);
    }

    if (strData == "L-" && maxSpeedMotor1 > 0)
    {
      maxSpeedMotor1 -= 5;
      mySerial.print(maxSpeedMotor1);
    }

    if (strData == "R-" && maxSpeedMotor2 > 0)
    {
      maxSpeedMotor2 -= 5;
      mySerial.print(maxSpeedMotor2);
    }

    if (strData.indexOf("CL") != -1 && strData.length() <= 5)
    {
      correctValueStartSpeedMotor1 = (byte)strData.substring(2).toInt();
      mySerial.print(correctValueStartSpeedMotor1);
    }

    if (strData.indexOf("CR") != -1 && strData.length() <= 5)
    {
      correctValueStartSpeedMotor2 = (byte)strData.substring(2).toInt();
      mySerial.print(correctValueStartSpeedMotor2);
    }

    if (strData == "Status")
    {
      mySerial.print("Light = ");
      mySerial.println(light ? "ON" : "OFF");
      mySerial.print("Max speed M1 = ");
      mySerial.println(maxSpeedMotor1);
      mySerial.print("Max speed M2 = ");
      mySerial.println(maxSpeedMotor2);
      mySerial.print("Start speed M1 = ");
      mySerial.println(correctValueStartSpeedMotor1);
      mySerial.print("Start speed M2 = ");
      mySerial.println(correctValueStartSpeedMotor2);
    }

    if (strData == "Save")
    {
      currentGenStruct.STRUCT_maxSpeedMotor1 = maxSpeedMotor1;
      currentGenStruct.STRUCT_maxSpeedMotor2 = maxSpeedMotor2;
      currentGenStruct.STRUCT_correctValueStartSpeedMotor1 = correctValueStartSpeedMotor1;
      currentGenStruct.STRUCT_correctValueStartSpeedMotor2 = correctValueStartSpeedMotor2;
      
      EEPROM.put(1, currentGenStruct); // записываем новые значения переменных

      mySerial.print("OK");
    }
  }


  //****************************************************** // Выполняем управление согласно полученной информации
  if (millis() - oldMillis >= interval)
  {
    oldMillis = millis();

    if (strData != "")
    {
      Serial.println(strData);
    }

    if (boolUp) forward();
    else if (boolRight) right();
    else if (boolDown) backward();
    else if (boolLeft) left();
    else if (boolCentr) stoping();
    else stoping();

    if (tempSpeedMotor1 != maxSpeedMotor1 && !noSpeedUpFlag)
    {
      if (tempSpeedMotor1 < 5)
      {
        tempSpeedMotor1 += correctValueStartSpeedMotor1;
      }
      tempSpeedMotor1 += 5;
    }

    if (tempSpeedMotor2 != maxSpeedMotor2 && !noSpeedUpFlag)
    {
      if (tempSpeedMotor2 < 5)
      {
        tempSpeedMotor2 += correctValueStartSpeedMotor2;
      }
      tempSpeedMotor2 += 5;
    }

    if (tempSpeedMotor1 != 0 || tempSpeedMotor2 != 0)
    {
      Serial.print(tempSpeedMotor1);
      Serial.print("\t");
      Serial.println(tempSpeedMotor2);
    }

    noSpeedUpFlag = false;
    ledForward();
  }
  strData = "";
}



//////////////////////////////////////////////////////////////////////////////
void forward() // движение вперед
{
  if (directionMotion == 2 || directionMotion == 3 || directionMotion == 4)
  {
    tempSpeedMotor1 = 0;
    tempSpeedMotor2 = 0;
  }
  analogWrite(pinPWM1, tempSpeedMotor1);
  analogWrite(pinIN1, 0);
  analogWrite(pinPWM2, tempSpeedMotor2);
  analogWrite(pinIN2, 0);
  directionMotion = 1;
}

void right() // движение вправо
{
  if (directionMotion == 1 || directionMotion == 2 || directionMotion == 3)
  {
    tempSpeedMotor1 = 0;
    tempSpeedMotor2 = 0;
  }
  analogWrite(pinPWM1, tempSpeedMotor1);
  analogWrite(pinIN1, 0);
  analogWrite(pinPWM2, 0);
  analogWrite(pinIN2, tempSpeedMotor2);
  directionMotion = 4;
}

void backward() // движение назад
{
  if (directionMotion == 1 || directionMotion == 3 || directionMotion == 4)
  {
    tempSpeedMotor1 = 0;
    tempSpeedMotor2 = 0;
  }
  analogWrite(pinPWM1, 0);
  analogWrite(pinIN1, tempSpeedMotor1);
  analogWrite(pinPWM2, 0);
  analogWrite(pinIN2, tempSpeedMotor2);
  directionMotion = 2;
}

void left() // движение влево
{
  if (directionMotion == 1 || directionMotion == 2 || directionMotion == 4)
  {
    tempSpeedMotor1 = 0;
    tempSpeedMotor2 = 0;
  }
  analogWrite(pinPWM1, 0);
  analogWrite(pinIN1, tempSpeedMotor1);
  analogWrite(pinPWM2, tempSpeedMotor2);
  analogWrite(pinIN2, 0);
  directionMotion = 3;
}

void stoping() // остановка
{
  tempSpeedMotor1 = 0;
  tempSpeedMotor2 = 0;
  analogWrite(pinPWM1, 0);
  analogWrite(pinIN1, 0);
  analogWrite(pinPWM2, 0);
  analogWrite(pinIN2, 0);
  directionMotion = 0;
  noSpeedUpFlag = true;
}

void ledForward() // светодиоды спереди
{
  digitalWrite(A0, light);
  digitalWrite(A2, light);
}
