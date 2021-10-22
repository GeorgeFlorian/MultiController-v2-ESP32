#pragma once
#include <state.h>

void WiFiEvent(WiFiEvent_t);

void ethConnection();

void wifiConnection();

// Connect to a network or enter AP_MODE
void startConnection();