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
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVStatusLibrary/RunStatus.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVResult, SvPb::ResultClassId);

////////////////////////////////////////////////////////////////////////////////
// .Title       :  SVResult
// -----------------------------------------------------------------------------
// .Description : Standard constructor
////////////////////////////////////////////////////////////////////////////////

SVResult::SVResult ( SVObjectClass* POwner, int StringResourceID )
				  :SVTaskObjectListClass( POwner, StringResourceID )
{
	init();
}

void SVResult::init()
{
	m_bUseOverlays = false;

	// Identify our type in the Output List
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Failed, SvPb::FailedEId, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Warned, SvPb::WarnedEId, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Passed, SvPb::PassedEId, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Passed.SetDefaultValue(BOOL(false), true);		// Default for Passed is FALSE !!!
	m_Passed.setSaveValueFlag(false);
	m_Failed.SetDefaultValue(BOOL(true), true);			// Default for Failed is TRUE !!!
	m_Failed.setSaveValueFlag(false);
	m_Warned.SetDefaultValue(BOOL(true), true);			// Default for Warned is TRUE !!!
	m_Warned.setSaveValueFlag(false);
}

SVResult::~SVResult()
{
}

bool SVResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	m_Failed.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_Warned.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_Passed.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVResult::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_inputObject.validateInput();

	return Result && ValidateLocal(pErrorMessages);
}

bool SVResult::IsFailed()
{
	BOOL RVal = true;
	m_Failed.GetValue( RVal );
	return( TRUE == RVal );
}

bool SVResult::IsWarned()
{
	BOOL RVal = true;
	m_Warned.GetValue( RVal );
	return( TRUE == RVal );
}

bool SVResult::IsGood()
{
	BOOL RVal = true;
	m_Passed.GetValue( RVal );
	return( TRUE == RVal );
}

SVRange* SVResult::GetResultRange()
{
	SvDef::SVObjectTypeInfoStruct info;
	info.m_ObjectType = SvPb::SVRangeObjectType;
	SVRange* pRange = dynamic_cast<SVRange*>(getFirstObject(info));
	return pRange;
}

bool SVResult::Run( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
		m_statusColor.SetValue(dwColor);

		m_Passed.SetValue(BOOL(true));
		m_Failed.SetValue(BOOL(false));
		m_Warned.SetValue(BOOL(false));
		return true;
	}
	else
	{
		// valid results to process.
		if( SVTaskObjectListClass::Run( rRunStatus, pErrorMessages ) )
		{
			//make sure statusColor is set correctly
			DWORD dwColor = rRunStatus.GetStatusColor();
			m_statusColor.SetValue(dwColor);

			// set our state according to the runStatus
			m_Passed.SetValue(BOOL(rRunStatus.IsPassed()));
			m_Failed.SetValue(BOOL(rRunStatus.IsFailed()));
			m_Warned.SetValue(BOOL(rRunStatus.IsWarned()));

			return true;
		}
	}

	rRunStatus.SetInvalid();
	return false;
}

const SVObjectClass* SVResult::getInput() const
{
	if(m_inputObject.IsConnected() )
	{
		return m_inputObject.GetInputObjectInfo().getObject();
	}

	return nullptr;
}

bool SVResult::onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	if( __super::onRun( rRunStatus, pErrorMessages ) && ValidateLocal(pErrorMessages) )
	{
		return true;
	}

	rRunStatus.SetInvalid();
	return false;
}

bool SVResult::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( !m_inputObject.IsConnected() || nullptr == m_inputObject.GetInputObjectInfo().getObject() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

} //namespace SvOp
