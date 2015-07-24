#include <Arduino.h>
#include <ADRC.h>

#define DEBUG

ADRC Adrc(Serial);

#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial Debug(2,3); // RX, TX
#endif

unsigned long adc_time;

// State variables
//
boolean led = false;
boolean button = false;
int adc_val;

#define BUTTON_PIN  8
#define LED_PIN    13
#define ADC_PIN    5

// State functions
//
void get_all_state(RCPMsg& msg)
{
	char buf[48];
	String reply = msg.createReply();

	reply += String("/button?");
	reply += String(button);
	reply += "\n";
	reply += String("/led?");
	reply += String(led);
	reply += "\n";
	reply += String("/adc?");
	reply += String(adc_val);

	reply.toCharArray(buf,48,0);
	Adrc.write(buf, RCP_ACK);
}

void put_all_state(RCPMsg& msg)
{
	char buf[32];
	String reply = msg.createReply();
	String selector = msg.selector();
	int value = msg.query().toInt();

	// Control the hardware
	if (selector == String("/led"))
	{
		led = value;
		digitalWrite(LED_PIN, led);
	}

	// Reply to the client
	reply.toCharArray(buf,32,0);
	Adrc.write(buf, RCP_SIG);
}

void signal_button(void)
{
	char buf[32];

	sprintf(buf, "s,u=0;/button?%d", button==0);
	Adrc.write(buf, RCP_SIG);

#ifdef DEBUG
	Debug.write(buf);
#endif
}

void signal_adc( void )
{
	char buf[16];

	sprintf(buf, "s,u=0;/adc?%d", adc_val);
	Adrc.write(buf, RCP_SIG);

#ifdef DEBUG
	Debug.write(buf);
#endif
}

void send_state(void)
{
	char buf[48];
	
	sprintf(buf, "s,u=0;/button?%d\n/led?%d\n/adc?%d", button,led,adc_val);
#ifdef DEBUG
	Debug.write(buf);
#endif
	Adrc.write(buf, RCP_SIG);

}

void setup()
{
	Adrc.begin(115200);

	pinMode(LED_PIN,OUTPUT); // LED pin
	digitalWrite(LED_PIN,led);
	pinMode(BUTTON_PIN, INPUT_PULLUP);
  
	button = digitalRead(BUTTON_PIN);
	adc_val = analogRead(ADC_PIN);
	send_state();
	
#ifdef DEBUG
	Debug.begin(19200);
	Debug.write("Hello world!\r\n");
#endif
	adc_time = millis();
}

void loop()
{
	if (Adrc.available())
	{
		RCPMsg msg = Adrc.read();
		//
	#ifdef DEBUG
		Debug.println(String("method=") + String(msg.method()));
		Debug.println(String("selector=") + msg.selector());
		Debug.println(String("query=") + msg.query());
	#endif
		//
		if (msg.isValid())
		{
			if (msg.method() == RCP_GET)
			get_all_state(msg);
			else if (msg.method() == RCP_PUT)
			put_all_state(msg);
		}
	}
	
	if( digitalRead(BUTTON_PIN) != button )
	{
		delay(100);
		button = digitalRead(BUTTON_PIN);
		signal_button();
	}
	
	if( millis()-adc_time > 500 )
	{
		int val = analogRead(ADC_PIN);
		if( val != adc_val )
		{
			adc_val = val;
			signal_adc();
		}
		adc_time = millis();
	}
			
}

// End of file

