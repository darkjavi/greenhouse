#include <PID_v1.h>

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
#include </home/darkjavi/src/greenhouse/arduino/greenHouseController/PID_v1/PID_v1.h>

#include <math.h>
class acs712
{
public:
    acs712(int pin = -1, unsigned short volts = 220)  : m_pin(pin), m_volts(volts)
    {
        if(m_pin > -1)
            pinMode(m_pin,INPUT);
    }
    int rawRead()
    {
        if(m_pin < 0)
            return -1;
        return analogRead(m_pin);
    }

    int interpolatedRead()
    {
        if(m_pin<0)
          return -1;
        int interpolatedReads = 10;
        double avgRead = 0;
        for (int i = 0 ; i < interpolatedReads ; i++)
        {
            int value = abs(rawRead() -511);
            avgRead = ((avgRead * i) + value) /(i+1);
        }
        return avgRead;
    }

    int currentPower()
    {
        int maxSensorPower = 30000; //30A
        double powerStep = maxSensorPower / (512);
        double avgRead = 0;
        for (int i = 0 ; i < 20 ; i++)
        {
            avgRead = ((avgRead * i) + interpolatedRead()) /(i+1);
        }
        double miliamps =  avgRead * powerStep;
        int watts =  m_volts * miliamps / 1000; //( P = V * I);
        if(watts < 10) return 0;
        return watts;
    }

    bool isValid()
    {
        if ( (m_pin < 0))
            return false;
        else
            return true;
    }

private:
    int m_pin;
    unsigned short m_volts;
};

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
        m_avgTemp = 20;
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

    int  currentTemp()
    {
        if(m_sensorPin<0)
            return -1;
        double temp = thermister(interpolatedRead());
        m_avgTemp = ((m_avgTemp *9)+temp)/10;
        if(isnan(temp))
            return -1;
        return temp;
    }

    bool isValid()  
    {
      if(m_sensorPin<0)
        return false;
      if(currentTemp() < TEMP_SENSOR_MIN_TEMP)
        return false;
      return true;
    }
private:
    int m_sensorPin;
    double m_avgTemp;
    int rawRead()
    {
        if(m_sensorPin<0)
          return -1;

        return analogRead(m_sensorPin);
    }

    int interpolatedRead()
    {
        if(m_sensorPin<0)
          return -1;
        int interpolatedReads = 200;
        double avgRead = 0;
        for (int i = 0 ; i < interpolatedReads ; i++)
        {
            avgRead = ((avgRead * i) + rawRead()) /(i+1);
        }
        return avgRead;
    }
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
    pump(int pump_pin = -1)
    {
        m_pumpPin = pump_pin;
        if(m_pumpPin > -1)
            pinMode(m_pumpPin, OUTPUT);
    }
    void pumpMl(int ml)
    {
        analogWrite(m_pumpPin,100);// a 254 pasaban cosas extrañas
        m_mlsRemaining += ml;
        m_pumping = true;
    }

    void flow()
    {
        m_flowSteps++;
    }

    void check()
    {
        Serial.print(m_flowSteps);
        if(m_pumping)
        {
            if(m_flowSteps == 0)
            {
                Serial.println("[WARN] WATER IS NOT FLOWING!");
            }
            double flow = 1000.0f/4380.0f * m_flowSteps;
            m_flowSteps = 0;
            m_pumped+=flow;
            if(m_mlsRemaining <= flow)
            {
                m_pumping = false;
                analogWrite(m_pumpPin,0);
                m_mlsRemaining = 0;
            }
            else
            {
                m_mlsRemaining -= flow;
                Serial.print("[INFO]PUMPED ");
                Serial.print(flow);
                Serial.print("ml . REMAINING");
                Serial.println(m_mlsRemaining);
            }
        }
        else
        {
            if(m_flowSteps == 0)
                return;
            double flow = 1000.0f/4380.0f * double(m_flowSteps);
            m_flowSteps = 0;
            m_pumped+=flow;
            Serial.print("[WARN] UNEXPECTED WATER FLOWING:");
            Serial.print(flow);
            Serial.print("ml");
            Serial.print(m_pumped);
            Serial.print("\n");
        }

    }

private:
    int          m_pumpPin;
    bool         m_pumping;
    double       m_pumped;
    double       m_mlsRemaining;
    unsigned int m_flowSteps;
};

class lamp{
public:
    lamp(int fanPin = -1, int tempPin = -1) :m_fanPID(&m_pidInput, &m_pidOutput, &m_pidSetpoint,consKp, consKi, consKd, DIRECT)
    {
        m_fan = fan(fanPin);
        m_tempSensor = tempSensor(tempPin);
        m_pidSetpoint = LAMP_TARGET_TEMPERATURE;
        m_pidInput = m_tempSensor.currentTemp();
        m_fanPID.SetMode(AUTOMATIC);
    }
    ~lamp(){switchOff();}
    vector<led>& leds()                     {return m_leds;}
    int          fanSpeed()                 {return m_fan.speed();}
    bool         haveFan()                  {return m_fan.isValid();}
    bool         haveTempSensor()           {return m_tempSensor.isValid();}
    int          temperature()              {return m_tempSensor.currentTemp();}
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
            delay(500);
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
            delay(200);
        }
    }
    void test()
    {
        m_fan.setSpeed(255);
        for(int i = 0 ; i<m_leds.size() ; i++)
        {
            m_leds[i].switchOn();
            delay(2000);
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
            int currentTemp = m_tempSensor.currentTemp();
			if(currentTemp < LAMP_MIN_TEMPERATURE)
				m_fan.setSpeed(0);
			else if (currentTemp > LAMP_CRITICAL_TEMP)
			{
				m_fan.setSpeed(255);
				switchOff();
			}
            else
            {
                m_pidInput = currentTemp;
                double gap = abs(m_pidSetpoint-m_pidInput); //distance away from setpoint
                if(gap<10)
                {  //we're close to setpoint, use conservative tuning parameters
                  m_fanPID.SetTunings(consKp, consKi, consKd);
                }
                else
                {
                   //we're far from setpoint, use aggressive tuning parameters
                   m_fanPID.SetTunings(aggKp, aggKi, aggKd);
                }
                m_fanPID.Compute();
                Serial.print("PID output");
                Serial.print(m_pidOutput);
                Serial.print("\n");
                //m_fan.setSpeed(m_pidOutput);
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
    double m_pidSetpoint, m_pidInput, m_pidOutput;                                          //I/O for PID
    PID m_fanPID;
    //static const double aggKp=40, aggKi=2, aggKd=10;                                     //original: aggKp=4, aggKi=0.2, aggKd=1, Aggressive Turning,50,20,20
    //static const double consKp=20, consKi=1, consKd=5;                                  //original consKp=1, consKi=0.05, consKd=0.25, Conservative Turning,20,10,10
    static const double aggKp=4, aggKi=0.2, aggKd=1;
    static const double consKp=1, consKi=0.05, consKd=0.25;
    //PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, REVERSE);
};
