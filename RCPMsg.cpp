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


#include "RCPMsg.h"

RCPMsg::RCPMsg(char *msg , int len)
{
  strncpy(m_msg, msg, RCP_DEFAULT_MSGSZ-1);
  m_len = len;
  
  m_valid = validate();
}

RCPMsg::~RCPMsg()
{
  // nothing
}

bool RCPMsg::isValid() const
{
  return m_valid;
}

bool RCPMsg::validate() const
{
  // Check boundary conditions
  if (m_len <= 0)
    return false;

  // Check the framing
  if (m_msg[0] != '\2' || m_msg[m_len-1] != '\3')
    return false;

  // Check for a valid method
  if (!(m_msg[1] == RCP_GET
    || m_msg[1] == RCP_ACK
    || m_msg[1] == RCP_PUT
    || m_msg[1] == RCP_NAK
    || m_msg[1] == RCP_SIG
    || m_msg[1] == RCP_DATA
    || m_msg[1] == RCP_EXE))
  {
    return false;
  }

  return true;
}

char RCPMsg::method() const
{
  return m_valid ? m_msg[1] : '\0';
}

String RCPMsg::selector() const
{
  String selector;

  if (!m_valid)
    return selector;

  char *pos = strstr(m_msg+2, "/"); // find selector
  if (pos == NULL)
    return selector;

  for (char *p =pos; *p != '?' && *p != '\3'; p++)
    selector += *p;

  return selector;
}

String RCPMsg::query() const
{
  String query;

  if (!m_valid)
    return query;

  char *pos = strstr(m_msg+2, "?"); // find query
  if (pos == NULL)
    return query;

  for (char *p =pos+1; *p != '\3'; p++)
    query += *p;

  return query;
}

String RCPMsg::message() const
{
  String message;

  if (!m_valid)
    return message;

  for (char *p =(char *)m_msg+1; *p != '\3'; p++)
    message += *p;

  return message;
}

String RCPMsg::header() const
{
  String header;

  if (!m_valid)
    return header;

  char *pos = strstr(m_msg+2, ";"); // find header end
  if (pos == NULL)
    return header;

  for (char *p =(char *)m_msg+1; p <= pos; p++)
    header += *p;

  return header;
}

String RCPMsg::createReply() const
{
	String reply;
	char m = method();

	if (m == RCP_GET)
	{
		// RCP_GET requires an RCP_ACK response
    reply += header();
		reply.setCharAt(0, RCP_ACK);
	}
	else if (m == RCP_PUT)
	{
		// RCP_PUT requires an RCP_SIG response
		reply += message(); // reply with the requesed state
		reply.setCharAt(0, RCP_SIG);
	}
	else // all other get a NAK
	{
		reply += header();
		reply.replace(";", ",rc=405;");
		reply.setCharAt(0, RCP_NAK);
	}
	
  return reply;
}

// End of file

