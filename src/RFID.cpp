#include <RFID.h>

String rf_id = "";
String p_old = "";
bool udp_on = false;
unsigned long start_buzzer = 0;

WiFiUDP udp;

int StrToHex(char str[])
{
    return (int)strtol(str, 0, 16);
}

// RFID routine
void rfidRoutine()
{
    if (rfid.activate_rfid)
    {
        // send command to reader
        if (rfid.reader_command)
        {
            // Serial1.print("i\r");
            Serial1.print("mt2000\r"); // Set read interval for the same read action
            delay(200);
            Serial1.print("mku\r"); // Unlocks read command and resets keys to ka=000000000000 and kb=FFFFFFFFFFFF;
            delay(200);
            Serial1.print("e0\r"); // Disable eeprom read; Stop reading Blocks
            delay(200);
            rfid.reader_command = false;

            while (Serial1.available())
            {
                String str = Serial1.readStringUntil((char)'>');
                Serial.println(str);
            }
            while (Serial1.available())
                Serial1.read();
        }

        // Initialise RFID UDP 0
        if (p_old != rfid.port_rfid && rfid.port_rfid != "not set")
        {
            p_old = rfid.port_rfid;
            Serial.println("New RFID Port: " + p_old);
            udp.stop();
            udp.begin(rfid.port_rfid.toInt());
            // only for first initialization
            udp_on = true;
        }

        if (Serial1.available() > 13)
        {
            Serial.println("Reading RFID Card...");
            char input[9];

            // String str = Serial1.readStringUntil((char)'\r');
            String str = Serial1.readStringUntil((char)'>');
            // Serial.println(str);

            rf_id = str.substring(3, 11);
            // Serial.println((String)"RFID HEX: '" + rf_id + "' - size: " + rf_id.length());
            rf_id.toCharArray(input, 9);

            // Serial.println(input);
            int val = StrToHex(input);
            // Serial.println(val);

            rf_id = String(val);
            while (rf_id.length() < 10)
            {
                rf_id = "0" + rf_id;
            }
            // Serial.println((String)"RFID DEC: '" + rf_id + "' - size: " + rf_id.length());

            while (Serial1.available())
            {
                Serial1.read();
            }
        }

        if (rf_id.length() == 10 && rf_id != "0000000000")
        {
            uint8_t buffer[40];
            String foobar = rf_id;
            rf_id = "Metrici multicontroller, key=" + rf_id;
            // Serial.println(rf_id);
            rf_id.toCharArray((char *)buffer, 40);
            // Serial.println((char *)buffer);

            // send packet to server
            if (rfid.ip_rfid.length() != 0 && rfid.ip_rfid != "not set" && udp_on)
            {
                udp.beginPacket(rfid.ip_rfid.c_str(), rfid.port_rfid.toInt());
                udp.write(buffer, sizeof(buffer));
                delay(30);
                logOutput(udp.endPacket() ? (String) "RFID: " + foobar + " was sent over UDP." : "WARNING: RFID not sent.");
                memset(buffer, 0, 40);
                digitalWrite(BUZZER, HIGH);
                start_buzzer = millis();
                // delay(100);
            }
            else
            {
                logOutput("ERROR ! Invalid IP Address for RFID. Please enter Metrici Server's IP !");
            }
        }
    }
    // stop buzzer
    if (digitalRead(BUZZER) == HIGH && (millis() - start_buzzer) > 100)
    {
        Serial.print("Buzzer Interval: ");
        Serial.println(millis() - start_buzzer);
        digitalWrite(BUZZER, LOW);
    }
}
