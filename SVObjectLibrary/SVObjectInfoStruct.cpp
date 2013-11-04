//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectInfoStruct
//* .File Name       : $Workfile:   SVObjectInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:50:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectClass.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"

SVObjectInfoStruct::SVObjectInfoStruct()
: PObject( NULL ), UniqueObjectID(), ObjectTypeInfo( SVInvalidObjectTypeInfo ), m_ObjectNameInfo()
{
}

SVObjectInfoStruct::SVObjectInfoStruct( const SVObjectInfoStruct& O2 )
: PObject( O2.PObject ), UniqueObjectID( O2.UniqueObjectID ), ObjectTypeInfo( O2.ObjectTypeInfo ), m_ObjectNameInfo( O2.m_ObjectNameInfo )
{
}

SVObjectInfoStruct::SVObjectInfoStruct( SVObjectClass* PO2 )
: PObject( NULL ), UniqueObjectID(), ObjectTypeInfo( SVInvalidObjectTypeInfo ), m_ObjectNameInfo()
{
	SetObject( PO2 );
}

SVObjectInfoStruct::SVObjectInfoStruct( SVObjectReference ref )
: PObject( NULL ), UniqueObjectID(), ObjectTypeInfo( SVInvalidObjectTypeInfo ), m_ObjectNameInfo()
{
	SetObject( ref );
}

SVObjectInfoStruct::~SVObjectInfoStruct()
{
}

const SVObjectInfoStruct& SVObjectInfoStruct::operator = ( const SVObjectInfoStruct& O2 )
{
	if( this != &O2 )
	{
		PObject = O2.PObject;
		UniqueObjectID = O2.UniqueObjectID;
		ObjectTypeInfo = O2.ObjectTypeInfo;
		m_ObjectNameInfo = O2.m_ObjectNameInfo;
	}

	return *this;
}

void SVObjectInfoStruct::clear()
{
	PObject = NULL;
	UniqueObjectID.clear();
	ObjectTypeInfo = SVInvalidObjectTypeInfo;
	m_ObjectNameInfo.clear();
	//m_UseIndex = false;
	//m_ArrayIndex = 0;
}

void SVObjectInfoStruct::ClearObjectInfo()
{
	PObject = NULL;
	UniqueObjectID.clear();
}

HRESULT SVObjectInfoStruct::SetObject( const SVGUID& p_rObjectID )
{
	HRESULT l_hrOk = S_OK;

	if( p_rObjectID != UniqueObjectID )
	{
		ClearObjectInfo();

		UniqueObjectID = p_rObjectID;
	}

	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVString& p_rName )
{
	HRESULT l_hrOk = S_OK;

	if( p_rName != m_ObjectNameInfo.GetObjectArrayName() )
	{
		ClearObjectInfo();

		l_hrOk = m_ObjectNameInfo.ParseObjectName( p_rName );
	}

	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectTypeInfoStruct& p_rTypeInfo )
{
	HRESULT l_hrOk = S_OK;

	clear();

	ObjectTypeInfo = p_rTypeInfo;

	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( SVObjectClass* p_psvObject )
{
	HRESULT l_hrOk = S_OK;

	try
	{
		SVObjectClass* l_pTemp = dynamic_cast<SVObjectClass*>( p_psvObject );

		if( l_pTemp != NULL )
		{
			clear();

			PObject = l_pTemp;

			ObjectTypeInfo = PObject->GetObjectInfo().ObjectTypeInfo;

			UniqueObjectID = PObject->GetUniqueObjectID();

			if( UniqueObjectID == SVInvalidGUID )
			{
				UniqueObjectID = PObject->GetObjectInfo().UniqueObjectID;
			}

			m_ObjectNameInfo.ParseObjectName( static_cast< LPCTSTR >( PObject->GetCompleteObjectName() ) );
		}
		else
		{
			ClearObjectInfo();
		}
	}
	catch( ... )
	{
		ClearObjectInfo();

		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectReference& p_svObject )
{
	HRESULT l_hrOk = SetObject( p_svObject.Object() );

	if( l_hrOk == S_OK )
	{
		UniqueObjectID = p_svObject.Guid();
		m_ObjectNameInfo = p_svObject.GetObjectNameInfo();
		//m_UseIndex = p_svObject.IsIndexPresent();
		//m_ArrayIndex = p_svObject.ArrayIndex();
	}

	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectInfoStruct& p_rObject )
{
	HRESULT l_hrOk = S_OK;

	if( this != &p_rObject )
	{
		PObject = p_rObject.PObject;
		UniqueObjectID = p_rObject.UniqueObjectID;
		ObjectTypeInfo = p_rObject.ObjectTypeInfo;
		m_ObjectNameInfo = p_rObject.m_ObjectNameInfo;
	}

	return l_hrOk;
}

bool SVObjectInfoStruct::operator == ( const SVObjectInfoStruct& rhs ) const
{
	bool l_Status = ( UniqueObjectID == rhs.UniqueObjectID ) != FALSE;

	/*
	if( l_Status && m_UseIndex )
	{
		l_Status = ( m_ArrayIndex == rhs.m_ArrayIndex );
	}
	*/

	if( m_ObjectNameInfo.m_IndexPresent )
	{
		l_Status = m_ObjectNameInfo.m_Index == rhs.m_ObjectNameInfo.m_Index;
	}

	return l_Status;
}

bool SVObjectInfoStruct::operator == ( const SVObjectReference& rhs ) const
{
	bool l_Status = true;

	if ( rhs.Object() != NULL )
	{
		l_Status = ( UniqueObjectID == rhs->GetUniqueObjectID() ) != FALSE;

		/*
		if( l_Status && m_UseIndex )
		{
			l_Status = ( m_ArrayIndex == rhs.ArrayIndex() );
		}
		*/

		if( m_ObjectNameInfo.m_IndexPresent )
		{
			l_Status = m_ObjectNameInfo.m_Index == rhs.GetObjectNameInfo().m_Index;
		}
	}
	else
	{
		l_Status = ( UniqueObjectID == SVInvalidGUID ) != FALSE;
	}

	return l_Status;
}

SVObjectReference SVObjectInfoStruct::GetObjectReference() const
{
	SVObjectReference l_Ref( PObject );

	/*
	if( m_UseIndex )
	{
		if( 0 <= m_ArrayIndex )
		{
			l_Ref.SetArrayIndex( m_ArrayIndex );
		}
		else
		{
			l_Ref.SetEntireArray();
		}
	}
	*/

	if( m_ObjectNameInfo.m_IndexPresent )
	{
		long l_Index = m_ObjectNameInfo.GetIndexValue();

		if( 0 <= l_Index )
		{
			l_Ref.SetArrayIndex( l_Index );
		}
		else
		{
			l_Ref.SetEntireArray();
		}
	}

	return l_Ref;
}

BOOL SVObjectInfoStruct::CheckExistence()
{
	BOOL l_bOk = true;

	l_bOk = l_bOk && UniqueObjectID != SVInvalidGUID;

	if( l_bOk )
	{
		SVObjectClass *l_psvObject = SVObjectManagerClass::Instance().GetObject( UniqueObjectID );

		l_bOk = l_bOk && l_psvObject == PObject;
	}

	l_bOk = l_bOk && dynamic_cast<SVObjectClass *>(PObject) != NULL;

	if( l_bOk )
	{
		GUID l_guidId = PObject->GetUniqueObjectID();

		l_bOk = l_bOk && l_guidId == UniqueObjectID;
	}

	return l_bOk;
}

SVObjectInfoArrayClass::SVObjectInfoArrayClass()
{
	m_CriticalSectionPtr = new SVCriticalSection;
}

SVObjectInfoArrayClass::~SVObjectInfoArrayClass()
{
}

BOOL SVObjectInfoArrayClass::Lock( DWORD p_TimeOutMilliseconds ) const
{
	BOOL l_Status = true;

	if( m_CriticalSectionPtr.empty() )
	{
		m_CriticalSectionPtr = new SVCriticalSection;
	}

	l_Status = !( m_CriticalSectionPtr.empty() );

	if( l_Status )
	{
		l_Status = m_CriticalSectionPtr->Lock( p_TimeOutMilliseconds );
	}

	return l_Status;
}

BOOL SVObjectInfoArrayClass::Unlock() const
{
	BOOL l_Status = !( m_CriticalSectionPtr.empty() );

	if( l_Status )
	{
		l_Status = m_CriticalSectionPtr->Unlock();
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectInfoStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:50:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   12 Jul 2012 14:03:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   10 Jul 2012 11:54:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added operator= to class/structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Jun 2012 17:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   30 Jan 2012 10:33:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to reduce unnecessary methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   25 Feb 2011 11:59:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Dec 2010 14:48:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jul 2009 08:35:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Sep 2005 07:35:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects to handle invalid items on the input and output lists.  Methods now verify object validity before use.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Aug 2005 11:26:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Aug 2005 11:26:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   removed warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jul 2005 15:49:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVObjectReference guids
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jul 2005 11:46:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVObjectReference interoperability
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jun 2005 14:49:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved SVObjectReference to its own file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2005 15:16:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added class SVObjectReference to allow array handling
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2005 11:44:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  262
 * SCR Title:  Improve performance when loading a configuration in SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added pragma to remove waring message and added functionality to check for existance in object manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:56:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   12 Jul 2004 13:05:18   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed merge error
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   12 Jul 2004 13:04:12   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  406
   SCR Title:  Implement External Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented SV_HIDDEN
   fixed SV_GET_KIND_OF
   added copy constructor / operator = to SVInObjectInfoStruct
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   12 Jul 2004 13:01:40   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  428
   SCR Title:  Improve Load Configuration time
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented GetCompleteObjectName optimization
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   04 Dec 2003 12:59:10   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  397
   SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed SVInObjectInfoStruct to have a const copy constructor
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   22 Apr 2003 11:35:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   25 Feb 2003 15:50:50   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated GetAncestor method to verify that pointer is not NULL.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   30 Jan 2003 15:08:24   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added SetImageDepth and GetImageDepth methods to class.  Updated constructor to initialize mlImageDepth attribute.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   10 Jan 2003 17:49:50   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     forgot the = sign for the const UINT message defines
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   10 Jan 2003 15:45:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new function VerifyImageForOverlay to determine which object has overlays on which image.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   17 Dec 2002 15:20:22   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  299
   SCR Title:  SIAC needs to be able to request Source Images
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     BOOL GetChildObjectByName( LPCTSTR tszName, SVObjectClass** ppObject );
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   09 Dec 2002 19:43:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   20 Nov 2002 09:10:44   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226, 272
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed TheClassRegister -> TheSVClassRegister
   Changed TheObjectManager -> TheSVObjectManager
   Added TheSVDataManager
   Removed references to PPQ
   removed references to SVObjectInfoStruct
   processMessage
   Added support for SVMSGID_GET_OBJECT_BY_NAME
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   19 Apr 2001 14:47:22   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Changes due to addition of the SVFileNameObjectClass and new GUID for the class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   06 Sep 2000 16:31:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  179
   SCR Title:  Create Color SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to include color SVIM changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   11 May 2000 08:56:02   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  139
   SCR Title:  Math Tool Invalid State
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SVM_CREATE_ALL_CLOSED_OBJECTS to
   also route this message to friend objects.
   
   Added IsDescendantOfType method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   07 May 2000 23:29:56   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  137
   SCR Title:  Remove the need to explicitly reference specific version numbers.
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Changed the Serialization () functions so that versioning does not need to be explicitly addressed unless a change actually affects file compatabillity.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   26 Apr 2000 16:13:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added GetImageInputContextGuid method.
   Added new message 
   SVM_SET_SHADOWED_ABSEXTENT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   19 Apr 2000 16:32:18   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  128
   SCR Title:  Suport for 3.11 versioning
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     SVObjectInfoStruct::Serialize () and 
   SVObjectClass::Serialize () were both changed to support program version number 3.11 beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   11 Apr 2000 17:45:54   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  121
   SCR Title:  Support for 3.06 versioning
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Changed SVObjectInfoStruct::Serialize () and
   SVObjectClass::Serialize () to support program version number 3.06.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   23 Mar 2000 14:03:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  117
   SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Introduced new SetDisabled() member function.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   14 Mar 2000 14:42:56   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised version number to 3.10 release (0x00030AFF)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   09 Mar 2000 08:52:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Version number from 3.10 Beta 3 to 3.10 Beta 4
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   07 Mar 2000 14:07:42   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Changed Version number to 3.1 Beta 3.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   25 Feb 2000 16:07:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added new method GetTransform.
   Added new message SVM_GET_CONTEXT_EXTENT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   21 Feb 2000 14:51:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added checkDrawAllowed method.
   Added comments for Move/Size messages to detail what is passed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   Feb 21 2000 10:45:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Versioning to v3.10 Beta 2.
   Add new Macro for Type Safe Casting.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   16 Feb 2000 17:00:48   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Modified Draw functions to use the DrawContext structure
   and removed unused methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   07 Feb 2000 20:44:36   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  93
   SCR Title:  Integrate Multiple Camera Image Input per IPD
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Added new Object function GetObjectIcon() which can be overwritten and be used as generic 'get icon func' for any kind of list or tree icon.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   04 Feb 2000 13:31:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Changed version to 3.10
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   Feb 03 2000 16:33:20   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  102
   SCR Title:  Versioning v3.05
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed version from v3.04 to v3.05.  Corrected multiple inheritance order in System.h.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   Jan 19 2000 15:18:50   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  88
   SCR Title:  Version 3.04 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to change v3.04 Beta 1 to v3.04 Released.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   Jan 14 2000 15:15:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  85
   SCR Title:  Version 3.04 Beta 1 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for v3.03 to v3.04 Beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   Jan 04 2000 08:58:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  81
   SCR Title:  Version 3.03 versioning changes
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to convert v3.02 to v3.03.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Dec 02 1999 08:36:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  72
   SCR Title:  Version 3.02 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Change v3.01 to v3.02.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Dec 02 1999 08:32:34   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed v3.01 to v3.02.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 15:48:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  67
   SCR Title:  Version 3.01 versioning changes.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes for v3.00 to v3.01 versioning.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:18:38   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.46   Nov 18 1999 09:05:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  59
   SCR Title:  Version 3.00 Release Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Release versioning changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.45   Nov 10 1999 12:21:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes v3.00 Beta 17 to v3.00 Beta 18.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.44   Nov 05 1999 11:18:34   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  56
   SCR Title:  Version 3.00 Beta 17 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 16 to v3.00 Beta 17.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.43   Nov 02 1999 09:09:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  55
   SCR Title:  Version 3.00 Beta 16 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes v3.00 Beta 15 to v3.00 Beta 16.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.42   Nov 02 1999 08:59:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  55
   SCR Title:  Version 3.00 Beta 16 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required for v3.00 Beta 15 to v3.00 Beta 16 change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.41   Nov 01 1999 16:57:34   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  54
   SCR Title:  Version 3.00 Beta 15 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 14 to v3.00 Beta 15
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.40   Oct 29 1999 17:58:50   steve
   Added function GetFriendList () to allow the Print File function to get a list of all "friends" for the PrintObject () function in SVObserverApp
   
      Rev 1.39   28 Oct 1999 08:42:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised GetObjectScript to address performance issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.37   Oct 22 1999 09:02:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  51
   SCR Title:  Version 3.00 Beta 13 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 12 to v3.00 Beta 13.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.36   Oct 20 1999 16:06:40   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  50
   SCR Title:  Version 3.00 Beta 12 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed version from v3.00 Beta 11 to v3.00 Beta 12.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.35   Oct 18 1999 12:50:06   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  47
   SCR Title:  Version 3.00 Beta 11 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes for v3.00 Beta 10 to v3.00 Beta 11.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.34   Oct 18 1999 11:53:20   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     RO - Bug fix - Added DataLinkID scripting.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.33   14 Oct 1999 17:00:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added HasDependents method to SVOutputInfoListClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.32   Oct 13 1999 15:37:12   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  46
   SCR Title:  Version 3.00 Beta 10 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Version v3.00 Beta 9 to v3.00 Beta 10 changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.31   Oct 13 1999 15:14:24   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Roberts bug fixes for Beta 10.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.30   Oct 12 1999 08:16:50   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  44
   SCR Title:  Version 3.00 Beta 9 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to change v3.00 Beta 8 to v3.00 Beta 9.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.29   Oct 08 1999 10:54:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  43
   SCR Title:  Version 3.00 Beta 7 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Versioning changes to Beta 8.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.28   Oct 04 1999 11:32:12   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  43
   SCR Title:  Version 3.00 Beta 7 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     V3.00 Beta 7 versioning.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.27   Oct 04 1999 09:36:20   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Completed FriendList locks.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.26   Sep 29 1999 14:21:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  41
   SCR Title:  Version 3.00 Beta 5 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     v3.00 Beta 5
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.25   Sep 27 1999 19:04:40   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  38
   SCR Title:  Version 3.00 Beta 4 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Version from 0x00030003 to 0x00030004.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.24   23 Sep 1999 16:25:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added message SVM_DESTROY_FRIEND_OBJECT.
   Revised DestroyFriends to send 
   SVM_DESTROY_FRIEND_OBJECT instead of
   SVM_DESTROY_CHILD_OBJECTS
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.23   Sep 23 1999 13:53:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     New version specific changes to Serialize methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.22   22 Sep 1999 18:35:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to fix restoration of friends from script
   (Added method to adjust friend aliases)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.21   Sep 21 1999 09:29:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  36
   SCR Title:  Version 3.00 Beta 2 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added 0x00030002 (v3.00 Beta 2) case statement  to all serialization methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.20   Sep 20 1999 15:36:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced new broadcast message router.
   Added new messages: Create_All_Closed_Objects.
        Notify_About_Creation.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.19   Sep 16 1999 12:38:00   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added Product Key Struct and other from SVPPQ.h.
   Added GetAncestor(ObjectType) and IsDescendantOf( PAncestorObject)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   10 Sep 1999 19:35:02   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Fixed bug in PutAttributesInObjectScript and
   PutFriendGuidsInObjectScript. 
   (changed GetObjectName() to GetName, to support
   objects named different than the class name)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   04 Sep 1999 17:24:34   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to save and restore uObjectAttributesAllowed and
   uObjectAttributesSet via ObjectScript.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   02 Sep 1999 08:44:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Add Message Qualifiers to better control routing of messages.
   Add new messsages for drawing/figures.
   Fixed Bug in CloseObject (Friends recurrsion)
   Add published attribute and defaults on construction of SVObjectClass..
   Added GetObjectColor and GetObjectState
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   01 Sep 1999 11:47:54   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised versioning to 3.0 Beta 1
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   Aug 31 1999 09:34:52   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  27
   SCR Title:  Results Picker
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Fixed bug in GetAllowedAttributes() method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   30 Aug 1999 19:31:44   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Introduced Input Interface Handling.
   ( GETFIRST - Message Handler )
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   30 Aug 1999 19:15:30   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SV_IS_KIND_OF not to call SV_VALIDATE_OBJECT.
   Revised SVM_CREATE_ALL_OBJECTS to route to Children only.
   Added Scripting support for Freinds Architecure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   27 Aug 1999 12:58:48   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Added SetupDialog() Function to SVObjectClass and Message.
   This gives every derived class the possibility to support his own Setup Dialog.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   27 Aug 1999 09:06:26   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     Add capability to script freind links.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   Aug 25 1999 22:50:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  1
   SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
   Checked in by:  Steve Jones
   Change Description:  
     Revised Validate method to be OnValidate method.
    ( OnValidate - local scope validation).
   Added Validate method (Routes to all owned objects)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   Aug 25 1999 16:43:32   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  27
   SCR Title:  Results Picker
   Checked in by:  F Roland "Nick" Bjorklund
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   25 Aug 1999 11:36:14   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Added SVObjectInfoArrayClass.
   Added Friend Mechanism.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/