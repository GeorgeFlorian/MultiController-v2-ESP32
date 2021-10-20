#include <Arduino.h>
#include <serverLogic.h>
#include <connection.h>
#include <wiegand.h>

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(true))
  {
    Serial.println(F("An Error has occurred while mounting SPIFFS ! Formatting in progress"));
    return;
  }
  // get settings from /config.json and update Live state
  if (fileToJson().isNull())
  {
    logOutput("ERROR: Could not get start-up configuration. Restarting...");
    restartSequence(2);
  }

  // network_conn();
  updateUser();

  //setting the pins for Outputs
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // listAllFiles();

  // connect to network
  start_connection();
  // start back-end server
  start_esp_server();
}

void loop()
{
  updateUser();
  // Outputs Routine
  relays.deltaTimer1 = millis();
  relays.deltaTimer2 = millis();

  if (relays.startTimer1 != 0 && !relays.manualClose1)
  {
    if (abs(relays.deltaTimer1 - relays.startTimer1) >= (output.getTimer1().toInt() * 1000))
    {
      digitalWrite(RELAY1, LOW);
      relays.state1 = "Off";
      StaticJsonDocument<384> relay_json;
      relay_json["relay1"]["state1"] = "Off";
      saveSettings(relay_json, "relay1");
      relay_json.clear();
      logOutput(" Relay 1 closed");
      relays.startTimer1 = 0;
    }
  }

  if (relays.startTimer2 != 0 && !relays.manualClose2)
  {
    if (abs(relays.deltaTimer2 - relays.startTimer2) > (output.getTimer2().toInt() * 1000))
    {
      digitalWrite(RELAY2, LOW);
      relays.state2 = "Off";
      StaticJsonDocument<384> relay_json;
      relay_json["relay2"]["state2"] = "Off";
      saveSettings(relay_json, "relay2");
      relay_json.clear();
      logOutput("Relay 2 closed");
      relays.startTimer2 = 0;
    }
  }

  wiegandRoutine();
}