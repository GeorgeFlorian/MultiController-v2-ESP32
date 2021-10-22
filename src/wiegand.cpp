#include <wiegand.h>

Wiegand::Wiegand() : wiegandArray(std::vector<bool>(26, false)), facilityCode(std::vector<bool>(8, false)), cardNumber(std::vector<bool>(16, false)), s1(""){};

void Wiegand::calculateFacilityCode(uint8_t dec)
{
    for (int8_t i = 7; i >= 0; --i)
    {
        facilityCode[i] = dec & 1;
        dec >>= 1;
    }
}
void Wiegand::calculateCardNumber(uint16_t dec)
{
    for (int8_t i = 15; i >= 0; --i)
    {
        cardNumber[i] = dec & 1;
        dec >>= 1;
    }
}
void Wiegand::calculateWiegand()
{
    wiegandArray.clear();
    wiegandArray.reserve(facilityCode.size() + cardNumber.size());
    wiegandArray.insert(wiegandArray.end(), facilityCode.begin(), facilityCode.end());
    wiegandArray.insert(wiegandArray.end(), cardNumber.begin(), cardNumber.end());
    // check for parity of the first 12 bits
    bool even = 0;
    for (int i = 0; i < 12; i++)
    {
        even ^= wiegandArray[i];
    }
    // check for parity of the last 12 bits
    bool odd = 1;
    for (int i = 12; i < 24; i++)
    {
        odd ^= wiegandArray[i];
    }
    // add 0 or 1 as first bit (leading parity bit - even) based on the number of 'ones' in the first 12 bits
    wiegandArray.insert(wiegandArray.begin(), even);
    // add 0 or 1 as last bit (trailing parity bit - odd) based on the number of 'ones' in the last 12 bits
    wiegandArray.push_back(odd);
}

void Wiegand::update(String fCode, String cNumber)
{
    while (fCode.length() < 3)
    {
        fCode = String(0) + fCode;
    }
    while (cNumber.length() < 5)
    {
        cNumber = String(0) + cNumber;
    }
    s1 = fCode + cNumber;
    calculateFacilityCode(fCode.toInt());
    calculateCardNumber(cNumber.toInt());
    calculateWiegand();
}
//returns a 26 length std::vector<bool>
std::vector<bool> Wiegand::getWiegandBinary()
{
    return wiegandArray;
}
// returns an 8 characters long String
String Wiegand::getCardID()
{
    return s1;
}

std::vector<bool> Wiegand::getFacilityCode_vector()
{
    return facilityCode;
}
std::vector<bool> Wiegand::getCardNumber_vector()
{
    return cardNumber;
}
uint8_t Wiegand::getFacilityCode_int()
{
    return s1.substring(0, 3).toInt();
}
uint16_t Wiegand::getCardNumber_int()
{
    return s1.substring(3, 8).toInt();
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
            wiegand_card.update(wiegandID.substring(0, wiegandID.indexOf(',')), wiegandID.substring((wiegandID.indexOf(',') + 1)));
            _array = wiegand_card.getWiegandBinary();
            Serial.print("Sending Wiegand: ");
            for (bool i : _array)
            {
                Serial.print(i);
            }
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
            logOutput((String) "Plate Number: " + copyNumber + " - Wiegand ID: " + wiegand_card.getCardID());

            Serial.println('\n');
            // DEBUG
            // Serial.println((String)"Facility Code: " + card.getFacilityCode_int());
            // std::vector<bool> fCode = card.getFacilityCode_vector();
            // Serial.print("Facility Code: ");
            // for(bool i : fCode) {
            //   Serial.print(i);
            // }
            // Serial.println('\n');

            // Serial.println((String)"Card Number: " + card.getCardNumber_int());
            // std::vector<bool> cNumber = card.getCardNumber_vector();
            // Serial.print("Card Number: ");
            // for(bool i : cNumber) {
            //   Serial.print(i);
            // }
            // Serial.println('\n');
            wiegand_state.working = false;
        }
        else
        {
            logOutput((String) "Plate Number: " + copyNumber + " does not have a Wiegand ID.");
            wiegand_state.working = false;
        }
    }
}