#include <Arduino.h>
#include <serverLogic.h>
#include <connection.h>
#include <wiegand.h>
#include <input_output.h>
#include <RFID.h>

void setPins()
{
  //setting the pins for Inputs
  pinMode(INPUT_1, INPUT_PULLUP);
  pinMode(INPUT_2, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);

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
}

void checkResetBtn()
{
  int q = 0;
  if (digitalRead(BUTTON) == LOW)
  {
    q++;
    delay(2000);
    if (digitalRead(BUTTON) == LOW)
    {
      q++;
    }
  }
  if (q == 2)
  {
    logOutput("WARNING: Reset button was pressed !");
    StaticJsonDocument<1024> json = factoryReset();
    if (JSONtoSettings(json))
    {
      restartSequence(2);
    }
    else
    {
      logOutput("Could not reset");
    }
  }
  q = 0;
}

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(true))
  {
    Serial.println(F("An Error has occurred while mounting SPIFFS ! Formatting in progress"));
    return;
  }

  setPins();

  checkResetBtn();

  // Read settings from /config.json and update live state
  if (readSettings().isNull())
  {
    logOutput("ERROR: Could not get start-up configuration. Restarting...");
    restartSequence(5);
  }
  // listAllFiles();

  // Connect to a network or enter AP_MODE
  startConnection();
  // Start back-end server
  startEspServer();

  // Serial.println("Update Check");
}

void loop()
{
  delay(1);
  if (changed_network_config)
  {
    Serial.println("Changed NETWORK configuration. Restarting...");
    restartSequence(2);
  }
  if (restart_flag)
  {
    Serial.println("Changed other configuration. Restarting...");
    restartSequence(2);
  }
  inputRoutine();
  outputRoutine();
  wiegandRoutine();
  rfidRoutine();
}