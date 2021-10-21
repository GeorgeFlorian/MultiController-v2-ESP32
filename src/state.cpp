#include <state.h>

// Live state of the controller

// Network Settings state
Settings::Settings() : connection(""), ip_type(""), ssid(""), password(""), ip_address(""), gateway(""), subnet(""), dns(""){};

Settings network_settings;

// Input state
Input::Input() : ip_1(""), port_1(""), ip_2(""), port_2(""){};

Input input;

// Output state
Output::Output() : timer1(""), timer2(""){};

void Output::setTimer1(String x)
{
    x.trim();
    if (x == "" && x.length() == 0)
        timer1 = "0";
    else
        timer1 = x;
}
void Output::setTimer2(String x)
{
    x.trim();
    if (x == "" && x.length() == 0)
        timer2 = "0";
    else
        timer2 = x;
}

int Output::getTimer1()
{
    return timer1.toInt();
}
int Output::getTimer2()
{
    return timer2.toInt();
}

Output output;

// Relays state
Relay::Relay() : state1(""), state2(""), manualClose1(false), manualClose2(false), startTimer1(0), startTimer2(0), deltaTimer1(0), deltaTimer2(0){};

Relay relays;

// Wiegand state
WiegandState::WiegandState() : pulse_width(""), pulse_gap(""), database_url(""), wiegand_flag(false), working(false), plate_number(""){};

void WiegandState::setPulseWidth(String x)
{
    x.trim();
    if (x == "" && x.length() == 0)
        pulse_width = "0";
    else
        pulse_width = x;
}
void WiegandState::setPulseGap(String x)
{
    x.trim();
    if (x == "" && x.length() == 0)
        pulse_gap = "0";
    else
        pulse_gap = x;
}
int WiegandState::getPulseWidth()
{
    return pulse_width.toInt();
}
int WiegandState::getPulseGap()
{
    return pulse_gap.toInt();
}

WiegandState wiegand_state;

// RFID state
RFID::RFID() : ip_rfid(""), port_rfid(""), activate_rfid(false), reader_command(true){};
RFID rfid;

// User state
User::User() : username(""), password(""), user_flag(false){};

void User::setUsername(String x)
{
    x.trim();
    if (x.length() <= 0)
        username = "";
    else
        username = x;
}
String User::getUsername()
{
    return username;
}

void User::setUserPassword(String x)
{
    x.trim();
    if (x.length() <= 0)
        password = "";
    else
        password = x;
}
String User::getUserPassword()
{
    return password;
}

User user;

StaticJsonDocument<1024> getLiveState()
{
    StaticJsonDocument<1024> doc;

    doc["network_settings"]["connection"] = network_settings.connection;
    doc["network_settings"]["ip_type"] = network_settings.ip_type;
    doc["network_settings"]["ssid"] = network_settings.ssid;
    doc["network_settings"]["password"] = network_settings.password;

    doc["network_settings"]["ip_address"] = network_settings.ip_address;
    doc["network_settings"]["gateway"] = network_settings.gateway;
    doc["network_settings"]["subnet"] = network_settings.subnet;
    doc["network_settings"]["dns"] = network_settings.dns;

    doc["input"]["ip_1"] = input.ip_1;
    doc["input"]["port_1"] = input.port_1;
    doc["input"]["ip_2"] = input.ip_2;
    doc["input"]["port_2"] = input.port_2;

    doc["output"]["timer1"] = output.getTimer1();
    doc["output"]["timer2"] = output.getTimer2();

    doc["relay1"]["state1"] = relays.state1;

    doc["relay2"]["state2"] = relays.state2;

    doc["wiegand"]["database_url"] = wiegand_state.database_url;
    doc["wiegand"]["pulse_width"] = wiegand_state.getPulseWidth();
    doc["wiegand"]["pulse_gap"] = wiegand_state.getPulseGap();

    doc["rfid"]["ip_rfid"] = rfid.ip_rfid;
    doc["rfid"]["port_rfid"] = rfid.port_rfid;

    doc["user"]["username"] = user.getUsername();
    doc["user"]["password"] = user.getUserPassword();

    return doc;
}

void updateLiveState(StaticJsonDocument<1024> &doc)
{
    network_settings.connection = doc["network_settings"]["connection"] | "Not working";
    network_settings.ip_type = doc["network_settings"]["ip_type"] | "Not working";
    if (network_settings.connection == "WiFi")
    {
        network_settings.ssid = doc["network_settings"]["ssid"] | "Not working";
        network_settings.password = doc["network_settings"]["password"] | "Not working";
    }
    else if (network_settings.connection == "Ethernet" || network_settings.connection == "")
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
    else if (network_settings.ip_type == "Static" || network_settings.ip_type == "")
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

    wiegand_state.database_url = doc["wiegand"]["database_url"] | "Not working";
    wiegand_state.setPulseWidth(doc["wiegand"]["pulse_width"] | "Not working");
    wiegand_state.setPulseGap(doc["wiegand"]["pulse_gap"] | "Not working");

    rfid.ip_rfid = doc["rfid"]["ip_rfid"] | "Not working";
    rfid.port_rfid = doc["rfid"]["port_rfid"] | "Not working";
    rfid.activate_rfid = (rfid.ip_rfid == "Not Set" || rfid.ip_rfid == "Not working" || rfid.port_rfid == "Not Set" || rfid.port_rfid == "Not working") ? false : true;

    user.setUsername(doc["user"]["username"] | "Not working");
    user.setUserPassword(doc["user"]["password"] | "Not working");
}

StaticJsonDocument<1024> readSettings()
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

        // if received value is empty do not change it in /config.json
        if (json[key][nested_key].as<String>().length() == 0)
        {
            if (nested_key == "timer1" || nested_key == "timer2")
                doc[key][nested_key] = "0";
            else if (nested_key == "pulse_width" || nested_key == "pulse_gap")
                doc[key][nested_key] = "90";
            // else if (nested_key == "database_url")
            //     doc[key][nested_key] = "Not Set";
            else if (nested_key == "username" || nested_key == "password")
                doc[key][nested_key] = "";
        }
        else
        {
            doc[key][nested_key] = json[key][nested_key];
            // if (nested_key == "ip_rfid" || nested_key == "port_rfid")
            // {
            //     doc[key][nested_key] = "";
            //     rfid.activate_rfid = true;
            // }
        }
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
            if (output.getTimer1() == 0)
            {
                relays.manualClose1 = true;
                // logOutput(" Relay 1 will remain open until it is manually closed !");
                Serial.println(" Relay 1 will remain open until it is manually closed !");
            }
            else
            {
                relays.manualClose1 = false;
                relays.startTimer1 = millis();
                logOutput((String) "Relay 1 will automatically close in " + output.getTimer1() + " seconds !");
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
            if (output.getTimer2() == 0)
            {
                relays.manualClose2 = true;
                Serial.println("Relay 2 will remain open until it is manually closed !");
            }
            else
            {
                relays.manualClose2 = false;
                logOutput((String) "Relay 2 will automatically close in " + output.getTimer2() + " seconds !");
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

void updateUser()
{
    if (user.getUsername().length() > 0 && user.getUserPassword().length() > 0)
        user.user_flag = true;
    else
        user.user_flag = false;
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

    doc["network_settings"]["connection"] = "Ethernet";
    doc["network_settings"]["ip_type"] = "Static";
    doc["network_settings"]["ssid"] = "";
    doc["network_settings"]["password"] = "";

    // doc["network_settings"]["connection"] = "WiFi";
    // doc["network_settings"]["ip_type"] = "DHCP";
    // doc["network_settings"]["ssid"] = "Jorj-2.4";
    // doc["network_settings"]["password"] = "cafea.amara";

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