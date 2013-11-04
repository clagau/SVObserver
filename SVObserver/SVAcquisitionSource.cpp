//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionSource
//* .File Name       : $Workfile:   SVAcquisitionSource.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:28  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#include "stdafx.h"
#include "SVAcquisitionSource.h"

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "SVFileNameManagerClass.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAcquisitionSourceClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SVAcquisitionSourceClass::SVAcquisitionSourceClass()
{
	AcquisitionType = AcNone;
	AcquisitionTriggerType = 0;
}

SVAcquisitionSourceClass::SVAcquisitionSourceClass( const SVAcquisitionSourceClass& svASC )
{
	*this = svASC;
}

SVAcquisitionSourceClass::SVAcquisitionSourceClass( int iAcqType, 
                                                    int iAcqTrigType, 
                                                    const SVFileNameArrayClass& svFileNameArray, 
                                                    const SVImageInfoClass& svImageInfo, 
                                                    const SVLightReference& svLightRef )
{
	AcquisitionType = iAcqType;
	AcquisitionTriggerType = iAcqTrigType;
	AcquisitionImageInfo = svImageInfo;
	AcquisitionLightReferenceArray	= svLightRef;

	SetFileNameArray( svFileNameArray );
}

SVAcquisitionSourceClass::SVAcquisitionSourceClass( int iAcqType, 
                                                    int iAcqTrigType, 
                                                    LPCTSTR szFileNameList, 
                                                    const SVImageInfoClass& svImageInfo, 
                                                    const SVLightReference& svLightRef )
{
	AcquisitionType = iAcqType;
	AcquisitionTriggerType = iAcqTrigType;
	AcquisitionImageInfo = svImageInfo;
	AcquisitionLightReferenceArray	= svLightRef;

	SetFileNameArray( szFileNameList );
}

SVAcquisitionSourceClass::~SVAcquisitionSourceClass()
{
	SVFileNameManagerClass svFileNameManager;

	for ( int i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.RemoveItem( &(msvFileNameArray.ElementAt(i)) );
	}
}

const SVAcquisitionSourceClass& SVAcquisitionSourceClass::operator=(const SVAcquisitionSourceClass& svASC)
{
	AcquisitionType = svASC.AcquisitionType;
	AcquisitionTriggerType = svASC.AcquisitionTriggerType;
	AcquisitionImageInfo = svASC.AcquisitionImageInfo;
	AcquisitionLightReferenceArray = svASC.AcquisitionLightReferenceArray;

	SetFileNameArray( svASC.msvFileNameArray );

	return *this;
}

SVFileNameArrayClass SVAcquisitionSourceClass::GetFileNameArray()
{
	return msvFileNameArray;
}

BOOL SVAcquisitionSourceClass::SetFileNameArray(LPCTSTR szFileNameList)
{
	BOOL bOk = TRUE;

	int i( 0 );

	SVFileNameManagerClass svFileNameManager;

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.RemoveItem( &(msvFileNameArray.ElementAt(i)) );
	}

	msvFileNameArray.SetFileNameList( szFileNameList );

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.AddItem( &(msvFileNameArray.ElementAt(i)) );

		CString csFullFileName = msvFileNameArray.ElementAt(i).GetFullFileName();
	}

	return bOk;
}

BOOL SVAcquisitionSourceClass::SetFileNameArray(const SVFileNameArrayClass& svFileNameArray)
{
	BOOL bOk = TRUE;

	int i( 0 );

	SVFileNameManagerClass svFileNameManager;

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.RemoveItem( &(msvFileNameArray.ElementAt(i)) );
	}

	msvFileNameArray = svFileNameArray;

	for ( i = 0; i < msvFileNameArray.GetSize(); i++ )
	{
		svFileNameManager.AddItem( &(msvFileNameArray.ElementAt(i)) );
	}

	return bOk;
}


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAcquisitionSourceListClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


//******************************************************************************
// Constructor(s):
//******************************************************************************
SVAcquisitionSourceListClass::SVAcquisitionSourceListClass() 
{
}

SVAcquisitionSourceListClass::SVAcquisitionSourceListClass(const SVAcquisitionSourceListClass& svASLC )
{
	*this = svASLC;
}

//******************************************************************************
// Destructor(s):
//******************************************************************************
SVAcquisitionSourceListClass::~SVAcquisitionSourceListClass() 
{
	DeleteAll();
}

//******************************************************************************
// Operator(s):
//******************************************************************************

const SVAcquisitionSourceListClass& SVAcquisitionSourceListClass::operator=(const SVAcquisitionSourceListClass& svASLC)
{
	DeleteAll();

	for( int i = 0; i < svASLC.GetSize(); i++ )
	{
		if( svASLC[i] )
		{
			Add( new SVAcquisitionSourceClass( *(svASLC[i]) ) );
		}
	}

	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Destroy Operator
////////////////////////////////////////////////////////////////////////////////
void SVAcquisitionSourceListClass::DeleteAll() 
{
	for( int i = GetSize() - 1; i >= 0; -- i )
		if( GetAt( i ) )
			delete( GetAt( i ) );

	RemoveAll();
}






//** EOF **

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAcquisitionSource.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:26:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   21 Jun 2007 11:51:42   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   08 Jul 2003 08:43:56   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed from SVLightReferenceArray to SVLightReference
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   17 Apr 2003 17:00:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   17 Apr 2003 16:58:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   17 Apr 2003 16:25:30   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   19 Nov 2002 09:59:26   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed include SVGrabHookStruct.h
   	
   Added SetFileVersion for AcquisitionLightReferenceArray
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
