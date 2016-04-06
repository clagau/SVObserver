//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOperator
//* .File Name       : $Workfile:   SVOperator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 11:21:50  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObject.h"
#pragma endregion Includes

class SVOperatorClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVOperatorClass );

public:
	SVOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOPERATOR );
	virtual ~SVOperatorClass();
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

protected:
	virtual DWORD_PTR processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext);

private:
	void init();
};

