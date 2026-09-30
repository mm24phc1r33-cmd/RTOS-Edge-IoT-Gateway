#include <WiFi.h>
#include <ThingSpeak.h>

/* ========================================================= */
/*                      Wi-Fi                                */
/* ========================================================= */

const char* WIFI_SSID = "monika choudhary";
const char* WIFI_PASSWORD = "1234512345";

unsigned long CHANNEL_ID = 3516762;
const char* WRITE_API_KEY = "XWD7T7K23VGV8SKV";

WiFiClient client;


/* ========================================================= */
/*                      STM32 UART                           */
/* ========================================================= */

#define STM32_RX 16
#define STM32_TX 17

HardwareSerial STM32Serial(2);

String rxBuffer = "";


/* ========================================================= */
/*                  PACKET INFORMATION                       */
/* ========================================================= */

String lastSensorPacket = "";

unsigned long lastReadyTime = 0;


/* ========================================================= */
/*                     Wi-Fi CONNECT                         */
/* ========================================================= */

void connectWiFi()
{
    Serial.println();

    Serial.println("========================================");
    Serial.println("          ESP32 STAGE 10.1");
    Serial.println("========================================");

    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    int retry = 0;

    while (
        WiFi.status() != WL_CONNECTED &&
        retry < 30
    )
    {
        delay(500);

        Serial.print(".");

        retry++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Wi-Fi Connected!");

        Serial.print("IP Address: ");

        Serial.println(
            WiFi.localIP()
        );

        Serial.print("RSSI: ");

        Serial.print(
            WiFi.RSSI()
        );

        Serial.println(" dBm");
    }
    else
    {
        Serial.println(
            "Wi-Fi Connection FAILED!"
        );
    }

    Serial.println(
        "========================================"
    );

    Serial.println();
}


/* ========================================================= */
/*              SEND ACK TO STM32                            */
/* ========================================================= */

void sendSensorACK()
{
    STM32Serial.println(
        "ACK,SENSOR_DATA"
    );

    Serial.println(
        "ESP32 -> STM32 : ACK,SENSOR_DATA"
    );
}


/* ========================================================= */
/*            SENSOR PACKET VALIDATION                       */
/* ========================================================= */

bool validateSensorPacket(
    String packet)
{
    if (!packet.startsWith(
            "SENSOR_DATA"))
    {
        return false;
    }

    if (packet.indexOf(
            "TEMP=") < 0)
    {
        return false;
    }

    if (packet.indexOf(
            "HUM=") < 0)
    {
        return false;
    }

    if (packet.indexOf(
            "STATUS=") < 0)
    {
        return false;
    }

    if (packet.indexOf(
            "ERROR=") < 0)
    {
        return false;
    }

    return true;
}


/* ========================================================= */
/*          EXTRACT VALUE FROM SENSOR PACKET                 */
/* ========================================================= */

int extractValue(
    String packet,
    String key)
{
    int startIndex =
        packet.indexOf(key);

    if (startIndex < 0)
    {
        return -1;
    }

    startIndex += key.length();

    int endIndex =
        packet.indexOf(
            ',',
            startIndex
        );

    if (endIndex < 0)
    {
        endIndex =
            packet.length();
    }

    String value =
        packet.substring(
            startIndex,
            endIndex
        );

    value.trim();

    return value.toInt();
}


/* ========================================================= */
/*              SEND DATA TO THINGSPEAK                      */
/* ========================================================= */

void uploadToThingSpeak(
    String packet)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(
            "ThingSpeak: Wi-Fi NOT CONNECTED"
        );

        return;
    }


    /* ===================================================== */
    /* Extract sensor values                                */
    /* ===================================================== */

    int temperature =
        extractValue(
            packet,
            "TEMP="
        );

    int humidity =
        extractValue(
            packet,
            "HUM="
        );

    int errorCount =
        extractValue(
            packet,
            "ERROR="
        );


    /* ===================================================== */
    /* Check extracted values                                */
    /* ===================================================== */

    if (
        temperature == -1 ||
        humidity == -1 ||
        errorCount == -1)
    {
        Serial.println(
            "ThingSpeak: Invalid sensor values"
        );

        return;
    }


    /* ===================================================== */
    /* Print values                                          */
    /* ===================================================== */

    Serial.println();
    Serial.println(
        "------ ThingSpeak Upload ------"
    );

    Serial.print(
        "Temperature: "
    );

    Serial.println(
        temperature
    );

    Serial.print(
        "Humidity: "
    );

    Serial.println(
        humidity
    );

    Serial.print(
        "Error Count: "
    );

    Serial.println(
        errorCount
    );


    /* ===================================================== */
    /* Set ThingSpeak fields                                */
    /* ===================================================== */

    ThingSpeak.setField(
        1,
        temperature
    );

    ThingSpeak.setField(
        2,
        humidity
    );

    ThingSpeak.setField(
        3,
        errorCount
    );


    /* ===================================================== */
    /* Upload                                                */
    /* ===================================================== */

    int response =
        ThingSpeak.writeFields(
            CHANNEL_ID,
            WRITE_API_KEY
        );


    /* ===================================================== */
    /* Upload result                                         */
    /* ===================================================== */

    if (response == 200)
    {
        Serial.println(
            "ThingSpeak: DATA UPLOADED SUCCESSFULLY"
        );
    }
    else
    {
        Serial.print(
            "ThingSpeak: UPLOAD FAILED"
        );

        Serial.print(
            " | HTTP CODE: "
        );

        Serial.println(
            response
        );
    }

    Serial.println(
        "--------------------------------"
    );
}


/* ========================================================= */
/*             PROCESS STM32 MESSAGE                        */
/* ========================================================= */

void processSTM32Message(
    String message)
{
    message.trim();

    if (message.length() == 0)
    {
        return;
    }


    Serial.println(
        "----------------------------------------"
    );

    Serial.print(
        "STM32 -> ESP32 : "
    );

    Serial.println(
        message
    );


    /* ===================================================== */
    /* STM32 READY                                           */
    /* ===================================================== */

    if (message == "STM32_READY")
    {
        Serial.println(
            "ESP32 STATUS: STM32 READY"
        );

        Serial.println(
            "Communication link OK"
        );
    }


    /* ===================================================== */
    /* SENSOR DATA                                           */
    /* ===================================================== */

    else if (
        message.startsWith(
            "SENSOR_DATA"))
    {
        Serial.println(
            "ESP32 STATUS: SENSOR DATA RECEIVED"
        );


        /* Store packet */

        lastSensorPacket =
            message;


        /* ================================================= */
        /* Validate packet                                  */
        /* ================================================= */

        if (validateSensorPacket(
                message))
        {
            Serial.println(
                "PACKET STATUS: VALID"
            );


            /* ============================================= */
            /* Print packet                                  */
            /* ============================================= */

            Serial.print(
                "Sensor Packet: "
            );

            Serial.println(
                message
            );


            /* ============================================= */
            /* Send ACK                                      */
            /* ============================================= */

            sendSensorACK();


            /* ============================================= */
            /* Upload to ThingSpeak                          */
            /* ============================================= */

            uploadToThingSpeak(
                message
            );
        }
        else
        {
            Serial.println(
                "PACKET STATUS: INVALID"
            );

            STM32Serial.println(
                "INVALID_PACKET"
            );

            Serial.println(
                "ESP32 -> STM32 : INVALID_PACKET"
            );
        }


        /* ================================================= */
        /* Wi-Fi status                                     */
        /* ================================================= */

        Serial.print(
            "Wi-Fi Status: "
        );

        if (
            WiFi.status() ==
            WL_CONNECTED)
        {
            Serial.println(
                "CONNECTED"
            );
        }
        else
        {
            Serial.println(
                "DISCONNECTED"
            );
        }
    }


    /* ===================================================== */
    /* UNKNOWN                                             */
    /* ===================================================== */

    else
    {
        Serial.println(
            "ESP32 STATUS: UNKNOWN MESSAGE"
        );
    }


    Serial.println(
        "----------------------------------------"
    );

    Serial.println();
}


/* ========================================================= */
/*                         SETUP                              */
/* ========================================================= */

void setup()
{
    Serial.begin(115200);

    delay(1000);


    /* ===================================================== */
    /* ESP32 UART2                                          */
    /* ===================================================== */

    STM32Serial.begin(
        115200,
        SERIAL_8N1,
        STM32_RX,
        STM32_TX
    );


    Serial.println();

    Serial.println(
        "========================================"
    );

    Serial.println(
        "       RTOS EDGE IoT GATEWAY"
    );

    Serial.println(
        "              STAGE 10.1"
    );

    Serial.println(
        "========================================"
    );

    Serial.println();

    Serial.println(
        "ESP32 UART2"
    );

    Serial.println(
        "GPIO16 = RX2"
    );

    Serial.println(
        "GPIO17 = TX2"
    );

    Serial.println(
        "Baud = 115200"
    );

    Serial.println();


    /* ===================================================== */
    /* Wi-Fi                                                */
    /* ===================================================== */

    connectWiFi();


    /* ===================================================== */
    /* ThingSpeak                                           */
    /* ===================================================== */

    if (WiFi.status() == WL_CONNECTED)
    {
        ThingSpeak.begin(client);

        Serial.println(
            "ThingSpeak: INITIALIZED"
        );
    }
    else
    {
        Serial.println(
            "ThingSpeak: NOT INITIALIZED"
        );
    }


    /* ===================================================== */
    /* Send ESP32 READY                                     */
    /* ===================================================== */

    STM32Serial.println(
        "ESP32_READY"
    );

    Serial.println(
        "ESP32 -> STM32 : ESP32_READY"
    );

    Serial.println();

    Serial.println(
        "Waiting for STM32 sensor data..."
    );

    Serial.println();
}


/* ========================================================= */
/*                         LOOP                              */
/* ========================================================= */

void loop()
{
    /* ===================================================== */
    /* Receive STM32 messages                               */
    /* ===================================================== */

    while (
        STM32Serial.available())
    {
        char c =
            STM32Serial.read();


        /* ================================================= */
        /* End of packet                                    */
        /* ================================================= */

        if (c == '\n')
        {
            processSTM32Message(
                rxBuffer
            );

            rxBuffer = "";
        }


        /* ================================================= */
        /* Ignore CR                                        */
        /* ================================================= */

        else if (c == '\r')
        {
            /* Ignore */
        }


        /* ================================================= */
        /* Store character                                  */
        /* ================================================= */

        else
        {
            if (rxBuffer.length() < 200)
            {
                rxBuffer += c;
            }
            else
            {
                rxBuffer = "";

                Serial.println(
                    "ERROR: RX BUFFER OVERFLOW"
                );
            }
        }
    }


    /* ===================================================== */
    /* Send ESP32_READY every 5 seconds                     */
    /* ===================================================== */

    if (
        millis() - lastReadyTime >=
        5000)
    {
        lastReadyTime =
            millis();

        STM32Serial.println(
            "ESP32_READY"
        );

        Serial.println(
            "ESP32 -> STM32 : ESP32_READY"
        );
    }


    /* ===================================================== */
    /* Wi-Fi monitoring                                     */
    /* ===================================================== */

    static unsigned long
        lastWiFiCheck = 0;


    if (
        millis() - lastWiFiCheck >=
        5000)
    {
        lastWiFiCheck =
            millis();


        if (
            WiFi.status() !=
            WL_CONNECTED)
        {
            Serial.println(
                "Wi-Fi disconnected!"
            );

            Serial.println(
                "Trying to reconnect..."
            );

            WiFi.disconnect();

            WiFi.begin(
                WIFI_SSID,
                WIFI_PASSWORD
            );
        }
    }


    delay(10);
}
