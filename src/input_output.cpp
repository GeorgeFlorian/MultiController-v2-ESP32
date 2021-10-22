#include <input_output.h>

String p1_old = "";
String p2_old = "";
bool was_pressed1 = false;
bool was_pressed2 = false;
unsigned long debounce_input1 = 0;
unsigned long debounce_input2 = 0;
unsigned long delta_timer = 0;
bool count_1 = false;
bool count_2 = false;
bool udp_on1 = false;
bool udp_on2 = false;

WiFiUDP udp1;
WiFiUDP udp2;

// Inductive Loop Routine
void inputRoutine()
{
    delta_timer = millis();
    // Initialise or re-initialise UDP 1
    if (p1_old != input.port_1 && input.port_1 != "not set" && input.port_1.length() != 0)
    {
        p1_old = input.port_1;
        Serial.println("New Input 1 Port: " + p1_old);
        udp1.stop();
        // delay(100);
        udp1.begin(input.port_1.toInt());
        // only for first initialization
        udp_on1 = true;
    }
    // check for bounce-back
    if (digitalRead(INPUT_1) == LOW && count_1 == false)
    {
        count_1 = true;
        debounce_input1 = millis();
    }
    // check if INPUT_1 is still LOW after 50 ms
    if ((delta_timer - debounce_input1) > 50)
    {
        count_1 = false;
        //Send UDP packet if trigger was sent from Input1 only once
        if (digitalRead(INPUT_1) == LOW && was_pressed1 == false)
        {
            was_pressed1 = true;
            logOutput("Trigger1 received.");
            uint8_t buffer[19] = "statechange,201,1\r";
            // send packet to server
            if (input.ip_1.length() != 0 && input.ip_1 != "not set" && udp_on1)
            {
                udp1.beginPacket(input.ip_1.c_str(), input.port_1.toInt());
                udp1.write(buffer, sizeof(buffer));
                delay(30);
                logOutput(udp1.endPacket() ? "UDP Packet 1 sent" : "WARNING: UDP Packet 1 not sent.");
                memset(buffer, 0, 19);
            }
            else
            {
                logOutput("ERROR ! Invalid IP Address for INPUT 1. Please enter Metrici Server's IP !");
            }
        }
    }

    if (digitalRead(INPUT_1) == HIGH && was_pressed1 == true)
    {
        was_pressed1 = false;
    }

    // Initialise or re-initialise UDP 2
    if (p2_old != input.port_2 && input.port_2 != "not set" && input.port_2.length() != 0)
    {
        p2_old = input.port_2;
        Serial.println("New Input 2 Port: " + p2_old);
        udp2.stop();
        // delay(100);
        udp2.begin(input.port_2.toInt());
        // only for first initialization
        udp_on2 = true;
    }

    // check for bounce-back
    if (digitalRead(INPUT_2) == LOW && count_2 == false)
    {
        count_2 = true;
        debounce_input2 = millis();
    }
    // check if INPUT_2 is still LOW after 50 ms
    if ((delta_timer - debounce_input2) > 50)
    {
        count_2 = false;
        //Send UDP packet if trigger was sent from Input2 only once
        if (digitalRead(INPUT_2) == LOW && was_pressed2 == false)
        {
            was_pressed2 = true;
            logOutput("Trigger2 received.");
            uint8_t buffer[19] = "statechange,201,1\r";
            // send packet to server
            if (input.ip_2.length() != 0 && input.ip_2 != "not set" && udp_on2)
            {
                udp2.beginPacket(input.ip_2.c_str(), input.port_2.toInt());
                udp2.write(buffer, sizeof(buffer));
                delay(30);
                logOutput(udp2.endPacket() ? "UDP Packet 2 sent" : "WARNING: UDP Packet 2 not sent.");
                memset(buffer, 0, 19);
            }
            else
            {
                logOutput("ERROR ! Invalid IP Address for INPUT 2. Please enter Metrici Server's IP !");
            }
        }
    }
    if (digitalRead(INPUT_2) == HIGH && was_pressed2 == true)
    {
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