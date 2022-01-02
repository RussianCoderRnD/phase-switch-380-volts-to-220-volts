#include <Arduino.h>
#include <ZMPT101B.h>          // библиотека управления датчиком напряжения
#include <TM1637TinyDisplay.h> // библиотека управления семисигментной матрицей

#define LineA A1    // входной PIN LineA
#define LineB A2    // входной PIN LineB
#define LineC A3    // входной PIN LineC
#define OUTLineA 10 // выходной PIN LineA
#define OUTLineB 11 // выходной PIN LineB
#define OUTLineC 12 // выходной PIN LineC
#define CLK A4      // пины управления семисигментной матрицей
#define DIO A5      // пины управления семисигментной матрицей

TM1637TinyDisplay display(CLK, DIO); // активация семисигментной матрици

uint8_t U;                  // переменная для вывода напряжение на дисплей
uint8_t valA, valB, valC;   // тип переменной для valA = analogRead(LineA), valB = analogRead(LineB), valC = analogRead(LineC)
uint8_t hiVolts = 250;      // значение верхнего уровня отключения
uint8_t lowVvolts = 160;    // значение нижнего уровня тоключения
boolean LA, LB, LC = false; // тип переменной для FLAG LA, LB, LC и присвоение " 0 " ;
uint8_t timeIt;             //переменная устанавлеваемоговремя итерации
int timeit2 = 50;

ZMPT101B voltageSensorLineA(LineA); // активаци датчика на пряжения с ПИНом
ZMPT101B voltageSensorLineB(LineB); // активаци датчика на пряжения с ПИНом
ZMPT101B voltageSensorLineC(LineC); // активаци датчика на пряжения с ПИНом

const uint8_t ANIMATION3[33][4] = {
    {0x08, 0x00, 0x00, 0x00}, // Frame 0
    {0x00, 0x08, 0x00, 0x00}, // Frame 1
    {0x00, 0x00, 0x08, 0x00}, // Frame 2
    {0x00, 0x00, 0x00, 0x08}, // Frame 3
    {0x00, 0x00, 0x00, 0x04}, // Frame 4
    {0x00, 0x00, 0x00, 0x02}, // Frame 5
    {0x00, 0x00, 0x00, 0x01}, // Frame 6
    {0x00, 0x00, 0x01, 0x00}, // Frame 7
    {0x00, 0x01, 0x00, 0x00}, // Frame 8
    {0x01, 0x00, 0x00, 0x00}, // Frame 9
    {0x20, 0x00, 0x00, 0x00}, // Frame 10
    {0x10, 0x00, 0x00, 0x00}, // Frame 11
    {0x08, 0x00, 0x00, 0x00}, // Frame 12
    {0x00, 0x08, 0x00, 0x00}, // Frame 13
    {0x00, 0x00, 0x08, 0x00}, // Frame 14
    {0x00, 0x00, 0x04, 0x00}, // Frame 15
    {0x00, 0x00, 0x02, 0x00}, // Frame 16
    {0x00, 0x00, 0x01, 0x00}, // Frame 17
    {0x00, 0x01, 0x00, 0x00}, // Frame 18
    {0x00, 0x20, 0x00, 0x00}, // Frame 19
    {0x00, 0x10, 0x00, 0x00}, // Frame 20
    {0x00, 0x08, 0x00, 0x00}, // Frame 21
    {0x00, 0x00, 0x08, 0x00}, // Frame 22
    {0x00, 0x00, 0x00, 0x08}, // Frame 23
    {0x00, 0x00, 0x00, 0x04}, // Frame 24
    {0x00, 0x00, 0x00, 0x02}, // Frame 25
    {0x00, 0x00, 0x00, 0x01}, // Frame 26
    {0x00, 0x00, 0x01, 0x00}, // Frame 27
    {0x00, 0x01, 0x00, 0x00}, // Frame 28
    {0x01, 0x00, 0x00, 0x00}, // Frame 29
    {0x20, 0x00, 0x00, 0x00}, // Frame 30
    {0x10, 0x00, 0x00, 0x00}, // Frame 31
    {0x00, 0x00, 0x00, 0x00}  // Frame 32
};
void setup()
{

  Serial.begin(9600);
  pinMode(LineA, INPUT);           // открытие порта на ВХОД
  pinMode(LineB, INPUT);           // открытие порта на ВХОД
  pinMode(LineC, INPUT);           // открытие порта на ВХОД
  pinMode(OUTLineA, OUTPUT);       // открытие порта OUTLineA на выход
  pinMode(OUTLineB, OUTPUT);       // открытие порта OUTLineB на выход
  pinMode(OUTLineC, OUTPUT);       // открытие порта OUTLineC на выход
  digitalWrite(OUTLineA, LOW);     // установка состояния OUTLineA = 0
  digitalWrite(OUTLineB, LOW);     // установка состояния OUTLineB = 0
  digitalWrite(OUTLineC, LOW);     // установка состояния OUTLineC = 0
  display.setBrightness(BRIGHT_0); // яркость дисплея, 0 - 7 (минимум - максимум)
  display.clear();                 // очистка дисплея
  display.showAnimation(ANIMATION3, FRAMES(ANIMATION3), TIME_MS(50));
  display.showString("G", 1, 1);
  display.showString("O", 1, 2);
  delay(1000);
}

//============================================================================
void loop()
{
  valA = voltageSensorLineA.getVoltageAC(50) * 5; // считывание пораметра с LineA и сохранение его в valA
  valB = voltageSensorLineB.getVoltageAC(50) * 5; // считывание пораметра с LineB и сохранение его в valB
  valC = voltageSensorLineC.getVoltageAC(50) * 5; // считывание пораметра с LineC и сохранение его в valC

  if (valA >= lowVvolts && valA <= hiVolts && valA > 150 && (LB + LC == false))
  {                              // если valA >= 170 И valA <=249 И valA > 150 && (LB+LC == false) то
    LA = 1;                      // состояние флага LA = 1
    digitalWrite(OUTLineB, LOW); // установка состояния OUTLineB = HIGH
    digitalWrite(OUTLineC, LOW); // установка состояния OUTLineC = HIGH
    delay(timeit2);
    digitalWrite(OUTLineA, HIGH);       // установка состояния OUTLineA = LOW
    U = valA;                           // присвоение переменной U присваевается значение фазы valA
    display.showNumber(U, false, 4, 0); // вывод на дисплей значение напряжения
    display.showString("A", 1, 0);      // вывод на дисплей буквы фазы
  }                                     // значения по напряжению
  else                                  // если условие if не выполненоб тогда
  {
    LA = 0;
  } // состояние флага LA = 0

  if (valB >= lowVvolts && valB <= hiVolts && valB > 150 && (LA + LC == false))
  {
    LB = 1;
    digitalWrite(OUTLineA, LOW);
    digitalWrite(OUTLineC, LOW);
    delay(timeit2);
    digitalWrite(OUTLineB, HIGH);
    U = valB;
    display.showNumber(U, false, 4, 0);
    display.showString("8", 1, 0);
  }
  else
  {
    LB = 0;
  }

  if (valC >= lowVvolts && valC <= hiVolts && valC > 150 && (LA + LB == false))
  {
    LC = 1;
    digitalWrite(OUTLineB, LOW);
    digitalWrite(OUTLineA, LOW);
    delay(timeit2);
    digitalWrite(OUTLineC, HIGH);
    U = valC;
    display.showNumber(U, false, 4, 0);
    display.showString("C", 1, 0);
  }
  else

  {
    LC = 0;
  }

  if (valA < lowVvolts || valA > hiVolts)
  {
    digitalWrite(OUTLineA, LOW);
  }

  if (valB < lowVvolts || valB > hiVolts)
  {
    digitalWrite(OUTLineB, LOW);
  }

  if (valC < lowVvolts || valC > hiVolts)
  {
    digitalWrite(OUTLineC, LOW);
  }

  /* Serial.print(String("valA = ") + valA + " V   ");
  Serial.print(String("valB = ") + valB + " V   ");
  Serial.println(String("valC = ") + valC + " V   ");
  delay(500);
  */
}
