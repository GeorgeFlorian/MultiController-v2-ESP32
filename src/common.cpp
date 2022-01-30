#include <common.h>

bool changed_network_config = false;
bool restart_flag = false;

ezButton input_1(INPUT_1);
ezButton input_2(INPUT_2);

void restartSequence(unsigned int countdown)
{
    for (int i = countdown; i > 0; i--)
    {
        delay(999);
    }
    ESP.restart();
}

void listAllFiles()
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file)
    {
        Serial.print("FILE: ");
        String fileName = file.name();
        Serial.println(fileName);
        file = root.openNextFile();
    }
    file.close();
    root.close();
}

void setPins()
{
  //setting the pins for Inputs
  // pinMode(INPUT_1, INPUT_PULLUP);
  // pinMode(INPUT_2, INPUT_PULLUP);
  pinMode(RST_BTN, INPUT_PULLUP);

  //setting the pins for Outputs
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  //setting the pins for Wiegand
  pinMode(W0, OUTPUT);
  pinMode(W1, OUTPUT);
  digitalWrite(W0, HIGH);
  digitalWrite(W1, HIGH);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  input_1.setDebounceTime(DEBOUNCE_TIME);
  input_2.setDebounceTime(DEBOUNCE_TIME);
}