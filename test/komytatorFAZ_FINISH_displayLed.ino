#include <ZMPT101B.h>
#include <AnalogKey.h>
#include <GyverButton.h>
#include <TM1637TinyDisplay.h>        // библиотека управления семисигментной матрицей
#include <Arduino.h>
#include <EEPROM.h>                   // библиотека EEPROM
#include "GyverTimer.h"               // библиотека таймера


#define INIT_ADDR 1023                // номер резервной ячейки. EEPROM
#define INIT_KEY 50                   // ключ первого запуска. 0-254, на выбор. EEPROM
#define LineA A1              // входной PIN LineA
#define LineB A2              // входной PIN LineB
#define LineC A3              // входной PIN LineC
#define OUTLineA 10            // выходной PIN LineA
#define OUTLineB 11            // выходной PIN LineB
#define OUTLineC 12            // выходной PIN LineC
#define keyPin1 3
#define keyPin2 4
#define keyPin3 5
#define CLK A4                        // пины управления семисигментной матрицей
#define DIO A5                        // пины управления семисигментной матрицей

GTimer myTimer3(MS);                  // создать таймер (по умолч. в режиме интервала)
GButton butt1(keyPin1);
GButton butt2(keyPin2);
GButton butt3(keyPin3);
TM1637TinyDisplay display(CLK, DIO);  // активация семисигментной матрици

bool flagR, flagHold, flagHold1, flagMi, flagP0, flag, flag0, flag00, flag000, flagT, flagTT, flag1, flag11, flag2, flag22, flag3, flag33, flag4, flag44 = false;
uint8_t U, i, ii, qqq, qqqq, w, ww, e, ee;
uint8_t valA, valB, valC ;      // тип переменной для valA = analogRead(LineA), valB = analogRead(LineB), valC = analogRead(LineC)
uint8_t hiVolts;          // значение верхнего уровня отключения
uint8_t lowVvolts;        // значение нижнего уровня тоключения
boolean LA, LB, LC = false;     // тип переменной для FLAG LA, LB, LC и присвоение " 0 " ;
uint32_t Now;
uint8_t timeIt;    //переменная устанавлеваемоговремя итерации
int timeit2;
uint32_t myTimer1;                // таймер
uint32_t eepromTimer = 0;         //EEPROM таймер
boolean eepromFlag = false;       //EEPROM флаг = 0

ZMPT101B voltageSensorLineA(LineA);
ZMPT101B voltageSensorLineB(LineB);
ZMPT101B voltageSensorLineC(LineC);

void setup() {
  EEPROMRead();
  Serial.begin(115200);                   // открытие Монитора порта
  pinMode(LineA, INPUT_PULLUP);
  pinMode(LineB, INPUT_PULLUP);
  pinMode(LineC, INPUT_PULLUP);
  pinMode(OUTLineA, OUTPUT);            // открытие порта OUTLineA на выход
  pinMode(OUTLineB, OUTPUT);            // открытие порта OUTLineB на выход
  pinMode(OUTLineC, OUTPUT);            // открытие порта OUTLineC на выход
  digitalWrite(OUTLineA, LOW);         // установка состояния OUTLineA = 0
  digitalWrite(OUTLineB, LOW);         // установка состояния OUTLineB = 0
  digitalWrite(OUTLineC, LOW);         // установка состояния OUTLineC = 0

  display.setBrightness(BRIGHT_7);      // яркость, 0 - 7 (минимум - максимум)
  display.clear(); display.showString("4", 1, 0); delay(500); display.clear(); display.showString("3", 1, 1); delay(500); display.clear(); display.showString("2", 1, 2); delay(500); display.clear(); display.showString("1", 1, 3); delay(500); display.clear(); display.showString("G", 1, 1); display.showString("O", 1, 2); delay(1000);


  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt1.setDirection(NORM_OPEN);
}

//============================================================================
void loop() {
  checkEEPROM();                                             // проверка EEPROM

  valA = voltageSensorLineA.getVoltageAC(50) * 5;            // считывание пораметра с LineA и сохранение его в valA
  valB = voltageSensorLineB.getVoltageAC(50) * 5;            // считывание пораметра с LineB и сохранение его в valB
  valC = voltageSensorLineC.getVoltageAC(50) * 5;            // считывание пораметра с LineC и сохранение его в valC
  butt1.tick();                                              // обязательная функция отработки. Должна постоянно опрашиваться
  bool btnState = !digitalRead(keyPin1);
  bool btnState1 = !digitalRead(keyPin2);
  bool btnState2 = !digitalRead(keyPin3);

  static uint32_t myTimer = 0;
  if (flagMi == false && millis() -  myTimer > 25000) {
    w = 55; ii = 55; flag000 = false; flag00 = false; // свой таймер на миллис, 0.5 секунды
    myTimer = millis(); flagMi = true; display.showNumber(U, false, 4, 0); display.showString("U", 1, 0); flagHold = false;
  }

  //===========================MENU==========================
  if (butt1.isHold()) {
    flagHold = true;
    flag00 = true;
    w = 50;
    flagMi = false;
  }
  if (btnState && !flag &&  w == 50 && i < 5) {
    flagHold = true;  flag00 = true ;
    flag = true;
    i++;
    if (i == 1) {
      display.showNumber(hiVolts, false, 4, 0);
      display.showString("H", 1, 0);
    }
    if (i == 2) {
      display.showNumber(lowVvolts, false, 4, 0);
      display.showString("L", 1, 0);
    }
    if (i == 3) {
      display.showNumber(timeIt, false, 4, 0);
      display.showString("t", 1, 0);
    }
  }
  if (!btnState && flag) {
    flag = false;
  }
  if (i == 4) i = 0;

  //===========================valA=valB=valC=============================
  if (btnState && !flag000) {
    flagHold = true;
    ii = 50;
    flagMi = false;
    flag000 = true;
    display.clear();
    display.showNumber(valA, false, 4, 0);
    display.showString("A", 1, 0);
    ee = 1;
  }
  if (btnState1 && !flag00 && !flagT && ii == 50 ) {
    flagHold = true;
    flagT = true;
    ee++;
    if (ee == 1) {
      display.showNumber(valA, false, 4, 0);
      display.showString("A", 1, 0);
    }
    if (ee == 2) {
      display.showNumber(valB, false, 4, 0);
      display.showString("B", 1, 0);
    }
    if (ee == 3) {
      display.showNumber(valC, false, 4, 0);
      display.showString("C", 1, 0);
    }
  }
  if (!btnState1 && flagT) {
    flagT = false;
  }
  if (ee == 3) ee = 0;


  //===========================hiVolts==========================
  if (btnState1 && !flag1 && w == 50 && i == 1) {
    flag1 = true;
    hiVolts++;
    setBright();
    display.showNumber(hiVolts, false, 4, 0); display.showString("H", 1, 0);
  }
  if (!btnState1 && flag1) {
    flag1 = false;
  }
  //--------------------------------------------------------
  if (btnState2 && !flag11 && w == 50 && i == 1) {
    flag11 = true;
    hiVolts--;
    setBright();
    display.showNumber(hiVolts, false, 4, 0); display.showString("H", 1, 0);
  }
  if (!btnState2 && flag11) {
    flag11 = false;
  }
  if (hiVolts == 270 ) {
    hiVolts = 0;
  }

  //===========================lowVvolts==========================
  if (btnState1 && !flag2 && w == 50 && i == 2) {
    flag2 = true;
    lowVvolts++;
    setBright();
    display.showNumber(lowVvolts, false, 4, 0); display.showString("L", 1, 0);
  }
  if (!btnState1 && flag2) {
    flag2 = false;
  }
  //--------------------------------------------------------
  if (btnState2 && !flag22 && w == 50 && i == 2) {
    flag22 = true;
    lowVvolts--;
    setBright();
    display.showNumber(lowVvolts, false, 4, 0); display.showString("L", 1, 0);
  }
  if (!btnState2 && flag22) {
    flag22 = false;
  }
  if (lowVvolts == 270 ) {
    lowVvolts = 0;
  }
  //--------------------------------------------------------
  if (btnState1 && !flag3 && w == 50 && i == 3) {
    flag3 = true;
    timeIt++;

    setBright();
    display.showNumber(timeIt, false, 4, 0); display.showString("t", 1, 0);
  }
  if (!btnState2 && flag3) {
    flag3 = false;
  }
  if (timeIt == 11 ) {
    timeIt = 0;
  }


  //=====================================================================================

  if (valA >= lowVvolts && valA <= hiVolts && valA > 150  && (LB + LC == false)) { // если valA >= 0 И valA <=100 И (LB+LC == false) то
    LA = 1;                                                             // состояние флага LA = 1
    digitalWrite(OUTLineB, LOW);                                  // установка состояния OUTLineB = HIGH
    digitalWrite(OUTLineC, LOW);                                  // установка состояния OUTLineC = HIGH
    delay(timeit2);
    digitalWrite(OUTLineA, HIGH);                                 // установка состояния OUTLineA = LOW
    U =  valA;
  }                                                  // значения по напряжению
  else                                                          // если условие if не выполненоб тогда
    LA = 0;                                                             // состояние флага LA = 0

  if (valB >= lowVvolts && valB <= hiVolts && valB > 150  && (LA + LC == false)) {
    LB = 1;
    digitalWrite(OUTLineA, LOW);
    digitalWrite(OUTLineC, LOW);
    delay(timeit2);
    digitalWrite(OUTLineB, HIGH);
    U =  valB;
  }
  else
    LB = 0;

  if (valC >= lowVvolts && valC <= hiVolts && valC > 150 && (LA + LB == false)) {
    LC = 1;
    digitalWrite(OUTLineB, LOW);
    digitalWrite(OUTLineA, LOW);
    delay(timeit2);
    digitalWrite(OUTLineC, HIGH);
    U =  valC;
  }
  else
    LC = 0;


   if (valA< lowVvolts || valA> hiVolts) {  digitalWrite(OUTLineA, LOW);  }

   if (valB< lowVvolts || valB> hiVolts) { digitalWrite(OUTLineB, LOW);  }

   if (valC< lowVvolts || valC> hiVolts) {  digitalWrite(OUTLineC, LOW);  }



  static uint32_t myTimerD = 0;
  if (flagHold == false && millis() - myTimerD > 5000) {
    display.showNumber(U, false, 4, 0);
    display.showString("U", 1, 0);
    myTimerD = millis();
  }
/*
  Serial.print(String("hiVolts = ") + hiVolts + " V   ");
  Serial.print(String("lowVvolts = ") + lowVvolts + " V   ");
  Serial.print(String("valA = ") + valA + " V   ");
  Serial.print(String("valB = ") + valB + " V   ");
  Serial.println(String("valC = ") + valC + " V   ");
  */
  if (timeIt == 0) {
    timeit2 = 0;
  }
  else if (timeIt == 1) {
    timeit2 = 10;
  }
  else if (timeIt == 2) {
    timeit2 = 20;
  }
  else if (timeIt == 3) {
    timeit2 = 30;
  }
  else if (timeIt == 4) {
    timeit2 = 40;
  }
  else if (timeIt == 5) {
    timeit2 = 50;
  }
  else if (timeIt == 6) {
    timeit2 = 60;
  }
  else if (timeIt == 7) {
    timeit2 = 70;
  }
  else if (timeIt == 8) {
    timeit2 = 80;
  }
  else if (timeIt == 9) {
    timeit2 = 90;
  }
  else if (timeIt == 10) {
    timeit2 = 100;
  }



}
//=======================setBright===================================
void setBright() {
  hiVolts = constrain(hiVolts, 170, 250); // ограничили от 0 до 100
  lowVvolts = constrain(lowVvolts, 170, 250); // ограничили от 0 до 100
  timeIt = constrain(timeIt, 0, 11);                   // ограничили от 0 до 100
  eepromFlag = true;                                // поднять флаг
  eepromTimer = millis();                           // сбросить таймер
}
//=======================checkEEPROM===================================
void checkEEPROM() {

  if (eepromFlag && (millis() - eepromTimer >= 10000) ) {// если флаг поднят и с последнего нажатия прошло 10 секунд (10 000 мс)
    eepromFlag = false;                           // опустили флаг
    EEPROM.put(0, hiVolts);                 // записали в EEPROM
    EEPROM.put(1, lowVvolts);
    EEPROM.put(9, timeIt);

  }
}
//=======================EEPROMRead===================================

void EEPROMRead() {
  if ( EEPROM.read(INIT_ADDR) != INIT_KEY) {     // первый запуск (ЕСЛИ INIT_ADDR (1023)не равен INIT_KEY (50) то записать EEPROM.write(INIT_ADDR, INIT_KEY);EEPROM.put(0, izmenenieTemp);
    EEPROM.write(INIT_ADDR, INIT_KEY);      // записали ключ
    EEPROM.put(0, hiVolts);         // записали стандартное значение температуры. в данном случае это значение переменной, объявленное выше
  }
  EEPROM.get(0, hiVolts);       // прочитали температуру

  if ( EEPROM.read(INIT_ADDR) != INIT_KEY) {     // первый запуск (ЕСЛИ INIT_ADDR (1023)не равен INIT_KEY (50) то записать EEPROM.write(INIT_ADDR, INIT_KEY);EEPROM.put(0, izmenenieTemp);
    EEPROM.write(INIT_ADDR, INIT_KEY);      // записали ключ
    EEPROM.put(1, lowVvolts);         // записали стандартное значение температуры. в данном случае это значение переменной, объявленное выш
  }
  EEPROM.get(1, lowVvolts);


  if ( EEPROM.read(INIT_ADDR) != INIT_KEY) {     // первый запуск (ЕСЛИ INIT_ADDR (1023)не равен INIT_KEY (50) то записать EEPROM.write(INIT_ADDR, INIT_KEY);EEPROM.put(0, izmenenieTemp);
    EEPROM.write(INIT_ADDR, INIT_KEY);      // записали ключ
    EEPROM.put(9, timeIt);         // записали стандартное значение температуры. в данном случае это значение переменной, объявленное выш
  }
  EEPROM.get(9, timeIt);



}
