//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassInfoStruct
//* .File Name       : $Workfile:   SVClassInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVClassInfoStruct.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTaskObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVInterfaceListClass
////////////////////////////////////////////////////////////////////////////////
SVInterfaceListClass::SVInterfaceListClass()
{
}

SVInterfaceListClass::SVInterfaceListClass( const SVInterfaceListClass& RIn )
{
	Copy( RIn );
}

SVInterfaceListClass SVInterfaceListClass::operator=( const SVInterfaceListClass& RIn )
{
	Copy( RIn );
	return( *this );
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVClassInfoStruct
////////////////////////////////////////////////////////////////////////////////
SVClassInfoStruct::SVClassInfoStruct()
{
	ClassId = SVInvalidGUID;
}

SVClassInfoStruct::SVClassInfoStruct( const SVClassInfoStruct& RClassInfo )
{
	ClassId					= RClassInfo.ClassId;
	ClassName				= RClassInfo.ClassName;
	ObjectTypeInfo			= RClassInfo.ObjectTypeInfo;
	DesiredInputInterface	= RClassInfo.DesiredInputInterface;
}

SVClassInfoStruct::~SVClassInfoStruct()
{
}

//******************************************************************************
// Operator(s):
//******************************************************************************
SVClassInfoStruct SVClassInfoStruct::operator=( const SVClassInfoStruct& RClassInfo )
{
	ClassId					= RClassInfo.ClassId;
	ClassName				= RClassInfo.ClassName;
	ObjectTypeInfo			= RClassInfo.ObjectTypeInfo;
	DesiredInputInterface	= RClassInfo.DesiredInputInterface;
	return( *this );
}


SVObjectClass* SVClassInfoStruct::Construct()
{
	SVObjectClass* pObject = NULL;

	SVObjectManagerClass::Instance().ConstructObject( ClassId, pObject );

	if( pObject )
	{
		pObject->SetName( ClassName );

		if( SV_IS_KIND_OF( pObject, SVTaskObjectClass ) )
		{
			// Get Input Interface...
			SVTaskObjectClass* pTaskObject = ( SVTaskObjectClass* ) pObject;
			SVInputInfoListClass inputInterface;
			pTaskObject->GetPrivateInputList( inputInterface );

			// Apply desired input interface...
			for( int i = 0; i < DesiredInputInterface.GetSize() && i < inputInterface.GetSize(); ++ i )
			{
				SVInObjectInfoStruct* pInInfo			= inputInterface[ i ];

				SVObjectTypeInfoStruct& rDesiredInType	= DesiredInputInterface[ i ];

				if( pInInfo )
					pInInfo->SetInputObjectType( rDesiredInType );

				SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

				ASSERT( pInInfo->GetInputObjectInfo().UniqueObjectID != SVInvalidGUID ||
				        info.EmbeddedID != SVInvalidGUID || 
				        info.ObjectType != SVNotSetObjectType ||
				        info.SubType != SVNotSetSubObjectType );
			}
		}
	}
	return pObject;
}


//******************************************************************************
// Constructor(s):
//******************************************************************************
SVClassInfoStructListClass::SVClassInfoStructListClass()
{
}

//******************************************************************************
// Destructor:
//******************************************************************************
SVClassInfoStructListClass::~SVClassInfoStructListClass() 
{
	RemoveAll();
}

//******************************************************************************
// Operator(s):
//******************************************************************************
int SVClassInfoStructListClass::Find( const GUID& ClassID ) 
{ 
	for( int i = 0; i < GetSize(); i++ )
	{
		if( !memcmp( &ClassID, &GetAt( i ).ClassId, sizeof( GUID ) ) )
			return i;
	}
	return -1;
};


long SVClassInfoStructListClass::Size() const
{
	return GetSize();
}

CString SVClassInfoStructListClass::GetClassName(int index) const
{
	return GetAt(index).ClassName;
}

GUID SVClassInfoStructListClass::GetClassID(int index) const
{
	return GetAt(index).ClassId;
}

SvOi::IClassInfoStructList* SvOi::createSVClassInfoStructList()
{
	return new SVClassInfoStructListClass();
}

void SvOi::deleteSVClassInfoStructList( SvOi::IClassInfoStructList* pObject )
{
	SVClassInfoStructListClass *pOb = dynamic_cast<SVClassInfoStructListClass*>(pObject);
	if (nullptr != pOb)
	{
		delete pOb;
	}
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVClassInfoStruct.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:53:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   18 Jun 2012 17:59:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   08 Dec 2010 14:50:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   08 Nov 2010 14:35:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate container types.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   15 Dec 2009 15:13:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   21 Jun 2007 12:01:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   17 Feb 2005 13:23:10   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   16 Feb 2005 13:18:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   17 Apr 2003 17:08:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   19 Nov 2002 11:00:06   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Cosmetic change: TheClassRegister -> TheSVClassRegister
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
