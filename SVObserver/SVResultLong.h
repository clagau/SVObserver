//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultLong
//* .File Name       : $Workfile:   SVResultLong.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:56:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVResult.h"
#pragma endregion Includes

class SVLongResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVLongResultClass );

public:
	SVLongResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_RESULT_LONG );
	virtual ~SVLongResultClass();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	const GUID & GetInputEmbeddedID() const;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

	SVLongValueObjectClass* getInputLong();

// Data Element(s):
protected:
	// Output
	SVLongValueObjectClass		value;
};

inline const GUID & SVLongResultClass::GetInputEmbeddedID() const
{
	const SVLongValueObjectClass * l_vo = static_cast <SVLongValueObjectClass *> (const_cast<SVLongResultClass *>(this)->getInput());
	if (l_vo)
	{
		return l_vo->GetEmbeddedID();
	}
	else
	{
		return SV_GUID_NULL;
	}
}

