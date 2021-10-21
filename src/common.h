#pragma once

#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <logs.h>

#define RELAY1 32
#define RELAY2 33

#define W0 14
#define W1 13
#define INPUT_1 15
#define INPUT_2 17

#define BUTTON 34
#define BUZZER 16

extern bool ap_mode;

void waitDelay(unsigned long, unsigned long, unsigned long);

void restartSequence(unsigned int);

void listAllFiles();
