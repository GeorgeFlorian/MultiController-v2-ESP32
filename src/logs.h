#pragma once
#include <Arduino.h>

extern String strlog;

struct RingBuffer
{
private:
    std::vector<String> buffer;
    size_t first;
    size_t last;
    size_t sz;

public:
    RingBuffer(int);

    bool empty() const;
    bool full() const;

    void push(String);
    void print() const;
};

extern RingBuffer circle;

void logOutput(String);