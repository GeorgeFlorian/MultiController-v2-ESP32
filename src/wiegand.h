#pragma once
#include <state.h>

class Wiegand
{
private:
    // Wiegand ID with parity bits
    std::vector<bool> wiegandOF;
    // Wiegand ID without parity bits
    std::vector<bool> wiegand26b;
    // Facility Code in binary
    std::vector<bool> facilityCode;
    // Card Number in binary
    std::vector<bool> cardNumber;
    String facility_code;
    String card_number;

    // calculate Facility Code in binary
    void fCodeToBinary(uint8_t dec);
    // Calculate Card Number in binary
    void cNumberToBinary(uint16_t dec);
    // Calculates two wiegand arrays
    // 'wiegandOF' with parity bits and 'wiegand26b' without them
    void calculateBothWiegandIDs();
    long convertToDec(String n);

public:
    void createWiegand(String fCode, String cNumber);
    // returns a 26 length Wiegand Binary with parity bits
    std::vector<bool> getWiegandBinary();
    // returns a 26 length Wiegand Binary with parity bits in String format
    String getWiegandBinaryInString();
    // returns Wiegand without parity bits
    String getWiegand26b();
    // returns wiegand id in Open Format
    String getWiegandOF();

    std::vector<bool> getFacilityCode_vector();
    std::vector<bool> getCardNumber_vector();
    String getFacilityCode_string();
    String getCardNumber_string();
    Wiegand();
};

void wiegandRoutine();