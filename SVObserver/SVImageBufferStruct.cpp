//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferStruct
//* .File Name       : $Workfile:   SVImageBufferStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:49:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageBufferStruct.h"

#include "SVImageProcessingClass.h"

HRESULT SVImageBufferStruct::CreateBuffer()
{
	DestroyBuffer();
	return SVImageBufferStructResourceManager::Create( this );
}

HRESULT SVImageBufferStruct::DestroyBuffer()
{
	return SVImageBufferStructResourceManager::Destroy( this );
}
HRESULT SVImageBufferStructResourceManager::Create( SVImageBufferStruct* pStruct )
{
	return SVImageProcessingClass::Instance().CreateImageBuffer( pStruct->info, pStruct->handle );
}

HRESULT SVImageBufferStructResourceManager::Destroy( SVImageBufferStruct* pStruct )
{
	pStruct->handle.clear();

	return S_OK;
}

void SVImageBufferStructResourceManager::Clear( SVImageBufferStruct& rStruct )
{
	rStruct.handle = SVSmartHandlePointer();
}

SVImageBufferStruct::operator SVScalarValue() const
{
	return SVScalarValue(strName, CString(), object);
}

SVImageBufferStruct::SVImageBufferStruct( const SVScalarValue& rhs )
{
	strName = rhs.strName;
	object = rhs.object;
}

const SVImageBufferStruct& SVImageBufferStruct::operator = ( const SVImageBufferStruct& rhs )
{
	handle = rhs.handle;
	info = rhs.info;
	strName = rhs.strName;
	object = rhs.object;
	overlay = rhs.overlay;
	return *this;
}

//******************************************************************************

//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImageBufferStruct.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:49:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Feb 2011 14:45:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Nov 2010 13:39:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 13:54:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:20:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
