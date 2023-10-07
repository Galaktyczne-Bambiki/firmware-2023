#include "Arduino.h"

#include <SoftwareSerial.h>
#include "EBYTE.h"
#include "DS18B20.h"
#include "PMS.h"


constexpr byte M0_PIN = D0;
constexpr byte M1_PIN = D1;
constexpr byte TX_PIN = D5;
constexpr byte RX_PIN = D6;
constexpr byte AUX_PIN = D7;
constexpr byte DS18B20_PIN = D4;
// constexpr byte PMS_RX_PIN = 10;

SoftwareSerial ESerial(RX_PIN, TX_PIN);
EBYTE Transceiver(&ESerial, M0_PIN, M1_PIN, AUX_PIN);
DS18B20 ds(DS18B20_PIN);
// SoftwareSerial PMS_Serial(PMS_RX_PIN, -1);

// PMS pms(PMS_Serial);
// PMS::DATA data;

struct DATA {
  float temperature;
} message;

unsigned long Last;

// #define SEND
#define RECEIVE

void setup_e32(void)
{
  ESerial.begin(9600);            // EBYTE

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

void loop() {
  if (ESerial.available()) 
	{
    Transceiver.GetStruct(&message, sizeof(message));
    Serial.printf("Got: %f\n\r", message.temperature);
    Last = millis();
  }
  else {
    if ((millis() - Last) > 1000) 
		{
      Serial.println("No data received");
      Last = millis();
    }
  }
}
#endif

#ifdef SEND
void setup() {
	Serial.begin(9600);
  // PMS_Serial.begin(PMS::BAUD_RATE);  // PMS5003
  setup_e32();
}

void loop() 
{

  // if (pms.read(data))
  // {
  //   Serial1.print("PM 1.0 (ug/m3): ");
  //   Serial1.println(data.PM_AE_UG_1_0);

  //   Serial1.print("PM 2.5 (ug/m3): ");
  //   Serial1.println(data.PM_AE_UG_2_5);

  //   Serial1.print("PM 10.0 (ug/m3): ");
  //   Serial1.println(data.PM_AE_UG_10_0);

  //   Serial1.println();
  // }

  message.temperature = ds.getTempC();
  Transceiver.SendStruct(&message, sizeof(message));
  Serial.printf("Sending: %f\n\r", message.temperature);
  delay(300);
}
#endif