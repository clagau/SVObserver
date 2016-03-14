//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectDPointArrayClass
//* .File Name       : $Workfile:   SVObjectDPointArrayClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:50:22  $
//******************************************************************************

#pragma once

#ifdef SV_SHOW_INCLUDES
	#pragma message( "Including " __FILE__ )
#endif

#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"

typedef SvCl::SVObjectArrayClassTemplate<SVDPointClass> SVObjectDPointArrayClass;

