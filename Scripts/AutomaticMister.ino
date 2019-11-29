//Automatic Mister
//By Anthony Brogni

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define TS_MINX 130
#define TS_MINY 904
#define TS_MAXX 938
#define TS_MAXY 138

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Elegoo_TFTLCD tft;

int hours = 8;
int minutes = 0;
int seconds = 0;

int times = 0;
int waitTimes = 0;
int wait = 15000;

int relay = 53;

bool reconfiguring = true;
bool spraying = false;

int assignedHours;
int assignedWait;

bool answered1 = false;
bool answered2 = false;
bool answered3 = false;
bool answered4 = false;

void setup() {
  Serial.begin(9600);

  tft.reset();

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
  } else if (identifier == 0x9328) {
  } else if (identifier == 0x4535) {
  } else if (identifier == 0x7575) {
  } else if (identifier == 0x9341) {
  } else if (identifier == 0x8357) {
  } else if (identifier == 0x0101)
  {
    identifier = 0x9341;
  }
  else
  {
    identifier = 0x9341;
  }

  tft.begin(identifier);
  boot();
  configSystem();
}

void loop()
{
  if (reconfiguring == false)
  {
    buttonPress();
    delay(8);
    if (times == 125)
    {
      times = 0;
      subtractTime();
    }
    else
    {
      times++;
    }
  }
}

void boot()
{
  tft.fillScreen(BLUE);
  tft.setRotation(0);
  tft.setCursor(35, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Automatic Mister v2.5");
  tft.setCursor(110, 200);
  tft.println("Made by");
  tft.setCursor(70, 220);
  tft.println("Anthony Brogni");
  tft.setCursor(60, 100);
  tft.setTextSize(4);
  tft.println("Welcome!");
  delay(4000);
  tft.fillScreen(BLUE);
}

void configSystem()
{
  tft.setCursor(45, 60);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Please take a");
  tft.setCursor(10, 85);
  tft.println("moment to help us");
  tft.setCursor(30, 110);
  tft.println("configure your");
  tft.setCursor(90, 135);
  tft.println("system!");
  delay(4000);
  tft.fillScreen(BLUE);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("The default spray interval");
  tft.setCursor(40, 20);
  tft.println("is every eight hours");
  tft.setCursor(5, 60);
  tft.println("Would you like it to stay");
  tft.setCursor(90, 80);
  tft.println("that way?");
  tft.fillRect(50, 150, 80, 50, BLACK);
  tft.drawRect(50, 150, 80, 50, WHITE);
  tft.setCursor(55, 158);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("Yes");
  tft.fillRect(180, 150, 80, 50, BLACK);
  tft.drawRect(180, 150, 80, 50, WHITE);
  tft.setCursor(200, 158);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("NO");
  while (answered1 == false)
  {
    TSPoint p = ts.getPoint();

    if (p.z > ts.pressureThreshhold) {

      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\n");

      p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
      p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

      if (p.x > 180 && p.x < 260 && p.y > 80 && p.y < 200)
      {
        //This is important, because the libraries are sharing pins
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        answered1 = true;

        tft.fillScreen(BLUE);

        config2();
      }
      else if (p.x > 50 && p.x < 130 && p.y > 80 && p.y < 200)
      {
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        tft.fillScreen(BLUE);

        answered1 = true;

        tft.setCursor(15, 0);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.println("How many hours would you");
        tft.setCursor(35, 15);
        tft.println("like to set the spray");
        tft.setCursor(95, 30);
        tft.println("interval to?");

        tft.fillRect(30, 80, 50, 50, BLACK);
        tft.drawRect(30, 80, 50, 50, WHITE);
        tft.setCursor(45, 88);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("2");
        tft.fillRect(130, 80, 50, 50, BLACK);
        tft.drawRect(130, 80, 50, 50, WHITE);
        tft.setCursor(145, 88);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("4");
        tft.fillRect(230, 80, 50, 50, BLACK);
        tft.drawRect(230, 80, 50, 50, WHITE);
        tft.setCursor(245, 88);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("6");
        tft.fillRect(30, 180, 50, 50, BLACK);
        tft.drawRect(30, 180, 50, 50, WHITE);
        tft.setCursor(45, 188);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("8");
        tft.fillRect(130, 180, 50, 50, BLACK);
        tft.drawRect(130, 180, 50, 50, WHITE);
        tft.setCursor(130, 188);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("10");
        tft.fillRect(230, 180, 50, 50, BLACK);
        tft.drawRect(230, 180, 50, 50, WHITE);
        tft.setCursor(230, 188);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("12");

        while (answered2 == false)
        {
          TSPoint p = ts.getPoint();

          if (p.z > ts.pressureThreshhold) {

            Serial.print("X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.print(p.y);
            Serial.print("\n");

            p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
            p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

            if (p.x > 230 && p.x < 280 && p.y > 80 && p.y < 130)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 2;

              tft.fillScreen(BLUE);

              config2();
            }
            else if (p.x > 130 && p.x < 180 && p.y > 80 && p.y < 130)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 4;

              tft.fillScreen(BLUE);

              config2();
            }
            else if (p.x > 30 && p.x < 80 && p.y > 80 && p.y < 130)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 6;

              tft.fillScreen(BLUE);

              config2();
            }
            else if (p.x > 230 && p.x < 280 && p.y > 180 && p.y < 230)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 8;

              tft.fillScreen(BLUE);

              config2();
            }
            else if (p.x > 130 && p.x < 180 && p.y > 180 && p.y < 230)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 10;

              tft.fillScreen(BLUE);

              config2();
            }
            else if (p.x > 30 && p.x < 80 && p.y > 180 && p.y < 230)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 12;

              tft.fillScreen(BLUE);

              config2();
            }
          }
        }
      }
    }
  }
}

void subtractTime()
{
  if (seconds == 0)
  {
    if (minutes == 0)
    {
      if (hours == 0)
      {
        if (spraying == false)
        {
          tft.fillRect(0, 15, 250, 16, BLUE);
        }
        if (answered2 == true)
        {
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = assignedHours;
        }
        else
        {
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = 8;
        }
        minutes = 0;
        seconds = 0;
      }
      else
      {
        if (spraying == false)
        {
          tft.fillRect(0, 15, 250, 16, BLUE);
        }
        hours = hours - 1;
        minutes = 59;
        seconds = 59;
      }
    }
    else
    {
      if (spraying == false)
      {
        tft.fillRect(0, 15, 250, 16, BLUE);
      }
      minutes = minutes - 1;
      seconds = 59;
    }
  }
  else
  {
    if (spraying == false)
    {
      tft.fillRect(0, 15, 250, 16, BLUE);
    }
    seconds = seconds - 1;
  }
  if (hours == 0)
  {
    if (minutes == 0)
    {
      if (seconds == 0)
      {
        if (spraying == false)
        {
          tft.fillRect(0, 15, 250, 16, BLUE);
        }
        if (answered2 == true)
        {
          tft.setTextSize(2);
          tft.setCursor(60, 16);
          tft.print("Misting Enclosure");
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = assignedHours;
        }
        else
        {
          tft.setTextSize(2);
          tft.setCursor(60, 16);
          tft.print("Misting Enclosure");
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = 8;
        }
        minutes = 0;
        seconds = 0;
      }
    }
    if (spraying == false)
    {
      tft.fillRect(0, 15, 300, 18, BLUE);
    }
  }
  if (spraying == false)
  {
    tft.setTextSize(2);
    tft.setCursor(85, 16);
    tft.print(hours);
    tft.print(" : ");
    tft.print(minutes);
    tft.print(" : ");
    tft.print(seconds);
  }
}

void endConfig()
{
  if (answered4 == true)
  {
    wait = assignedWait;
  }
  tft.setCursor(25, 110);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Configuration Complete!");
  delay(4000);
  tft.fillScreen(BLUE);
  if (answered2 == true)
  {
    hours = assignedHours;
  }
  tft.setRotation(1);
  tft.setTextColor(WHITE);
  tft.setCursor(30, 0);
  tft.setTextSize(2);
  tft.print("Time Until Next Spray: ");
  tft.setTextSize(2);
  tft.setCursor(85, 16);
  tft.print(hours);
  tft.print(" : ");
  tft.print(minutes);
  tft.print(" : ");
  tft.print(seconds);
  tft.fillRect(75, 85, 150, 50, BLACK);
  tft.drawRect(75, 85, 150, 50, WHITE);
  tft.setCursor(90, 95);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("Spray");
  tft.fillRect(45, 200, 225, 35, BLACK);
  tft.drawRect(45, 200, 225, 35, WHITE);
  tft.setCursor(50, 210);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Reconfigure System");
  reconfiguring = false;
}

void buttonPress()
{
  TSPoint p = ts.getPoint();

  if (p.z > ts.pressureThreshhold) {

    p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
    p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

    if (p.x > 45 && p.x < 270 && p.y > 200 && p.y < 235)
    {
      //This is important, because the libraries are sharing pins
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      hours = 8;
      minutes = 0;
      seconds = 0;

      answered1 = false;
      answered2 = false;
      answered3 = false;
      answered4 = false;

      reconfiguring = true;
      tft.fillScreen(BLUE);
      configSystem();
    }
    if (p.x > 75 && p.x < 225 && p.y > 85 && p.y < 135)
    {
      //This is important, because the libraries are sharing pins
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      spraying = true;
      tft.fillRect(0, 15, 250, 16, BLUE);
      tft.setTextSize(2);
      tft.setCursor(60, 16);
      tft.print("Misting Enclosure");
      digitalWrite(relay, HIGH);
      while (spraying == true)
      {
        delay(1000);
        subtractTime();
        waitTimes = waitTimes + 1000;
        if (waitTimes == wait)
        {
          waitTimes = 0;
          digitalWrite(relay, LOW);
          spraying = false;
        }
      }
      tft.fillRect(0, 15, 300, 18, BLUE);
    }
  }
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
}

void config2()
{
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("The default spray duration");
  tft.setCursor(45, 20);
  tft.println("is fifteen seconds");
  tft.setCursor(5, 60);
  tft.println("Would you like it to stay");
  tft.setCursor(90, 80);
  tft.println("that way?");
  tft.fillRect(50, 150, 80, 50, BLACK);
  tft.drawRect(50, 150, 80, 50, WHITE);
  tft.setCursor(55, 158);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("Yes");
  tft.fillRect(180, 150, 80, 50, BLACK);
  tft.drawRect(180, 150, 80, 50, WHITE);
  tft.setCursor(200, 158);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.print("NO");

  while (answered3 == false)
  {
    TSPoint p = ts.getPoint();

    if (p.z > ts.pressureThreshhold) {

      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\n");

      p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
      p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

      if (p.x > 180 && p.x < 260 && p.y > 80 && p.y < 200)
      {
        //This is important, because the libraries are sharing pins
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        answered3 = true;

        tft.fillScreen(BLUE);

        endConfig();
      }
      else if (p.x > 50 && p.x < 130 && p.y > 80 && p.y < 200)
      {
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        tft.fillScreen(BLUE);

        answered3 = true;

        tft.setCursor(5, 0);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.println("How many seconds would you");
        tft.setCursor(35, 15);
        tft.println("like to set the spray");
        tft.setCursor(95, 30);
        tft.println("duration to?");

        tft.fillRect(30, 80, 50, 50, BLACK);
        tft.drawRect(30, 80, 50, 50, WHITE);
        tft.setCursor(45, 88);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("5");
        tft.fillRect(130, 80, 50, 50, BLACK);
        tft.drawRect(130, 80, 50, 50, WHITE);
        tft.setCursor(132, 88);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("10");
        tft.fillRect(230, 80, 50, 50, BLACK);
        tft.drawRect(230, 80, 50, 50, WHITE);
        tft.setCursor(232, 88);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("15");
        tft.fillRect(30, 180, 50, 50, BLACK);
        tft.drawRect(30, 180, 50, 50, WHITE);
        tft.setCursor(32, 188);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("20");
        tft.fillRect(130, 180, 50, 50, BLACK);
        tft.drawRect(130, 180, 50, 50, WHITE);
        tft.setCursor(132, 188);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("25");
        tft.fillRect(230, 180, 50, 50, BLACK);
        tft.drawRect(230, 180, 50, 50, WHITE);
        tft.setCursor(232, 188);
        tft.setTextColor(WHITE);
        tft.setTextSize(4);
        tft.print("30");

        while (answered4 == false)
        {
          TSPoint p = ts.getPoint();

          if (p.z > ts.pressureThreshhold) {

            Serial.print("X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.print(p.y);
            Serial.print("\n");

            p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
            p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);

            if (p.x > 230 && p.x < 280 && p.y > 80 && p.y < 130)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 5000;

              tft.fillScreen(BLUE);

              endConfig();
            }
            else if (p.x > 130 && p.x < 180 && p.y > 80 && p.y < 130)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 10000;

              tft.fillScreen(BLUE);

              endConfig();
            }
            else if (p.x > 30 && p.x < 80 && p.y > 80 && p.y < 130)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 15000;

              tft.fillScreen(BLUE);

              endConfig();
            }
            else if (p.x > 230 && p.x < 280 && p.y > 180 && p.y < 230)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 20000;

              tft.fillScreen(BLUE);

              endConfig();
            }
            else if (p.x > 130 && p.x < 180 && p.y > 180 && p.y < 230)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 25000;

              tft.fillScreen(BLUE);

              endConfig();
            }
            else if (p.x > 30 && p.x < 80 && p.y > 180 && p.y < 230)
            {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 30000;

              tft.fillScreen(BLUE);

              endConfig();
            }
          }
        }
      }
    }
  }
}
