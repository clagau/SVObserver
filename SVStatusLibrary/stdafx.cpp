//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:41:04  $
//******************************************************************************

#include "stdafx.h"

#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )
#pragma message( "_WIN32_WINNT=" SV_STRINGIZE( _WIN32_WINNT ) )

