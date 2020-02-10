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
#pragma endregion Includes

class SVObjectClass;

namespace SvPb
{
	enum ClassIdEnum;
}

class SVClassRegisterClass
{ 
public: 
	SVClassRegisterClass();
	virtual ~SVClassRegisterClass();

	virtual SvPb::ClassIdEnum GetClassID() const;
	
	virtual SVObjectClass* Construct();

	virtual LPCTSTR GetClassName() const;
};	

