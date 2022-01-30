#include <input_output.h>

String p1_old = "";
String p2_old = "";
bool was_pressed1 = false;
bool was_pressed2 = false;
bool udp_on1 = false;
bool udp_on2 = false;

unsigned int isPressedCount1 = 0;
unsigned int isPressedCount2 = 0;
unsigned int isNotPressedCount1 = 0;
unsigned int isNotPressedCount2 = 0;
unsigned int debounceError1 = 0;
unsigned int debounceError2 = 0;

WiFiUDP udp1;
WiFiUDP udp2;

// Inductive Loop Routine
void inputRoutine()
{
    input_1.loop();
    input_2.loop();

    // Initialise or re-initialise UDP 1 when PORT changes
    if (p1_old != input.port_1 && input.port_1 != "Not Set")
    {
        p1_old = input.port_1;
        Serial.println("New INPUT1 Port: " + p1_old);
        udp1.stop();
        delay(100);
        udp1.begin(input.port_1.toInt());
        // only for first initialization
        udp_on1 = true;
    }

    // Input 1
    if (input_1.isPressed())
    {
        Serial.print("Input 1 was triggered: ");
        Serial.println(++isPressedCount1);
        was_pressed1 = true;
    }

    if (input_1.isReleased())
    {
        Serial.print("Input 1 was released: ");
        Serial.println(++isNotPressedCount1);
    }

    debounceError1 = isPressedCount1 - isNotPressedCount1;

    if (abs(debounceError1) == 1 && was_pressed1)
    {
        logOutput("Trigger1 received.");
        was_pressed1 = false;
        uint8_t buffer[19] = "statechange,201,1\r";
        // send packet to server
        if (input.ip_1.length() != 0 && input.ip_1 != "Not Set" && udp_on1)
        {
            udp1.beginPacket(input.ip_1.c_str(), input.port_1.toInt());
            udp1.write(buffer, sizeof(buffer));
            // delay(30);
            logOutput(udp1.endPacket() ? "UDP Packet 1 sent" : "WARNING: UDP Packet 1 not sent.");
            memset(buffer, 0, 19);
        }
        else
        {
            logOutput("ERROR ! Invalid IP Address for INPUT 1. Please enter Metrici Server's IP !");
        }
    }
    else if (abs(debounceError1) > 1 && was_pressed1)
    {
        Serial.println("Debounce error. Resseting debounce counters for Input 1.");
        isNotPressedCount1 = 0;
        isPressedCount1 = 1;
        was_pressed1 = false;
    }

    // Initialise or re-initialise UDP 2 when PORT changes
    if (p2_old != input.port_2 && input.port_2 != "Not Set")
    {
        p2_old = input.port_2;
        Serial.println("New INPUT2 Port: " + p2_old);
        udp2.stop();
        delay(100);
        udp2.begin(input.port_2.toInt());
        // only for first initialization
        udp_on2 = true;
    }

    // Input 2
    if (input_2.isPressed())
    {
        Serial.print("Input 2 was triggered: ");
        Serial.println(++isPressedCount2);
        was_pressed2 = true;
    }

    if (input_2.isReleased())
    {
        Serial.print("Input 2 was released ");
        Serial.println(++isNotPressedCount2);
    }

    debounceError2 = isPressedCount2 - isNotPressedCount2;

    if (abs(debounceError2) == 1 && was_pressed2)
    {
        logOutput("Trigger2 received.");
        was_pressed2 = false;
        uint8_t buffer[19] = "statechange,201,1\r";
        // send packet to server
        if (input.ip_2.length() != 0 && input.ip_2 != "Not Set" && udp_on2)
        {
            udp2.beginPacket(input.ip_2.c_str(), input.port_2.toInt());
            udp2.write(buffer, sizeof(buffer));
            // delay(30);
            logOutput(udp2.endPacket() ? "UDP Packet 2 sent" : "WARNING: UDP Packet 2 not sent.");
            memset(buffer, 0, 19);
        }
        else
        {
            logOutput("ERROR ! Invalid IP Address for INPUT 2. Please enter Metrici Server's IP !");
        }
    }
    else if (abs(debounceError2) > 1 && was_pressed2)
    {
        Serial.println("Debounce error. Resseting debounce counters for Input 2.");
        isNotPressedCount2 = 0;
        isPressedCount2 = 1;
        was_pressed2 = false;
    }
}

void outputRoutine()
{
    // Outputs Routine
    relays.deltaTimer1 = millis();
    relays.deltaTimer2 = millis();

    if (relays.startTimer1 != 0 && !relays.manualClose1)
    {
        if (abs(relays.deltaTimer1 - relays.startTimer1) >= (output.getTimer1() * 1000))
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
        if (abs(relays.deltaTimer2 - relays.startTimer2) > (output.getTimer2() * 1000))
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