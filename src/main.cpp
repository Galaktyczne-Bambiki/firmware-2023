#include "Arduino.h"

#include <SoftwareSerial.h>
#include "EBYTE.h"
#include "DS18B20.h"
#include "PMS.h"


constexpr byte M0_PIN = 3;
constexpr byte M1_PIN = 15;
constexpr byte TX_PIN = 13;
constexpr byte RX_PIN = 12;
constexpr byte AUX_PIN = 14;
constexpr byte DS18B20_PIN = D4;
constexpr byte PMS_RX_PIN = 10;

SoftwareSerial ESerial(RX_PIN, TX_PIN);
SoftwareSerial PMS_Serial(PMS_RX_PIN, -1);
EBYTE Transceiver(&ESerial, M0_PIN, M1_PIN, AUX_PIN);
DS18B20 ds(DS18B20_PIN);

PMS pms(PMS_Serial);
PMS::DATA data;

#define SEND
// #define RECEIVE

void setup() {
	Serial.begin(9600);             // Monitor
  ESerial.begin(9600);            // EBYTE

  #ifdef SEND
  PMS_Serial.begin(PMS::BAUD_RATE);  // PMS5003
  #endif

	Transceiver.init();

	Serial.println(Transceiver.GetAirDataRate());
  Serial.println(Transceiver.GetChannel());
  Transceiver.SetAddressH(1);
  Transceiver.SetAddressL(1);
  int Chan = 15;
  Transceiver.SetChannel(Chan);
  Transceiver.SetPullupMode(1);
  Transceiver.SaveParameters(PERMANENT);
  Transceiver.PrintParameters();
}
 
struct DATA {
  float temperature;
} __attribute__((packed)) MyData;
unsigned long Last;

#ifdef RECEIVE
void loop() {
  if (ESerial.available()) 
	{
    Transceiver.GetStruct(&MyData, sizeof(MyData));
    Serial.print("Count: "); Serial.println(MyData.Count);
    Last = millis();
  }
  else {
    if ((millis() - Last) > 1000) 
		{
      Serial.println("Searching: ");
      Last = millis();
    }
  }
}
#endif

#ifdef SEND
void loop() 
{

  if (pms.read(data))
  {
    Serial1.print("PM 1.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_1_0);

    Serial1.print("PM 2.5 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_2_5);

    Serial1.print("PM 10.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_10_0);

    Serial1.println();
  }

  MyData.temperature = ds.getTempC();
  Transceiver.SendStruct(&MyData, sizeof(MyData));
  Serial.printf("Sending: %f\n\r", MyData.temperature);
  delay(200);
}
#endif