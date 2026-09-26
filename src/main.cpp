#include <Arduino.h>
void setup()
{
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
}

void loop()
{
  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n');
    Serial.print("You entered: ");
    Serial.println(input);
  }
}