#include <Arduino.h>
#include <serverLogic.h>
#include <connection.h>
#include <wiegand.h>
#include <input_output.h>
#include <RFID.h>

void checkResetBtn()
{
  int q = 0;
  if (digitalRead(RST_BTN) == LOW)
  {
    q++;
    delay(2000);
    if (digitalRead(RST_BTN) == LOW)
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
  enableCore1WDT();
  delay(100);
  Serial.begin(115200);
  delay(100);

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

  // Connect to a network
  startConnection();
  // Start back-end server
  startEspServer();

  // Serial.println("Update Check");

  // for debugging Wiegand
  // wiegand_state.wiegand_flag = true;
  // wiegand_state.plate_number = "B059811";
  // wiegand_state.database_url = "https://dev2.metrici.ro/io/lpr/get_wiegand_id.php";
  Serial.print("WiFi MAC - ");
  Serial.println(WiFi.macAddress());
  Serial.print("ETH MAC - ");
  Serial.println(ETH.macAddress());
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