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

#include "SVCommandInspectionRunOnce.h"
#include "SVInspectionProcess.h"
#include "SVTaskObject.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVTaskObjectValueInterface::SVTaskObjectValueInterface()
: m_psvTaskObject( NULL )
{
}

SVTaskObjectValueInterface::SVTaskObjectValueInterface( SVTaskObjectClass *p_psvTaskObject )
: m_psvTaskObject( p_psvTaskObject )
{
}

SVTaskObjectValueInterface::~SVTaskObjectValueInterface()
{
}

SVTaskObjectClass *SVTaskObjectValueInterface::GetTaskObject()
{
	return m_psvTaskObject;
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
		::AfxMessageBox( "ERROR: Cannot Add Input Request (LPCTSTR) .\nPlease contact technical support specialist.", MB_ICONERROR | MB_OK );
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
		::AfxMessageBox( "ERROR: Cannot Add Input Request (double).\nPlease contact technical support specialist.", MB_ICONERROR | MB_OK );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::AddInputRequest( const SVGUID& p_rTaskId, const SVGUID& p_rEmbeddedId, LPCTSTR p_szValue )
{
	HRESULT l_hrOk = S_FALSE;

	SVObjectTypeInfoStruct objectInfo;
	objectInfo.EmbeddedID = p_rEmbeddedId;

	SVObjectClass* l_pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( p_rTaskId, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

	if( l_pObject != NULL )
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

	SVObjectClass* l_pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( p_rTaskId, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

	if( l_pObject != NULL )
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
		::AfxMessageBox( "ERROR: Cannot Add Input Request Marker.\nPlease contact technical support specialist.", MB_ICONERROR | MB_OK );
	}

	return l_hrOk;
}

HRESULT SVTaskObjectValueInterface::RunOnce( SVToolClass *p_psvTool )
{
	HRESULT l_hrOk = S_FALSE;

	try
	{
		SVInspectionProcess* pInspection = m_psvTaskObject->GetInspection();
		SVGUID InspectionId(SVInvalidGUID);
		if(nullptr != pInspection )
		{
			InspectionId = pInspection->GetUniqueObjectID();
		}

		SVGUID l_ToolId;

		if( p_psvTool != NULL )
		{
			l_ToolId = p_psvTool->GetUniqueObjectID();
		}

		SVCommandInspectionRunOncePtr l_CommandPtr = new SVCommandInspectionRunOnce( InspectionId, l_ToolId );
		SVObjectSynchronousCommandTemplate< SVCommandInspectionRunOncePtr > l_Command( InspectionId, l_CommandPtr );

		l_hrOk = l_Command.Execute( 120000 );
	}
	catch( ... )
	{
		::AfxMessageBox( "ERROR: Cannot Run Once.\nPlease contact technical support specialist.", MB_ICONERROR | MB_OK );
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

	SVObjectClass* l_pObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( p_rTaskId, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ));

	if( l_pObject != NULL )
	{
		SVValueObjectClass* l_pValueObject = dynamic_cast< SVValueObjectClass* >( l_pObject );

		if( l_pValueObject != NULL )
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

	if( l_pObject != NULL )
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

	if( l_pObject != NULL )
	{
		l_hrOk = l_pObject->SetObjectValue( p_rValueName, p_rVariantValue );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTaskObjectValueInterface.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 13:07:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:18:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:35:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2012 11:22:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
