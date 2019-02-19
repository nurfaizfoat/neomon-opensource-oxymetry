#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <LM35.h>

#define REPORTING_PERIOD_MS     1000
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 5, 4, U8X8_PIN_NONE);

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
MAX30100 sensor;

LM35 temp(A0);

#define splash_screen_width 128
#define splash_screen_height 64
static const unsigned char splash_screen_128x64_bits[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x3F, 0xF0, 0xE1, 0xF3, 0x1F, 0xE0,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0x7F, 0xF0,
  0xE1, 0xF3, 0x1F, 0xFC, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0xFF, 0xFF, 0xFF, 0xF0, 0xE3, 0xF3, 0x1F, 0xFE, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xE3, 0xF3, 0x1F, 0xFF,
  0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0xE7, 0xF3, 0x01, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0xFF, 0xFF, 0xFF, 0xF0, 0xE7, 0xF3, 0x81, 0x1F, 0x7E, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xEF, 0xF3, 0x9F, 0x0F,
  0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x03, 0xFF, 0xFF, 0xF0,
  0xEF, 0xF3, 0x9F, 0x0F, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x00, 0xF8, 0xFF, 0xF0, 0xFD, 0xF3, 0x9F, 0x0F, 0x7C, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0x7F, 0x00, 0xF0, 0xFF, 0xF0, 0xFD, 0xF3, 0x81, 0x1F,
  0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x7F, 0x00, 0xE0, 0xFF, 0xF0,
  0xF9, 0xF3, 0x81, 0x3F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F,
  0x00, 0xC0, 0xFF, 0xF0, 0xF9, 0xF3, 0x1F, 0xFF, 0x3F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0x3F, 0x00, 0xC0, 0xFF, 0xF0, 0xF1, 0xF3, 0x1F, 0xFF,
  0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x3F, 0x8C, 0xC1, 0xFF, 0xF0,
  0xE1, 0xF3, 0x1F, 0xFC, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0F,
  0x8C, 0xC1, 0x7F, 0xF0, 0xE1, 0xF3, 0x1F, 0xE0, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0x07, 0x00, 0xC0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x30, 0x00, 0x1F, 0xE0,
  0x87, 0x0F, 0xE0, 0x01, 0x1F, 0x3E, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F,
  0x70, 0x00, 0xFE, 0xE0, 0x87, 0x0F, 0xFC, 0x0F, 0x1F, 0x3E, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x1F, 0x03, 0x00, 0x0F, 0xE0, 0xC7, 0x0F, 0xFE, 0x1F,
  0x3F, 0x3E, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x1F, 0x03, 0xC3, 0xFF, 0xE0,
  0xC7, 0x1F, 0xFF, 0x3F, 0x3F, 0x3E, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F,
  0x86, 0xC3, 0x1F, 0xE0, 0xCF, 0x1F, 0x3F, 0x3F, 0x7F, 0x3E, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0x1F, 0xFC, 0xC1, 0xFF, 0xF0, 0xCF, 0x9F, 0x1F, 0x7E,
  0x7F, 0x3E, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F, 0x78, 0xE0, 0xFF, 0xF0,
  0xEF, 0x9E, 0x0F, 0x7C, 0xFF, 0x3E, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x1F,
  0x00, 0xE0, 0xFF, 0xF0, 0xEE, 0x9E, 0x0F, 0x7C, 0xFF, 0x3E, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0x1F, 0x00, 0xE0, 0xFF, 0xF0, 0xFC, 0x9E, 0x0F, 0x7C,
  0xDF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F, 0x00, 0xF0, 0xFF, 0xF0,
  0xFC, 0xBC, 0x1F, 0x7E, 0xDF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x3F,
  0x00, 0xF0, 0xFF, 0xF0, 0x7C, 0xBC, 0x3F, 0x7F, 0x9F, 0x3F, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0x00, 0xF8, 0xFF, 0xF0, 0x78, 0x3C, 0xFF, 0x3F,
  0x9F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x01, 0xFC, 0xFF, 0xF0,
  0x78, 0x3C, 0xFF, 0x3F, 0x1F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0xFF, 0xFF, 0xFF, 0xF8, 0x38, 0x7C, 0xFC, 0x0F, 0x1F, 0x3E, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x30, 0x7C, 0xE0, 0x01,
  0x1F, 0x3E, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF,
  0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

uint32_t tsLastReport = 0;
uint32_t last_beat=0;
byte beat=0;



// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.print( "Beat!" );
  Serial.print( " | " );
  show_beat();  
  last_beat=millis();
}

void show_beat()
{
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  if (beat==0) {
    u8g2.drawGlyph(118, 10, 0x2665);
    beat=1;
  }
  else
  {
    beat=0;
  }
}

void splash_screen()
{
  u8g2.firstPage();
    do {
      u8g2.drawXBMP(0, 0, splash_screen_width, splash_screen_height, splash_screen_128x64_bits);
      u8g2.setFont(u8g2_font_micro_mr);
      u8g2.drawStr(59, 49, "v0.0.8_rc");
    } while (u8g2.nextPage());
  delay(2500);
}

void display_values() {


  u8g2.firstPage();
    do {
      
      // Line
      u8g2.drawLine(1, 55, 80, 55);
      
      // Battery
      u8g2.drawBox(1,58,8,7);
      u8g2.drawBox(9,59,1,4);
      u8g2.setFont(u8g2_font_4x6_mr);
      u8g2.drawStr(12, 64, "100%");

      // Time & Date
      u8g2.setFont(u8g2_font_4x6_mr);
      u8g2.drawStr(54, 64, "04:56AM");

      // Hearbeat
      u8g2.setFont(u8g2_font_8x13_tf);
      u8g2.drawStr(90, 10, "BPM");
      u8g2.setFont(u8g2_font_fub17_tf);
      u8g2.setCursor(90, 30);
      u8g2.print(pox.getHeartRate(), 0); // to get round number add 0

      // Temperature
      u8g2.setFont(u8g2_font_8x13_tf);
      u8g2.drawGlyph(90, 44, 0x00b0);
      u8g2.setFont(u8g2_font_8x13_tf);
      u8g2.drawStr(98, 44, "C");
      u8g2.setFont(u8g2_font_fub17_tf);
//      u8g2.drawStr(90, 64, "null");
      u8g2.setCursor(90, 64);
      u8g2.print(temp.cel() * 10, 0);
      
      // Blood Oxy
      u8g2.setFont(u8g2_font_8x13_tf);
      u8g2.drawStr(1, 10, "SpO");
      u8g2.setFont(u8g2_font_8x13_tf);
      u8g2.drawGlyph(25, 10, 0x00b2);
      u8g2.setFont(u8g2_font_8x13_tf);
      u8g2.drawGlyph(31, 10, 0x0025);
      u8g2.setFont(u8g2_font_fub35_tf);
      u8g2.setCursor(1, 50);
      u8g2.print(pox.getSpO2());

      show_beat();
 
  } while (u8g2.nextPage());
}




void setup()
{
    Serial.begin(115200);
    u8g2.begin();
    pox.begin();
    sensor.setLedsCurrent(MAX30100_LED_CURR_7_6MA, MAX30100_LED_CURR_7_6MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
    splash_screen();
}

void loop()
{
  pulse_oxy();
  
}


void pulse_oxy ()
{
  // Make sure to call update as fast as possible
  pox.update();
  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      display_values();
      
      Serial.print( "BPM: " );
      Serial.print(pox.getHeartRate());
      Serial.print( " | " );

      Serial.print( "SpO2: " );
      Serial.print(pox.getSpO2());
      Serial.println( "%" );
      Serial.print( " | " );

      Serial.print( "Temp: " );
      Serial.print(temp.cel());
      Serial.println( "C" );

      tsLastReport = millis();
  }
}



