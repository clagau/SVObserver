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

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVRESULTLONG_H
#define SVRESULTLONG_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"
#include "SVValueObject.h"

class SVLongResultClass : public SVResultClass  
{
	SV_DECLARE_CLASS( SVLongResultClass );

public:
	SVLongResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_LONG );
	virtual ~SVLongResultClass();

public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

	const GUID & GetInputEmbeddedID() const;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

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
		return SVInvalidGUID;
	}
}


#endif	// SVRESULTLONG_H

