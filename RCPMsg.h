// This module defines the RCPMsg class of the ADRC Arduino Library.
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

#ifndef RCPMSG_H
#define RCPMSG_H

#include <Arduino.h>

// Default RCP buffer size
#define RCP_DEFAULT_MSGSZ 96

// Define the RCP method frame types
#define RCP_GET 'g'
#define RCP_PUT 'p'
#define RCP_SIG 's'
#define RCP_EXE 'x'

// To be depricated in proxy V1.0
#define RCP_ENUM 'e'
#define RCP_FILE 'f'

// Define the RCP operational frame types
#define RCP_ACK 'a'
#define RCP_NAK 'n'
#define RCP_DATA 'd'


class RCPMsg
{
public:
  RCPMsg(char *msg , int len);
  ~RCPMsg();

  bool isValid() const;
  String createReply() const;

  char method() const;
  // TODO String attribute(char *name) const;
  String selector() const;
  String query() const;
  String message() const;
  String header() const;

protected:
  bool validate() const;
  
private:
  char m_msg[RCP_DEFAULT_MSGSZ]; // messages cannot be longer than this
  int m_len; // length of contents of m_msg
  bool m_valid;
};

#endif // RCPMSG_H

// End of file

