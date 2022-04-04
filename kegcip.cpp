#include "pinout.h"


#include <stdio.h>

#include <Arduino.h>
#include <U8g2lib.h>


#include <RotaryEncoder.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#include <TeensyThreads.h>

#include "io_board.h"





#define LED_PERIOD_MS             300

OneWire oneWire(PIN_ONE_WIRE_BUS); 
DallasTemperature sensor(&oneWire);

RotaryEncoder menuselect(PIN_MENUSELECT_1, PIN_MENUSELECT_2);

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI u8g2(
  U8G2_R0, 
  PIN_LCD_SPI_CLK,
  PIN_LCD_SPI_MOSI,
  PIN_LCD_SPI_CS,
  PIN_LCD_DC,
  PIN_LCD_RST);  



 
 
static char tempS[8];

static bool advanceMenu = false;
static bool ledState = false;
static bool startRequest = false;
static int relay = 0;

bool level1 = false;
bool level2 = false;






static void temp_thread();
static void display_thread();
static void relay_thread();
static void led_thread();

static int tempThreadId = -1;
static int displayThreadId = -1;
static int relayThreadId = -1;
static int ledThreadId = -1;



void displayRelay(int relay) {
  u8g2.setFont(u8g2_font_10x20_tr);
  char str[3];
  sprintf(str, "%d", relay);
  u8g2.drawStr(54,20,str);
//  u8g2.sendBuffer();
}

void displayTemperature(const char * tempS) {
  u8g2.setFont(u8g2_font_5x7_tr);
  char msg[32];
  sprintf(msg, "Temperature: %s °C", tempS);
  u8g2.drawStr(1, 42, msg);
}

void displayStart() {
  u8g2.setFont(u8g2_font_5x7_tr);
  char msg[32];
  sprintf(msg, "START");
  u8g2.drawStr(1, 60, msg);
}

void displayLevels(bool level1, bool level2) {
  u8g2.setFont(u8g2_font_5x7_tr);
  char msg[32];
  sprintf(msg, "L1: %d, L2: %d", level1, level2);
  u8g2.drawStr(60, 60, msg);
}
  

void displayMenu(bool advance) {
    u8g2.setFont(u8g2_font_5x7_tr);
    char msg[32];
    sprintf(msg, "%s", advance?"Expert":"Rookie");
    u8g2.drawStr(1, 50, msg);
}


void setLed(bool state) {
   digitalWrite(PIN_BUTTON_LED, state?HIGH:LOW);    
}

static void temp_thread() {
    static float oldtemperature = 0;
    while (true) {
      sensor.requestTemperatures();
      float temperature = sensor.getTempCByIndex(0);
      
      // printf does not support float
  
      dtostrf(temperature, 4, 2, tempS);
      printf("temp: %s °C\n", tempS);

      if (oldtemperature != temperature) {
        threads.restart(displayThreadId);
      }
      oldtemperature = temperature;
      
      delay(2000);
    }
}


static void display_thread() {
  int myId = threads.id();
  while (true) {

    threads.suspend(myId);
    threads.yield();

    u8g2.clearBuffer();
    // This takes a while ...
    displayRelay(relay);
    displayTemperature(tempS);
    displayMenu(advanceMenu);

    if (startRequest) {
      displayStart();
    }

    displayLevels(level1, level2);
    
    u8g2.sendBuffer();
    
  }
}


static void relay_thread() {
  int myId = threads.id();
  while (true) {

    threads.suspend(myId);
    threads.yield();

    // takes time.
    setRelay(relay);
  }
}

static void led_thread() {
  int myId = threads.id();

  threads.suspend(myId);
  threads.yield();
 
  while (true) {

    if (advanceMenu) {
        ledState = !ledState;
    } else {
        ledState = false;
    }
    setLed(ledState);

    threads.delay(LED_PERIOD_MS);
  }
}



static void rotaryClickIsr() {
  printf("IRQ\n");

  advanceMenu = !advanceMenu;
  
  startRequest = false;
  threads.restart(displayThreadId);
}

static void startIsr() {
  static int cpt = 0;
  printf("start IRQ (%d)\n", cpt++);

  startRequest = true;
  threads.restart(displayThreadId);
}



void setup() {
    Serial.begin(115200);

    // Setup rotary click
    pinMode(PIN_MENUCLICK, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_MENUCLICK), rotaryClickIsr, FALLING);

    // Setup start button
    pinMode(PIN_START_BUTTON, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(PIN_START_BUTTON), startIsr, RISING);

    // Setup LED
    pinMode(PIN_BUTTON_LED, OUTPUT);

    relayBoardSetup();

    // Liquid levels 
    pinMode(PIN_LEVEL1, INPUT_PULLUP);
    pinMode(PIN_LEVEL2, INPUT_PULLUP);
    

    tempThreadId    = threads.addThread(temp_thread, 0);
    displayThreadId = threads.addThread(display_thread, 0);
    relayThreadId   = threads.addThread(relay_thread, 0);
    // TODO Find out why this delay is mandatory
    delay(1000);
    ledThreadId     = threads.addThread(led_thread, 0);

    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_10x20_tr);
    u8g2.drawStr(1, 20, "KegCip Ready !");
    u8g2.sendBuffer();

}




void loop() {

  static int oldpos = 0;

  // Handle rotary events
  menuselect.tick();
  int pos = menuselect.getPosition();

  int motion = 0;

  if (oldpos != pos) {
    motion = pos-oldpos;
    oldpos = pos;
    relay += motion;
    if (relay < 0)
      relay = 0;

    if (relay > 15)
      relay = 15;

    threads.restart(relayThreadId);
    threads.restart(displayThreadId);
    
  }

  // Led blinking handling

  int ledThreadState = threads.getState(ledThreadId);

  if (advanceMenu) {
    if (ledThreadState == Threads::SUSPENDED)
      threads.restart(ledThreadId);
  }
  else {
    if (ledThreadState == Threads::RUNNING) {
      threads.suspend(ledThreadId);
      setLed(false);
    }
  }

  static bool oldLevel1 = false;
  static bool oldLevel2 = false;

  // Liquid Level
  level1 = digitalRead(PIN_LEVEL1);
  level2 = digitalRead(PIN_LEVEL2);

  if (oldLevel1 != level1 || 
      oldLevel2 != level2) {
       printf("L1: %d, L2: %d\n", level1, level2);
      threads.restart(displayThreadId);
  }

  oldLevel1 = level1;
  oldLevel2 = level2;
 
  delay(10);
}