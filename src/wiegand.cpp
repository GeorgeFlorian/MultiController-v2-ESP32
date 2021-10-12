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