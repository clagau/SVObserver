//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResult.cpp
//* .File Name       : $Workfile:   SVResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   14 Jan 2015 16:43:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResult.h"
#include "SVGlobal.h"
#include "SVRange.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct SVResultClassCancelData : public SVCancelData	// this does not need to be visible to anyone but this file.
{
	SVResultClassCancelData() {m_pRangeData = nullptr;}
	virtual ~SVResultClassCancelData() {if (m_pRangeData) delete m_pRangeData;}
	SVCancelData* m_pRangeData;
};

SV_IMPLEMENT_CLASS( SVResultClass, SVResultClassGuid );

////////////////////////////////////////////////////////////////////////////////
// .Title       :  SVResultClass
// -----------------------------------------------------------------------------
// .Description : Standard constructor
////////////////////////////////////////////////////////////////////////////////

SVResultClass::SVResultClass ( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVResultClass::init()
{
	m_bUseOverlays = false;

	// Identify our type in the Output List
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SVResetItemNone );
	RegisterEmbeddedObject( &warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SVResetItemNone );
	RegisterEmbeddedObject( &passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SVResetItemNone );

	// Set Embedded defaults
	passed.SetDefaultValue( FALSE, TRUE );			// Default for Passed is FALSE !!!
	failed.SetDefaultValue( TRUE, TRUE );			// Default for Failed is TRUE !!!
	warned.SetDefaultValue( TRUE, TRUE );			// Default for Warned is TRUE !!!

	// Set up the Default Inputs/Outputs
	addDefaultInputObjects();
}

SVResultClass::~SVResultClass()
{
}

BOOL SVResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = true;
	}

	// Set / Reset Printable Flags
	failed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	warned.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	passed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

DWORD_PTR SVResultClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;

	return( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

BOOL SVResultClass::IsFailed()
{
	BOOL RVal = TRUE;
	failed.GetValue( RVal );
	return( RVal );
}

BOOL SVResultClass::IsWarned()
{
	BOOL RVal = TRUE;
	warned.GetValue( RVal );
	return( RVal );
}

BOOL SVResultClass::IsGood()
{
	BOOL RVal = TRUE;
	passed.GetValue( RVal );
	return( RVal );
}

SVRangeClass* SVResultClass::GetResultRange()
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVRangeObjectType;
	SVRangeClass* pRange = reinterpret_cast<SVRangeClass*>(SVSendMessage( this, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&info) ));
	return pRange;
}

CRect SVResultClass::Draw( HDC DC, CRect R )
{
	return R;
}

BOOL SVResultClass::Run( SVRunStatusClass& RRunStatus )
{
	SVValueObjectClass* pValue = getInput();
	
	long	resultSize = -1;
	if (nullptr != pValue)
	{
		resultSize = pValue->GetResultSize();
	}

	// This is testing to verify that a result was found.  Analyzers such as 
	// Blob, Character, and Barcode deal with variable numbers of valid 
	// results, and need to deal with the possibillity of not finding blobs,
	// characters, or getting a barcode read.
	if (0 == resultSize)
	{
		// no results to process.
		// if there are no results outside the range, we want the ResultObject
		// to pass.

		//make sure the statusColor is set correctly
		DWORD dwColor = RRunStatus.GetStatusColor();
		statusColor.SetValue(RRunStatus.m_lResultDataIndex, dwColor);

		passed.SetValue( RRunStatus.m_lResultDataIndex, true );
		failed.SetValue( RRunStatus.m_lResultDataIndex, false );
		warned.SetValue( RRunStatus.m_lResultDataIndex, false );
		return TRUE;
	}
	else
	{
		// valid results to process.
		if( SVTaskObjectListClass::Run(RRunStatus) )
		{
			//make sure statusColor is set correctly
			DWORD dwColor = RRunStatus.GetStatusColor();
			statusColor.SetValue(RRunStatus.m_lResultDataIndex, dwColor);

			// set our state according to the runStatus
			passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
			failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

			return TRUE;
		}
	}

	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

SVValueObjectClass* SVResultClass::getInput()
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast <SVValueObjectClass*> ( inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}


BOOL SVResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVTaskObjectListClass::onRun( RRunStatus ) )
	{
		return TRUE;
	}

	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

bool SVResultClass::CanCancel()
{
	return true;
}

HRESULT SVResultClass::GetCancelData(SVCancelData*& rpCancelData)
{
	ASSERT(nullptr == rpCancelData);
	SVResultClassCancelData* pData = new SVResultClassCancelData;
	rpCancelData = pData;

	if ( SVRangeClass* pRange = GetResultRange() )
	{
		pRange->GetCancelData( pData->m_pRangeData );
	}

	return S_OK;
}

HRESULT SVResultClass::SetCancelData(SVCancelData* pCancelData)
{
	SVResultClassCancelData* pData = dynamic_cast<SVResultClassCancelData*> (pCancelData);
	if ( pData )
	{
		if ( SVRangeClass* pRange = GetResultRange() )
		{
			return pRange->SetCancelData( pData->m_pRangeData );
		}
		else
		{
			return S_OK;
		}
	}
	else
	{
		return S_FALSE;
	}
}

SV_IMPLEMENT_CLASS( SVAnalyzeFeatureClass, SVAnalyzeFeatureClassGuid );

SVAnalyzeFeatureClass::SVAnalyzeFeatureClass( LPCSTR ObjectName )
					  :SVTaskObjectClass( ObjectName )
{
	feature	= 0L;
	resultArray = nullptr;
	featureResultRange = nullptr;
	resultArraySize = 0;
	failed = true;
	warned = true;

	resultString = nullptr;
}

SVAnalyzeFeatureClass::~SVAnalyzeFeatureClass()
{
	if( featureResultRange )
		delete( featureResultRange );

	if( resultArray )
		delete( resultArray );

	if( resultString )
		delete( resultString );
}

BOOL SVAnalyzeFeatureClass::Create( LPCSTR ObjectName, long F )
{
	ASSERT( ObjectName != _T( "" ) );

	if( ObjectName != _T( "" ))
	{
		SetName( ObjectName );
		feature = F;
		return true;
	}
	return false;
}

BOOL SVAnalyzeFeatureClass::IsFailed() const
{
	return failed;
}

BOOL SVAnalyzeFeatureClass::IsWarned() const
{
	return warned;
}

long SVAnalyzeFeatureClass::getFeature() const
{
	return feature;
}
