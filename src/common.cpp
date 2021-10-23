#include <common.h>

bool changed_network_config = false;
bool restart_flag = false;

void waitDelay(unsigned long current, unsigned long previous, unsigned long interval)
{
    if (current - previous > interval)
    {
        previous += interval;
    }
}

void restartSequence(unsigned int countdown)
{
    for (int i = countdown; i > 0; i--)
    {
        delay(999);
    }
    ESP.restart();
}

void listAllFiles()
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file)
    {
        Serial.print("FILE: ");
        String fileName = file.name();
        Serial.println(fileName);
        file = root.openNextFile();
    }
    file.close();
    root.close();
}