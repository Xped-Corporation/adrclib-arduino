// This module defines the main class of the ADRC Arduino Library.
//
// Copyright (c) 2015 Xped Holdings Limited <info@xped.com>
//
// This file is part of the adrclib-arduino repository on GitHub.
//
// This file may be used under the terms of the GNU General Public
// License version 3.0 as published by the Free Software Foundation
// and appearing in the file LICENSE included in the packaging of
// this file. Alternatively you may (at your option) use any later 
// version of the GNU General Public License if such license has been
// publicly approved by Xped Holdings Limited (or its successors,
// if any) and the KDE Free Qt Foundation.
//
// If you are unsure which license is appropriate for your use, please
// contact the sales department at sales@xped.com.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


#include "ADRC.h"


ADRC::ADRC(HardwareSerial& port)
	: _Serial(port)
{
  state = FRAME_PREAMBLE;
  cs = 0;
}

ADRC::~ADRC()
{
}

void ADRC::begin(long baud)
{
  _Serial.begin(baud);
}

RCPMsg ADRC::read()
{
  return RCPMsg(msg_payload, msg_length);
}

void ADRC::write(char *data, char msg_type)
{
  // SIGNAL format:
  // 3A C3 <len> 24 00 00 02 <data> 03 <crc>
  // ACK/NAK format:
  // 3A C3 <len> 20 00 00 02 <data> 03 <crc>

  int len=strlen(data);
  byte type = (msg_type == RCP_SIG) ? 0x24 : 0x20;
  unsigned char crc = 0;
  char hdr[] = {  // allow for STX & ETX
    0x3a, 0xc3, len+2, type, 0x00, 0x00   };

  // Header
  for (int i=0; i < (int)sizeof(hdr); i++)
  {
    if (i > 1)
      crc ^= (unsigned char)*(hdr+i);
    _Serial.write(*(hdr+i));
  }

  // Payload
  _Serial.write('\x02');
  crc ^= 0x02;

  for (int i=0; i < len; i++)
  {
    crc ^= (unsigned char)*(data+i);
    _Serial.write(*(data+i));
  }

  _Serial.write('\x03');
  crc ^= 0x03;

  // CRC
  _Serial.write(crc);
}

bool ADRC::available()
{
  if (!_Serial.available())
    return false;
    
  unsigned char c = _Serial.read();

  switch(state)
  {
  case FRAME_PREAMBLE:
    if(c == 0x3a)
      state++;
    break;
  case FRAME_SOF:
    if(c == 0xc3)
      state++;
    else
      state = FRAME_PREAMBLE;
    break;
  case FRAME_LENGTH:
    msg_length = c;
    cs = c;
    state++;
    break;
  case FRAME_MSG_ID:
    msg_id = c;
    cs ^= c;
    state++;
    break;
  case FRAME_DST:
    cs ^= c;
    state++;
    break;
  case FRAME_SRC:
    cs ^= c;
    state++;
    n = 0;
    break;
  case FRAME_PAYLOAD:
    msg_payload[n] = c;
    n++;
    cs ^= c;
    if( n == msg_length )
    {
      state++;
    }
    break;
  case FRAME_CS:
    state++;
    state = FRAME_PREAMBLE;
    if(cs == c)  // checksum ok
      return true;
    break;
  }

  return false;
}

bool ADRC::setRML(const char *rml)
{
  return false;
}

// End of file

