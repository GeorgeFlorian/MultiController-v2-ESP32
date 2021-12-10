#include <connection.h>

static bool eth_connected = false;

IPAddress ip_address;
IPAddress gateway;
IPAddress subnet;
IPAddress dns;

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
        Serial.print(F("ETH MAC - "));
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

void wifiConnection()
{
    if (network_settings.ip_type == "Static")
    {
        ip_address.fromString(network_settings.ip_address);
        gateway.fromString(network_settings.gateway);
        subnet.fromString(network_settings.subnet);
        dns.fromString(network_settings.dns);

        if (!WiFi.config(ip_address, gateway, subnet, dns))
        {
            logOutput("WARNING: Couldn't configure STATIC IP ! Obtaining DHCP IP !");
        }
    }

    WiFi.begin(network_settings.ssid.c_str(), network_settings.password.c_str());
    WiFi.setHostname("Metrici-MultiController-WiFi");

    int k = 0;
    while (WiFi.status() != WL_CONNECTED && k < 20)
    {
        k++;
        delay(1000);
        Serial.println((String) "[" + k + "] - Establishing WiFi Connection ... ");
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        logOutput("(1) Could not access Wireless Network ! Trying again...");
        logOutput("Controller will restart in 5 seconds !");
        restartSequence(5);
    }

    network_settings.ip_address = WiFi.localIP().toString();
    network_settings.gateway = WiFi.gatewayIP().toString();
    network_settings.subnet = WiFi.subnetMask().toString();
    network_settings.dns = WiFi.dnsIP().toString();
    // logOutput((String) "Hostname: " + WiFi.getHostname());
    logOutput((String) "IP address: " + WiFi.localIP().toString());
    logOutput((String) "Gateway: " + WiFi.gatewayIP().toString());
    logOutput((String) "Subnet: " + WiFi.subnetMask().toString());
    logOutput((String) "DNS: " + WiFi.dnsIP().toString());
}

void ethConnection()
{
    // uint8_t mac[] = {};
    // esp_wifi_get_mac(WIFI_IF_STA, &mac[0]);
    // esp_eth_set_mac(&mac[0]);
    // Serial.print("ETH MAC: ");
    // Serial.println(ETH.macAddress());

    ETH.begin();

    if (network_settings.ip_type == "Static")
    {
        ip_address.fromString(network_settings.ip_address);
        gateway.fromString(network_settings.gateway);
        subnet.fromString(network_settings.subnet);
        dns.fromString(network_settings.dns);

        if (!ETH.config(ip_address, gateway, subnet, dns))
        {
            logOutput("WARNING: Couldn't configure STATIC IP ! Obtaining DHCP IP !");
        }
    }

    int k = 0;
    while (!eth_connected && k < 20)
    {
        k++;
        delay(1000);
        Serial.println((String) "[" + k + "] - Establishing ETHERNET Connection ... ");
    }

    if (!eth_connected)
    {
        logOutput("(1) Could not connect to network ! Trying again...");
        logOutput("Controller will restart in 5 seconds !");
        restartSequence(5);
    }

    network_settings.ip_address = ETH.localIP().toString();
    network_settings.gateway = ETH.gatewayIP().toString();
    network_settings.subnet = ETH.subnetMask().toString();
    network_settings.dns = ETH.dnsIP().toString();
    // logOutput((String) "Hostname: " + ETH.getHostname());
    logOutput((String) "IP address: " + ETH.localIP().toString());
    logOutput((String) "Gateway: " + ETH.gatewayIP().toString());
    logOutput((String) "Subnet: " + ETH.subnetMask().toString());
    logOutput((String) "DNS: " + ETH.dnsIP().toString());
}

void startConnection()
{
    if (!WiFi.mode(WIFI_STA))
    {
        logOutput("ERROR: Controller couldn't go in STA_MODE. Restarting in 5 seconds.");
        restartSequence(5);
        return;
    }

    Serial.println((String) "WiFi.getMode() [1 = STA / 2 = AP] : " + WiFi.getMode());

    WiFi.onEvent(WiFiEvent);

    if (network_settings.connection == "WiFi")
        wifiConnection();
    else if (network_settings.connection == "Ethernet")
        ethConnection();

    // debug purpose - checking live state
    // Serial.println(network_settings.ip_address);
    // Serial.println(network_settings.gateway);
    // Serial.println(network_settings.subnet);
    // Serial.println(network_settings.dns);

    changed_network_config = false;
}