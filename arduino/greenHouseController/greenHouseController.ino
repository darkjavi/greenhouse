#include <StandardCplusplus.h>
#include <serstream>
#include <string>
#include <vector>
#include <iterator>

#define LED0_PIN    5
#define LED0_WATTS  10

#define LED1_PIN    6
#define LED1_WATTS  10

#define LED2_PIN    7
#define LED2_WATTS  10

#define LED3_PIN    8
#define LED3_WATTS  10

#define LED4_PIN    9
#define LED4_WATTS  10

#define LED5_PIN    10
#define LED5_WATTS  10

#define LED6_PIN    11
#define LED6_WATTS  10

//#define LED7_PIN    12
//#define LED7_WATTS  10

//#define LAMP_TEMP_PIN A1
//#define LAMP_FAN_PIN  5

#define PUMP_PIN        3
#define PUMP_FLOW_PIN   2

using namespace std;
// <iostream> declares cout/cerr, but the application must define them
// because it's up to you what to do with them.
namespace std
{
  ohserialstream cout(Serial);
}

class led{
public:
    led(int pin, int watts = 10) : m_pin(pin) , m_watts(watts) {}
    void switchOn()  {digitalWrite(m_pin, HIGH);m_state=true;}
    void switchOff() {digitalWrite(m_pin, LOW);m_state=false;}
    int  watts()     {return m_watts;}
    bool isOn()      {return m_state;}
private:
    bool   m_state;
    int    m_pin;
    int    m_watts;
};
class fan{
public:
    fan(int pin=-1) : m_pin(pin) {}
private:
    int m_pin;
};

class pump
{
public:
    pump(int pump_pin = -1, int flow_pin = -1)
    {
        m_pumpPin = pump_pin;
        m_flowPin = flow_pin;
    }

    void flow()
    {
        cout << "[DEBUG]Something flew!";
    }

private:
    int m_pumpPin;
    int m_flowPin;
};

class lamp{
public:
    lamp()
    {
    #ifdef LED0_PIN
        led l0(LED0_PIN,LED0_WATTS);
        m_leds.push_back(l0);
    #endif
    #ifdef LED1_PIN
        led l1(LED1_PIN,LED1_WATTS);
        m_leds.push_back(l1);
    #endif
    #ifdef LED2_PIN
        led l2(LED2_PIN,LED2_WATTS);
        m_leds.push_back(l2);
    #endif
    #ifdef LED3_PIN
        led l3(LED3_PIN,LED3_WATTS);
        m_leds.push_back(l3);
    #endif
    #ifdef LED4_PIN
        led l4(LED4_PIN,LED4_WATTS);
        m_leds.push_back(l4);
    #endif
    #ifdef LED5_PIN
        led l5(LED5_PIN,LED5_WATTS);
        m_leds.push_back(l5);
    #endif
    #ifdef LED6_PIN
        led l6(LED6_PIN,LED6_WATTS);
        m_leds.push_back(l6);
    #endif
    #ifdef LED7_PIN
        led l7(LED7_PIN,LED7_WATTS);
        m_leds.push_back(l7);
    #endif

    #ifdef LAMP_FAN_PIN
        m_has_fan = true;
        //m_fan = fan(LAMP_FAN_PIN);
    #else
        m_has_fan = false;
    #endif

    #ifdef LAMP_TEMP_PIN
        m_has_temp = true;
    #else
        m_has_temp = false;
    #endif
    }
    vector<led>&   leds()  {return m_leds;}
    bool        has_fan()  {return m_has_fan;}
    bool has_tempSensor()  {return m_has_temp;}
    void switchOn()
    {
        for(int i = 0 ; i < m_leds.size() ; i++)
        {
            m_leds[i].switchOn();
            cout << "[INFO]Lamp power: <<" << activePower() << "\n";
            delay(100);
        }
    }
    void switchOff()
    {
        for(int i = 0 ; i < m_leds.size() ; i++)
        {
            m_leds[i].switchOff();
            cout << "[INFO]Lamp power: <<" << activePower() << "\n";
            delay(50);
        }
    }
    void test()
    {
        for(int i = 0 ; i<m_leds.size() ; i++)
        {
            m_leds[i].switchOn();
            delay(200);
            m_leds[i].switchOff();
        }
    }
    int activePower()
    {
        int power = 0;
        for(int i = 0 ; i < m_leds.size() ; i++)
        {
            if(m_leds[i].isOn()) power += m_leds[i].watts();
        }
        return power;
    }
    int totalPower()
    {
        int power = 0;
        for(int i = 0 ; i < m_leds.size() ; i++)
        {
            power += m_leds[i].watts();
        }
        return power;
    }

    void check()
    {

    }

private:
    vector<led> m_leds;
    bool        m_has_fan;
    fan         m_fan;
    bool        m_has_temp;
};

lamp greenHouseLamp;
pump greenHousePump;

void sendData()
{
    cout << "[DATA]" <<
            "LAMP_TOTALPOWER:"      << greenHouseLamp.totalPower()  <<
            "LAMP_ACTIVEPOWER:"     << greenHouseLamp.activePower() <<
            "\n";
}

void setup()
{
    cout << "[INFO]INIT!\n";
    Serial.begin(115200);
    greenHouseLamp = lamp();
#ifdef PUMP_PIN
    attachInterrupt(0,flow,RISING);
#endif

    cout << "[INFO]Lamp has "<<greenHouseLamp.leds().size() << " leds\n";
    cout << "[INFO]Lamp has "<<greenHouseLamp.totalPower()<<"Watts\n";
    if(greenHouseLamp.has_fan()) cout << "[INFO]Lamp has fan control on pin: \n";
    else cout << "[INFO]Lamp DOESN'T have fan control!\n";
    if(greenHouseLamp.has_tempSensor()) cout << "[INFO]Lamp has temp sensor on pin:\n";
    else cout << "[INFO]Lamp DOESN'T have temp sensor!\n";
    greenHouseLamp.test();
    greenHouseLamp.switchOn();
    greenHouseLamp.switchOff();
}

void loop()
{
    greenHouseLamp.check();
    sendData();
    delay(1000);
}

