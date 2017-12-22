//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectValueInterface
//* .File Name       : $Workfile:   SVTaskObjectValueInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:07:10  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVTaskObjectValueInterface.h"

#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVTaskObject.h"
#include "InspectionCommands/CommandFunctionHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVTaskObjectValueInterface::SVTaskObjectValueInterface()
: m_pTaskObject( nullptr )
{
}

SVTaskObjectValueInterface::SVTaskObjectValueInterface( SVTaskObjectClass* pTaskObject )
: m_pTaskObject( pTaskObject )
{
}

SVTaskObjectValueInterface::~SVTaskObjectValueInterface()
{
}

void SVTaskObjectValueInterface::SetTaskObject( SVTaskObjectClass* pTaskObject )
{
	m_pTaskObject = pTaskObject;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVObjectReference& rObjectRef, const _variant_t& rValue )
{
	HRESULT Result = E_FAIL;

	try
	{
		SvOi::IInspectionProcess* pInspection = m_pTaskObject->GetInspectionInterface();

		if( pInspection->AddInputRequest( rObjectRef.getObject()->GetUniqueObjectID(), rValue, rObjectRef.ArrayIndex() ) )
		{
			Result = S_OK;
		}
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_CannotAddInputRequest, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10205 );
	}

	return Result;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, const _variant_t& rValue )
{
	HRESULT Result( E_FAIL );

	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = rEmbeddedId;

	SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (SVObjectManagerClass::Instance().getFirstObject( rTaskId, objectInfo ) );

	if( nullptr != pObject )
	{
		Result = AddInputRequest( SVObjectReference( pObject ), rValue );
	}

	return Result;
}

HRESULT SVTaskObjectValueInterface::AddInputRequestMarker()
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SvOi::IInspectionProcess* pInspection = m_pTaskObject->GetInspectionInterface();

		if( pInspection->AddInputRequestMarker() )
		{
			l_hrOk = S_OK;
		}
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_CannotAddInputRequestMarker, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10207 );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::RunOnce( const SVGUID& rToolID)
{
	HRESULT hrOk = S_FALSE;

	try
	{
		const SVGUID& rInspectionID = m_pTaskObject->GetInspection()->GetUniqueObjectID();
		SvPB::InspectionRunOnceRequest requestMessage;
		requestMessage.mutable_inspectionid()->CopyFrom(SvCmd::setGuidToMessage(rInspectionID));
		requestMessage.mutable_taskid()->CopyFrom(SvCmd::setGuidToMessage(rToolID));
		hrOk = SvCmd::InspectionCommandsSynchronous(rInspectionID, &requestMessage, nullptr);
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_CannotRunOnce, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10208 );
	}

	return hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVInputRequestStructMap& rMap )
{
	HRESULT l_hrOk = S_OK;

	SVInputRequestStructMap::const_iterator iter;
	for ( iter = rMap.begin(); iter != rMap.end(); ++iter )
	{
		AddInputRequest( iter->first.m_ObjectRef, iter->second );
	}

	AddInputRequestMarker();

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::GetValue( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, _variant_t& rValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = rEmbeddedId;

	const SvOi::IValueObject* pValueObject = dynamic_cast<const SvOi::IValueObject*> (SVObjectManagerClass::Instance().getFirstObject( rTaskId, objectInfo ));

	if( nullptr != pValueObject)
	{
		l_hrOk = pValueObject->getValue( rValue );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::GetObjectValue( const SVGUID& rObjectId, const std::string& rValueName, _variant_t& rValue ) const
{
	HRESULT l_hrOk = S_OK;

	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( rObjectId );

	if( nullptr != pObject )
	{
		l_hrOk = pObject->GetObjectValue( rValueName, rValue );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

