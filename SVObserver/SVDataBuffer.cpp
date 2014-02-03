//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataBuffer
//* .File Name       : $Workfile:   SVDataBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 10:32:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageProcessingClass.h"

#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVDataBuffer.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVDataBufferInfoClass::SVDataBufferInfoClass()
{
	POwnerTool			= NULL;
	Length				= 0L;
	Type				= 0L;
}


SVDataBufferInfoClass::SVDataBufferInfoClass( SVDataBufferInfoClass& S2 )
{
	POwnerTool		= S2.POwnerTool;
	Length			= S2.Length;
	Type			= S2.Type;
	HBuffer			= S2.HBuffer;
}

SVDataBufferInfoClass SVDataBufferInfoClass::operator=( SVDataBufferInfoClass& S2 )
{
	POwnerTool		= S2.POwnerTool;
	Length			= S2.Length;
	Type			= S2.Type;
	HBuffer			= S2.HBuffer;

	return ( *this );
}


SV_IMPLEMENT_CLASS( SVDataBufferClass, SVDataBufferClassGuid );

SVDataBufferClass::SVDataBufferClass( SVObjectClass* POwner, int StringResourceID )
			      :SVObjectAppClass( POwner, StringResourceID )
{
	init();
	
}

void SVDataBufferClass::init()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVDataBufferClassObjectType;
}

SVDataBufferClass::~SVDataBufferClass()
{
	CloseObject();
}

BOOL SVDataBufferClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVObjectAppClass::CreateObject( PCreateStructure ) )
	{
		if( dataInfo.Type != 0 && dataInfo.Length > 0 )
		{
			dataInfo.POwnerTool = GetTool();

			// Create data buffer...

			bOk = SVImageProcessingClass::Instance().CreateDataBuffer(&dataInfo ) == S_OK;
		}
	}

	isCreated = bOk;

	return bOk;
}


BOOL SVDataBufferClass::CloseObject()
{
	BOOL rc = SVObjectAppClass::CloseObject();

	rc = (SVImageProcessingClass::Instance().DestroyDataBuffer( &dataInfo ) == S_OK ) && rc;

	return rc;
}

BOOL SVDataBufferClass::Resize( int NewLength )
{
	return SVImageProcessingClass::Instance().ResizeDataBuffer( &dataInfo, NewLength ) == S_OK;
}

SVDataBufferInfoClass& SVDataBufferClass::GetDataBufferInfo()
{
	return dataInfo;
}


SVDataBufferHandleStruct& SVDataBufferClass::GetDataBufferHandle()
{
	return dataInfo.HBuffer;
}

LONG_PTR SVDataBufferClass::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

	return SVObjectAppClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDataBuffer.cpp_v  $
 * 
 *    Rev 1.1   01 Feb 2014 10:32:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:05:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   04 Nov 2010 13:30:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   15 Dec 2009 15:49:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   24 Jul 2007 11:47:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   17 Feb 2005 13:37:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   16 Feb 2005 13:30:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   08 Jul 2003 09:04:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   17 Apr 2003 17:30:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   29 Jan 2003 15:11:40   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage method to adjust image depth correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Dec 2002 19:12:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Nov 2002 11:43:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System, translated to using Imaging class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:14:46   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 17:34:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Replacement for old image result buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
