//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketLibrary
//* .File Name       : $Workfile:   SVSocketLibrary.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:16:58  $
//******************************************************************************
#include "stdafx.h"
#include <winsock2.h>
#include "SVSocketLibrary.h"

SVSocketError::ErrorEnum SVSocketLibrary::Init()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	WSADATA hWSAData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &hWSAData);
	if (iResult != NO_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
	}
	return error;
}

SVSocketError::ErrorEnum SVSocketLibrary::Destroy()
{
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	int iResult = WSACleanup();
	if (iResult != NO_ERROR)
	{
		error = SVSocketError::GetLastSocketError();
	}
	return error;
}

