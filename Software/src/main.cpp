/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#include <SoftwareSerial.h>


const int RXArdu = 13;//7;//15;//13;//3;//15;
const int TXArdu = 15;//8;//16;//15;//1;//16;
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