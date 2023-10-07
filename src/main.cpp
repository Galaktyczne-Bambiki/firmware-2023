#include "Arduino.h"

#include <SoftwareSerial.h>
#include "EBYTE.h"
#include "DS18B20.h"

constexpr byte M0_PIN = D0;
constexpr byte M1_PIN = D1;
constexpr byte TX_PIN = D5;
constexpr byte RX_PIN = D6;
constexpr byte AUX_PIN = D7;
constexpr byte DS18B20_PIN = D4;

SoftwareSerial ESerial(RX_PIN, TX_PIN);
EBYTE Transceiver(&ESerial, M0_PIN, M1_PIN, AUX_PIN);
DS18B20 ds(DS18B20_PIN);

struct DATA {
  float temperature;
} message;

// #define SEND
#define RECEIVE

void setup_e32(void)
{
  ESerial.begin(9600);

	Transceiver.init();
	Serial.println(Transceiver.GetAirDataRate());
  Serial.println(Transceiver.GetChannel());

  Transceiver.SetAddressH(0);
  Transceiver.SetAddressL(0);
  Transceiver.SetSpeed(0b00011100);  
  Transceiver.SetChannel(1);
  Transceiver.SetOptions(0b01000100);
  Transceiver.SaveParameters(PERMANENT);
  
  Transceiver.PrintParameters();
}

#ifdef RECEIVE
void setup() {
	Serial.begin(9600);
  setup_e32();
}

void loop() 
{
  if (ESerial.available()) 
	{
    Transceiver.GetStruct(&message, sizeof(message));
    Serial.printf("%.3f\n\r", message.temperature);
  }
}
#endif

#ifdef SEND
void setup() {
	Serial.begin(9600);
  setup_e32();
}

void loop() 
{
  message.temperature = ds.getTempC();
  Transceiver.SendStruct(&message, sizeof(message));
  Serial.printf("Sending: %f\n\r", message.temperature);
  delay(300);
}
#endif