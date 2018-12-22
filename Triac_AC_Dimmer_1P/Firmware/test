//#include <EEPROM.h>
int dimming=100,ac_dimm;
char incomB='0';
String openhab="";


void setup()
{
	Serial.begin(9600);
	Serial.println("Setup...");
	pinMode(3,OUTPUT);                        // Set AC Dimmer
	delay(1000);
	Serial.println("Start 0-255#");
	attachInterrupt(0, start_dimming, RISING); //pin 2
	myPrint();
}

void loop()
{
	myIncoming();

}

void myIncoming()
{
	if(Serial.available()>0)
	{
		incomB=Serial.read();
		if(incomB=='\n' || incomB=='#')
		{
			if(openhab.toInt()>=0 && openhab.toInt()<256)
			{
				dimming=openhab.toInt();
			}
			openhab="";
			myPrint();
		}
		else
			openhab+=incomB;
	}
}


void start_dimming()
{
	if(dimming>ac_dimm)
		ac_dimm++;
	if(dimming<ac_dimm)
		ac_dimm--;
	if(ac_dimm>1)
	{
		delayMicroseconds(999);
		delayMicroseconds(31*(256-ac_dimm));
		digitalWrite(3, HIGH);
		delayMicroseconds(20);
		digitalWrite(3, LOW);
		//Serial.println("test");
	}
}

void myPrint()
{
	Serial.print("dimming = ");
	Serial.println(dimming);
}
