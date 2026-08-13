#include <Wire.h>
#include <RTClib.h>
#include <FastLED.h>

// --- Configuration ---
#define NUM_LEDS      30                                           
#define DATA_PIN      8    
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB
#define BRIGHTNESS    50  

CRGB leds[NUM_LEDS];                                               
RTC_DS3231 rtc; 

// --- Color Customization ---
const CRGB clockColor = CRGB::White;

// --- PHYSICAL LED STRIP DIGIT STARTING POSITIONS ---             
const int START_HOUR_TENS = 0;   
const int START_HOUR_ONES = 7;   
const int START_COLON     = 14;  
const int START_MIN_TENS  = 16;  
const int START_MIN_ONES  = 23;  

// --- 7-Segment Lookup Table ---                                  
const byte segmentMap[10][7] = {                                   
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

void displayDigit(int number, int startIndex, CRGB color) {        
  leds[startIndex + 0] = (segmentMap[number][6] == 1) ? color : CRGB::Black; 
  leds[startIndex + 1] = (segmentMap[number][1] == 1) ? color : CRGB::Black; 
  leds[startIndex + 2] = (segmentMap[number][0] == 1) ? color : CRGB::Black; 
  leds[startIndex + 3] = (segmentMap[number][5] == 1) ? color : CRGB::Black; 
  leds[startIndex + 4] = (segmentMap[number][4] == 1) ? color : CRGB::Black; 
  leds[startIndex + 5] = (segmentMap[number][3] == 1) ? color : CRGB::Black; 
  leds[startIndex + 6] = (segmentMap[number][2] == 1) ? color : CRGB::Black; 
}

void setup() {                                                     
  Serial.begin(9600);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  if (!rtc.begin()) {                                              
    Serial.println("Couldn't find RTC module!");
    while (1);
  }

  // >>> LINE 66: KEEP THIS COMMENTED OUT NOW WITH '//' <<<
  // rtc.adjust(DateTime(2026, 6, 17, 16, 04, 0)); 
}

void loop() {                                                      
  DateTime now = rtc.now();
  
  int hours = now.hour();
  int minutes = now.minute();
  int seconds = now.second();

  if (hours == 0) {                                                
    hours = 12; 
  } else if (hours > 12) {
    hours = hours - 12; 
  }

  int hourDigit1 = hours / 10;                                     
  int hourDigit2 = hours % 10;
  int minDigit1  = minutes / 10;
  int minDigit2  = minutes % 10;

  FastLED.clear();                                                 

  displayDigit(hourDigit1, START_HOUR_TENS, clockColor);
  displayDigit(hourDigit2, START_HOUR_ONES, clockColor);

  if (seconds % 2 == 0) {                                          
    leds[START_COLON]     = clockColor;
    leds[START_COLON + 1] = clockColor;
  } else {
    leds[START_COLON]     = CRGB::Black;
    leds[START_COLON + 1] = CRGB::Black;
  }

  displayDigit(minDigit1, START_MIN_TENS, clockColor);             
  displayDigit(minDigit2, START_MIN_ONES, clockColor);

  FastLED.show();                                                  

  Serial.print(hourDigit1); Serial.print(hourDigit2);
  Serial.print(":");
  Serial.print(minDigit1); Serial.println(minDigit2);

  delay(200);                                                      
}