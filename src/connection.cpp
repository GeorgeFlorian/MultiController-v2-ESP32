#include <connection.h>

static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_ETH_START:
        Serial.println(F("ETH Started"));
        //set eth hostname here
        if (!ETH.setHostname("Metrici-MultiController-Eth"))
        {
            Serial.println(F("Ethernet hostname failed to configure"));
        }
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Serial.println(F("ETH Connected"));
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        eth_connected = true;
        Serial.print(F("ETH MAC: "));
        Serial.print(ETH.macAddress());
        Serial.print(F(", IPv4: "));
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(F(", FULL_DUPLEX"));
        }
        Serial.print(F(", "));
        Serial.print(ETH.linkSpeed());
        Serial.println(F("Mbps"));
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Serial.println(F("ETH Disconnected"));
        eth_connected = false;
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Serial.println(F("ETH Stopped"));
        eth_connected = false;
        break;
    default:
        break;
    }
}

void eth_connection()
{
    ETH.begin();

    IPAddress local_sta(192, 168, 100, 10);
    IPAddress gateway_sta(192, 168, 100, 1);
    IPAddress subnet_sta(255, 255, 255, 0);
    IPAddress primary_dns(8, 8, 8, 8);

    if (!ETH.config(local_sta, gateway_sta, subnet_sta, primary_dns))
    {
        Serial.println(F("Couldn't configure STATIC IP ! Obtaining DHCP IP !"));
    }

    int k = 0;
    while (!eth_connected && k < 20)
    {
        Serial.println((String) "[" + (k + 1) + "] - Establishing ETHERNET Connection ... ");
        delay(1000);
        k++;
    }

    if (!eth_connected)
    {
        Serial.println(F("(1) Could not connect to network ! Trying again..."));
        Serial.println(F("Controller will restart in 5 seconds !"));
        delay(5000);
        ESP.restart();
    }
}

void wifi_connection()
{
}

void start_connection()
{
    WiFi.onEvent(WiFiEvent);
    Serial.print("SSID: ");
    Serial.println(network_settings.ssid);
    Serial.print("Password: ");
    Serial.println(network_settings.password);

    WiFi.begin(network_settings.ssid.c_str(), network_settings.password.c_str());

    int k = 0;
    while (WiFi.status() != WL_CONNECTED && k < 20)
    {
        k++;
        delay(1000);
    }

    if (WiFi.status() != WL_CONNECTED)
        ESP.restart();

    Serial.print("WiFi.getMode() ");
    Serial.println(WiFi.getMode());
}