/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "pass.hpp"

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/*Sensor*/
const int RXArdu = 13;//7;//15;//13;//3;//15;
const int TXArdu = 15;//8;//16;//15;//1;//16;
const int baudrate_uart = 9600;

SoftwareSerial UART(RXArdu, TXArdu); // RX, TX

int co2_ppm_=0;

uint8_t co2_level_cmd[9] = {0xFF, // starting byte
                            0x01, // sensor no
                            0x86, // command
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x00,
                            0x79};

uint8_t answer[9]{0, 0, 0, 0, 0, 0, 0, 0, 0};

int printMessage(uint8_t *message, int bytes) {
    if (message[1] == 0x86) 
    {
        Serial.print("Co2 level message received: ");
        uint8_t highLevel = message[2];
        uint8_t lowLevel = message[3];
        int co2level = highLevel * 256 + lowLevel;
        co2_ppm_=co2level;
        Serial.print(co2level);
        Serial.print(" ppm\n");
    }
    return co2_ppm_;
}
void resetAnswer(uint8_t * x){
    for (int i = 0; i < 9; ++i) {
        x[i]=0;
    }
}

/*Conexion wifi y mqtt*/
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish co2_concentracion = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/co2_concentracion");



void setup() 
{
    /*Comunicacion serie*/
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    while (!Serial) { }
    Serial.println("Serial Connected");

    /*Sensor*/
    // set the data rate for the SoftwareSerial port
    UART.begin(baudrate_uart);

    /*Conexion wifi*/
    WiFi.begin(R_SSID, PASS);
    int counter=10;
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
        counter--;
        if(counter==0)
        {
            // reset me
            Serial.println("Conexion Wifi 1 FALLIDA... reseteando...");
            ESP.reset();
        }
    }
    Serial.println("Conexion Wifi 1 EXITOSA");

}

void loop() // run over and over
{

    /*Mqtt server*/
    /*if (mqtt.connected()) 
    {
        Serial.println("Conexion MQTT FALLIDA... saliendo del programa...");
        delay(5000);
        //return;       //¿ENTIENDO QUE ESTE RETURN ES POR SI NO SEGUIMOS CONECTADOS FINALIZAR EL PROGAMA 
    }*/

    uint8_t retries = 5;
    int ret=0;
    while ((ret = mqtt.connect()) != 0)  // connect will return 0 for connected  
    {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Reintentando conexion MQTT... 5 segundos ...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
            // reset me
            Serial.println("Reconexion MQTT FALLIDA... reseteando...");
            ESP.reset();
       }
    }

    /*Sensor*/
    resetAnswer(answer);
    UART.write(co2_level_cmd, 9);
    UART.readBytes(answer, 9);

    Serial.print("I just read: ");
    for (int i = 0; i < 9; ++i) {
        Serial.print(answer[i], HEX);
        Serial.print(" ");
    }
    Serial.print('\n');
    co2_ppm_=printMessage(answer,9);
    Serial.print("COMPROBANDO VALOR CO2: ");
    Serial.println(co2_ppm_);
    //delay(1000);

    /*Envio de datos por wifi y mqtt*/
    if (! co2_concentracion.publish(co2_ppm_)) 
    { 
        Serial.println("Envio wifi de co2 FALLIDO...");
    } 
    else 
    {
        Serial.println("Envio wifi de co2 EXITOSO");
    }

    delay(15000);

}


/*
 int co2ppm() 
{
  static byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  static byte response[9] = {0};

  co2.write(cmd, 9);
  co2.readBytes(response, 9);

  unsigned int responseHigh = (unsigned int) response[2];
  unsigned int responseLow = (unsigned int) response[3];

  return (256 * responseHigh) + responseLow;
}*/