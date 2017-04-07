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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone );

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
	failed.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	warned.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	passed.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVResultClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);
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
	SVRangeClass* pRange = dynamic_cast<SVRangeClass*>(getFirstObject(info));
	return pRange;
}

bool SVResultClass::Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	const SvOi::IValueObject* pValueObject = dynamic_cast<const SvOi::IValueObject*> (getInput());
	
	long	resultSize = -1;
	if( nullptr != pValueObject )
	{
		resultSize = pValueObject->getResultSize();
	}

	// This is testing to verify that a result was found.  Analyzers such as 
	// Blob, Character, and Barcode deal with variable numbers of valid 
	// results, and need to deal with the possibillity of not finding blobs,
	// characters, or getting a barcode read.
	if (0 == resultSize)
	{
		clearRunErrorMessages();
		// no results to process.
		// if there are no results outside the range, we want the ResultObject
		// to pass.

		//make sure the statusColor is set correctly
		DWORD dwColor = rRunStatus.GetStatusColor();
		m_statusColor.SetValue(dwColor, rRunStatus.m_lResultDataIndex );

		passed.SetValue( BOOL(true), rRunStatus.m_lResultDataIndex );
		failed.SetValue( BOOL(false), rRunStatus.m_lResultDataIndex );
		warned.SetValue( BOOL(false), rRunStatus.m_lResultDataIndex );
		return true;
	}
	else
	{
		// valid results to process.
		if( SVTaskObjectListClass::Run( rRunStatus, pErrorMessages ) )
		{
			//make sure statusColor is set correctly
			DWORD dwColor = rRunStatus.GetStatusColor();
			m_statusColor.SetValue(dwColor, rRunStatus.m_lResultDataIndex );

			// set our state according to the runStatus
			passed.SetValue( BOOL(rRunStatus.IsPassed()), rRunStatus.m_lResultDataIndex );
			failed.SetValue( BOOL(rRunStatus.IsFailed()), rRunStatus.m_lResultDataIndex );
			warned.SetValue( BOOL(rRunStatus.IsWarned()), rRunStatus.m_lResultDataIndex );

			return true;
		}
	}

	SetInvalid();
	rRunStatus.SetInvalid();
	return false;
}

const SVObjectClass* SVResultClass::getInput() const
{
	if( m_inputObjectInfo.IsConnected() )
	{
		return m_inputObjectInfo.GetInputObjectInfo().m_pObject;
	}

	return nullptr;
}

bool SVResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	if( __super::onRun( rRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages) )
	{
		return true;
	}

	SetInvalid();
	rRunStatus.SetInvalid();
	return false;
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

bool SVResultClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( !m_inputObjectInfo.IsConnected() || nullptr == m_inputObjectInfo.GetInputObjectInfo().m_pObject ) 
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
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