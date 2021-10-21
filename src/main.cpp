#include <Arduino.h>
#include <serverLogic.h>
#include <connection.h>
#include <wiegand.h>
#include <input_output.h>
#include <RFID.h>

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(true))
  {
    Serial.println(F("An Error has occurred while mounting SPIFFS ! Formatting in progress"));
    return;
  }
  // Read settings from /config.json and update live state
  if (readSettings().isNull())
  {
    logOutput("ERROR: Could not get start-up configuration. Restarting...");
    restartSequence(2);
  }

  // network_conn();
  updateUser();

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
  // listAllFiles();

  // connect to network
  start_connection();
  // start back-end server
  start_esp_server();
}

void loop()
{
  // updateUser();
  inputRoutine();
  outputRoutine();
  wiegandRoutine();
  rfidRoutine();
}