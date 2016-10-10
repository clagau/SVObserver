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

#include "GuiCommands/InspectionRunOnce.h"
#include "SVInspectionProcess.h"
#include "SVTaskObject.h"
#include "SVTool.h"
#include "TextDefinesSvO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVTaskObjectValueInterface::SVTaskObjectValueInterface()
: m_psvTaskObject( nullptr )
{
}

SVTaskObjectValueInterface::SVTaskObjectValueInterface( SVTaskObjectClass *p_psvTaskObject )
: m_psvTaskObject( p_psvTaskObject )
{
}

SVTaskObjectValueInterface::~SVTaskObjectValueInterface()
{
}

void SVTaskObjectValueInterface::SetTaskObject( SVTaskObjectClass *p_psvTaskObject )
{
	m_psvTaskObject = p_psvTaskObject;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( SVValueObjectReference p_svObjectRef, LPCTSTR p_szValue )
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SVInspectionProcess* pInspection = m_psvTaskObject->GetInspection();

		if( pInspection->AddInputRequest( p_svObjectRef, p_szValue ) )
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

HRESULT SVTaskObjectValueInterface::AddInputRequest( SVValueObjectReference p_svObjectRef, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SVInspectionProcess* pInspection = m_psvTaskObject->GetInspection();

		if( pInspection->AddInputRequest( p_svObjectRef, p_dValue ) )
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

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, LPCTSTR p_szValue )
{
	HRESULT l_hrOk = S_FALSE;

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = p_rEmbeddedId;

	SVObjectClass* l_pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( p_rTaskId, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

	if( nullptr != l_pObject )
	{
		l_hrOk = AddInputRequest( l_pObject, p_szValue );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = p_rEmbeddedId;

	SVObjectClass* l_pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( p_rTaskId, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

	if( nullptr != l_pObject )
	{
		l_hrOk = AddInputRequest( l_pObject, p_dValue );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequestMarker()
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SVInspectionProcess* pInspection = m_psvTaskObject->GetInspection();

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

HRESULT SVTaskObjectValueInterface::RunOnce( SVToolClass *p_psvTool )
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SVInspectionProcess* pInspection = m_psvTaskObject->GetInspection();
		SVGUID InspectionId( SV_GUID_NULL );
		if(nullptr != pInspection )
		{
			InspectionId = pInspection->GetUniqueObjectID();
		}

		SVGUID l_ToolId;

		if( nullptr != p_psvTool )
		{
			l_ToolId = p_psvTool->GetUniqueObjectID();
		}

		GuiCmd::InspectionRunOncePtr l_CommandPtr = new GuiCmd::InspectionRunOnce( InspectionId, l_ToolId );
		SVObjectSynchronousCommandTemplate< GuiCmd::InspectionRunOncePtr > l_Command( InspectionId, l_CommandPtr );

		l_hrOk = l_Command.Execute( TWO_MINUTE_CMD_TIMEOUT );

		if ((nullptr != p_psvTool) && (SUCCEEDED (l_hrOk) || E_FAIL == l_hrOk))
		{
			HRESULT	hrTemp = p_psvTool->getFirstTaskMessage().getMessage().m_MessageCode;

			if (!SUCCEEDED (hrTemp))
			{
				// overwrite hrOk only if hrTemp contains error information.
				l_hrOk = hrTemp;
			}
		}

	}
	catch( ... )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_Error_CannotRunOnce, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10208 );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( SVInputRequestStructMap p_map )
{
	HRESULT l_hrOk = S_OK;

	SVInputRequestStructMap::iterator iter;
	for ( iter = p_map.begin(); iter != p_map.end(); ++iter )
	{
		AddInputRequest( iter->first.ref, iter->second );
	}

	AddInputRequestMarker();

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::GetValue( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, VARIANT& p_rValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = p_rEmbeddedId;

	SVObjectClass* l_pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( p_rTaskId, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

	if( nullptr != l_pObject )
	{
		SVValueObjectClass* l_pValueObject = dynamic_cast< SVValueObjectClass* >( l_pObject );

		if( nullptr != l_pValueObject )
		{
			l_hrOk = l_pValueObject->GetValue( p_rValue );
		}
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::GetObjectValue( const SVGUID& p_rObjectId, const SVString& p_rValueName, VARIANT& p_rVariantValue ) const
{
	HRESULT l_hrOk = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( p_rObjectId );

	if( nullptr != l_pObject )
	{
		l_hrOk = l_pObject->GetObjectValue( p_rValueName, p_rVariantValue );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::SetObjectValue( const SVGUID& p_rObjectId, const SVString& p_rValueName, const _variant_t& p_rVariantValue )
{
	HRESULT l_hrOk = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( p_rObjectId );

	if( nullptr != l_pObject )
	{
		l_hrOk = l_pObject->SetObjectValue( p_rValueName, p_rVariantValue );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

