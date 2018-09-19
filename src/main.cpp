#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include "ArduinoJson.h"

#include "PN532.h"
#include "PN532_HSU.h"

#include "Adafruit_INA219.h"

#include "NfcAdapter.h"
#include "NdefMessage.h"
#include "MifareUltralight.h"

/*  Device settings  */
const char  *uuid = "002:001:001";
char        *sessionId = NULL;

/*  WiFi settings  */
const char  *ssid = "Livebox-7AA1";
const char  *password = "Campus_Plex";

/*  PN532 settings  */
PN532_HSU   pn532hsu(Serial2);
NfcAdapter  nfc(pn532hsu);

/*  INA219 settings */
Adafruit_INA219     ina219;
double	shuntvoltage = 0;
double	busvoltage = 0;
double	current_mA = 0;
double	loadvoltage = 0;
double	energy = 0;

/*  API settings  */
HTTPClient  http;
const char  *apiKey = "5b706bfd24ee8498b7f48f88";


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

void    send_production(unsigned int value)
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

int     write_session_id()
{
    NdefMessage message = NdefMessage();

    delay(100);
    nfc.begin();
    Serial.print("write_session_id: ");
    Serial.println(sessionId);
    message.addTextRecord(sessionId);
    if (nfc.tagPresent(5000)) {
        nfc.format();
        nfc.erase();
        nfc.write(message);
        Serial.println("Tag written");
    }
    else {
        Serial.println("Tag was not present to write session ID");
        return (1);
    }
    return (0);
}

void	ina219values() {
	shuntvoltage = ina219.getShuntVoltage_mV();
    Serial.println(shuntvoltage);
	busvoltage = ina219.getBusVoltage_V();
    Serial.println(busvoltage);
	current_mA = ina219.getCurrent_mA();
    Serial.println(current_mA);
	loadvoltage = busvoltage + (shuntvoltage / 1000);
    Serial.println(loadvoltage);
	energy += loadvoltage * current_mA / 3600;
    Serial.println(energy);
}

void    setup()
{
    Serial.begin(115200);

    ina219.begin();

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
}

void loop()
{
    ina219values();
    send_production((loadvoltage * current_mA) / 1000);
    delay(1000);
}
