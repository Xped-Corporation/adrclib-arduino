#include <Arduino.h>
#include <ADRC.h>

//#define DEBUG

ADRC Adrc(Serial);

#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial Debug(2,3); // RX, TX
#endif

// State variables
//
int on = 0;

// State functions
//
void get_all_state(RCPMsg& msg)
{
  char buf[16];
  String reply = msg.createReply();

  reply += String("/on?");
  reply += String(on);

  reply.toCharArray(buf,16,0);
  Adrc.write(buf, RCP_ACK);
}

void put_all_state(RCPMsg& msg)
{
  char buf[16];
  String reply = msg.createReply();
  String selector = msg.selector();
  int value = msg.query().toInt();

  // Control the hardware
  if (selector == String("/on"))
  {
    on = value;
    digitalWrite(13, on);
  }

  // Reply to the client
  reply.toCharArray(buf,16,0);
  Adrc.write(buf, RCP_SIG);
}

void setup()
{
  Adrc.begin(115200);

  pinMode(13,OUTPUT); // LED pin
  digitalWrite(13,0);

#ifdef DEBUG
  Debug.begin(38400);
#endif
}

void loop()
{
#ifdef DEBUG
  Debug.write("Hello world!");
#endif

  while(true)
  {
    if (Adrc.available())
    {
      RCPMsg msg = Adrc.read();
      //
#ifdef DEBUG
      Debug.print(String("method=") + String(msg.method()));
      Debug.print(String("selector=") + msg.selector());
      Debug.print(String("query=") + msg.query());
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
  }
}

// End of file

