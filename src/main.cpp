#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include "ArduinoJson.h"

#include "PN532.h"
#include "PN532_HSU.h"

#include "NfcAdapter.h"
#include "NdefMessage.h"
#include "MifareUltralight.h"

/*  Device settings  */
const char  *uuid = "002:001:001";
const char  *sessionId;

/*  WiFi settings  */
const char  *ssid = "Livebox-7AA1";
const char  *password = "";

/*  PN532 settings  */
PN532_HSU   pn532hsu(Serial2);
NfcAdapter  nfc(pn532hsu);
NdefMessage message = NdefMessage();

/*  API settings  */
HTTPClient  http;
const char  *apiKey = "5b706bfd24ee8498b7f48f88";


const char      *get_session_id()
{
    StaticJsonBuffer<256>   jsonUUIDBuffer;
    JsonObject              &moduleUUID = jsonUUIDBuffer.createObject();
    JsonArray               &UUIDarray = moduleUUID.createNestedArray("UUID");
    char                    buffer[256];
    int                     respCode;
    const char              *id;

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
        id = root["moduleIDS"][0];
    }
    else {
        id = NULL;
        Serial.print("Failed to get the session ID. Code ");
        Serial.println(respCode);
    }
    http.end();
    return (id);
}

void    send_production(unsigned int value)
{
    StaticJsonBuffer<256>   jsonProdBuffer;
    JsonObject              &production = jsonProdBuffer.createObject();
    JsonObject              &device = production.createNestedObject("production");
    char                    buffer[256];
    int                     respCode;
    const char              *code;

    Serial.println((long unsigned int)&production);
    Serial.println((long unsigned int)&device);

    production["apiKey"] = apiKey;
    device[uuid] = value;
    production.printTo(buffer, 256);
    Serial.println(buffer);
    http.begin("http://91.121.155.83:10000/module/production/send");
    http.addHeader("Content-Type", "application/json");
    if ((respCode = http.POST(buffer)) == 200) {
        String response = http.getString();
        Serial.println(response);
        JsonObject &root = jsonProdBuffer.parseObject(response);
        code = root["code"];
        // if (strcmp(code, "GENERIC_OK") != 0) {
        //     Serial.println("Error");
        // }
        // else {
        //     Serial.println(code);
        // }
        Serial.println(code);
    }
    else {
        Serial.print("Production send failed. Code ");
        Serial.println(respCode);
    }
    http.end();
}

void    setup()
{
    // nfc.begin();
    Serial.begin(115200);
    Serial.println("--- Centrale Fitness Beta Module [Test] ---");
    delay(1000);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
        delay(1000);
    Serial.print("Connected to the WiFi network ");
    Serial.println(ssid);

    sessionId = get_session_id();
    while (sessionId == NULL)
        delay(1000);

    // moduleUUID.printTo(stringBuffer);
    // Serial.println(stringBuffer.c_str());
    // message.addTextRecord("Centrale Fitness");
    nfc.begin();
}

void loop()
{
    send_production(1);
    if (nfc.tagPresent(5000)) {
        Serial.println("Tag is present");
        if (nfc.write(message)) {
            Serial.println("Write success!");
        }
        else {
            Serial.println("Write failed :(");
        }
    }
    delay(1000);
}
