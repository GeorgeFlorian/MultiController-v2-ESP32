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

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (filename.indexOf(".bin") > 0)
  {
    if (!index)
    {
      // logOutput("The update process has started...");
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
        // logOutput("Update complete");
        Serial.flush();
        ESP.restart();
      }
    }
  }
  else if (filename.indexOf(".json") > 0)
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
      // logOutput((String)filename + " was successfully uploaded! File size: " + index + len);
      // close the file handle as the upload is now done
      request->_tempFile.close();
      // request->redirect("/files");
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

struct Output
{
  String timer1 = "";
  String timer2 = "";
} output;

struct Relay
{
  String state1 = "";
  String state2 = "";

} relay;

struct Wiegand
{
  String database_url = "";
  String pulse_width = "";
  String pulse_gap = "";
} wiegand;

struct RFID
{
  String ip_rfid = "";
  String port_rfid = "";
} rfid;

struct User
{
  String user_name = "";
  String user_pass = "";

  bool user_flag = false;
} user;

// Get settings from /config.json
StaticJsonDocument<768> settingsToJSON()
{
  StaticJsonDocument<768> doc;
  File file = SPIFFS.open("/config.json");
  if (!file)
    Serial.println(F("Could not open file to read config !!!"));

  int file_size = file.size();
  if (file_size > 768)
  {
    Serial.println(F("Config file bigger than JSON document. Alocate more capacity !"));
    doc.clear();
    return doc;
  }

  // Deserialize file to JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  file.close();

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

  output.timer1 = doc["output"]["timer1"] | "Not working";
  output.timer2 = doc["output"]["timer2"] | "Not working";

  // get live relay state ???
  relay.state1 = doc["relay"]["state1"] | "Not working";
  relay.state2 = doc["relay"]["state2"] | "Not working";

  wiegand.database_url = doc["wiegand"]["database_url"] | "Not working";
  wiegand.pulse_width = doc["wiegand"]["pulse_width"] | "Not working";
  wiegand.pulse_gap = doc["wiegand"]["pulse_gap"] | "Not working";

  rfid.ip_rfid = doc["rfid"]["ip_rfid"] | "Not working";
  rfid.port_rfid = doc["rfid"]["port_rfid"] | "Not working";

  //Output JSON Document to Serial
  // serializeJsonPretty(doc, Serial);
  // Serial.println();

  return doc;
}

// Save or change settings in /config.json
bool saveSettings(StaticJsonDocument<768> doc)
{
  File file = SPIFFS.open("/config.json", "w");
  if (!file)
    Serial.println(F("Could not open file to write config !!!"));

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
    network_settings.gateway = WiFi.gatewayIP().toString();
    network_settings.subnet = WiFi.subnetMask().toString();
    network_settings.dns = WiFi.dnsIP().toString();
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

  output.timer1 = doc["output"]["timer1"] | "Not working";
  output.timer2 = doc["output"]["timer2"] | "Not working";

  relay.state1 = doc["relay"]["state1"] | "Not working";
  relay.state2 = doc["relay"]["state2"] | "Not working";

  wiegand.database_url = doc["wiegand"]["database_url"] | "Not working";
  wiegand.pulse_width = doc["wiegand"]["pulse_width"] | "Not working";
  wiegand.pulse_gap = doc["wiegand"]["pulse_gap"] | "Not working";

  rfid.ip_rfid = doc["rfid"]["ip_rfid"] | "Not working";
  rfid.port_rfid = doc["rfid"]["port_rfid"] | "Not working";

  // Serialize JSON document to file
  if (serializeJsonPretty(doc, file) == 0)
  {
    doc.clear();
    file.close();
    Serial.println(F("Failed to write to file"));
    return 0;
  }

  doc.clear();
  file.close();
  return 1;
}

StaticJsonDocument<768> softReset()
{
  StaticJsonDocument<768> doc;

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
  if (network_settings.ip_type == "Static")
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

  doc["relay"]["state1"] = "Off";
  doc["relay"]["state2"] = "Off";

  doc["wiegand"]["database_url"] = "Not Set";
  doc["wiegand"]["pulse_width"] = "90";
  doc["wiegand"]["pulse_gap"] = "90";

  doc["rfid"]["ip_rfid"] = "Not Set";
  doc["rfid"]["port_rfid"] = "Not Set";
  return doc;
}

StaticJsonDocument<768> factoryReset()
{
  StaticJsonDocument<768> doc;

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

  doc["relay"]["state1"] = "Off";
  doc["relay"]["state2"] = "Off";

  doc["wiegand"]["database_url"] = "Not Set";
  doc["wiegand"]["pulse_width"] = "90";
  doc["wiegand"]["pulse_gap"] = "90";

  doc["rfid"]["ip_rfid"] = "Not Set";
  doc["rfid"]["port_rfid"] = "Not Set";
  return doc;
}

void update_settings(StaticJsonDocument<384> json, String key)
{
  StaticJsonDocument<768> doc;
  File file = SPIFFS.open("/config.json", "r");
  if (!file)
  {
    Serial.println("Could not open file to read config !!!");
    return;
  }

  int file_size = file.size();
  if (file_size > 768)
  {
    Serial.println("Config file bigger than JSON document. Alocate more capacity !");
    doc.clear();
    return;
  }

  // Deserialize file to JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println("Failed to read file, using default configuration");

  file.close();
  String nested_key = "";

  for (JsonPair i : json[key].as<JsonObject>())
  {
    nested_key = i.key().c_str();
    Serial.print("json[key][nested_key].as<String>() : '");
    Serial.print(json[key][nested_key].as<String>());
    Serial.println(" '");
    if (json[key][nested_key].as<String>().length() == 0)
      // doc[key][nested_key].set("Not Set");
      Serial.println();
    else
      doc[key][nested_key] = json[key][nested_key];
  }

  file = SPIFFS.open("/config.json", "w");
  if (!file)
  {
    Serial.println("Could not open file to read config !!!");
    return;
  }
  // Serial.println("Inside update settings: ");
  // serializeJsonPretty(doc, Serial);

  // Serialize JSON document to file
  if (serializeJsonPretty(doc, file) == 0)
  {
    doc.clear();
    file.close();
    Serial.println("Failed to write to file");
    return;
  }
  doc.clear();
  file.close();

  settingsToJSON();
}
StaticJsonDocument<384> getState()
{
  StaticJsonDocument<768> doc;
  StaticJsonDocument<384> state_json;

  File file = SPIFFS.open("/config.json", "r");
  if (!file)
  {
    Serial.println("Could not open file to read config !!!");
    return doc;
  }

  int file_size = file.size();
  if (file_size > 768)
  {
    Serial.println("Config file bigger than JSON document. Alocate more capacity !");
    doc.clear();
    return doc;
  }
  // Deserialize file to JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println("Failed to read file, using default configuration");

  file.close();
  String key = "relay";
  String nested_key = "";

  for (JsonPair i : doc[key].as<JsonObject>())
  {
    nested_key = i.key().c_str();
    state_json[key][nested_key] = doc[key][nested_key];
  }
  doc.clear();
  return state_json;
}

// void WiFiEvent(WiFiEvent_t event)
// {
//   switch (event)
//   {
//   case SYSTEM_EVENT_ETH_START:
//     Serial.println(F("ETH Started"));
//     //set eth hostname here
//     if (!ETH.setHostname("Metrici-MultiController-Eth"))
//     {
//       Serial.println(F("Ethernet hostname failed to configure"));
//     }
//     break;
//   case SYSTEM_EVENT_ETH_CONNECTED:
//     Serial.println(F("ETH Connected"));
//     break;
//   case SYSTEM_EVENT_ETH_GOT_IP:
//     eth_connected = true;
//     Serial.print(F("ETH MAC: "));
//     Serial.print(ETH.macAddress());
//     Serial.print(F(", IPv4: "));
//     Serial.print(ETH.localIP());
//     if (ETH.fullDuplex())
//     {
//       Serial.print(F(", FULL_DUPLEX"));
//     }
//     Serial.print(F(", "));
//     Serial.print(ETH.linkSpeed());
//     Serial.println(F("Mbps"));
//     break;
//   case SYSTEM_EVENT_ETH_DISCONNECTED:
//     Serial.println(F("ETH Disconnected"));
//     eth_connected = false;
//     break;
//   case SYSTEM_EVENT_ETH_STOP:
//     Serial.println(F("ETH Stopped"));
//     eth_connected = false;
//     break;
//   default:
//     break;
//   }
// }

void network_conn()
{
  ETH.begin();
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

  IPAddress local_sta(192, 168, 100, 10);
  IPAddress gateway_sta(192, 168, 100, 1);
  IPAddress subnet_sta(255, 255, 255, 0);
  IPAddress primary_dns(8, 8, 8, 8);

  if (!ETH.config(local_sta, gateway_sta, subnet_sta, primary_dns))
  {
    Serial.println(F("Couldn't configure STATIC IP ! Obtaining DHCP IP !"));
  }
}

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(true))
  {
    Serial.println(F("An Error has occurred while mounting SPIFFS ! Formatting in progress"));
    return;
  }
  // WiFi.onEvent(WiFiEvent);
  // network_conn();

  //setting the pins for Outputs
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  listAllFiles();
  settingsToJSON();
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

  Serial.println(WiFi.getMode());

  server.on("/api/settings/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              StaticJsonDocument<768> json = settingsToJSON();

              String response;
              serializeJsonPretty(json, response);
              Serial.print('\n');
              // serializeJsonPretty(json, Serial);
              json.clear();
              request->send(200, "application/json", response);
            });

  server.on("/api/relay/state", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              StaticJsonDocument<384> json = getState();

              String response;
              serializeJsonPretty(json, response);
              Serial.print('\n');
              // serializeJsonPretty(json, Serial);
              json.clear();
              request->send(200, "application/json", response);
            });

  server.on("/api/backup", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/config.json", String(), true); });

  server.on("/api/soft-reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              StaticJsonDocument<768> json = softReset();

              saveSettings(json);

              String response;
              serializeJsonPretty(json, response);
              Serial.print('\n');
              json.clear();
              request->send(200, "application/json", response);
            });

  server.on("/api/factory-reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              StaticJsonDocument<768> json = factoryReset();

              saveSettings(json);

              String response;
              serializeJsonPretty(json, response);
              Serial.print('\n');
              json.clear();
              request->send(200, "application/json", response);
            });

  server.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/config.json", String(), true); });

  server.on("/relay1/on", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.user_name.c_str(), user.user_pass.c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              digitalWrite(RELAY1, HIGH);
              relay.state1 = "On";
              // logOutput("Relay1 is ON");
              // if (output.timer1.toInt() == 0)
              // {
              //   needManualCloseRelayOne = true;
              //   logOutput(" Relay 1 will remain open until it is manually closed !");
              // }
              // else
              // {
              //   needManualCloseRelayOne = false;
              //   logOutput(" Relay 1 will automatically close in " + output.timer1 + " seconds !");
              // }
              // startTimeRelayOne = millis();
              // Serial.println("Do I get here ? /relay1/on");
              // request->send(200, "text/plain", "Relay 1 is ON");
              request->send(200);
            });

  server.on("/relay1/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (user.user_flag)
              {
                if (!request->authenticate(user.user_name.c_str(), user.user_pass.c_str()))
                  return request->requestAuthentication(NULL, false);
              }
              digitalWrite(RELAY1, LOW);
              relay.state1 = "Off";
              // logOutput("Relay1 is ON");
              // if (output.timer1.toInt() == 0)
              // {
              //   needManualCloseRelayOne = true;
              //   logOutput(" Relay 1 will remain open until it is manually closed !");
              // }
              // else
              // {
              //   needManualCloseRelayOne = false;
              //   logOutput(" Relay 1 will automatically close in " + output.timer1 + " seconds !");
              // }
              // startTimeRelayOne = millis();
              // Serial.println("Do I get here ? /relay1/on");
              // request->send(200, "text/plain", "Relay 1 is ON");
              request->send(200);
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

                                        update_settings(network, "network_settings");
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

                                        update_settings(input_data, "input");
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

                                        update_settings(output_data, "output");
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

                                        update_settings(wiegand_data, "wiegand");
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

                                        update_settings(rfid_data, "rfid");
                                        Serial.println("Received Settings: ");
                                        serializeJsonPretty(rfid_data, Serial);
                                        Serial.print('\n');
                                        rfid_data.clear();
                                        request->send(200);
                                        // Serial.println(response);
                                      });

  server.addHandler(network_handler);
  server.addHandler(input_handler);
  server.addHandler(output_handler);
  server.addHandler(wiegand_handler);
  server.addHandler(rfid_handler);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("/dashboard"); });

  server.serveStatic("/settings", SPIFFS, "/www/settings.html");
  server.serveStatic("/dashboard", SPIFFS, "/www/index.html");
  server.serveStatic("/user", SPIFFS, "/ap/user_ap.html").setFilter(ON_AP_FILTER);
  server.serveStatic("/user", SPIFFS, "/www/user_sta.html").setFilter(ON_STA_FILTER);

  server.serveStatic("/", SPIFFS, "/").setAuthentication("", "");
  // server.onNotFound([](AsyncWebServerRequest *request)
  //                   { request->redirect("/dashboard"); });

  server.onFileUpload(handleUpload);
  server.begin();
}

void loop()
{
}