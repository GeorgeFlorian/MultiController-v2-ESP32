#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"

AsyncWebServer server(80);
static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_ETH_START:
    Serial.println("ETH Started");
    //set eth hostname here
    if (!ETH.setHostname("Metrici-MultiController-Eth"))
    {
      Serial.println("Ethernet hostname failed to configure");
    }
    break;
  case SYSTEM_EVENT_ETH_CONNECTED:
    Serial.println("ETH Connected");
    break;
  case SYSTEM_EVENT_ETH_GOT_IP:
    eth_connected = true;
    Serial.print("ETH MAC: ");
    Serial.print(ETH.macAddress());
    Serial.print(", IPv4: ");
    Serial.print(ETH.localIP());
    if (ETH.fullDuplex())
    {
      Serial.print(", FULL_DUPLEX");
    }
    Serial.print(", ");
    Serial.print(ETH.linkSpeed());
    Serial.println("Mbps");
    break;
  case SYSTEM_EVENT_ETH_DISCONNECTED:
    Serial.println("ETH Disconnected");
    eth_connected = false;
    break;
  case SYSTEM_EVENT_ETH_STOP:
    Serial.println("ETH Stopped");
    eth_connected = false;
    break;
  default:
    break;
  }
}

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

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

void setup()
{
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);
  network_conn();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", "{\"message\":\"Welcome\"}"); });

  server.on("/get-message", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              StaticJsonDocument<100> data;
              if (request->hasParam("message"))
              {
                data["message"] = request->getParam("message")->value();
              }
              else
              {
                data["message"] = "No message parameter";
              }
              String response;
              serializeJson(data, response);
              request->send(200, "application/json", response);
            });

  AsyncCallbackJsonWebHandler *handler =
      new AsyncCallbackJsonWebHandler("/post-message", [](AsyncWebServerRequest *request, JsonVariant &json)
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
                                        String response;
                                        serializeJson(data, response);
                                        request->send(200, "application/json", response);
                                        Serial.println(response);
                                      });
  server.addHandler(handler);

  server.onNotFound(notFound);

  server.begin();
}

void loop()
{
}