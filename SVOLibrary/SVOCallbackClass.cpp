// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCallbackClass
// * .File Name       : $Workfile:   SVOCallbackClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:53:54  $
// ******************************************************************************

#include "stdafx.h"
#include "SVOCallbackClass.h"

/*
This constructor initializes the attributes to the desired default values.
*/
SVOCallbackClass::SVOCallbackClass()
{
	mpCallback = NULL;
  mpvOwner = NULL;
  mpvCaller = NULL;

	m_lRefCount = 0;

	mRequestQueue.Create();
	mResponseQueue.Create();
}

/*
This constructor initializes the attributes of this object from the provided object's values.
*/
SVOCallbackClass::SVOCallbackClass(const SVOCallbackClass& rData)
{
	mpCallback = NULL;
  mpvOwner = NULL;
  mpvCaller = NULL;

	m_lRefCount = 0;

	mRequestQueue.Create();
	mResponseQueue.Create();

	*this = rData;
}

/*
This destructor clears and destroys the two internal queues of this objects.
*/
SVOCallbackClass::~SVOCallbackClass()
{
	mRequestQueue.Destroy();
	mResponseQueue.Destroy();
}

/*
This operator copies the attributes of the provided object's values to this object.
*/
const SVOCallbackClass& SVOCallbackClass::operator=(const SVOCallbackClass& rData)
{
	mpCallback = rData.mpCallback;
  mpvOwner = rData.mpvOwner;
  mpvCaller = rData.mpvCaller;

	m_lRefCount = rData.m_lRefCount;

	mRequestQueue.RemoveAll();
	mResponseQueue.RemoveAll();

	return *this;	
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVOCallbackClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:53:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 10:58:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 09:22:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Mar 2003 12:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added new member variable m_lRefCount.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:15:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
