#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
// #include <server.h>
#include <SPIFFS.h>
#include <Update.h>

#define RELAY1 32
#define RELAY2 33

#define W0 14
#define W1 13
#define INPUT_1 15
#define INPUT_2 17

#define BUTTON 34
#define BUZZER 16

AsyncWebServer server(80);
static bool eth_connected = false;

//------------------------- struct circular_buffer
String strlog;

struct ring_buffer
{
  ring_buffer(size_t cap) : buffer(cap) {}

  bool empty() const { return sz == 0; }
  bool full() const { return sz == buffer.size(); }

  void push(String str)
  {
    if (last >= buffer.size())
      last = 0;
    buffer[last] = str;
    ++last;
    if (full())
      first = (first + 1) % buffer.size();
    else
      ++sz;
  }
  void print() const
  {
    strlog = "";
    if (first < last)
      for (size_t i = first; i < last; ++i)
      {
        strlog += (buffer[i] + "<br>");
      }
    else
    {
      for (size_t i = first; i < buffer.size(); ++i)
      {
        strlog += (buffer[i] + "<br>");
      }
      for (size_t i = 0; i < last; ++i)
      {
        strlog += (buffer[i] + "<br>");
      }
    }
  }

private:
  std::vector<String> buffer;
  size_t first = 0;
  size_t last = 0;
  size_t sz = 0;
};
//------------------------- struct circular_buffer
ring_buffer circle(10);

//------------------------- logOutput(String)
void logOutput(String string1)
{
  circle.push(string1);
  Serial.println(string1);
}

//------------------------- restart_sequence()
void restart_sequence(unsigned int countdown)
{
  for (int i = countdown; i >= 0; i--)
  {
    delay(999);
  }
  server.reset();
  delay(100);
  ESP.restart();
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (filename.indexOf(".bin") > 0)
  {
    if (!index)
    {
      logOutput("The update process has started...");
      // if filename includes spiffs, update the spiffs partition
      int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
      if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
      {
        Update.printError(Serial);
      }
    }

    if (Update.write(data, len) != len)
    {
      Update.printError(Serial);
    }

    if (final)
    {
      if (filename.indexOf("spiffs") > -1)
      {
        request->send(200, "text/html", "<div style=\"margin:0 auto; text-align:center; font-family:arial;\">The device entered AP Mode ! Please connect to it.</div>");
      }
      else
      {
        request->send(200, "text/html", "<div style=\"margin:0 auto; text-align:center; font-family:arial;\">Congratulation ! </br> You have successfully updated the device to the latest version. </br>Please wait 10 seconds until the device reboots, then press on the \"Go Home\" button to go back to the main page.</br></br> <form method=\"post\" action=\"http://" + WiFi.localIP().toString() + "\"><input type=\"submit\" name=\"goHome\" value=\"Go Home\"/></form></div>");
      }

      if (!Update.end(true))
      {
        Update.printError(Serial);
      }
      else
      {
        logOutput("Update complete");
        Serial.flush();
        ESP.restart();
      }
    }
  }
  else if (filename == "config.json")
  {
    if (!index)
    {
      // logOutput((String) "Started uploading: " + filename);
      // open the file on first call and store the file handle in the request object
      request->_tempFile = SPIFFS.open("/" + filename, "w");
    }
    if (len)
    {
      // stream the incoming chunk to the opened file
      request->_tempFile.write(data, len);
    }
    if (final)
    {
      logOutput((String)filename + " was successfully uploaded !");
      Serial.println((String) "File size: " + index + len);
      // close the file handle as the upload is now done
      request->_tempFile.close();
      // request->send(200);
      request->redirect("/settings");
    }
  }
}

//------------------------- listAllFiles()
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

// Live state of the controller
struct Settings
{
  String connection = "";
  String ip_type = "";
  String ssid = "";
  String password = "";
  String ip_address = "";
  String gateway = "";
  String subnet = "";
  String dns = "";
} network_settings;

struct Input
{
  String ip_1 = "";
  String port_1 = "";
  String ip_2 = "";
  String port_2 = "";
} input;

class Output
{
private:
  String timer1 = "";
  String timer2 = "";

public:
  void setTimer1(String x)
  {
    x.trim();
    if (x == "" && x.length() == 0)
      timer1 = "0";
    else
      timer1 = x;
  }
  void setTimer2(String x)
  {
    x.trim();
    if (x == "" && x.length() == 0)
      timer2 = "0";
    else
      timer2 = x;
  }

  String getTimer1()
  {
    return timer1;
  }
  String getTimer2()
  {
    return timer2;
  }
} output;

struct Relay
{
  String state1 = "";
  String state2 = "";
  bool manualClose1 = false;
  bool manualClose2 = false;
  long startTimer1 = 0;
  long startTimer2 = 0;
  long deltaTimer1 = 0;
  long deltaTimer2 = 0;

} relays;

class Wiegand
{
private:
  String pulse_width = "";
  String pulse_gap = "";

public:
  String database_url = "";
  void setPulseWidth(String x)
  {
    x.trim();
    if (x == "" && x.length() == 0)
      pulse_width = "0";
    else
      pulse_width = x;
  }
  void setPulseGap(String x)
  {
    x.trim();
    if (x == "" && x.length() == 0)
      pulse_gap = "0";
    else
      pulse_gap = x;
  }
  String getPulseWidth()
  {
    return pulse_width;
  }
  String getPulseGap()
  {
    return pulse_gap;
  }

} wiegand;

struct RFID
{
  String ip_rfid = "";
  String port_rfid = "";
} rfid;

class User
{
private:
  String username = "";
  String password = "";

public:
  void setUsername(String x)
  {
    x.trim();
    if (x.length() <= 0)
      username = "";
    else
      username = x;
  }
  String getUsername()
  {
    return username;
  }

  void setUserPassword(String x)
  {
    x.trim();
    if (x.length() <= 0)
      password = "";
    else
      password = x;
  }
  String getUserPassword()
  {
    return password;
  }

  bool user_flag = false;
} user;

void updateLiveState(StaticJsonDocument<1024> &doc)
{
  network_settings.connection = doc["network_settings"]["connection"] | "Not working";
  network_settings.ip_type = doc["network_settings"]["ip_type"] | "Not working";
  if (network_settings.connection == "WiFi")
  {
    network_settings.ssid = doc["network_settings"]["ssid"] | "Not working";
    network_settings.password = doc["network_settings"]["password"] | "Not working";
  }
  else if (network_settings.connection == "Ethernet")
  {
    network_settings.ssid = "";
    network_settings.password = "";
  }
  if (network_settings.ip_type == "DHCP")
  {
    network_settings.ip_address = WiFi.localIP().toString();
    doc["network_settings"]["ip_address"] = WiFi.localIP().toString();

    network_settings.gateway = WiFi.gatewayIP().toString();
    doc["network_settings"]["gateway"] = WiFi.gatewayIP().toString();

    network_settings.subnet = WiFi.subnetMask().toString();
    doc["network_settings"]["subnet"] = WiFi.subnetMask().toString();

    network_settings.dns = WiFi.dnsIP().toString();
    doc["network_settings"]["dns"] = WiFi.dnsIP().toString();
  }
  else if (network_settings.ip_type == "Static")
  {
    network_settings.ip_address = doc["network_settings"]["ip_address"] | "Not working";
    network_settings.gateway = doc["network_settings"]["gateway"] | "Not working";
    network_settings.subnet = doc["network_settings"]["subnet"] | "Not working";
    network_settings.dns = doc["network_settings"]["dns"] | "Not working";
  }

  input.ip_1 = doc["input"]["ip_1"] | "Not working";
  input.port_1 = doc["input"]["port_1"] | "Not working";
  input.ip_2 = doc["input"]["ip_2"] | "Not working";
  input.port_2 = doc["input"]["port_2"] | "Not working";

  output.setTimer1(doc["output"]["timer1"] | "Not working");
  output.setTimer2(doc["output"]["timer2"] | "Not working");

  relays.state1 = doc["relay1"]["state1"] | "Not working";

  relays.state2 = doc["relay2"]["state2"] | "Not working";

  wiegand.database_url = doc["wiegand"]["database_url"] | "Not working";
  wiegand.setPulseWidth(doc["wiegand"]["pulse_width"] | "Not working");
  wiegand.setPulseGap(doc["wiegand"]["pulse_gap"] | "Not working");

  rfid.ip_rfid = doc["rfid"]["ip_rfid"] | "Not working";
  rfid.port_rfid = doc["rfid"]["port_rfid"] | "Not working";

  user.setUsername(doc["user"]["username"] | "Not working");
  user.setUserPassword(doc["user"]["password"] | "Not working");
}

// Get settings from /config.json
StaticJsonDocument<1024> settingsToJSON()
{
  StaticJsonDocument<1024> doc;
  // Open file to read
  File file = SPIFFS.open("/config.json");
  if (!file)
  {
    logOutput("ERROR: Failed to get configuration.");
    Serial.println("Could not open file to read config !!!");
    doc.clear();
    return doc;
  }

  int file_size = file.size();
  if (file_size > 1024)
  {
    logOutput("ERROR: Failed to get configuration.");
    Serial.println(F("Config file bigger than JSON document. Alocate more capacity !"));
    doc.clear();
    file.close();
    return doc;
  }

  // Deserialize file to JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    logOutput("ERROR: Failed to get configuration.");
    Serial.println("Failed to deserialize file to JSON document.");
    Serial.println(error.c_str());
    doc.clear();
    file.close();
    return doc;
  }

  file.close();

  updateLiveState(doc);
  // serializeJsonPretty(doc, Serial);

  return doc;
}

// Update settings
bool JSONtoSettings(StaticJsonDocument<1024> doc)
{
  // Open file to write
  File file = SPIFFS.open("/config.json", "w");
  if (!file)
  {
    logOutput("ERROR: Failed to reset. Try again.");
    Serial.println(F("Could not open file to write config !!!"));
    return 0;
  }

  // Serialize JSON document to file
  if (serializeJsonPretty(doc, file) == 0)
  {
    doc.clear();
    file.close();
    logOutput("ERROR: Failed to reset. Try again.");
    Serial.println(F("Failed to write to file"));
    return 0;
  }

  doc.clear();
  file.close();

  return 1;
}

// Save settings in /config.json
void saveSettings(StaticJsonDocument<384> json, String key)
{
  StaticJsonDocument<1024> doc;
  File file = SPIFFS.open("/config.json", "r");
  if (!file)
  {
    logOutput("ERROR: Failed to save settings. Try again.");
    Serial.println("Could not open file to read config !!!");
    return;
  }

  int file_size = file.size();
  if (file_size > 1024)
  {
    logOutput("ERROR: Failed to save settings. Try again.");
    Serial.println("Config file bigger than JSON document. Alocate more capacity !");
    doc.clear();
    return;
  }

  // Deserialize file to JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    logOutput("ERROR: Failed to save settings. Try again.");
    Serial.println("Failed to read file, using default configuration");
    return;
  }

  file.close();
  String nested_key = "";

  for (JsonPair i : json[key].as<JsonObject>())
  {
    nested_key = i.key().c_str();
    json[key][nested_key].as<String>().trim();
    // Serial.print("json[key][nested_key].as<String>() : '");
    // Serial.print(json[key][nested_key].as<String>());
    // Serial.println("'");

    if (json[key][nested_key].as<String>().length() == 0)
    {
      if (nested_key == "timer1" || nested_key == "timer2")
        doc[key][nested_key] = "0";
      else if (nested_key == "pulse_width" || nested_key == "pulse_gap")
        doc[key][nested_key] = "90";
      else if (nested_key == "database_url")
        doc[key][nested_key] = "Not Set";
      else if (nested_key == "username" || nested_key == "password")
        doc[key][nested_key] = "";
    }
    else
      doc[key][nested_key] = json[key][nested_key];
  }

  file = SPIFFS.open("/config.json", "w");
  if (!file)
  {
    logOutput("ERROR: Failed to save settings. Try again.");
    Serial.println("Could not open file to read config !!!");
    return;
  }

  // Serialize JSON document to file
  if (serializeJsonPretty(doc, file) == 0)
  {
    doc.clear();
    file.close();
    logOutput("ERROR: Failed to save settings. Try again.");
    Serial.println("Failed to write to file");
    return;
  }
  // Update Live state
  updateLiveState(doc);

  doc.clear();
  file.close();
}

StaticJsonDocument<1024> softReset()
{
  StaticJsonDocument<1024> doc;

  doc["network_settings"]["connection"] = network_settings.connection;
  doc["network_settings"]["ip_type"] = network_settings.ip_type;
  if (network_settings.connection == "WiFi")
  {
    doc["network_settings"]["ssid"] = network_settings.ssid;
    doc["network_settings"]["password"] = network_settings.password;
  }
  else if (network_settings.connection == "Ethernet")
  {
    network_settings.ssid = "Ethernet Connection";
    network_settings.password = "Ethernet Connection";
  }
  if (network_settings.ip_type == "DHCP")
  {
    doc["network_settings"]["ip_address"] = WiFi.localIP().toString();
    doc["network_settings"]["gateway"] = WiFi.gatewayIP().toString();
    doc["network_settings"]["subnet"] = WiFi.subnetMask().toString();
    doc["network_settings"]["dns"] = WiFi.dnsIP().toString();
  }
  else if (network_settings.ip_type == "Static")
  {
    doc["network_settings"]["ip_address"] = network_settings.ip_address;
    doc["network_settings"]["gateway"] = network_settings.gateway;
    doc["network_settings"]["subnet"] = network_settings.subnet;
    doc["network_settings"]["dns"] = network_settings.dns;
  }

  doc["input"]["ip_1"] = "Not Set";
  doc["input"]["port_1"] = "Not Set";
  doc["input"]["ip_2"] = "Not Set";
  doc["input"]["port_2"] = "Not Set";

  doc["output"]["timer1"] = "0";
  doc["output"]["timer2"] = "0";

  doc["relay1"]["state1"] = "Off";

  doc["relay2"]["state2"] = "Off";

  doc["wiegand"]["database_url"] = "Not Set";
  doc["wiegand"]["pulse_width"] = "90";
  doc["wiegand"]["pulse_gap"] = "90";

  doc["rfid"]["ip_rfid"] = "Not Set";
  doc["rfid"]["port_rfid"] = "Not Set";

  doc["user"]["username"] = "";
  doc["user"]["password"] = "";
  updateLiveState(doc);
  logOutput("Soft Reset succeeded !");
  return doc;
}

StaticJsonDocument<1024> factoryReset()
{
  StaticJsonDocument<1024> doc;

  // doc["network_settings"]["connection"] = "Ethernet";
  // doc["network_settings"]["ip_type"] = "Static";
  // doc["network_settings"]["ssid"] = "";
  // doc["network_settings"]["password"] = "";

  doc["network_settings"]["connection"] = "WiFi";
  doc["network_settings"]["ip_type"] = "DHCP";
  doc["network_settings"]["ssid"] = "Jorj-2.4";
  doc["network_settings"]["password"] = "cafea.amara";

  doc["network_settings"]["ip_address"] = "";
  doc["network_settings"]["gateway"] = "";
  doc["network_settings"]["subnet"] = "";
  doc["network_settings"]["dns"] = "";

  doc["input"]["ip_1"] = "Not Set";
  doc["input"]["port_1"] = "Not Set";
  doc["input"]["ip_2"] = "Not Set";
  doc["input"]["port_2"] = "Not Set";

  doc["output"]["timer1"] = "0";
  doc["output"]["timer2"] = "0";

  doc["relay1"]["state1"] = "Off";

  doc["relay2"]["state2"] = "Off";

  doc["wiegand"]["database_url"] = "Not Set";
  doc["wiegand"]["pulse_width"] = "90";
  doc["wiegand"]["pulse_gap"] = "90";

  doc["rfid"]["ip_rfid"] = "Not Set";
  doc["rfid"]["port_rfid"] = "Not Set";

  doc["user"]["username"] = "";
  doc["user"]["password"] = "";
  updateLiveState(doc);
  logOutput("Factory Reset succeeded !");
  return doc;
}

void updateRelay(StaticJsonDocument<384> json)
{
  String oldState1 = relays.state1;
  String oldState2 = relays.state2;
  relays.state1 = json["relay1"]["state1"].as<String>();
  relays.state2 = json["relay2"]["state2"].as<String>();

  if (oldState1 != relays.state1)
  {
    if (relays.state1 == "On")
    {
      digitalWrite(RELAY1, HIGH);
      logOutput("Relay1 is ON");
      if (output.getTimer1().toInt() == 0)
      {
        relays.manualClose1 = true;
        // logOutput(" Relay 1 will remain open until it is manually closed !");
        Serial.println(" Relay 1 will remain open until it is manually closed !");
      }
      else
      {
        relays.manualClose1 = false;
        relays.startTimer1 = millis();
        logOutput(" Relay 1 will automatically close in " + output.getTimer1() + " seconds !");
      }
    }
    else if (relays.state1 == "Off")
    {
      digitalWrite(RELAY1, LOW);
      relays.manualClose1 = true;
      logOutput(" Relay 1 is Off");
    }
  }

  if (oldState2 != relays.state2)
  {
    if (relays.state2 == "On")
    {
      digitalWrite(RELAY2, HIGH);
      logOutput("Relay 2 is ON");
      if (output.getTimer2().toInt() == 0)
      {
        relays.manualClose2 = true;
        Serial.println("Relay 2 will remain open until it is manually closed !");
      }
      else
      {
        relays.manualClose2 = false;
        logOutput("Relay 2 will automatically close in " + output.getTimer2() + " seconds !");
      }
      relays.startTimer2 = millis();
    }
    else if (relays.state2 == "Off")
    {
      digitalWrite(RELAY2, LOW);
      logOutput(" Relay 2 is OFF");
      relays.manualClose2 = true;
    }
  }
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_ETH_START:
    Serial.println(F("ETH Started"));
    //set eth hostname here
    if (!ETH.setHostname("Metrici-MultiController-Eth"))
    {
      Serial.println(F("Ethernet hostname failed to configure"));
    }
    break;
  case SYSTEM_EVENT_ETH_CONNECTED:
    Serial.println(F("ETH Connected"));
    break;
  case SYSTEM_EVENT_ETH_GOT_IP:
    eth_connected = true;
    Serial.print(F("ETH MAC: "));
    Serial.print(ETH.macAddress());
    Serial.print(F(", IPv4: "));
    Serial.print(ETH.localIP());
    if (ETH.fullDuplex())
    {
      Serial.print(F(", FULL_DUPLEX"));
    }
    Serial.print(F(", "));
    Serial.print(ETH.linkSpeed());
    Serial.println(F("Mbps"));
    break;
  case SYSTEM_EVENT_ETH_DISCONNECTED:
    Serial.println(F("ETH Disconnected"));
    eth_connected = false;
    break;
  case SYSTEM_EVENT_ETH_STOP:
    Serial.println(F("ETH Stopped"));
    eth_connected = false;
    break;
  default:
    break;
  }
}

void eth_connectionc()
{
  ETH.begin();

  IPAddress local_sta(192, 168, 100, 10);
  IPAddress gateway_sta(192, 168, 100, 1);
  IPAddress subnet_sta(255, 255, 255, 0);
  IPAddress primary_dns(8, 8, 8, 8);

  if (!ETH.config(local_sta, gateway_sta, subnet_sta, primary_dns))
  {
    Serial.println(F("Couldn't configure STATIC IP ! Obtaining DHCP IP !"));
  }

  int k = 0;
  while (!eth_connected && k < 20)
  {
    Serial.println((String) "[" + (k + 1) + "] - Establishing ETHERNET Connection ... ");
    delay(1000);
    k++;
  }

  if (!eth_connected)
  {
    Serial.println(F("(1) Could not connect to network ! Trying again..."));
    Serial.println(F("Controller will restart in 5 seconds !"));
    delay(5000);
    ESP.restart();
  }
}

void wifi_connection()
{
}

void checkUser()
{
  if (user.getUsername().length() > 0 && user.getUserPassword().length() > 0)
    user.user_flag = true;
  else
    user.user_flag = false;
}

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(true))
  {
    Serial.println(F("An Error has occurred while mounting SPIFFS ! Formatting in progress"));
    return;
  }
  // get settings from /config.json and update Live state
  if (settingsToJSON().isNull())
  {
    logOutput("ERROR: Could not get start-up configuration. Restarting...");
    restart_sequence(2);
  }

  WiFi.onEvent(WiFiEvent);
  // network_conn();
  checkUser();

  //setting the pins for Outputs
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  // listAllFiles();

  Serial.print("SSID: ");
  Serial.println(network_settings.ssid);
  Serial.print("Password: ");
  Serial.println(network_settings.password);

  WiFi.begin(network_settings.ssid.c_str(), network_settings.password.c_str());

  int k = 0;
  while (WiFi.status() != WL_CONNECTED && k < 20)
  {
    k++;
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED)
    ESP.restart();

  Serial.print("WiFi.getMode() ");
  Serial.println(WiFi.getMode());

  server.on("/api/settings/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              StaticJsonDocument<1024> json = settingsToJSON();

              String response;
              serializeJsonPretty(json, response);
              // Serial.print('\n');
              // serializeJsonPretty(json, Serial);
              json.clear();
              request->send(200, "application/json", response);
            });

  server.on("/api/backup", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/config.json", String(), true); });

  server.on("/api/soft-reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              StaticJsonDocument<1024> json = softReset();

              JSONtoSettings(json);

              // String response;
              // serializeJsonPretty(json, response);
              // Serial.print('\n');
              json.clear();
              request->send(200);
            });

  server.on("/api/factory-reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              StaticJsonDocument<1024> json = factoryReset();

              JSONtoSettings(json);

              // String response;
              // serializeJsonPretty(json, response);
              // Serial.print('\n');
              json.clear();
              request->send(200);
            });

  server.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              request->send(SPIFFS, "/config.json", String(), true);
            });

  server.on("/relay1/on", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              StaticJsonDocument<384> relay_json;
              relay_json["relay1"]["state1"] = "On";
              updateRelay(relay_json);
              saveSettings(relay_json, "relay1");
              relay_json.clear();
              request->send(200, "text/plain", "Relay 1 is ON");
            });

  server.on("/relay1/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUserPassword().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              StaticJsonDocument<384> relay_json;
              relay_json["relay1"]["state1"] = "Off";
              updateRelay(relay_json);
              saveSettings(relay_json, "relay1");
              relay_json.clear();

              request->send(200, "text/plain", "Relay 1 is OFF");
            });

  server.on("/relay2/on", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              StaticJsonDocument<384> relay_json;
              relay_json["relay2"]["state2"] = "On";
              updateRelay(relay_json);
              saveSettings(relay_json, "relay2");
              relay_json.clear();
              request->send(200, "text/plain", "Relay 1 is ON");
            });
  server.on("/relay2/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              StaticJsonDocument<384> relay_json;
              relay_json["relay2"]["state2"] = "Off";
              updateRelay(relay_json);
              saveSettings(relay_json, "relay2");
              relay_json.clear();

              request->send(200, "text/plain", "Relay 1 is OFF");
            });

  server.on("/api/logs", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              circle.print();
              request->send(200, "text/plain", strlog);
            });
  // POST
  AsyncCallbackJsonWebHandler *network_handler =
      new AsyncCallbackJsonWebHandler("/api/network/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<384> network;
                                        if (json.is<JsonArray>())
                                        {
                                          network = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          network = json.as<JsonObject>();
                                        }

                                        saveSettings(network, "network_settings");
                                        Serial.println("Received Settings: ");
                                        serializeJsonPretty(network, Serial);
                                        Serial.print('\n');
                                        network.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                      });

  AsyncCallbackJsonWebHandler *input_handler =
      new AsyncCallbackJsonWebHandler("/api/input/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<384> input_data;
                                        if (json.is<JsonArray>())
                                        {
                                          input_data = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          input_data = json.as<JsonObject>();
                                        }

                                        saveSettings(input_data, "input");
                                        Serial.println("Received Settings: ");
                                        serializeJsonPretty(input_data, Serial);
                                        Serial.print('\n');
                                        input_data.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                      });
  AsyncCallbackJsonWebHandler *output_handler =
      new AsyncCallbackJsonWebHandler("/api/output/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<384> output_data;
                                        if (json.is<JsonArray>())
                                        {
                                          output_data = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          output_data = json.as<JsonObject>();
                                        }

                                        saveSettings(output_data, "output");
                                        Serial.println("Received Settings: ");
                                        serializeJsonPretty(output_data, Serial);
                                        Serial.print('\n');
                                        output_data.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                      });
  AsyncCallbackJsonWebHandler *wiegand_handler =
      new AsyncCallbackJsonWebHandler("/api/wiegand/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<384> wiegand_data;
                                        if (json.is<JsonArray>())
                                        {
                                          wiegand_data = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          wiegand_data = json.as<JsonObject>();
                                        }

                                        saveSettings(wiegand_data, "wiegand");
                                        Serial.println("Received Settings: ");
                                        serializeJsonPretty(wiegand_data, Serial);
                                        Serial.print('\n');
                                        wiegand_data.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                      });
  AsyncCallbackJsonWebHandler *rfid_handler =
      new AsyncCallbackJsonWebHandler("/api/rfid/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<384> rfid_data;
                                        if (json.is<JsonArray>())
                                        {
                                          rfid_data = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          rfid_data = json.as<JsonObject>();
                                        }

                                        saveSettings(rfid_data, "rfid");
                                        Serial.println("Received Settings: ");
                                        serializeJsonPretty(rfid_data, Serial);
                                        Serial.print('\n');
                                        rfid_data.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                      });
  AsyncCallbackJsonWebHandler *relay_handler =
      new AsyncCallbackJsonWebHandler("/api/relay/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<384> relay_data;
                                        if (json.is<JsonArray>())
                                        {
                                          relay_data = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          relay_data = json.as<JsonObject>();
                                        }

                                        updateRelay(relay_data);

                                        if (!relay_data["relay1"]["state1"].isNull())
                                        {
                                          saveSettings(relay_data, "relay1");
                                        }
                                        if (!relay_data["relay2"]["state2"].isNull())
                                        {
                                          saveSettings(relay_data, "relay2");
                                        }

                                        Serial.println("Received Settings /api/relay/post: ");
                                        serializeJsonPretty(relay_data, Serial);
                                        Serial.print('\n');
                                        relay_data.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                      });
  AsyncCallbackJsonWebHandler *user_handler =
      new AsyncCallbackJsonWebHandler("/api/user/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<384> user_data;
                                        if (json.is<JsonArray>())
                                        {
                                          user_data = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          user_data = json.as<JsonObject>();
                                        }

                                        saveSettings(user_data, "user");
                                        Serial.println("Received Settings /api/user/post: ");
                                        serializeJsonPretty(user_data, Serial);
                                        Serial.print('\n');
                                        user_data.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                        restart_sequence(1);
                                      });

  server.addHandler(network_handler);
  server.addHandler(input_handler);
  server.addHandler(output_handler);
  server.addHandler(wiegand_handler);
  server.addHandler(rfid_handler);
  server.addHandler(relay_handler);
  server.addHandler(user_handler);

  if (user.user_flag)
  {
    server.serveStatic("/settings", SPIFFS, "/www/settings.html").setAuthentication(user.getUsername().c_str(), user.getUserPassword().c_str());
    server.serveStatic("/dashboard", SPIFFS, "/www/index.html").setAuthentication(user.getUsername().c_str(), user.getUserPassword().c_str());
    server.serveStatic("/user", SPIFFS, "/ap/user_ap.html").setFilter(ON_AP_FILTER).setAuthentication(user.getUsername().c_str(), user.getUserPassword().c_str());
    server.serveStatic("/user", SPIFFS, "/www/user_sta.html").setFilter(ON_STA_FILTER).setAuthentication(user.getUsername().c_str(), user.getUserPassword().c_str());
  }
  else
  {
    server.serveStatic("/settings", SPIFFS, "/www/settings.html");
    server.serveStatic("/dashboard", SPIFFS, "/www/index.html");
    server.serveStatic("/user", SPIFFS, "/ap/user_ap.html").setFilter(ON_AP_FILTER);
    server.serveStatic("/user", SPIFFS, "/www/user_sta.html").setFilter(ON_STA_FILTER);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("/dashboard"); });
  server.serveStatic("/", SPIFFS, "/");

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404); });

  server.onFileUpload(handleUpload);
  server.begin();
}

void loop()
{
  delay(1000);
  checkUser();
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
}