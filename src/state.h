#pragma once
#include <WiFi.h>
#include <ETH.h>
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

    int getTimer1();
    int getTimer2();
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
    unsigned long startTimer1;
    unsigned long startTimer2;
    unsigned long deltaTimer1;
    unsigned long deltaTimer2;
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
    bool wiegand_flag;
    bool working;
    String plate_number;
    void setPulseWidth(String x);
    void setPulseGap(String x);
    int getPulseWidth();
    int getPulseGap();
    WiegandState();
};
extern WiegandState wiegand_state;

struct RFID
{
public:
    String ip_rfid;
    String port_rfid;
    bool activate_rfid;
    bool reader_command;
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

    bool userFlag();

    User();
};
extern User user;

// Gets settings from live state and returns a json document to be sent to /api/settings/get
StaticJsonDocument<1024> getLiveState();

// Updates live state
void updateLiveState(StaticJsonDocument<1024> &doc);

// Updates relays state
void updateRelay(StaticJsonDocument<384> json);

// Reads settings from /config.json and updates live state
StaticJsonDocument<1024> readSettings();

// Saves settings to /config.json
void saveSettings(StaticJsonDocument<384>, String);

// Writes passed json doc to /config.json after a reset or an update and updates live state
bool JSONtoSettings(StaticJsonDocument<1024>);

// Resets settings but keeps Network Configuration
StaticJsonDocument<1024> softReset();

// Resets all settings
StaticJsonDocument<1024> factoryReset();