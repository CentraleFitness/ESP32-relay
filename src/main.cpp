#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>

#include "ArduinoJson.h"

#include "PN532.h"
#include "emulatetag.h"
#define PN532_USE_SPI

#ifdef PN532_USE_HSU
#include "PN532_HSU.h"
#endif

#ifdef PN532_USE_SPI
#include "PN532_SPI.h"
#endif

#include "INA219.h"

#include "NfcAdapter.h"
#include "NdefMessage.h"
#include "MifareUltralight.h"

/*  Device settings  */
const char  *uuid = "002:001:001";
uint8_t     uid[3] = {0x12, 0x34, 0x56};
uint8_t     ndefBuf[64];
NdefMessage message;
char        *sessionId = NULL;

/*  WiFi settings  */
const char  *ssid = "Centrale_Fitness";
const char  *password = "iswhatyouneed";

/*  PN532 settings  */
// PN532_HSU   pn532hsu(Serial2);
#ifdef PN532_USE_SPI
PN532_SPI   pn532(SPI, 5);
#endif
#ifdef PN532_USE_HSU
PN532_HSU   pn532;
#endif
// NfcAdapter  nfc(pn532);
EmulateTag  nfc(pn532);

/*  INA219 settings */

#ifdef INA219_CONNECTED
Adafruit_INA219     ina219;
double	shuntvoltage = 0;
double	busvoltage = 0;
double	current_mA = 0;
double	loadvoltage = 0;
double	energy = 0;
#endif

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

// bool                nfc_write(const char *buffer, bool must_write = true, bool check = true)
// {
//     NdefMessage     message = NdefMessage();
//     NfcTag          tag;
//     bool            success = false;

//     message.addTextRecord(sessionId);
//     do  {
//         if (nfc.tagPresent(5000))
//             success = nfc.write(message);
//             if (!success)
//                 Serial.println("Write failed");
//         delay(100);
//     } while (must_write && !success);
//     if (check) {
//         while (!nfc.tagPresent(5000))
//             delay(100);
//         tag = nfc.read();
//         Serial.print("(New) value on tag: ");
//         tag.print();
//     }
//     return (success);
// }

// int     write_session_id()
// {
//     Serial.print("write_session_id: ");
//     Serial.println(sessionId);
//     nfc_write(sessionId, true, false);
//     Serial.println("write_session_id done");
//     return (0);
// }

#ifdef INA219_CONNECTED
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
#endif

void    setup()
{
    int     encodedSize;
    memset(ndefBuf, 0, 64);
    Serial.begin(115200);

#ifdef INA219_CONNECTED
    ina219.begin();
#endif

    // nfc.begin();
    delay(3000);
    Serial.println("--- Centrale Fitness Beta Module [Test] ---");
    // delay(1000);
    // Serial.print("Connecting to the WiFi");
    // WiFi.begin(ssid, password);
    // while (WiFi.status() != WL_CONNECTED) {
    //     Serial.print('.');
    //     delay(1000);
    // }
    // Serial.print("Connected to the WiFi network ");
    // Serial.println(ssid);
    // while (get_session_id())
    //     delay(1000);
    // while (write_session_id())
    //     delay(1000);

    message = NdefMessage();
    message.addTextRecord("CentraleFitness");
    encodedSize = message.getEncodedSize();
    Serial.print("Encoded size: ");
    Serial.println(encodedSize);
    message.encode(ndefBuf);
    nfc.setNdefFile(ndefBuf, encodedSize);
    nfc.setUid(uid);
    nfc.init();

    Serial.println("Setup done.");
    delay(1000);
}

void loop()
{
    delay(1000);
#ifdef INA219_CONNECTED
    ina219values();
    send_production((loadvoltage * current_mA) / 1000);
#endif
    // send_production(random(0, 10));
    // nfc.setNdefFile(ndefBuf, 22);
    Serial.println(nfc.emulate(5000));
    if(nfc.writeOccured()){
       Serial.println("\nWrite occured !");
       uint8_t* tag_buf;
       uint16_t length;
       nfc.getContent(&tag_buf, &length);
       NdefMessage msg = NdefMessage(tag_buf, length);
       msg.print();
    }
}
