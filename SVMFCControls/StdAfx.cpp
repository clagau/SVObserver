//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:57:34  $
//******************************************************************************

#include "stdafx.h"

#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )
#pragma message( "_WIN32_WINNT=" SV_STRINGIZE( _WIN32_WINNT ) )

