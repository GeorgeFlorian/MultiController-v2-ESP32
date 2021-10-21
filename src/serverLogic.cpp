#include <serverLogic.h>

AsyncWebServer server(80);

void handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
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
                                        restartSequence(1);
                                    });

// Main server function
void startEspServer()
{
    server.on("/api/settings/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  if (user.user_flag)
                  {
                      if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                          return request->requestAuthentication(NULL, false);
                  }
                  StaticJsonDocument<1024> json = getLiveState();

                  String response;
                  serializeJson(json, response);
                  // Serial.print('\n');
                  // serializeJsonPretty(json, Serial);
                  json.clear();
                  request->send(200, "application/json", response);
              });

    server.on("/api/backup", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  if (user.user_flag)
                  {
                      if (!request->authenticate(user.getUsername().c_str(), user.getUserPassword().c_str()))
                          return request->requestAuthentication(NULL, false);
                  }
                  request->send(SPIFFS, "/config.json", String(), true);
              });

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

    server.on("/wiegand", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  if (wiegand_state.working)
                  {
                      request->send(200, "text/plain", "Warning: Another Wiegand is being sent.");
                  }
                  else
                  {
                      if (request->hasArg("number"))
                      {
                          wiegand_state.plate_number = request->arg("number");
                          wiegand_state.wiegand_flag = true;
                          Serial.println("(Inside POST /wiegand) - Plate Number: " + wiegand_state.plate_number);
                          // size_t size = request->contentLength();
                          // Serial.print("Post Size: ");
                          // Serial.println(size);
                          // logOutput((String)"The Plate Number is: " + wiegand_state.plate_number);
                          request->send(200, "text/plain", (String) "Plate Number: " + wiegand_state.plate_number + " received. Sending Wiegand ID.");
                      }
                      else
                      {
                          Serial.println("Post did not have a 'number' field.");
                          wiegand_state.wiegand_flag = false;
                          request->send(200, "text/plain", "Post did not have a 'number' field.");
                      }
                  }
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
        server.serveStatic("/user", SPIFFS, "/www/user_sta.html").setFilter(ON_STA_FILTER);
        server.serveStatic("/user", SPIFFS, "/ap/user_ap.html").setFilter(ON_AP_FILTER);
    }

    server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=600");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->redirect("/dashboard"); });
    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404); });

    server.onFileUpload(handleUpload);
    server.begin();
}