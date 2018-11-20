#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include "ArduinoJson.h"

#include "PN532.h"
#include "PN532_SPI.h"

#include "Adafruit_INA219.h"

#include "NfcAdapter.h"
#include "NdefMessage.h"
#include "MifareUltralight.h"

/*  Device settings  */
const char  *uuid = "002:001:001";
char        *sessionId = NULL;

/*  WiFi settings  */
const char  *ssid = "Centrale_Fitness";
const char  *password = "iswhatyouneed";

/*  PN532 settings  */
PN532_SPI   pn532(SPI, 5);
NfcAdapter  nfc(pn532);

/*  INA219 settings */
Adafruit_INA219     ina219;
double	            loadvoltage = 0;
double              current_W = 0;

/*  API settings  */
HTTPClient  http;
const char  *apiKey = "5b706bfd24ee8498b7f48f89";


int                         get_session_id()
{
    StaticJsonBuffer<256>   jsonUUIDBuffer;
    JsonObject              &moduleUUID = jsonUUIDBuffer.createObject();
    JsonArray               &UUIDarray = moduleUUID.createNestedArray("UUID");
    char                    buffer[256];
    int                     respCode;

    if (sessionId) {
        free(sessionId);
        sessionId = NULL;
    }
    moduleUUID["apiKey"] = apiKey;
    UUIDarray.add(uuid);
    moduleUUID.printTo(buffer, 256);
    // Serial.println(buffer);
    http.begin("http://91.121.155.83:10000/module/get/ids");
    http.addHeader("Content-Type", "application/json");
    if ((respCode = http.POST(buffer)) == 200) {
        String response = http.getString();
        // Serial.println(response);
        JsonObject &root = jsonUUIDBuffer.parseObject(response);
        sessionId = (char *)malloc(sizeof(char) * 32);
        if (!sessionId)
            return (1);
        strcpy(sessionId, root["moduleIDS"][0]);
        respCode = 0;
    }
    else {
        Serial.print("Failed to get the session ID. Code ");
        Serial.println(respCode);
        return (1);
    }
    // Serial.print("get_session_id(): ");
    // Serial.println(sessionId);
    http.end();
    return (0);
}

void    send_production(double value)
{
    StaticJsonBuffer<256>   jsonProdBuffer;
    JsonObject              &production = jsonProdBuffer.createObject();
    JsonObject              &device = production.createNestedObject("production");
    char                    buffer[256];
    int                     respCode;
    const char              *code;

    production["apiKey"] = apiKey;
    device[uuid] = value;
    production.printTo(buffer, 256);
    Serial.print("Sending: ");
    Serial.println(buffer);
    http.begin("http://91.121.155.83:10000/module/production/send");
    http.addHeader("Content-Type", "application/json");
    if ((respCode = http.POST(buffer)) == 200) {
        String response = http.getString();
        Serial.println(response);
        JsonObject &jresp = jsonProdBuffer.parseObject(response);
        code = jresp["code"];
        // if (strcmp(code, "GENERIC_OK") != 0) {
        //     Serial.println("Error");
        // }
        // else {
        //     Serial.println(code);
        // }
        // Serial.println(code);
    }
    else {
        Serial.print("Production send failed. Code ");
        Serial.println(respCode);
    }
    http.end();
}

bool                nfc_write(const char *buffer, bool must_write = true, bool check = true)
{
    NdefMessage     message = NdefMessage();
    NfcTag          tag;
    bool            success = false;

    message.addTextRecord(sessionId);
    do  {
        if (nfc.tagPresent(5000))
            success = nfc.write(message);
            if (!success)
                Serial.println("Write failed");
        delay(100);
    } while (must_write && !success);
    if (check) {
        while (!nfc.tagPresent(5000))
            delay(100);
        tag = nfc.read();
        Serial.print("(New) value on tag: ");
        tag.print();
    }
    return (success);
}

int     write_session_id()
{
    Serial.print("write_session_id: ");
    Serial.println(sessionId);
    nfc_write(sessionId, true, false);
    Serial.println("write_session_id done");
    return (0);
}

void	ina219values() {
	loadvoltage = ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000);
	current_W = loadvoltage * ina219.getCurrent_mA() / 3600;
    Serial.println("Current " + String(current_W) + "W");
}

void    setup()
{
    Serial.begin(9600);
    ina219.begin();
    nfc.begin();
    delay(3000);
    Serial.println("--- Centrale Fitness Beta Module [Test] ---");
    delay(1000);
    Serial.print("Connecting to the WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.print("Connected to the WiFi network ");
    Serial.println(ssid);
    while (get_session_id())
        delay(1000);
    while (write_session_id())
        delay(1000);
    Serial.println("Setup done.");
    //delay(10000);
}

void loop()
{
    delay(1000);
    ina219values();
    send_production(current_W);
    // send_production(random(0, 10));
}
