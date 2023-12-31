//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "InspectionEngine/RunStatus.h"
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
	// Identify our type in the Output List
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;

	m_pwf.registerPwfAndSetDefaults(*this);
}

SVResult::~SVResult()
{
}

bool SVResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	m_pwf.setPwfObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVResult::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	m_inputObject.validateInput();

	return Result && ValidateLocal(pErrorMessages);
}

SVRange* SVResult::GetResultRange()
{
	SvDef::SVObjectTypeInfoStruct info;
	info.m_ObjectType = SvPb::SVRangeObjectType;
	SVRange* pRange = dynamic_cast<SVRange*>(getFirstObject(info));
	return pRange;
}

bool SVResult::Run( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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

		m_pwf.setWarnedFailedStatus(false, false);
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
			m_pwf.m_Passed.SetValue(BOOL(rRunStatus.isPassed()));
			m_pwf.m_Failed.SetValue(BOOL(rRunStatus.isFailed()));
			m_pwf.m_Warned.SetValue(BOOL(rRunStatus.isWarned()));

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

bool SVResult::onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	return true;
}

} //namespace SvOp
