#include <StandardCplusplus.h>
#include <serstream>
#include <string>
#include <vector>
#include <iterator>
#include </home/darkjavi/sketchbook/libraries/StandardCplusplus/StandardCplusplus.h>
#include </home/darkjavi/sketchbook/libraries/StandardCplusplus/serstream>
#include </home/darkjavi/sketchbook/libraries/StandardCplusplus/string>
#include </home/darkjavi/sketchbook/libraries/StandardCplusplus/vector>
#include </home/darkjavi/sketchbook/libraries/StandardCplusplus/iterator>

#define DEBUG

#define LED0_PIN    4
#define LED0_WATTS  30

#define LED1_PIN    7
#define LED1_WATTS  10

#define LED2_PIN    8
#define LED2_WATTS  10

#define LED3_PIN    9
#define LED3_WATTS  10

//#define LED4_PIN    9
//#define LED4_WATTS  10

//#define LED5_PIN    10
//#define LED5_WATTS  10

//#define LED6_PIN    11
//#define LED6_WATTS  10

//#define LED7_PIN    12
//#define LED7_WATTS  10

#define LAMP_TEMP_PIN A0
#define LAMP_FAN_PIN  5

#define TEMP_SENSOR_MIN_TEMP    5        //minimal possible temp(less will be considered as misread!)
#define LAMP_MIN_TEMPERATURE 	30 	// treshold to start fans
#define LAMP_TARGET_TEMPERATURE 40  // target temperature
#define LAMP_CRITICAL_TEMP   	70	// shutdown temperature


#define PUMP_PIN        3
#define PUMP_FLOW_PIN   2

using namespace std;
// <iostream> declares cout/cerr, but the application must define them
// because it's up to you what to do with them.
namespace std
{
  ohserialstream cout(Serial);
}

#include "greenHouseAccesories.h"

lamp ghLamp;
pump ghPump;
unsigned int ips = 0;

void sendData()
{
    cout << "[DATA]" <<
//            "LAMP_TOTALPOWER:"      << ghLamp.totalPower()  <<
            "LAMP_ACTIVEPOWER:"     << ghLamp.activePower() <<
            "IPS:"  << ips <<
            "\n";
            ips = 0;
}

void setup()
{
  Serial.begin(115200);
    cout << "[INFO]INIT!\n";
    int fanPin = -1;
    int tempPin = -1;
#ifdef LAMP_TEMP_PIN
    tempPin = LAMP_TEMP_PIN;          
#endif
#ifdef LAMP_FAN_PIN
	fanPin = LAMP_FAN_PIN;
#endif
    ghLamp = lamp(fanPin,tempPin);
#ifdef LED0_PIN
	ghLamp.addLed(LED0_PIN,LED0_WATTS);   
#endif
#ifdef LED1_PIN
    ghLamp.addLed(LED1_PIN,LED1_WATTS);   
#endif    
#ifdef LED2_PIN
    ghLamp.addLed(LED2_PIN,LED2_WATTS);   
#endif
#ifdef LED3_PIN
    ghLamp.addLed(LED3_PIN,LED3_WATTS);   
#endif
#ifdef LED4_PIN
    ghLamp.addLed(LED4_PIN,LED4_WATTS);   
#endif
#ifdef LED5_PIN
    ghLamp.addLed(LED5_PIN,LED5_WATTS);   
#endif
#ifdef LED6_PIN
    ghLamp.addLed(LED6_PIN,LED6_WATTS);   
#endif
#ifdef LED7_PIN
    ghLamp.addLed(LED7_PIN,LED7_WATTS);   
#endif

    cout << "[INFO]Lamp have "<<ghLamp.leds().size() << " leds\n";
    cout << "[INFO]Lamp have "<<ghLamp.totalPower()<<"Watts\n";
    if(ghLamp.haveFan()) cout << "[INFO]Lamp have fan control on pin:"<<fanPin<<"\n";
    else cout << "[INFO]Lamp DOESN'T have fan control!\n";
    if(ghLamp.haveTempSensor()) cout << "[INFO]Lamp have temp sensor on pin:"<<tempPin<<"\n";
    else cout << "[INFO]Lamp DOESN'T have temp sensor!\n";
    ghLamp.test();
    ghLamp.switchOn();
//    delay(1000);
//    ghLamp.switchOff();
//    delay(1000);
//    ghLamp.leds()[0].switchOn();
//    attachInterrupt(0,int0,RISING);
}

void int0()
{
//  noInterrupts();
    ips++;
//  interrupts();
}

void loop()
{
    ghLamp.check();
    sendData();
    delay(1000);
    char *msg;
	while (Serial.available()) {
		// lee el byte entrante:
		int	incomingByte = Serial.read();
		// dice lo que ha recibido:
		Serial.print("He recibido: ");
		Serial.println(incomingByte, DEC);
	}
}
