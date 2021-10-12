#pragma once
#include <WiFi.h>
#include <common.h>

// Live state of the controller
struct Settings
{
public:
    String connection;
    String ip_type;
    String ssid;
    String password;
    String ip_address;
    String gateway;
    String subnet;
    String dns;
    Settings();
};
extern Settings network_settings;

struct Input
{
public:
    String ip_1;
    String port_1;
    String ip_2;
    String port_2;
    Input();
};
extern Input input;

class Output
{
private:
    String timer1;
    String timer2;

public:
    void setTimer1(String x);
    void setTimer2(String x);

    String getTimer1();
    String getTimer2();
    Output();
};
extern Output output;

struct Relay
{
public:
    String state1;
    String state2;
    bool manualClose1;
    bool manualClose2;
    long startTimer1;
    long startTimer2;
    long deltaTimer1;
    long deltaTimer2;
    Relay();
};
extern Relay relays;

class WiegandState
{
private:
    String pulse_width;
    String pulse_gap;

public:
    String database_url;
    void setPulseWidth(String x);
    void setPulseGap(String x);
    String getPulseWidth();
    String getPulseGap();
    WiegandState();
};
extern WiegandState wiegand_state;

struct RFID
{
public:
    String ip_rfid;
    String port_rfid;
    RFID();
};
extern RFID rfid;

class User
{
private:
    String username;
    String password;

public:
    void setUsername(String x);
    String getUsername();

    void setUserPassword(String x);
    String getUserPassword();

    bool user_flag;

    User();
};
extern User user;

void updateLiveState(StaticJsonDocument<1024> &doc);

void updateRelay(StaticJsonDocument<384> json);

void updateUser();

// Get settings from /config.json
StaticJsonDocument<1024> fileToJson();

// Save settings in /config.json
void saveSettings(StaticJsonDocument<384>, String);

/*
Write settings to /config.json
Function used when resetting settings
*/
bool JSONtoSettings(StaticJsonDocument<1024>);

StaticJsonDocument<1024> softReset();

StaticJsonDocument<1024> factoryReset();