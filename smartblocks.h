#ifndef Smartblocks_h
#define Smartblocks_h

#include <WiFi.h>
#include <WiFiClientSecure.h>

// Everything above this line should not be edited.

// ---------- ---[]--- [ Parameters ] ---[]--- ----------
// For a list of parameters go and visit https://wiki.smartblocks.dev/
#define SB_UserAgent "SmartBlocks/1.0"
//#define use_ssl
#define debug
// ---------- ---[]--- [ Parameters ] ---[]--- ----------

// Everything below this line should not be edited.

class Smartblocks {
public:

    /**
     * Instatiate a Smartblock with default parameters
     *
     * The Block will try to connect to a network with an SSID of "Smartblocks".
     * It'll try to use the password "smartblocks". The webinterface should be at smartblocks.dev:443
     */
    Smartblocks();

    /**
     * Instatiate a Smartblock
     * @param ssid SSID of the desired Access-Point
     * @param password The accesskey for this AP
     * @param serverip The ip of the webinterface
     * @param port The port for the server
     */
    Smartblocks(const char *ssid, const char *password, const char *serverip, int port);

    /**
     * Write a value to the specified key
     * @param key The key
     * @param value The value
     * @return Returns the response from the server
     */
    int write(const String& key, const String& value);

//    /**
//     * WRite a integer value to the specified key
//     * @param key  The key
//     * @param value  The value
//     * @return Returns the reponse from the server
//     */
//    int write(String key, int value);

    /**
     * Reads from the specified key
     * @param key The key
     * @return Returns the response from the server
     */
    String read(const String& key);

private: // Private stuff
    const char *_ssid;
    const char *_password;
    const char *_serverip;
    int _port;
};


#ifdef use_ssl
WiFiClientSecure client;
#else
WiFiClient client;
#endif


Smartblocks::Smartblocks() {
    if (Serial) {
#ifdef use_ssl
        Serial.println("[Smartblocks] Using secure WifiClient");
        _port = 443;
#else
        Serial.println("[Smartblocks] Using standard WifiClient");
        _port = 80;
#endif
    }

    _ssid = "Smartblocks";
    _password = "smartblocks";
    _serverip = "smartblocks.dev";


    WiFi.mode(WIFI_STA);

    if (Serial) Serial.println("[Smartblocks] Trying to connect to \"Smartblocks\" with password \"smartblocks\"!\n");

    WiFi.begin(_ssid, _password);

    if (Serial) Serial.print("[Smartblocks] Waiting for connection...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (Serial) Serial.print(".");
    }
    if (Serial) {
        Serial.println("");
        Serial.print("[Smartblocks] IP Address: ");
        Serial.println(WiFi.localIP());
    }

    if (Serial) Serial.println("[Smartblocks] Connected!");
    WiFi.setSleep(false);
}

Smartblocks::Smartblocks(const char *ssid, const char *password, const char *serverip, int port) {
    if (Serial) {
#ifdef use_ssl
        Serial.println("[Smartblocks] Using SSL");
#else
        Serial.println("[Smartblocks] Not using SSL");
#endif
    }
    if (strlen(ssid) > 0) {
        _ssid = ssid;
    } else {
        _ssid = "Smartblocks";
    }

    if (strlen(password) > 0) {
        _password = password;
    } else {
        _password = "smartblocks";
    }

    _serverip = serverip;

    if (port == 0) _port = 80;
    else _port = port;

    WiFi.mode(WIFI_STA);

    if (strlen(password) > 0) {
        if (Serial) {
            Serial.print("[Smartblocks] Trying to connect to ");
            Serial.print(ssid);
            Serial.print(" with password ");
            Serial.print(password);
            Serial.print("!\n");
        }
        WiFi.begin(_ssid, _password);
    } else {
        if (Serial) {
            Serial.print("[Smartblocks] Trying to connect to ");
            Serial.print(ssid);
            Serial.print(" without a password!\n");
        }
        WiFi.begin(_ssid);
    }

    if (Serial) Serial.print("[Smartblocks] Waiting for connection");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (Serial) Serial.print(".");
    }
    if (Serial) {
        Serial.println("");

        Serial.print("[Smartblocks] IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.println("[Smartblocks] Connected!");
    }

    WiFi.setSleep(false);
}

int Smartblocks::write(const String& key, const String& value) {
    int responsecode = -1;

    if (!client.connect(_serverip, _port)) {
        return -1;
    } else {

        unsigned long start = millis();

        String head = "";

        head = "GET /api/update/" + WiFi.macAddress() + "/entry/" + key + "/" + value + " HTTP/1.1\n" + "Host: " +
               _serverip + "\n" + "User-Agent: " + SB_UserAgent + "\n" + "x-smartblock-id:" + WiFi.macAddress() + "\n" +
               "Connection: close\n" + "\r\n\r\n";

        client.print(head);

#ifdef debug
        Serial.print("Writing took: ");
        Serial.print(millis() - start);
        Serial.println("ms");
#endif

        start = millis();
        int contentlength = 0;


        while (client.connected()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
                break;
            } else if (line.startsWith("Content-Length: ")) {
                line.replace("Content-Length: ", "");
                contentlength = line.toInt();
            } else if (line.startsWith("HTTP/1.1 ")) {
                line.replace("HTTP/1.1 ", "");
                responsecode = line.substring(0, line.indexOf(' ')).toInt();
            }
        }
        int lenge = 0;
        while (client.connected() && client.available()) {
            lenge++;
            if (lenge >= contentlength) break;

        }
        client.stop();

#ifdef debug
        Serial.print("Receiving took: ");
        Serial.print(millis() - start);
        Serial.println("ms");
#endif

    }
    return responsecode;
}

/*int Smartblocks::write(String key, int value) {
    int responsecode = -1;

    if (!client.connect(_serverip, _port)) // Starte Verbindung
    {
        return 0;
    } else {
        unsigned long start = millis();

        String headstr =
                "GET /api/update/" + WiFi.macAddress() + "/entry/" + key + "/" + value + " HTTP/1.1\n" + "Host: " +
                _serverip + "\n" + "User-Agent: " + SB_UserAgent + "\n" + "x-smartblock-id:" + WiFi.macAddress() +
                "\n" + "Connection: close\n" + "\r\n\r\n";
        client.print(headstr);

#ifdef debug
        Serial.print("Send took: ");
        Serial.print(millis() - start);
        Serial.println("ms");

        start = millis();
#endif

        unsigned long head = 0;

        int contentlength = 0;

        while (client.connected()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
                break;
            } else if (line.startsWith("Content-Length: ")) {
                line.replace("Content-Length: ", "");
                contentlength = line.toInt();
                if (contentlength < 1) break;
            } else if (line.startsWith("HTTP/1.1 ")) {
                line.replace("HTTP/1.1 ", "");
                responsecode = line.substring(0, line.indexOf(' ')).toInt();
            }
        }
#ifdef debug
        head = millis() - start;
        start = millis();
#endif
        int lenge = 0;
        while (client.connected() && client.available()) {
            lenge++;
            if (lenge >= contentlength) break;

        }
        client.stop();
#ifdef debug
        Serial.print("Receive took: [Head]: ");
        Serial.print(head);
        Serial.print("ms - [Body]: ");
        Serial.print(millis() - start);
        Serial.print("ms @ ");
        Serial.print(contentlength);
        Serial.print(" bytes\n");
#endif
    }
    return responsecode;
}*/

String Smartblocks::read(const String& key) {

    String response = "";
    if (!client.connect(_serverip, _port)) // Starte Verbindung
    {
        return "ERR";
    } else {

        String head =
                "GET /api/get/" + WiFi.macAddress() + "/entry/" + key + "/" + " HTTP/1.1\n" + "Host: " + _serverip +
                "\n" + "User-Agent: " + SB_UserAgent + "\n" + "x-smartblock-id:" + WiFi.macAddress() + "\n" +
                "Connection: close\n" + "\r\n\r\n";

        client.print(head);


        int contentlength = 0;
        while (client.connected()) {
            String line = client.readStringUntil('\n');
#ifdef debug
            Serial.println(line);
#endif
            if (line == "\r") {
                break;
            } else if (line.startsWith("Content-Length: ")) {
                line.replace("Content-Length: ", "");
                contentlength = line.toInt();
            }
        }
        int lenge = 0;
#ifdef debug
        Serial.println("Printing body:");
#endif
        while (client.connected() && client.available()) {
            char c = client.read();
            response = response + c;
            if (response.endsWith("    ") || lenge >= contentlength) break;
            lenge++;

        }
        client.stop();
#ifdef debug
        Serial.println(response);

        Serial.println("");
        Serial.print("Content-Length: ");
        Serial.print(contentlength);
        Serial.println("");
#endif
    }
    return response;
}

#endif
