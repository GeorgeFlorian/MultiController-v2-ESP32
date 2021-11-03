#include <wiegand.h>

Wiegand::Wiegand() : wiegandOF(std::vector<bool>(26, false)), wiegand26b(std::vector<bool>(24, false)), facilityCode(std::vector<bool>(8, false)), cardNumber(std::vector<bool>(16, false)), facility_code(""), card_number(""){};

// calculate Facility Code in binary
void Wiegand::fCodeToBinary(uint8_t dec)
{
    for (int8_t i = 7; i >= 0; --i)
    {
        facilityCode[i] = dec & 1;
        dec >>= 1;
    }
}
// Calculate Card Number in binary
void Wiegand::cNumberToBinary(uint16_t dec)
{
    for (int8_t i = 15; i >= 0; --i)
    {
        cardNumber[i] = dec & 1;
        dec >>= 1;
    }
}
// Calculates two wiegand arrays
// 'wiegandOF' with parity bits and 'wiegand26b' without them
void Wiegand::calculateBothWiegandIDs()
{
    wiegandOF.clear();
    wiegand26b.clear();
    // Add binary Facility Code
    wiegandOF.insert(wiegandOF.end(), facilityCode.begin(), facilityCode.end());
    // Add binary Card Number
    wiegandOF.insert(wiegandOF.end(), cardNumber.begin(), cardNumber.end());
    // Add binary Facility Code
    wiegand26b.insert(wiegand26b.end(), facilityCode.begin(), facilityCode.end());
    // Add binary Card Number
    wiegand26b.insert(wiegand26b.end(), cardNumber.begin(), cardNumber.end());

    // check for parity of the first 12 bits
    bool even = 0;
    for (int i = 0; i < 12; i++)
    {
        even ^= wiegandOF[i];
    }
    // check for parity of the last 12 bits
    bool odd = 1;
    for (int i = 12; i < 24; i++)
    {
        odd ^= wiegandOF[i];
    }
    // add 0 or 1 as first bit (leading parity bit - even) based on the number of 'ones' in the first 12 bits
    wiegandOF.insert(wiegandOF.begin(), even);
    // add 0 or 1 as last bit (trailing parity bit - odd) based on the number of 'ones' in the last 12 bits
    wiegandOF.push_back(odd);
}
long Wiegand::convertToDec(String n)
{
    // Serial.print("In convertToDec(), n= ");
    // Serial.println(n);
    String num = n;
    long dec_value = 0;

    // Initializing base value to 1, i.e 2^0
    int base = 1;

    int len = num.length();
    for (int i = len - 1; i >= 0; i--)
    {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }

    return dec_value;
}

void Wiegand::createWiegand(String fCode, String cNumber)
{
    while (fCode.length() < 3)
    {
        fCode = String(0) + fCode;
    }
    while (cNumber.length() < 5)
    {
        cNumber = String(0) + cNumber;
    }
    // decimal values
    facility_code = fCode;
    card_number = cNumber;
    fCodeToBinary(facility_code.toInt());
    cNumberToBinary(card_number.toInt());
    calculateBothWiegandIDs();
}
// returns a 26 length Wiegand Binary with parity bits
std::vector<bool> Wiegand::getWiegandBinary()
{
    return wiegandOF;
}
// returns a 26 length Wiegand Binary with parity bits in String format
String Wiegand::getWiegandBinaryInString()
{
    String binary_str = "";
    for (bool i : wiegandOF)
    {
        binary_str += (String)i;
    }
    return binary_str;
}
// returns Wiegand without parity bits
String Wiegand::getWiegand26b()
{
    String binary_str = "";
    for (bool i : wiegand26b)
    {
        binary_str += (String)i;
    }
    long dec = convertToDec(binary_str);

    return (String)dec;
}
// returns wiegand id in Open Format
String Wiegand::getWiegandOF()
{
    String binary_str = "";
    for (bool i : wiegandOF)
    {
        binary_str += (String)i;
    }
    long dec = convertToDec(binary_str);

    return (String)dec;
}

std::vector<bool> Wiegand::getFacilityCode_vector()
{
    return facilityCode;
}
std::vector<bool> Wiegand::getCardNumber_vector()
{
    return cardNumber;
}
String Wiegand::getFacilityCode_string()
{
    return facility_code;
}
String Wiegand::getCardNumber_string()
{
    return card_number;
}

Wiegand wiegand_card;

void wiegandRoutine()
{
    // Wiegand Routine
    if (wiegand_state.wiegand_flag)
    {
        if (wiegand_state.database_url == "not set")
        {
            logOutput("ERROR: Wiegand's Database URL is not set. Can't send plate number.");
            wiegand_state.wiegand_flag = false;
            exit(1);
        }
        wiegand_state.working = true;
        wiegand_state.wiegand_flag = false;
        String copyNumber = wiegand_state.plate_number;
        String wiegandID = "";
        int pulse = wiegand_state.getPulseWidth();
        int gap = wiegand_state.getPulseGap();
        HTTPClient wiegandHTTP;
        wiegandHTTP.begin((String)wiegand_state.database_url + "?plate_number=" + copyNumber);
        int httpCode = wiegandHTTP.GET();
        if (httpCode > 0)
        {
            wiegandID = wiegandHTTP.getString();
            Serial.println((String) "HTTP Code: " + httpCode);
            Serial.println((String) "Wiegand ID Payload: " + wiegandID);
        }
        else
        {
            logOutput("Error on HTTP request ! Please check if Database URL is correct.");
        }
        wiegandHTTP.end();

        if (wiegandID.substring(0, wiegandID.indexOf(',')) != "-1" && wiegandID.substring((wiegandID.indexOf(',') + 1)) != "-1")
        {
            std::vector<bool> _array;
            String facility_code = wiegandID.substring(0, wiegandID.indexOf(','));
            String card_number = wiegandID.substring((wiegandID.indexOf(',') + 1));

            wiegand_card.createWiegand(facility_code, card_number);

            _array = wiegand_card.getWiegandBinary();
            Serial.print("Sending Wiegand: ");
            for (bool i : _array)
            {
                Serial.print(i);
            }

            String wiegand26b_string = wiegand_card.getWiegandBinaryInString();
            logOutput((String) "Sending Wiegand: " + wiegand26b_string);
            Serial.print("Sending Wiegand length: ");
            Serial.println(wiegand26b_string.length());

            portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
            portENTER_CRITICAL(&mux);
            for (int i = 0; i < 26; i++)
            {
                if (_array[i] == 0)
                {
                    digitalWrite(W0, LOW);
                    delayMicroseconds(pulse);
                    digitalWrite(W0, HIGH);
                }
                else
                {
                    digitalWrite(W1, LOW);
                    delayMicroseconds(pulse);
                    digitalWrite(W1, HIGH);
                }
                delayMicroseconds(gap);
            }
            portEXIT_CRITICAL(&mux);

            Serial.println();
            logOutput((String) "Plate Number: " + copyNumber + " - Facility Code: " + wiegand_card.getFacilityCode_string() + " - Card Number: " + wiegand_card.getCardNumber_string());
            logOutput((String) "Plate Number: " + copyNumber + " - Wiegand 26b: " + wiegand_card.getWiegand26b() + " - Wiegand OF: " + wiegand_card.getWiegandOF());

            Serial.println('\n');
            // DEBUG
            // Facility Code in binary
            std::vector<bool> fCode = wiegand_card.getFacilityCode_vector();
            Serial.print("Facility Code: ");
            for (bool i : fCode)
            {
                Serial.print(i);
            }
            Serial.println('\n');
            // Card Number in binary
            std::vector<bool> cNumber = wiegand_card.getCardNumber_vector();
            Serial.print("Card Number: ");
            for (bool i : cNumber)
            {
                Serial.print(i);
            }
            Serial.println('\n');

            wiegand_state.working = false;
        }
        else
        {
            logOutput((String) "Plate Number: " + copyNumber + " does not have a Wiegand ID.");
            wiegand_state.working = false;
        }
    }
}