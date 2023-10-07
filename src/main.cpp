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
// constexpr byte DS18B20_PIN = 2;
// constexpr byte PMS_RX_PIN = 10;

SoftwareSerial ESerial(RX_PIN, TX_PIN);
EBYTE Transceiver(&ESerial, M0_PIN, M1_PIN, AUX_PIN);
// DS18B20 ds(DS18B20_PIN);
// SoftwareSerial PMS_Serial(PMS_RX_PIN, -1);

// PMS pms(PMS_Serial);
// PMS::DATA data;

struct DATA {
  unsigned int temperature;
} message;

unsigned long Last;

// #define SEND
#define RECEIVE

void setup_e32(void)
{
  ESerial.begin(9600);            // EBYTE

	Transceiver.init();
  // Transceiver.SetSpeed(0);
  // Transceiver.SetOptions(0);

	Serial.println(Transceiver.GetAirDataRate());
  Serial.println(Transceiver.GetChannel());

  // Transceiver.SetAirDataRate(ADR_1200);   // change the air data rate
  // Transceiver.SetAddressH(1);   // set the high address byte
  // Transceiver.SetAddressL(1);   // set the low address byte
  // Transceiver.SetChannel(4);     // set the channel (0-32 is pretty typical)
  // Transceiver.SetTransmitPower(0b00);
  // Transceiver.SetTransmissionMode(MODE_NORMAL);
  // Transceiver.SetMode(MODE_NORMAL);
  // Transceiver.SetFECMode(OPT_FECENABLE);
  // Transceiver.SetPullupMode(0x1);
  // Transceiver.SaveParameters(TEMPORARY);  // save the parameters to the EBYTE EEPROM, you can save temp if periodic changes are needed
  
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
	Serial.begin(9600);             // Monitor
  setup_e32();
}

void loop() {
  if (ESerial.available()) 
	{
    Transceiver.GetStruct(&message, sizeof(message));
    Serial.printf("Got: %d\n\r", message.temperature);
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
	Serial.begin(9600);             // Monitor
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

  // message.temperature = ds.getTempC();
  message.temperature = 255;
  Transceiver.SendStruct(&message, sizeof(message));
  // Transceiver.flush();
  Serial.printf("Sending: %d\n\r", message.temperature);
  delay(1000);
}
#endif