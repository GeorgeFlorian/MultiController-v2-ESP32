#pragma once
#include <state.h>

class Wiegand
{
private:
    std::vector<bool> wiegandArray;
    std::vector<bool> facilityCode;
    std::vector<bool> cardNumber;
    String s1;

    void calculateFacilityCode(uint8_t dec);
    void calculateCardNumber(uint16_t dec);
    void calculateWiegand();

public:
    void update(String fCode, String cNumber);
    //returns a 26 length std::vector<bool>
    std::vector<bool> getWiegandBinary();
    // returns an 8 characters long String
    String getCardID();

    std::vector<bool> getFacilityCode_vector();
    std::vector<bool> getCardNumber_vector();
    uint8_t getFacilityCode_int();
    uint16_t getCardNumber_int();
    Wiegand();
} ;

void wiegandRoutine();