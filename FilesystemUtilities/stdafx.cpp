///\copyright 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file stdafx.cpp

#include "stdafx.h"

#pragma once


#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )
#pragma message( "_WIN32_WINNT=" SV_STRINGIZE( _WIN32_WINNT ) )

