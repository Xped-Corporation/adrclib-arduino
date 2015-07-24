#include <Arduino.h>
#include <ADRC.h>

/* Demonstrate all RML widgets */

//#define DEBUG

ADRC Adrc(Serial);

#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial Debug(2,3); // RX, TX
#endif

unsigned long trigger_time;
unsigned long val_time;
unsigned long text_time;
boolean led = false;

typedef struct
{
	const char* name;
	int value;
	int min;
	int max;
	boolean changed;
} state_var;

state_var vars[] =
{
//enum elements
	{"/sp",0,0,1,true},
	{"/lg",0,0,1,true},
	{"/sw",0,0,1,true},
	{"/ec",0,0,1,true},
	{"/bk",0,0,1,true},
	{"/st",0,0,3,true},
	{"/fl",0,0,3,true},
	
// range elements
	{"/led",0,0,15,true},
	{"/prg",0,0,135,true},
	{"/rem",0,0,135,true},
	{"/cdn",0,0,90,true},
	{"/bdp",0,0,90,true},
};
#define N_VARS (sizeof(vars)/sizeof(state_var))

char *msg_strings[] =
{
	"The quick",
	"brown fox",
	"jumps over",
	"the lazy",
	"dog",""
};
#define N_STRINGS 6

int textval = 0;

#define LED_PIN    13

// State functions
//
void get_all_state(RCPMsg& msg)
{
	char buf[96];
	byte n;
	
	String reply = msg.createReply();

	for(n=0;n<N_VARS;n++)
	{
		reply += String(vars[n].name);
		reply += String(vars[n].value);
		if( n < N_VARS-1 )
			reply += "\n";
		vars[n].changed = false;
	}

	reply.toCharArray(buf,96,0);
	Adrc.write(buf, RCP_ACK);
}

void put_all_state(RCPMsg& msg)
{
	char buf[16];
	byte n;
	
	String reply = msg.createReply();
	String selector = msg.selector();
	int value = msg.query().toInt();

	// Control the hardware
	// triggers
	if (
		selector == String("/vu") ||
		selector == String("/vd") ||
		selector == String("/cu") ||
		selector == String("/cd") ||
		selector == String("/gd") ||
		selector == String("/ok") ||
		selector == String("/ip") ||
		selector == String("/up") ||
		selector == String("/dn") ||
		selector == String("/lf") ||
		selector == String("/rg") ||
		selector == String("/sl") )
	{
		led = 1;
		trigger_time = millis();
		digitalWrite(LED_PIN, led);
	//	return;
	}
	else
	{
		for(n = 0; n < N_VARS; n++)
		{
			if( selector == String(vars[n].name) )
			{
				vars[n].value = value;
				break;
			}
		}
	}
	
	// Reply to the client
	reply.toCharArray(buf,16,0);
	Adrc.write(buf, RCP_SIG);
}

void signal_val(byte n)
{
	char buf[16];

	sprintf(buf, "s,u=0;%s?%d", vars[n].name, vars[n].value);
	vars[n].changed = false;
	Adrc.write(buf, RCP_SIG);

#ifdef DEBUG
	Debug.write(buf);
#endif
}

void send_state(void)
{
	char buf[96];
	byte n;
	String reply = String("s,u=0;");
	
	for(n=0;n<N_VARS;n++)
	{
		if( vars[n].changed )
		{
			reply += String(vars[n].name);
			reply += String(vars[n].value);
			reply += "\n";
			vars[n].changed = false;
		}
	}
	reply.trim();
	reply.toCharArray(buf,96,0);
#ifdef DEBUG
	Debug.write(buf);
#endif
	Adrc.write(buf, RCP_SIG);

}

void change_val(void)
{
	static byte n = 0;
	char dir;
	byte newval;
	
	//n = 5;
	
	newval = vars[n].value + 1;
	if( newval > vars[n].max )
		newval = vars[n].min;
	
	vars[n].value = newval;
	signal_val(n);
	
	n++;
	if( n >= N_VARS )
		n = 0;
}
	

void send_text(int n)
{
	char buf[32];
	
	sprintf(buf, "s,u=0;/pre?%s", msg_strings[n]);
	Adrc.write(buf, RCP_SIG);
}
	
void setup()
{
	Adrc.begin(115200);

	pinMode(LED_PIN,OUTPUT); // LED pin
	digitalWrite(LED_PIN,led);
	val_time = millis();
	text_time = millis();
  
	send_state();
	
#ifdef DEBUG
	Debug.begin(38400);
	Debug.write("Hello world!\r\n");
#endif
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
	
	if( led && millis()-trigger_time > 250 )
	{
		led = 0;
		digitalWrite(LED_PIN,0);
	}
	
	if( millis()-val_time > 500 )
	{
		change_val();
		delay(10);
		
		val_time = millis();
	}
	
	if( millis()-text_time > 900 )
	{
		textval++;
		if( textval >= N_STRINGS )
			textval = 0;
		send_text(textval);
		
		text_time = millis();
	}
			
}

// End of file

