#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <SPIFFS.h>

AsyncWebServer server(80);
static bool eth_connected = false;

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

struct Settings
{
  String connection = "";
  String type = "";
  String ssid = "";
  String ip_address = "";
  String gateway = "";
  String subnet = "";
  String dns = "";
} settings;

struct IO
{
  String ip_1 = "";
  String port_1 = "";
  String ip_2 = "";
  String port_2 = "";
  String timer_1 = "";
  String timer_2 = "";
} io;

struct Wiegand
{
  String database_url = "";
  String pulse_width = "90";
  String pulse_gap = "90";
} wiegand;

struct RFID
{
  String ip_rfid = "";
  String port_rfid = "";
} rfid;

StaticJsonDocument<768> readSettings(const char *filename)
{
  StaticJsonDocument<768> doc;
  File file = SPIFFS.open(filename);
  if (!file)
    Serial.println("Could not open file to read config !!!");

  int file_size = file.size();
  if (file_size > 768)
  {
    Serial.println("Json file bigger than Json document. Alocate more capacity !");
    doc.clear();
    return doc;
  }

  // Deserialize file to JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  settings.connection = doc["settings"]["connection"] | "Not working";
  settings.type = doc["settings"]["type"] | "Not working";
  settings.ssid = doc["settings"]["ssid"] | "Not working";
  settings.ip_address = doc["settings"]["ip_address"] | "Not working";
  settings.gateway = doc["settings"]["gateway"] | "Not working";
  settings.subnet = doc["settings"]["subnet"] | "Not working";
  settings.dns = doc["settings"]["dns"] | "Not working";

  io.ip_1 = doc["io"]["ip_1"] | "Not working";
  io.port_1 = doc["io"]["port_1"] | "Not working";
  io.ip_2 = doc["io"]["ip_2"] | "Not working";
  io.port_2 = doc["io"]["port_2"] | "Not working";
  io.timer_1 = doc["io"]["timer_1"] | "Not working";
  io.timer_2 = doc["io"]["timer_2"] | "Not working";

  wiegand.database_url = doc["wiegand"]["database_url"] | "Not working";
  wiegand.pulse_width = doc["wiegand"]["pulse_width"] | "Not working";
  wiegand.pulse_gap = doc["wiegand"]["pulse_gap"] | "Not working";

  rfid.ip_rfid = doc["rfid"]["ip_rfid"] | "Not working";
  rfid.port_rfid = doc["rfid"]["port_rfid"] | "Not working";

  //Output JSON Document to Serial
  // serializeJsonPretty(doc, Serial);
  // Serial.println();

  file.close();
  return doc;
}

void saveSettings(const char *filename)
{
  File file = SPIFFS.open(filename, "w");
  if (!file)
    Serial.println("Could not open file to write config !!!");

  StaticJsonDocument<768> doc;

  doc["settings"]["connection"] = settings.connection;
  doc["settings"]["type"] = settings.type;
  doc["settings"]["ssid"] = settings.ssid;
  doc["settings"]["ip_address"] = settings.ip_address;
  doc["settings"]["gateway"] = settings.gateway;
  doc["settings"]["subnet"] = settings.subnet;
  doc["settings"]["dns"] = settings.dns;

  doc["io"]["ip_1"] = io.ip_1;
  doc["io"]["port_1"] = io.port_1;
  doc["io"]["ip_2"] = io.ip_2;
  doc["io"]["port_2"] = io.port_2;
  doc["io"]["timer_1"] = io.timer_1;
  doc["io"]["timer_2"] = io.timer_2;

  doc["wiegand"]["database_url"] = wiegand.database_url;
  doc["wiegand"]["pulse_width"] = wiegand.pulse_width;
  doc["wiegand"]["pulse_gap"] = wiegand.pulse_gap;

  doc["rfid"]["ip_rfid"] = rfid.ip_rfid;
  doc["rfid"]["port_rfid"] = rfid.port_rfid;

  // Serialize JSON document to file
  if (serializeJson(doc, file) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }

  doc.clear();
  file.close();
}

// void WiFiEvent(WiFiEvent_t event)
// {
//   switch (event)
//   {
//   case SYSTEM_EVENT_ETH_START:
//     Serial.println("ETH Started");
//     //set eth hostname here
//     if (!ETH.setHostname("Metrici-MultiController-Eth"))
//     {
//       Serial.println("Ethernet hostname failed to configure");
//     }
//     break;
//   case SYSTEM_EVENT_ETH_CONNECTED:
//     Serial.println("ETH Connected");
//     break;
//   case SYSTEM_EVENT_ETH_GOT_IP:
//     eth_connected = true;
//     Serial.print("ETH MAC: ");
//     Serial.print(ETH.macAddress());
//     Serial.print(", IPv4: ");
//     Serial.print(ETH.localIP());
//     if (ETH.fullDuplex())
//     {
//       Serial.print(", FULL_DUPLEX");
//     }
//     Serial.print(", ");
//     Serial.print(ETH.linkSpeed());
//     Serial.println("Mbps");
//     break;
//   case SYSTEM_EVENT_ETH_DISCONNECTED:
//     Serial.println("ETH Disconnected");
//     eth_connected = false;
//     break;
//   case SYSTEM_EVENT_ETH_STOP:
//     Serial.println("ETH Stopped");
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
    Serial.println("(1) Could not connect to network ! Trying again...");
    Serial.println("Controller will restart in 5 seconds !");
    delay(5000);
    ESP.restart();
  }

  IPAddress local_sta(192, 168, 100, 10);
  IPAddress gateway_sta(192, 168, 100, 1);
  IPAddress subnet_sta(255, 255, 255, 0);
  IPAddress primary_dns(8, 8, 8, 8);

  if (!ETH.config(local_sta, gateway_sta, subnet_sta, primary_dns))
  {
    Serial.println("Couldn't configure STATIC IP ! Obtaining DHCP IP !");
  }
}

void setup()
{
  Serial.begin(115200);
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS ! Formatting in progress");
    return;
  }
  // WiFi.onEvent(WiFiEvent);
  // network_conn();

  listAllFiles();

  WiFi.begin("Jorj_2.4", "cafea.amara");

  int k = 0;
  while (WiFi.status() != WL_CONNECTED && k < 20)
  {
    k++;
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED)
    ESP.restart();

  Serial.println(WiFi.getMode());

  // server.on("/settings", HTTP_ANY, [](AsyncWebServerRequest *request)
  //           { request->send(SPIFFS, "/index.html", "text/html"); });

  // server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request)
  //           { request->send(SPIFFS, "/index.html", "text/html"); });

  // GET

  // DefaultHeaders::Instance().addHeader("Content-Encoding", "gzip");

  server.on("/api/settings/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              StaticJsonDocument<768> settings_json = readSettings("/config.json");
              // if (request->hasParam("message"))
              // {
              //   settings["message"] = request->getParam("message")->value();
              // }
              // else
              // {
              //   settings["message"] = "No message parameter";
              // }

              String response;
              serializeJson(settings_json, response);
              // Serial.print("/api/settings/get response: ");
              // Serial.println(response);
              // serializeJsonPretty(settings_json, Serial);
              settings_json.clear();
              request->send(200, "application/json", response);
            });

  // POST
  AsyncCallbackJsonWebHandler *handler =
      new AsyncCallbackJsonWebHandler("/api/settings/post", [](AsyncWebServerRequest *request, JsonVariant &json)
                                      {
                                        StaticJsonDocument<200> data;
                                        if (json.is<JsonArray>())
                                        {
                                          data = json.as<JsonArray>();
                                        }
                                        else if (json.is<JsonObject>())
                                        {
                                          data = json.as<JsonObject>();
                                        }

                                        // send the data that we receive back
                                        String response;
                                        Serial.println("/api/settings/post response: ");
                                        serializeJson(data, response);
                                        serializeJson(data, Serial);
                                        request->send(200);
                                        // Serial.println(response);
                                      });
  server.addHandler(handler);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("/dashboard"); });
  // server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  // server.serveStatic("/settings", SPIFFS, "/settings.html");
  // server.serveStatic("/dashboard", SPIFFS, "/dashboard.html");

  server.serveStatic("/settings", SPIFFS, "/www/settings.html");
  server.serveStatic("/dashboard", SPIFFS, "/www/index.html");
  // server.serveStatic("/", SPIFFS, "/img");
  // server.serveStatic("/", SPIFFS, "/ap").setFilter(ON_AP_FILTER);
  // server.serveStatic("/", SPIFFS, "/www").setDefaultFile("index.html").setFilter(ON_STA_FILTER).setAuthentication("", "");
  server.serveStatic("/", SPIFFS, "/");
  // server.onNotFound([](AsyncWebServerRequest *request)
  //                   { request->redirect("/dashboard"); });

  server.begin();
}

void loop()
{
}