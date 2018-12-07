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

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

class SVObjectClass;

class SVClassRegisterClass
{ 
public: 
	SVClassRegisterClass();
	virtual ~SVClassRegisterClass();

	virtual SVGUID GetClassID() const;
	
	virtual SVObjectClass* Construct();

	virtual LPCTSTR GetClassName() const;
};	

