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

#include "StdAfx.h"
#include "SVTaskObjectValueInterface.h"

#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "GuiCommands/InspectionRunOnce.h"
#include "SVOCore/SVTaskObject.h"
//#include "SVInspectionProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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

HRESULT SVTaskObjectValueInterface::AddInputRequest( SVValueObjectReference objectRef, LPCTSTR szValue )
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (m_pTaskObject->GetInspection());

		if( pInspection->AddInputRequest( objectRef->GetUniqueObjectID(), szValue ) )
		{
			l_hrOk = S_OK;
		}
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_CannotAddInputRequest, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10205 );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( SVValueObjectReference objectRef, double dValue )
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (m_pTaskObject->GetInspection());

		if( nullptr != pInspection && pInspection->AddInputRequest( objectRef->GetUniqueObjectID(), dValue ) )
		{
			l_hrOk = S_OK;
		}
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_CannotAddInputRequest, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10206 );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, LPCTSTR szValue )
{
	HRESULT l_hrOk = S_FALSE;

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = rEmbeddedId;

	SVObjectClass* l_pObject = dynamic_cast<SVObjectClass*> (SVObjectManagerClass::Instance().getFirstObject( rTaskId, objectInfo ) );

	if( nullptr != l_pObject )
	{
		l_hrOk = AddInputRequest( l_pObject, szValue );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, double dValue )
{
	HRESULT l_hrOk = S_FALSE;

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = rEmbeddedId;

	SVObjectClass* l_pObject = dynamic_cast<SVObjectClass*> (SVObjectManagerClass::Instance().getFirstObject( rTaskId, objectInfo ) );

	if( nullptr != l_pObject )
	{
		l_hrOk = AddInputRequest( l_pObject, dValue );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequestMarker()
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (m_pTaskObject->GetInspection());

		if( pInspection->AddInputRequestMarker() )
		{
			l_hrOk = S_OK;
		}
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_CannotAddInputRequestMarker, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10207 );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::RunOnce( const SVGUID& rToolID)
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		const SVGUID& rInspectionID = m_pTaskObject->GetInspection()->GetUniqueObjectID();
		GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( rInspectionID, rToolID );
		SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( rInspectionID, l_CommandPtr );

		l_hrOk = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_CannotRunOnce, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10208 );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVInputRequestStructMap& rMap )
{
	HRESULT l_hrOk = S_OK;

	SVInputRequestStructMap::const_iterator iter;
	for ( iter = rMap.begin(); iter != rMap.end(); ++iter )
	{
		AddInputRequest( iter->first.m_ObjectRef, iter->second.c_str() );
	}

	AddInputRequestMarker();

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::GetValue( const SVGUID& rTaskId, const SVGUID& rEmbeddedId, VARIANT& rValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = rEmbeddedId;

	const SVObjectClass* l_pObject = dynamic_cast<const SVObjectClass*> (SVObjectManagerClass::Instance().getFirstObject( rTaskId, objectInfo ) );

	if( nullptr != l_pObject )
	{
		const SVValueObjectClass* l_pValueObject = dynamic_cast<const SVValueObjectClass* >( l_pObject );

		if( nullptr != l_pValueObject )
		{
			l_hrOk = l_pValueObject->GetValue( rValue );
		}
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::GetObjectValue( const SVGUID& rObjectId, const SVString& rValueName, VARIANT& rVariantValue ) const
{
	HRESULT l_hrOk = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( rObjectId );

	if( nullptr != l_pObject )
	{
		l_hrOk = l_pObject->GetObjectValue( rValueName, rVariantValue );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

