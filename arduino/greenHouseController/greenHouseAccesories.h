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

#include <math.h>
class led{
public:
    led(int pin, int watts = 10) : m_pin(pin) , m_watts(watts)
    {
        if(m_pin > -1)
            pinMode(pin, OUTPUT);
    }
    void switchOn()  {digitalWrite(m_pin, HIGH);m_state=true;}
    void switchOff() {digitalWrite(m_pin, LOW);m_state=false;}
    int  watts()     {return m_watts;}
    bool isOn()      {return m_state;}
private:
    bool   m_state;
    int    m_pin;
    int    m_watts;
};

class tempSensor{
public:
    tempSensor(int sensorPin = -1)
    {
        m_sensorPin = sensorPin;
        if(m_sensorPin > -1)
            pinMode(m_sensorPin, INPUT);
        m_avgTemp = 30;
    }
    static double thermister(int RawADC)
	{
		double Temp;
		Temp = log(((10240000/RawADC) - 10000));
		Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
		Temp = Temp - 273.15;            // Convert Kelvin to Celcius
		//Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
		return Temp;
	}
    int read()
    {
        if(m_sensorPin<0)
          return -99;
        //if(!isValid())
        //    return 0;

        int value = analogRead(m_sensorPin);
		double temp = thermister(value);
		int tempInt;
		tempInt = temp;
        m_avgTemp = ((m_avgTemp *9)+temp)/10;
	#ifdef DEBUG
		cout << "[DEBUG] Temp pin value:"<< value ;
		cout << " Estimated temp:"<< temp;
		cout << " Average temp:"<< m_avgTemp <<"\n";
	#endif
        if(isnan(temp))
            return -99;

        return temp;
	}
    bool isValid()  
    {
      if(m_sensorPin<0)
        return false;
      if(isnan(read()))
        return false;
      if(read() < TEMP_SENSOR_MIN_TEMP)
        return false;
      return true;
    }
private:
    int m_sensorPin;
    double m_avgTemp;
};

class fan{
public:
    fan(int mosfetPin=-1) : m_pin(mosfetPin)
    {
        if(m_pin > -1)
            pinMode(m_pin, OUTPUT);
    }
    bool isValid()  {return m_pin>=0;}
    void setSpeed(int speed = 255) 
    {
      analogWrite(m_pin,speed);
      m_speed = speed;
    }
    int  speed()	{return m_speed;}
private:
    int m_pin;
    int m_speed;
};

class pump
{
public:
    pump(int pump_pin = -1, int flow_pin_int = -1)
    {
        m_pumpPin = pump_pin;
        m_flowPin = flow_pin_int;
    }

    void flow()
    {
        cout << "[DEBUG]Something flew!\n";
    }

private:
    int m_pumpPin;
    int m_flowPin;
};

class lamp{
public:
    lamp(int fanPin = -1, int tempPin = -1)
    {
        m_fan = fan(fanPin);
        m_tempSensor = tempSensor(tempPin);
    }
    ~lamp(){switchOff();}
    vector<led>& leds()                     {return m_leds;}
    int          fanSpeed()                 {return m_fan.speed();}
    bool         haveFan()                  {return m_fan.isValid();}
    bool         haveTempSensor()           {return m_tempSensor.isValid();}
    int          temperature()              {return m_tempSensor.read();}
    void         addLed(int pin, int watts) 
    {
		m_leds.push_back(led(pin,watts));
	#ifdef DEBUG
		cout << "[DEBUG] Added led on pin:"<<pin<<" - " << watts << "watts\n";
	#endif
    }

    void switchOn()
    {
       m_fan.setSpeed(128);
       for(int i = 0 ; i < m_leds.size() ; i++)
        {
            m_leds[i].switchOn();
        #ifdef DEBUG
            cout << "[DEBUG]Lamp power: " << activePower() << "W\n";
        #endif
            delay(100);
        }
    }
    void switchOff()
    {
        for(int i = 0 ; i < m_leds.size() ; i++)
        {
            m_leds[i].switchOff();
        #ifdef DEBUG
            cout << "[DEBUG]Lamp power: " << activePower() << "W\n";
        #endif
            delay(50);
        }
    }
    void test()
    {
        m_fan.setSpeed(255);
        for(int i = 0 ; i<m_leds.size() ; i++)
        {
            m_leds[i].switchOn();
            delay(200);
            m_leds[i].switchOff();
            m_fan.setSpeed(m_fan.speed()/2);
        }
            m_fan.setSpeed(0);
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
		if(m_tempSensor.isValid())
		{
			int currentTemp = m_tempSensor.read();
			if(currentTemp < LAMP_MIN_TEMPERATURE)
				m_fan.setSpeed(0);
			else if (currentTemp > LAMP_CRITICAL_TEMP)
			{
				m_fan.setSpeed(255);
				switchOff();
			}
			else if(currentTemp > LAMP_MIN_TEMPERATURE)
                        {
                          if(currentTemp > LAMP_TARGET_TEMPERATURE)
                          {
                            m_fan.setSpeed(200);
                          }
                          else
                          {
                            m_fan.setSpeed(128);
                          }
                        }
		}
		else
        {//Si no hay sensor de temperatura ponemos el ventilador
			if(m_fan.isValid())
			{
				m_fan.setSpeed(255);
			}
		}
    }

private:
    vector<led> m_leds;
    fan         m_fan;
    tempSensor  m_tempSensor;
};
