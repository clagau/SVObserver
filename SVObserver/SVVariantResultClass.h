//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantResultClass
//* .File Name       : $Workfile:   SVVariantResultClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:19:34  $
//******************************************************************************

#if !defined(AFX_SVVARIANTRESULTCLASS_H__CA11DFAC_CF8C_4EAB_AC1B_52A03437666D__INCLUDED_)
#define AFX_SVVARIANTRESULTCLASS_H__CA11DFAC_CF8C_4EAB_AC1B_52A03437666D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"
#include "SVValueObject.h"

class SVVariantResultClass : public SVResultClass  
{
public:
	SVVariantResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_VARIANT );
	virtual ~SVVariantResultClass();
	SV_DECLARE_CLASS( SVVariantResultClass );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVValueObjectClass* GetInputValue();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	// Input
	SVInObjectInfoStruct		m_InputValueObjectInfo;

	// Output
	SVVariantValueObjectClass	m_Value;
};

#endif // !defined(AFX_SVVARIANTRESULTCLASS_H__CA11DFAC_CF8C_4EAB_AC1B_52A03437666D__INCLUDED_)

