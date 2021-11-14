/*
  17 switch keyboard

          B5(9)   B6(10)  B2(16)   B3(14)
  __________________________________________
  B4(8) |backspace  /       *        -
  E6(7) |   7       8       9       N/A
  D7(6) |   4       5       6        +
  C6(5) |   1       2       3       N/A
  D4(4) |  N/A      0       .      Enter

  LCD I2C 16x2
  ____________
  GND  GND
  VCC  VCC
  2    SDA
  3    SCL

  pcb, 17 1n4148 diodes, 3x2U pcb stabilizers, pro micro, 16x2 lcd screen, 2 position switch, 17 switches, 17 keycaps, tp4056, lipo 800/600 mAh battery, 4 m2 screws, 4 m3 screws, 4 orings

  enter - first press - shows result
        - second press - repeat the operation...
        - changes value of "left" to result of the operation

  del / num - press - deletes last pressed character
            - press after the operation changes value of "left" to result of the operation and deletes last character
            - hold - changes mode saves left, middle, right
            - hold after the operation changes value of "left" to result of the operation and changes mode

  DISPLAY
  arduino 32 bit float  - rounded to 2 decimal places
  display up max        - 12 + 2 + 2 (numbers + operation + dot)
  display down          - 14 + 1 + 1 (result, numbers + positive/negative + dot)
*/

//keyboard
#include <Keyboard.h>

//lcd
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

//pins
const byte rows[] = {8, 7, 6, 5, 4};
const int rowcount = sizeof(rows) / sizeof(rows[0]);

const byte cols[] = {9, 10, 16, 14};
const int colcount = sizeof(cols) / sizeof(cols[0]);

//keymap (comment up)
const uint8_t keymap[rowcount][colcount] = {8, '/', '*', '-', 55, 56, 57, 32, 52, 53, 54, '+', 49, 50, 51, 32, 32, 48, '.', 10}; // 17 (+3)

//state of key
bool state[rowcount][colcount] = {}; // 17 (+3)

//debounce timer for every switch
unsigned long debounce[rowcount][colcount] = {0}; // 17 (+3)

//debounce delay
const byte debouncedelay = 5;

//change to calculator
bool hybrid = 1;

//for calculator
String left = "", right = "";

byte middle = 0;

bool afterenter = 0;

float result = 0;

void setup() {
  //state 1 (inactive)
  for (int k = 0; k < rowcount; k++)
    for (int m = 0; m < colcount; m++)
      state[k][m] = 1;

  //matrix inputs
  for (int i = 0; i < rowcount; i++)
    pinMode(rows[i], INPUT);

  for (int j = 0; j < colcount; j++)
    pinMode(cols[j], INPUT_PULLUP);

  lcd.begin();
  lcd.noBlink();
  lcd.noCursor();
  lcd.noBacklight();
  lcd.setCursor(0, 0);

  Keyboard.begin();
}

void printingsequencetoprow()
{
  lcd.clear();

  lcd.print(left);

  if (middle)
    lcd.print(char(middle));

  lcd.print(right);
}

void printingsequencebottomrow()
{
  lcd.setCursor(0, 1);

  lcd.print(result);

  lcd.setCursor(0, 0);
}

void deletelastchar()
{
  if (middle == 0)
  {
    int lastIndex = left.length() - 1;
    left.remove(lastIndex);
  }
  else if (right.toFloat())
  {
    int lastIndex = right.length() - 1;
    right.remove(lastIndex);
  }
  else
    middle = 0;
}

void loop() {
  //normal keyboard operating mode
  //i - row j - col
  for (int i = 0; i < rowcount; i++)
  {
    byte row = rows[i];

    pinMode(row, OUTPUT);
    digitalWrite(row, LOW);

    for (int j = 0; j < colcount; j++)
    {
      byte col = cols[j];

      pinMode(col, INPUT_PULLUP);
      delay(1);

      //press
      if (digitalRead(col) == 0)
      {
        if (state[i][j] == 1)
        {
          //assign the pressed key
          char key = keymap[i][j];

          if (hybrid == 0)
          {
            //numpad mode
            state[i][j] = 0;
            debounce[i][j] = millis();
            Keyboard.press(key);

            //to calculator
            if (key == 8)
            {
              int temp = 0;
              while (digitalRead(col) == 0)
              {

                temp += 1;
                delay(30);

                if (temp == 100)
                {
                  hybrid = !hybrid;

                  lcd.clear();

                  lcd.setCursor(0, 0);

                  lcd.print("calculator");

                  delay(500);

                  //printing sequence top row
                  printingsequencetoprow();
                }
              }
            }
          }
          else
          {
            //calculator mode
            if (afterenter == 0)
            {
              //'.'
              if (key == 46)
              {
                if (middle == 0)
                {
                  if (left.indexOf('.') == -1)
                  {
                    if (left.toFloat())
                      left += ".";
                    else
                      left += "0.";
                  }
                }
                else
                {
                  if (right.indexOf('.') == -1)
                  {
                    if (right.toFloat())
                      right += ".";
                    else
                      right += "0.";
                  }
                }
              }

              //'0-10'
              if (key > 47 && key < 58)
              {
                if (middle == 0)
                {
                  left += char(key);
                }
                else
                {
                  right += char(key);
                }
              }

              //'/, *, -, +'
              if (key == '/')
              {
                middle = '/';
              }

              if (key == '*')
              {
                middle = '*';
              }

              if (key == '-')
              {
                middle = '-';
              }

              if (key == '+')
              {
                middle = '+';
              }

              //printing sequence top row
              printingsequencetoprow();

              //enter
              if (key == 10)
              {
                result = left.toFloat();

                if (middle == '/')
                {

                  if (right.toFloat())
                  {
                    result = left.toFloat() / right.toFloat();
                    left = result;
                  }
                  else
                  {
                    lcd.clear();

                    lcd.print("nie ma tak dobrze");
                  }
                }

                if (middle == '*')
                {
                  result = left.toFloat() * right.toFloat();
                  left = result;
                }

                if (middle == '-')
                {
                  result = left.toFloat() - right.toFloat();
                  left = result;
                }

                if (middle == '+')
                {
                  result = left.toFloat() + right.toFloat();
                  left = result;
                }

                //printing sequence bottom row
                printingsequencebottomrow();

                afterenter = 1;
              }

              if (key == 8)
              {
                bool dontremove = 0;
                byte temp = 0;

                //to numpad
                while (digitalRead(col) == 0)
                {
                  temp += 1;
                  delay(30);

                  if (temp == 100)
                  {
                    dontremove = 1;

                    hybrid = !hybrid;

                    lcd.clear();

                    lcd.setCursor(0, 0);

                    lcd.print("numpad");

                    delay(500);

                    lcd.clear();
                  }
                }

                if (dontremove == 0)
                {
                  //del / num
                  deletelastchar();

                  //printing sequence top row
                  printingsequencetoprow();
                }
              }
            }
            else
            {
              //'0-10' || "."
              if ((key > 47 && key < 58) || (key == 46))
              {
                lcd.clear();

                left = char(key);

                middle = 0;

                right = "";

                if (key == 46)
                {
                  left = "0.";
                }

                afterenter = 0;

                //printing sequence top row
                printingsequencetoprow();
              }

              //'/, *, -, +'
              if (key == '/')
              {
                middle = '/';

                right = "";

                afterenter = 0;

                //printing sequence top row
                printingsequencetoprow();
              }

              if (key == '*')
              {
                middle = '*';

                right = "";

                afterenter = 0;

                //printing sequence top row
                printingsequencetoprow();
              }

              if (key == '-')
              {
                middle = '-';

                right = "";

                afterenter = 0;

                //printing sequence top row
                printingsequencetoprow();
              }

              if (key == '+')
              {
                middle = '+';

                right = "";

                afterenter = 0;

                //printing sequence top row
                printingsequencetoprow();
              }

              //enter
              if (key == 10)
              {
                //printing sequence top row
                printingsequencetoprow();

                result = left.toFloat();

                if (middle == '/')
                {

                  if (right.toFloat())
                  {
                    result = left.toFloat() / right.toFloat();
                    left = result;
                  }
                  else
                  {
                    lcd.clear();

                    lcd.print("nie ma tak dobrze");
                  }
                }

                if (middle == '*')
                {
                  result = left.toFloat() * right.toFloat();
                  left = result;
                }

                if (middle == '-')
                {
                  result = left.toFloat() - right.toFloat();
                  left = result;
                }

                if (middle == '+')
                {
                  result = left.toFloat() + right.toFloat();
                  left = result;
                }

                //printing sequence bottom row
                printingsequencebottomrow();
              }

              //del / num
              if (key == 8)
              {
                bool dontremove = 0;
                byte temp = 0;

                while (digitalRead(col) == 0)
                {
                  temp += 1;
                  delay(30);

                  //to calculator
                  if (temp == 100)
                  {
                    dontremove = 1;
                    hybrid = !hybrid;

                    middle = 0;

                    right = "";

                    lcd.clear();

                    lcd.setCursor(0, 0);

                    lcd.print("numpad");

                    delay(500);

                    lcd.clear();
                  }
                }
                if (dontremove == 0)
                {
                  right = "";

                  middle = 0;

                  //del
                  deletelastchar();

                  //printing sequence top row
                  printingsequencetoprow();
                }
                afterenter = 0;
              }
            }
          }
          state[i][j] = 0;
        }
      }

      //release
      if (state[i][j] == 0)
      {
        if (digitalRead(col) == 1)
        {
          if (millis() - debounce[i][j] > debouncedelay)
          {
            {
              state[i][j] = 1;
              Keyboard.release(keymap[i][j]);
            }
          }
        }
      }
      pinMode(col, INPUT);
    }
    pinMode(row, INPUT);
  }
}
