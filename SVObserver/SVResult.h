//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResult.h
//* .File Name       : $Workfile:   SVResult.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Jan 2015 16:43:46  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVVector.h"
#include "ISVCancel.h"
#include "SVOCore/SVTaskObjectList.h"
#pragma endregion Includes

class SVRangeClass;
class SVIPDoc;

enum
{
	SV_DEFAULT_MAX_BLOB_NUMBER			= 100,
	SV_DEFAULT_TEXT_HEIGHT				= 20,
	SV_DEFAULT_NUMBER_OF_INTENSITIES	= 256,
	SV_NUMBER_OF_MATCH_STRINGS			= 1,
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : Base Class of module SVResult 
////////////////////////////////////////////////////////////////////////////////

class SVResultClass : public SVTaskObjectListClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVResultClass );

public:
	SVResultClass ( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = nullptr, 
		int StringResourceID = IDS_CLASSNAME_SVRESULT 
	);

	virtual ~SVResultClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	// ISVCancel interface
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

	BOOL IsFailed();
	BOOL IsWarned();
	BOOL IsGood();

	// derived class overrides...

	virtual SVRangeClass* GetResultRange();
	virtual CRect Draw( HDC DC, CRect R );

	virtual BOOL Run(SVRunStatusClass& RRunStatus);
	
	
	// Only valid for single input Results that can use the inputObjectInfo 
	// shortcut.
	SVValueObjectClass* getInput();


protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	void init();

public:
	// Input: Pointer to the Input Object
	// This gives an easy shortcut to the input object.  Really only useful 
	// with objects that KNOW they only have a single input object (otherwise walk 
	// the input object list).  Objects that can use this are Double and Long.
	SVInObjectInfoStruct		inputObjectInfo;

protected:
	// Passed, if TRUE ( Reset Value: FALSE )
	SVBoolValueObjectClass	passed;
	// Warned, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	warned;
	// Failed, if TRUE ( Reset Value: TRUE )
	SVBoolValueObjectClass	failed;
};

class SVAnalyzeFeatureClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVAnalyzeFeatureClass );

public:
	SVAnalyzeFeatureClass( LPCSTR ObjectName = "Unknown Analyze Feature" );
	virtual ~SVAnalyzeFeatureClass();

	BOOL Create( LPCSTR ObjectName, long F );

	BOOL IsFailed() const;
	BOOL IsWarned() const;

	long getFeature() const;

protected:
	long feature;
	double* resultArray;
	SVRangeClass* featureResultRange;
	int resultArraySize;
	BOOL failed;
	BOOL warned;
	char* resultString;
};
