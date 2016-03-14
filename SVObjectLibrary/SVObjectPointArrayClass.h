//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectPointArrayClass
//* .File Name       : $Workfile:   SVObjectPointArrayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:58:18  $
//******************************************************************************

#pragma once

#ifdef SV_SHOW_INCLUDES
	#pragma message( "Including " __FILE__ )
#endif

#include "SVUtilityLibrary/SVPOINT.h"
#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"

typedef SvCl::SVObjectArrayClassTemplate<SVPOINT> SVObjectSVPointArrayClass;

