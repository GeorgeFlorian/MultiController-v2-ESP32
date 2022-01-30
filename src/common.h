#pragma once

#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <logs.h>
#include <ezButton.h>

#define RELAY1 32
#define RELAY2 33

#define W0 14
#define W1 13
#define INPUT_1 15
#define INPUT_2 17

#define RST_BTN 34
#define BUZZER 16

#define DEBOUNCE_TIME 20

extern bool changed_network_config;
extern bool restart_flag;

extern ezButton input_1;
extern ezButton input_2;

void restartSequence(unsigned int);

void listAllFiles();

void setPins();