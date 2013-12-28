#include <StandardCplusplus.h>
#include <serstream>
#include <string>
#include <vector>
#include <iterator>
#include "DHT.h"
#include <PID_v1.h>

#include "/home/darkjavi/sketchbook/libraries/StandardCplusplus/StandardCplusplus.h"
#include "/home/darkjavi/sketchbook/libraries/StandardCplusplus/serstream"
#include "/home/darkjavi/sketchbook/libraries/StandardCplusplus/string"
#include "/home/darkjavi/sketchbook/libraries/StandardCplusplus/vector"
#include "/home/darkjavi/sketchbook/libraries/StandardCplusplus/iterator"
#include "/home/darkjavi/sketchbook/libraries/DHT_sensor/DHT.h"

//#define DEBUG

#define LED0_PIN    4
#define LED0_WATTS  30

#define LED1_PIN    7
#define LED1_WATTS  60

//#define LED2_PIN    8
//#define LED2_WATTS  10

//#define LED3_PIN    9
//#define LED3_WATTS  10

//#define LED4_PIN    9
//#define LED4_WATTS  10

//#define LED5_PIN    10
//#define LED5_WATTS  10

//#define LED6_PIN    11
//#define LED6_WATTS  10

//#define LED7_PIN    12
//#define LED7_WATTS  10

#define LAMP_TEMP_PIN A0
//#define LAMP_FAN_PIN  5

#define TEMP_SENSOR_MIN_TEMP    5        //minimal possible temp(less will be considered as misread!)
#define LAMP_MIN_TEMPERATURE 	15 	// treshold to start fans
#define LAMP_TARGET_TEMPERATURE 25  // target temperature
#define LAMP_CRITICAL_TEMP   	70	// shutdown temperature

#define DHT_PIN         12
#define ACS712_PIN      A1
#define PUMP_PIN        5
#define PUMP_FLOW_INT   0


using namespace std;
// <iostream> declares cout/cerr, but the application must define them
// because it's up to you what to do with them.
namespace std
{
  ohserialstream cout(Serial);
}

#include "greenHouseAccesories.h"

lamp ghLamp;
pump ghPump(PUMP_PIN);
DHT  ghDHTSensor(DHT_PIN,DHT11);
acs712 ghCurrentSensor(ACS712_PIN);

unsigned int ips = 0;

void sendData()
{
    float h = ghDHTSensor.readHumidity();
    float t = ghDHTSensor.readTemperature();
    if (isnan(t) || isnan(h))
    {
        t = -99;
        h = -99;
    }

   String data_str;
    data_str+="[DATA]";
    data_str+="POWER:";
    data_str+=ghLamp.activePower();
    data_str+=",";
    data_str+="LAMP_TEMP:";
    data_str+=ghLamp.temperature();
    data_str+=",";
    data_str+="LAMP_FAN:";
    data_str+=ghLamp.fanSpeed();
    data_str+=",";
    data_str+="AMBIENT_TEMP:";
    data_str+= (int)t;
    data_str+=",";
    data_str+="AMBIENT_HUMID:";
    data_str+= (int)h;
    data_str+=",";
    data_str+="POWER_DRAIN:";
    data_str+=ghCurrentSensor.currentPower();
    data_str+=",";
    data_str+="IPS:";
    data_str+=(int)ips;
    Serial.println(data_str);
    ips = 0;
}

void receiveData()
{
    if(!Serial.available())
        return;
    char firstByte = Serial.read();
    if(firstByte==0x10)//PING
        Serial.println("[PONG]");
    else
        Serial.println("UNKOWN COMMAND:");
    while(Serial.available())
    {
        Serial.read();
    }
}

void setup()
{
  Serial.begin(115200);
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

#ifdef PUMP_FLOW_INT
    attachInterrupt(0,flowSensor,RISING);
#endif


    Serial.println("[INIT!]");
    ghDHTSensor.begin();
    cout << "[INFO]Lamp have "<<ghLamp.leds().size() << " leds\n";
    cout << "[INFO]Lamp have "<<ghLamp.totalPower()<<"Watts\n";
    if(ghLamp.haveFan()) cout << "[INFO]Lamp have fan control on pin:"<<fanPin<<"\n";
    else cout << "[INFO]Lamp DOESN'T have fan control!\n";
    if(ghLamp.haveTempSensor()) cout << "[INFO]Lamp have temp sensor on pin:"<<tempPin<<"\n";
    else cout << "[INFO]Lamp DOESN'T have temp sensor!\n";
//    ghLamp.test();
//    ghLamp.switchOn();
//    delay(1000);
//    ghLamp.switchOff();
//    delay(1000);
    ghLamp.leds()[1].switchOn();
    delay(1000);
    //ghPump.pumpMl(50);

}

void flowSensor()
{
    ghPump.flow();
}

void loop()
{
    ghLamp.check();
    ghPump.check();
    sendData();
    receiveData();
    //delay(500);
}
