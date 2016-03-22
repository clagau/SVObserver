//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassRegisterClass
//* .File Name       : $Workfile:   SVClassRegisterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:22  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVGUID.h"

class SVObjectClass;

class SVClassRegisterClass
{ 
public: 
	SVClassRegisterClass();
	
	virtual SVGUID GetClassID();
	
	virtual SVObjectClass* Construct();

	virtual LPCTSTR GetClassName();
};	

