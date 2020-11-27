/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 2 (connect to TX of other device)
 * TX is digital pin 3 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <Arduino.h>
#include <SoftwareSerial.h>


const int RXArdu = 2;
const int TXArdu = 3;
const int baudrate_uart = 9600;

SoftwareSerial UART(RXArdu, TXArdu); // RX, TX

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

void printMessage(uint8_t *message, int bytes) {
    if (message[1] == 0x86) {
        Serial.print("Co2 level message received: ");
        uint8_t highLevel = message[2];
        uint8_t lowLevel = message[3];
        int co2level = highLevel * 256 + lowLevel;
        Serial.print(co2level);
        Serial.print(" ppm\n");
    }
}
void resetAnswer(uint8_t * x){
    for (int i = 0; i < 9; ++i) {
        x[i]=0;
    }
}


void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {  // wait for serial port to connect. Needed for Native USB only
    }


    Serial.println("Serial Connected");

    // set the data rate for the SoftwareSerial port
    UART.begin(baudrate_uart);

}

void loop() // run over and over
{

    resetAnswer(answer);
    UART.write(co2_level_cmd, 9);
    UART.readBytes(answer, 9);

    Serial.print("I just read: ");
    for (int i = 0; i < 9; ++i) {
        Serial.print(answer[i], HEX);
        Serial.print(" ");
    }
    Serial.print('\n');
    printMessage(answer,9);
    delay(1000);
}