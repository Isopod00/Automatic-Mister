//Automatic Sprayer
//By Anthony Brogni

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

// The control pins for the LCD can be assigned to any digital or analog pins, but using the analog pins allows for doubling up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define TS_MINX 130
#define TS_MINY 130
#define TS_MAXX 940
#define TS_MAXY 940

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
// If using the shield, all control and data lines are fixed, and a simpler declaration can optionally be used: Elegoo_TFTLCD tft;

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
  } else if (identifier == 0x0101) {
    identifier = 0x9341;
  }
  else {
    identifier = 0x9341;
  }
  tft.begin(identifier);
  boot();
  configSystem();
}

void loop() {
  if (reconfiguring == false) {
    buttonPress();
    delay(8);
    if (times == 125) {
      times = 0;
      subtractTime();
    }
    else {
      times++;
    }
  }
}

void boot() {
  tft.fillScreen(BLUE);
  tft.setRotation(2);
  tft.setCursor(20, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Automatic Sprayer");
  tft.setCursor(100, 20);
  tft.println("v2.5");
  tft.setCursor(80, 280);
  tft.println("Made by");
  tft.setCursor(40, 300);
  tft.println("Anthony Brogni");
  tft.setCursor(30, 140);
  tft.setTextSize(4);
  tft.println("Welcome!");
  delay(4000);
  tft.fillScreen(BLUE);
}

void configSystem() {
  tft.setCursor(25, 100);
  tft.setTextSize(3);
  tft.println("Please take");
  tft.setCursor(20, 125);
  tft.println("a moment to");
  tft.setCursor(35, 150);
  tft.println("configure");
  tft.setCursor(20, 175);
  tft.println("your system!");
  delay(4000);
  tft.fillScreen(BLUE);
  tft.setCursor(20, 0);
  tft.setTextSize(2);
  tft.println("The default spray");
  tft.setCursor(15, 20);
  tft.println("interval is every");
  tft.setCursor(50, 40);
  tft.println("eight hours.");
  tft.setCursor(20, 100);
  tft.println("Would you like it");
  tft.setCursor(20, 120);
  tft.println("to stay that way?");
  tft.fillRect(20, 210, 80, 50, BLACK);
  tft.drawRect(20, 210, 80, 50, WHITE);
  tft.setCursor(25, 220);
  tft.setTextSize(4);
  tft.print("Yes");
  tft.fillRect(150, 210, 80, 50, BLACK);
  tft.drawRect(150, 210, 80, 50, WHITE);
  tft.setCursor(170, 220);
  tft.print("NO");
  while (answered1 == false) {
    TSPoint p = ts.getPoint();

    if (p.z > ts.pressureThreshhold) {
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\n");

      if (p.x > 280 && p.x < 400 && p.y > 185 && p.y < 420) {
        //This is important, because the libraries are sharing pins
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        answered1 = true;

        tft.fillScreen(BLUE);

        config2();
      }
      else if (p.x > 255 && p.x < 385 && p.y > 610 && p.y < 860) {
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        tft.fillScreen(BLUE);

        answered1 = true;

        tft.setCursor(0, 0);
        tft.setTextSize(2);
        tft.println("How many hours would");
        tft.setCursor(5, 20);
        tft.println("you like to set the");
        tft.setCursor(10, 40);
        tft.println("spray interval to?");

        tft.fillRect(15, 120, 50, 50, BLACK);
        tft.drawRect(15, 120, 50, 50, WHITE);
        tft.setCursor(30, 128);
        tft.setTextSize(4);
        tft.print("2");
        tft.fillRect(95, 120, 50, 50, BLACK);
        tft.drawRect(95, 120, 50, 50, WHITE);
        tft.setCursor(110, 128);
        tft.print("4");
        tft.fillRect(175, 120, 50, 50, BLACK);
        tft.drawRect(175, 120, 50, 50, WHITE);
        tft.setCursor(190, 128);
        tft.print("6");
        tft.fillRect(15, 200, 50, 50, BLACK);
        tft.drawRect(15, 200, 50, 50, WHITE);
        tft.setCursor(30, 208);
        tft.print("8");
        tft.fillRect(95, 200, 50, 50, BLACK);
        tft.drawRect(95, 200, 50, 50, WHITE);
        tft.setCursor(96, 208);
        tft.print("10");
        tft.fillRect(175, 200, 50, 50, BLACK);
        tft.drawRect(175, 200, 50, 50, WHITE);
        tft.setCursor(176, 208);
        tft.print("12");

        while (answered2 == false) {
          TSPoint p = ts.getPoint();

          if (p.z > ts.pressureThreshhold) {

            Serial.print("X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.print(p.y);
            Serial.print("\n");

            if (p.x > 520 && p.x < 640 && p.y > 180 && p.y < 295) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 2;
            }
            else if (p.x > 520 && p.x < 640 && p.y > 445 && p.y < 580) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 4;
            }
            else if (p.x > 520 && p.x < 640 && p.y > 700 && p.y < 850) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 6;
            }
            else if (p.x > 285 && p.x < 430 && p.y > 180 && p.y < 295) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 8;
            }
            else if (p.x > 285 && p.x < 430 && p.y > 445 && p.y < 580) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 10;
            }
            else if (p.x > 285 && p.x < 430 && p.y > 700 && p.y < 850) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered2 = true;
              assignedHours = 12;
            }
            if (answered2 == true) {
              tft.fillScreen(BLUE);
              config2();
            }
          }
        }
      }
    }
  }
}

void subtractTime() {
  if (seconds == 0) {
    if (minutes == 0) {
      if (hours == 0) {
        if (spraying == false) {
          tft.fillRect(72, 19, 250, 18, BLUE);
        }
        if (answered2 == true) {
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = assignedHours;
        }
        else {
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = 8;
        }
        minutes = 0;
        seconds = 0;
      }
      else {
        if (spraying == false) {
          tft.fillRect(72, 19, 250, 18, BLUE);
        }
        hours = hours - 1;
        minutes = 59;
        seconds = 59;
      }
    }
    else {
      if (spraying == false) {
        tft.fillRect(72, 19, 250, 18, BLUE);
      }
      minutes = minutes - 1;
      seconds = 59;
    }
  }
  else {
    if (spraying == false) {
      tft.fillRect(72, 19, 250, 18, BLUE);
    }
    seconds = seconds - 1;
  }
  if (hours == 0) {
    if (minutes == 0) {
      if (seconds == 0) {
        if (spraying == false) {
          tft.fillRect(0, 0, 300, 50, BLUE);
        }
        if (answered2 == true) {
          tft.setTextSize(2);
          tft.setCursor(10, 20);
          tft.print("Spraying Enclosure");
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = assignedHours;
        }
        else {
          tft.setTextSize(2);
          tft.setCursor(10, 20);
          tft.print("Spraying Enclosure");
          digitalWrite(relay, HIGH);
          delay(wait);
          digitalWrite(relay, LOW);
          hours = 8;
        }
        minutes = 0;
        seconds = 0;
      }
    }
    if (spraying == false) {
      tft.fillRect(0, 19, 300, 18, BLUE);
    }
  }
  if (spraying == false) {
    tft.setTextSize(2);
    tft.setCursor(20, 1);
    tft.print("Time Until Next");
    tft.setCursor(5, 21);
    tft.print("Spray: ");
    tft.setCursor(85, 21); 
    tft.print(hours);
    tft.print(" : ");
    tft.print(minutes);
    tft.print(" : ");
    tft.print(seconds);
  }
}

void endConfig() {
  if (answered4 == true) {
    wait = assignedWait;
  }
  tft.setCursor(45, 155);
  tft.setTextSize(2);
  tft.println("Configuration");
  tft.setCursor(50, 175);
  tft.println("is complete!");
  delay(4000);
  tft.fillScreen(BLUE);
  if (answered2 == true) {
    hours = assignedHours;
  }
  tft.setRotation(2);
  tft.setCursor(20, 1);
  tft.print("Time Until Next");
  tft.setCursor(5, 21);
  tft.print("Spray: ");
  tft.print(hours);
  tft.print(" : ");
  tft.print(minutes);
  tft.print(" : ");
  tft.print(seconds);
  tft.fillRect(50, 112, 130, 50, BLACK);
  tft.drawRect(50, 112, 130, 50, WHITE);
  tft.setCursor(55, 120);
  tft.setTextSize(4);
  tft.print("Spray");
  tft.fillRect(9, 252, 225, 35, BLACK);
  tft.drawRect(9, 252, 225, 35, WHITE);
  tft.setCursor(15, 260);
  tft.setTextSize(2);
  tft.print("Reconfigure System");
  reconfiguring = false;
}

void buttonPress() {
  TSPoint p = ts.getPoint();

  if (p.z > ts.pressureThreshhold) {

    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\n");

    if (p.x > 200 && p.x < 250 && p.y > 201 && p.y < 843) {
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
    if (p.x > 520 && p.x < 660 && p.y > 295 && p.y < 705) {
      //This is important, because the libraries are sharing pins
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      spraying = true;
      tft.fillRect(0, 0, 300, 50, BLUE);
      tft.setTextSize(2);
      tft.setCursor(10, 20);
      tft.print("Spraying Enclosure");
      digitalWrite(relay, HIGH);
      while (spraying == true) {
        delay(1000);
        subtractTime();
        waitTimes = waitTimes + 1000;
        if (waitTimes == wait) {
          waitTimes = 0;
          digitalWrite(relay, LOW);
          spraying = false;
        }
      }
      tft.fillRect(0, 15, 300, 30, BLUE);
      tft.setCursor(20, 1);
      tft.print("Time Until Next");
      tft.setCursor(5, 21);
      tft.print("Spray: ");
    }
  }
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
}

void config2()
{
  tft.setCursor(20, 0);
  tft.setTextSize(2);
  tft.println("The default spray");
  tft.setCursor(5, 20);
  tft.println("duration is fifteen");
  tft.setCursor(80, 40);
  tft.println("seconds.");
  tft.setCursor(15, 80);
  tft.println("Would you like it ");
  tft.setCursor(15, 100);
  tft.println("to stay that way?");
  tft.fillRect(20, 210, 80, 50, BLACK);
  tft.drawRect(20, 210, 80, 50, WHITE);
  tft.setCursor(25, 220);
  tft.setTextSize(4);
  tft.print("Yes");
  tft.fillRect(150, 210, 80, 50, BLACK);
  tft.drawRect(150, 210, 80, 50, WHITE);
  tft.setCursor(170, 220);
  tft.print("NO");

  while (answered3 == false) {
    TSPoint p = ts.getPoint();

    if (p.z > ts.pressureThreshhold) {

      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\n");

      if (p.x > 280 && p.x < 400 && p.y > 185 && p.y < 420) {
        //This is important, because the libraries are sharing pins
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        answered3 = true;

        tft.fillScreen(BLUE);

        endConfig();
      }
      else if (p.x > 255 && p.x < 385 && p.y > 610 && p.y < 860) {
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        tft.fillScreen(BLUE);

        answered3 = true;

        tft.setCursor(20, 0);
        tft.setTextSize(2);
        tft.println("How many seconds");
        tft.setCursor(15, 20);
        tft.println("would you like to");
        tft.setCursor(40, 40);
        tft.println("set the spray");
        tft.setCursor(45, 60);
        tft.println("duration to?");

         tft.fillRect(15, 120, 50, 50, BLACK);
        tft.drawRect(15, 120, 50, 50, WHITE);
        tft.setCursor(30, 128);
        tft.setTextSize(4);
        tft.print("5");
        tft.fillRect(95, 120, 50, 50, BLACK);
        tft.drawRect(95, 120, 50, 50, WHITE);
        tft.setCursor(96, 128);
        tft.print("10");
        tft.fillRect(175, 120, 50, 50, BLACK);
        tft.drawRect(175, 120, 50, 50, WHITE);
        tft.setCursor(176, 128);
        tft.print("15");
        tft.fillRect(14, 200, 51, 50, BLACK);
        tft.drawRect(14, 200, 51, 50, WHITE);
        tft.setCursor(17, 208);
        tft.print("20");
        tft.fillRect(94, 200, 51, 50, BLACK);
        tft.drawRect(94, 200, 51, 50, WHITE);
        tft.setCursor(97, 208);
        tft.print("25");
        tft.fillRect(174, 200, 51, 50, BLACK);
        tft.drawRect(174, 200, 51, 50, WHITE);
        tft.setCursor(177, 208);
        tft.print("30");

        while (answered4 == false) {
          TSPoint p = ts.getPoint();

          if (p.z > ts.pressureThreshhold) {

            Serial.print("X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.print(p.y);
            Serial.print("\n");

            if (p.x > 520 && p.x < 640 && p.y > 180 && p.y < 295) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 5000;
            }
            else if (p.x > 520 && p.x < 640 && p.y > 445 && p.y < 580) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 10000;
            }
            else if (p.x > 520 && p.x < 640 && p.y > 700 && p.y < 850) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 15000;
            }
            else if (p.x > 285 && p.x < 430 && p.y > 180 && p.y < 295) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 20000;
            }
            else if (p.x > 285 && p.x < 430 && p.y > 445 && p.y < 580) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 25000;
            }
            else if (p.x > 285 && p.x < 430 && p.y > 700 && p.y < 850) {
              //This is important, because the libraries are sharing pins
              pinMode(XM, OUTPUT);
              pinMode(YP, OUTPUT);

              answered4 = true;
              assignedWait = 30000;
            }
            if (answered4 == true) {
             tft.fillScreen(BLUE);
             endConfig();
            }
          }
        }
      }
    }
  }
}