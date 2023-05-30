#define BLYNK_TEMPLATE_ID "TMPLr5v9dbdK"
#define BLYNK_DEVICE_NAME "Smoke Detector"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial

#define APP_DEBUG


#define USE_NODE_MCU_BOARD

#include "BlynkEdgent.h"
#include <MQ2.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUZZ      12 //D6 
#define LED       14 //D5

int pin = A0;
float lpg, co, smoke;
MQ2 mq2(pin);
int button1 = 0;
int button2 = 0;
SimpleTimer timer;

void setup()
{
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  pinMode(BUZZ, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(BUZZ, LOW);
  digitalWrite(LED, LOW);

  mq2.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
   // for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(" IoT Smoke ");
  display.setCursor(0, 20);
  display.println("  Detector  ");
  display.display();
  delay(1000);
  timer.setInterval(1000L, sendSensorData);

}

void loop() {
  timer.run();
  BlynkEdgent.run();
}
void sendSensorData()
{
  float* values = mq2.read(true);
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  lpg = mq2.readLPG();

  if (button1 == 1)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("    LPG   ");
    display.setCursor(10, 30);
    display.print(lpg);
    display.setTextSize(1);
    display.print(" PPM");
    delay(5000);
    display.display();
  }
  else if (button2 == 1)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("    CO    ");
    display.setCursor(10, 30);
    display.print(co);
    display.setTextSize(1);
    display.print(" PPM");
    delay(5000);
    display.display();
  }
  else {

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("   SMOKE   ");
    display.setCursor(10, 30);
    display.print(smoke);
    display.setTextSize(1);
    display.print(" PPM");
    delay(5000);
    display.display();
  }

  Blynk.virtualWrite(V1, smoke);
  Blynk.virtualWrite(V2, lpg);
  Blynk.virtualWrite(V3, co);
  if (smoke > 50 ) {
    Blynk.logEvent("smoke", "Smoke Detected!");
    digitalWrite(BUZZ, HIGH);
    digitalWrite(LED, HIGH);
  }
  else {
    digitalWrite(BUZZ, LOW);
    digitalWrite(LED, LOW);
  }

}

BLYNK_WRITE(V4)
{
  button1 = param.asInt();
}

BLYNK_WRITE(V5)
{
  button2 = param.asInt();
}