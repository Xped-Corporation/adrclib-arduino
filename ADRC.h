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

#ifndef ADRC_H
#define ADRC_H

#include <Arduino.h>
#include "RCPMsg.h"


class ADRC
{
public:
  ADRC(HardwareSerial& port);
  ~ADRC();

  // Shield API
  void begin(long baud = 38400);
  bool available();
  RCPMsg read();
  void write(char *data, char msg_type);
  bool setRML(const char *rml);

  // TODO Simple FileIO API
  //bool fileExists(char unit, const char *fileName);
  //bool fileDelete(char unit, const char *fileName);
  //int fileRead(char unit, const char *fileName, char *buf, int len);
  //int fileWrite(char unit, const char *fileName, int mode, const char *data, int datalen);

protected:
  typedef enum
  {
    RCP_WAITING_STX,
    RCP_WAITING_TYPE,
    RCP_RECV_FRAME,
    RCP_RECV_DFRAME
  } 
  RcpState;

enum
{
  FRAME_IDLE,
  FRAME_PREAMBLE,
  FRAME_SOF,
  FRAME_LENGTH,
  FRAME_MSG_ID,
  FRAME_DST,
  FRAME_SRC,
  FRAME_PAYLOAD,
  FRAME_CS,
};

private:
  HardwareSerial& _Serial;
  //
  byte state;
  byte msg_length;
  byte msg_id;
  byte cs;
  char msg_payload[RCP_DEFAULT_MSGSZ];
  byte n;
};

#endif // ADRC_H

// End of file

