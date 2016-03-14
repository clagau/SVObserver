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

#include "stdafx.h"
#include "SVResult.h"
#include "SVGlobal.h"
#include "SVRange.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL MACROS:
//******************************************************************************
//#define WIT_svWitFontParams(o) ((SVWitFontParams *) ((o)->val.data))
//#define WIT_svImVec(sv) ((WitImageVector *) &((sv)->imVec))
//#define WIT_svStrVec(sv) ((StringVector *) &((sv)->strVec))

struct SVResultClassCancelData : public SVCancelData	// this does not need to be visible to anyone but this file.
{
	SVResultClassCancelData() {m_pRangeData = NULL;}
	virtual ~SVResultClassCancelData() {if (m_pRangeData) delete m_pRangeData;}
	SVCancelData* m_pRangeData;
};

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVResultClass
//* Note(s)    : Base Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVResultClass, SVResultClassGuid );

////////////////////////////////////////////////////////////////////////////////
// .Title       :  SVResultClass
// -----------------------------------------------------------------------------
// .Description : Standard constructor
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :08.07.1998 RO			First Implementation
//	:19.01.1999 RO			Supplements for 3.0, Change Request ...
//							Calls now SVTaskObjectClass constructor instead of
//							SVObjectClass constructor
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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;

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
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		bOk = TRUE;
	}

	// Set / Reset Printable Flags
	failed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	warned.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	passed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

DWORD_PTR SVResultClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

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
	SVRangeClass* pRange = reinterpret_cast<SVRangeClass*>(SVSendMessage( this, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ));
	return pRange;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Draw
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:06.06.1998 RO          Supplement, look for sub results too
////////////////////////////////////////////////////////////////////////////////
CRect SVResultClass::Draw( HDC DC, CRect R )
{
	/* SEJ (OLD)
	CString strText;
	SVObjectClass* pObject = GetOwnerInfo().PObject;
	if( GetSize() <= 0 )
	{
		::SetTextColor( DC, SV_DEFAULT_INACTIVE_COLOR );
		if( pObject )
			strText = _T( "( " ) + pObject->GetName() + _T( ", No Result )" );
		else
			if( pOwnerTool )
				strText = _T( "( " ) + pOwnerTool->GetName() + _T( ", No Result )" );
			else
				strText = _T( "( Unknown Owner, No Result )" );

		::TextOut( DC, R.left, R.top, strText, strText.GetLength() );
		
		R.top += SV_DEFAULT_TEXT_HEIGHT + 2;
	}
	else
	{
		// Store left position...
		long left = R.left;

		if( pOwnerTool && pOwnerTool->GetAnalyzer() &&
			pOwnerTool->GetAnalyzer()->GetResultObject() == this )
		{
			::SetTextColor( DC, SV_DEFAULT_TEXT_COLOR );
			strText = pOwnerTool->GetName();
			::TextOut( DC, R.left, R.top, strText, strText.GetLength() );
			R.top += SV_DEFAULT_TEXT_HEIGHT;

			// Indent
			R.left += SV_DEFAULT_TEXT_HEIGHT;
		}

		if( pObject )
		{
			::SetTextColor( DC, SV_DEFAULT_TEXT_COLOR );
			strText = _T( "." ) + pObject->GetName();
			::TextOut( DC, R.left, R.top, strText, strText.GetLength() );
			R.top += SV_DEFAULT_TEXT_HEIGHT;

			// Indent
			R.left += SV_DEFAULT_TEXT_HEIGHT;
		}

		// Draw all sub results...
		for( int i = 0; i < GetSize(); ++i )
			if( GetAt( i ) )
				R = GetAt( i )->Draw( DC, R );

		// Restore old left position...
		R.left = left;

		R.top += 2;
	}
	*/
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
	ASSERT(rpCancelData == NULL);
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

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAnalyzeFeatureClass
//* Note(s)    : Base Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVAnalyzeFeatureClass, SVAnalyzeFeatureClassGuid );

SVAnalyzeFeatureClass::SVAnalyzeFeatureClass( LPCSTR ObjectName )
					  :SVTaskObjectClass( ObjectName )
{
	feature				= 0L;
	resultArray			= NULL;
	featureResultRange	= NULL;
	resultArraySize		= 0;
	failed = TRUE;
	warned = TRUE;

	resultString		= NULL;
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
		return TRUE;
	}
	return FALSE;
}

void SVAnalyzeFeatureClass::create( int MaxNumber, SVRangeClass* R )
{
	resultArray		= new double[MaxNumber];
	resultArraySize = MaxNumber;
	if( resultString )
		delete( resultString );
	resultString	= new char[ resultArraySize * 20 ];

	for( int i = 0; i < resultArraySize; ++i )
		resultArray[i] = 0.0;

	if( R == NULL )
		featureResultRange = new SVRangeClass( this ); //0.0, 0.0, 0.0, 0.0, GetName() );
	else
		featureResultRange = R;
}

BOOL SVAnalyzeFeatureClass::IsFailed()
{
	return failed;
}

BOOL SVAnalyzeFeatureClass::IsWarned()
{
	return warned;
}

void SVAnalyzeFeatureClass::setFeatureResultRange( SVRangeClass* R )
{
	if( featureResultRange != R )
	{
		delete( featureResultRange );
		featureResultRange = R;
	}
}

SVRangeClass* SVAnalyzeFeatureClass::getFeatureResultRange()
{
	return( featureResultRange );
}

double* SVAnalyzeFeatureClass::getResultArray()
{
	return resultArray;
}

long SVAnalyzeFeatureClass::getFeature()
{
	return feature;
}

TCHAR* SVAnalyzeFeatureClass::GetResultArrayString( int N )
{
	TCHAR num[20];

	if( resultString && N )
	{
		_stprintf( resultString, _T( "%.2f, " ), resultArray[ 0 ] );
		for( register int i = 1; i < N && i < resultArraySize; ++i )
		{
			_stprintf( num, _T( "%.2f, " ), resultArray[ i ] );
			_tcscat( resultString, num );		
		}
		resultString[ _tcslen( resultString ) - 2 ] = '\0';	// Delete last Comma!
		return( resultString );
	}

	return( _T( "\0" ) );
}

