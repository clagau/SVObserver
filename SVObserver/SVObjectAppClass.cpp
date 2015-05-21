//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAppClass
//* .File Name       : $Workfile:   SVObjectAppClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:45:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectAppClass.h"
#include "SVAnalyzer.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVObjectAppClass, SVObjectAppClassGuid )

SVObjectAppClass::SVObjectAppClass(LPCSTR ObjectName)
				  :SVObjectClass(ObjectName) 
{
	m_psvInspection = NULL;
	m_psvTool = NULL;
	m_psvAnalyzer	= NULL;
}

SVObjectAppClass::SVObjectAppClass(SVObjectClass* POwner, int StringResourceID)
: SVObjectClass( POwner, StringResourceID ) 
{
	m_psvInspection = NULL;
	m_psvTool = NULL;
	m_psvAnalyzer	= NULL;
}

SVObjectAppClass::~SVObjectAppClass()
{
}

BOOL SVObjectAppClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	UpdateConnections( PCreateStruct );
	
	BOOL l_bOk = SVObjectClass::CreateObject( PCreateStruct );
	
	isCreated = l_bOk;
	
	return l_bOk;	
}

HRESULT SVObjectAppClass::ConnectObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	HRESULT l_Status = S_OK;

	if( PCreateStruct != NULL && PCreateStruct->OwnerObjectInfo.PObject != this && PCreateStruct->OwnerObjectInfo.UniqueObjectID != GetUniqueObjectID() )
	{
		UpdateConnections( PCreateStruct );
	
		l_Status = SVObjectClass::ConnectObject( PCreateStruct );
	}
	
	return l_Status;	
}

SVInspectionProcess* SVObjectAppClass::GetInspection() const
{
	return m_psvInspection;
}

SVToolClass *SVObjectAppClass::GetTool() const
{
	return m_psvTool;
}

SVAnalyzerClass *SVObjectAppClass::GetAnalyzer() const
{
	return m_psvAnalyzer;
}

#pragma region virtual methods (IObjectAppClass)
SvOi::IObjectClass* SVObjectAppClass::GetToolInterface() const
{
	return m_psvTool;
}

DWORD_PTR SVObjectAppClass::CreateChildObject(SvOi::IObjectClass& rChildObject, DWORD context)
{
	SVObjectClass* pObject = dynamic_cast<SVObjectClass*>(&rChildObject);
	return CreateChildObject(pObject, context);
}
#pragma endregion virtual methods (IObjectAppClass)

DWORD_PTR SVObjectAppClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		case SVMSGID_CREATE_CHILD_OBJECT:
		{
			SVObjectClass* pChildObject = ( SVObjectClass* ) DwMessageValue;
			return CreateChildObject(pChildObject, static_cast<DWORD>(DwMessageContext));
		}

		case SVMSGID_CONNECT_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			SVObjectClass* pChildObject = ( SVObjectClass* ) DwMessageValue;
			if( SV_IS_KIND_OF( pChildObject, SVObjectClass ) )
			{
				SVAnalyzerLevelCreateStruct createStruct;

				createStruct.OwnerObjectInfo = this;
				createStruct.AnalyzerObjectInfo = GetAnalyzer();
				createStruct.ToolObjectInfo	= GetTool();
				createStruct.InspectionObjectInfo	= GetInspection();
				
				DWORD_PTR l_Return = SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );

				return l_Return;
			}

			return SVMR_NOT_PROCESSED;
		}
	}

	return( SVObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

void SVObjectAppClass::UpdateConnections( SVObjectLevelCreateStruct* PCreateStruct )
{
	m_psvAnalyzer = dynamic_cast<SVAnalyzerClass *>(this);

	if( m_psvAnalyzer == NULL )
	{
		SVAnalyzerLevelCreateStruct *l_psvTemp = dynamic_cast<SVAnalyzerLevelCreateStruct *>(PCreateStruct);

		if( l_psvTemp != NULL )
		{
			m_psvAnalyzer = dynamic_cast<SVAnalyzerClass *>(l_psvTemp->AnalyzerObjectInfo.PObject);
		}
	}
	
	m_psvTool = dynamic_cast<SVToolClass *>(this);

	if( m_psvTool == NULL )
	{
		SVToolLevelCreateStruct *l_psvTemp = dynamic_cast<SVToolLevelCreateStruct *>(PCreateStruct);

		if( l_psvTemp != NULL )
		{
			m_psvTool = dynamic_cast<SVToolClass *>(l_psvTemp->ToolObjectInfo.PObject);
		}
	}
	
	m_psvInspection = dynamic_cast<SVInspectionProcess*> (this);

	if( nullptr == m_psvInspection )
	{
		SVInspectionLevelCreateStruct *l_psvTemp = dynamic_cast<SVInspectionLevelCreateStruct *>(PCreateStruct);

		if( nullptr != l_psvTemp )
		{
			m_psvInspection = dynamic_cast<SVInspectionProcess*> (l_psvTemp->InspectionObjectInfo.PObject);
		}
	}
}

DWORD_PTR SVObjectAppClass::createAllObjectsFromChild( SVObjectClass* pChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo = this;
	createStruct.AnalyzerObjectInfo = GetAnalyzer();
	createStruct.ToolObjectInfo	= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();

	return SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
}

DWORD_PTR SVObjectAppClass::CreateChildObject( SVObjectClass* pChildObject, DWORD context )
{
	if( IsCreated() && SV_IS_KIND_OF( pChildObject, SVObjectClass ) )
	{
		long l_LastIndex = 1;
		SVInspectionProcess* pInspection = GetInspection();

		if( nullptr != pInspection )
		{
			SVProductInfoStruct l_Product = pInspection->LastProductGet( SV_INSPECTION );

			if( !( l_Product.empty() ) )
			{
				SVDataManagerHandle l_Handle;
				l_Product.GetResultDataIndex( l_Handle );
				l_LastIndex = l_Handle.GetIndex();
			}
		}

		// Set first object depth...
		pChildObject->SetObjectDepthWithIndex( objectDepth, l_LastIndex );
		pChildObject->SetImageDepth( mlImageDepth );

		DWORD_PTR l_Return = createAllObjectsFromChild( pChildObject );

		if( ( context & SVMFResetObject ) == SVMFResetObject )
		{
			::SVSendMessage( pChildObject, SVM_RESET_ALL_OBJECTS, NULL, NULL );
		}

		if( ( context & SVMFSetDefaultInputs ) == SVMFSetDefaultInputs )
		{
			GetInspection()->SetDefaultInputs();
		}

		if( ( context & SVMFResetInspection ) == SVMFResetInspection )
		{
			::SVSendMessage( GetInspection(), SVM_RESET_ALL_OBJECTS, NULL, NULL );
		}

		return l_Return;
	}

	return SVMR_NOT_PROCESSED;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVObjectAppClass.cpp_v  $
 * 
 *    Rev 1.3   15 May 2014 13:45:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 11:39:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Oct 2013 15:20:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Made GetInspection, GetTool, and GetAnalyzer methods const.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:35:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Jan 2013 12:23:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jul 2012 17:23:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Dec 2010 13:15:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Jun 2010 10:59:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:08:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Oct 2007 16:45:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added virtual destructor in this class because it uses multiple inheritence
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:10:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
