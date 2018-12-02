#include <Arduino.h>
#include <Wire.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>

#include "ArduinoJson.h"

#include <Adafruit_INA219.h>

#include "secondary.hpp"

/*  WiFi settings  */
const char  *ssid = "Centrale_Fitness";
const char  *password = "iswhatyouneed";

/* Secondary device settings */
HardwareSerial      HSerial(1);
Secondary           secondary(&HSerial);

/*  INA219 settings */
Adafruit_INA219     ina219;
double	            loadvoltage = 0;
double              curr_watts = 0;

/*  API settings  */
HTTPClient  http;
const char  *apiKey = "5bf72d7efb56e5539cb102ee";
char        *sessionId;
const char  *uuid = "002:001:001";


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
    // const char              *code;

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
        // code = jresp["code"];
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

void	read_ina219_values() {
	loadvoltage = ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000);
    curr_watts = (loadvoltage * ina219.getCurrent_mA()) / 1000;
    if (curr_watts < 0.01)
        curr_watts = 0;
    Serial.println("Current: " + String(curr_watts) + "W");
}

void    setup()
{
    Serial.begin(9600);

    ina219.begin();
    secondary.turn_on();
    while (!secondary.isOn())
        delay(50);
    secondary.setLEDProgram(5);
    // delay(3000);
    Serial.println("--- Centrale Fitness Beta Module [Test] ---");

    // delay(1000);

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
    Serial.print("Session id: ");
    Serial.println(sessionId);

    while (!secondary.setNFCId(sessionId))
        delay(50);
    Serial.println("Session Id wrote");
    secondary.setLEDProgram(9);

    //delay(10000);
}

void loop()
{
    delay(1000);
    read_ina219_values();
    send_production(curr_watts);
    // send_production(random(0, 10));
    // nfc.setNdefFile(ndefBuf, 22);
}
