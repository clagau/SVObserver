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

#include "SVContainerLibrary/SVVector.h"
#include "ISVCancel.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVTaskObjectList.h"

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
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:06.06.1998 RO			Supplements for sub results -> change deriving from
//							CObject to CArray
//	:11.06.1998 RO			Add Calculate( SVDataObjectClass* ) member function
//	:19.01.1999 RO			Supplements for 3.0, Change Request ...
//							Is now derived from SVTaskObjectClass instead of
//							SVObjectClass 
////////////////////////////////////////////////////////////////////////////////
class SVResultClass : public SVTaskObjectListClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVResultClass );

public:
	SVResultClass ( 
		BOOL BCreateDefaultTaskList = FALSE, 
		SVObjectClass* POwner = NULL, 
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
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

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

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVAnalyzeFeatureClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVAnalyzeFeatureClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVAnalyzeFeatureClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVAnalyzeFeatureClass( LPCSTR ObjectName = "Unknown Analyze Feature" );
// Standard destructor
	virtual ~SVAnalyzeFeatureClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
	BOOL Create( LPCSTR ObjectName, long F );

	void create( int MaxNumber = SV_DEFAULT_MAX_BLOB_NUMBER, SVRangeClass* R = NULL );

////////////////////////////////////////////////////////////////////////////////
// is... Operator
////////////////////////////////////////////////////////////////////////////////
	BOOL IsFailed();
	BOOL IsWarned();

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************
public:
	void setFeatureResultRange( SVRangeClass* R );

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	SVRangeClass*	getFeatureResultRange();
	double*			getResultArray();
	long			getFeature();
	TCHAR*			GetResultArrayString( int N );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	long			feature;
	double*			resultArray;
	SVRangeClass*	featureResultRange;
	int				resultArraySize;
	BOOL			failed;
	BOOL			warned;

	char*			resultString;
};

