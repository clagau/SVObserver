// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVORequestClass
// * .File Name       : $Workfile:   SVORequestClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVORequestClass.h"

/*
This constructor initializes the local attributes and calls the Reset method.
*/
SVORequestClass::SVORequestClass()
{
	mpvOwner = NULL;
	mpvParent = NULL;
	mpvExtraData = NULL;

	Reset();
}

/*
This constructor initializes the local attributes, calls the Reset method, and calls the assignment operator.
*/
SVORequestClass::SVORequestClass(const SVORequestClass &rRequest)
{
	mpvOwner = NULL;
	mpvParent = NULL;
	mpvExtraData = NULL;

	Reset();

	*this = rRequest;
}

/*
This destructor is a placeholder and performs no operations.
*/
SVORequestClass::~SVORequestClass()
{
}

/*
This operator assigns the data from the provided object into this object.
*/
const SVORequestClass &SVORequestClass::operator=(const SVORequestClass &rRequest)
{
	mbIsValid = rRequest.mbIsValid;
	mbIsComplete = rRequest.mbIsComplete;

	mpvOwner = rRequest.mpvOwner;
	mpvParent = rRequest.mpvParent;
	mpvExtraData = rRequest.mpvExtraData;

	m_StartTick = rRequest.m_StartTick;

	mResponse = rRequest.mResponse;

	mDMHandle.Assign( rRequest.mDMHandle, rRequest.mDMHandle.GetLockType() );

	return *this;
}

/*
This method sets all the local attributes to the desired defaults.
*/
HRESULT SVORequestClass::Reset()
{
	HRESULT hrOk = S_OK;

	mbIsValid = FALSE;
	mbIsComplete = FALSE;

	m_StartTick = SVClock::GetTimeStamp();

	if ( mResponse.Reset() != S_OK )
	{
		hrOk = S_FALSE;
	}

	mDMHandle.clear();

	return hrOk;
}

/*
This method returns the current validity state of this object.
*/
bool SVORequestClass::IsValid() const
{
	return mbIsValid;
}

/*
This method returns the current completion state of this object.
*/
bool SVORequestClass::IsComplete() const
{
	return mbIsComplete;
}

/*
This method returns the state of the validity attribute.
*/
HRESULT SVORequestClass::GetIsValid( bool &rbIsValid ) const
{
	HRESULT hrOk = S_OK;

	rbIsValid = mbIsValid;

	return hrOk;
}

/*
This method sets the state of the validity attribute.
*/
HRESULT SVORequestClass::SetIsValid( bool bIsValid )
{
	HRESULT hrOk = S_OK;

	mbIsValid = bIsValid;

	return hrOk;
}

/*
This method returns the state of the completion attribute.
*/
HRESULT SVORequestClass::GetIsComplete( bool &rbIsComplete )
{
	HRESULT hrOk = S_OK;

	rbIsComplete = mbIsComplete;

	return hrOk;
}

/*
This method sets the state of the completion attribute.
*/
HRESULT SVORequestClass::SetIsComplete( bool bIsComplete )
{
	HRESULT hrOk = S_OK;

	mbIsComplete = bIsComplete;

	return hrOk;
}

/*
This method returns the pointer of the owner object of this object.
*/
HRESULT SVORequestClass::GetOwner( void **ppvOwner )
{
	HRESULT hrOk = S_OK;

	*ppvOwner = mpvOwner;

	return hrOk;
}

/*
This method sets the pointer of the owner object of this object.
*/
HRESULT SVORequestClass::SetOwner( void *pvOwner )
{
	HRESULT hrOk = S_OK;

	mpvOwner = pvOwner;

	return hrOk;
}

/*
This method returns the pointer of the parent object of this object.
*/
HRESULT SVORequestClass::GetParent( void **ppvParent )
{
	HRESULT hrOk = S_OK;

	*ppvParent = mpvParent;

	return hrOk;
}

/*
This method sets the pointer of the parent object of this object.
*/
HRESULT SVORequestClass::SetParent( void *pvParent )
{
	HRESULT hrOk = S_OK;

	mpvParent = pvParent;

	return hrOk;
}

/*
This method returns the pointer of the context data of this object.
*/
HRESULT SVORequestClass::GetExtraData( void **ppvExtraData )
{
	HRESULT hrOk = S_OK;

	*ppvExtraData = mpvExtraData;

	return hrOk;
}

/*
This method sets the pointer of the context data of this object.
*/
HRESULT SVORequestClass::SetExtraData( void *pvExtraData )
{
	HRESULT hrOk = S_OK;

	mpvExtraData = pvExtraData;

	return hrOk;
}

/*
This method returns the start time stamp of this object.
*/
HRESULT SVORequestClass::GetStartTick( SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	p_rTick = m_StartTick;

	return hrOk;
}

/*
This method sets the start time stamp of this object.
*/
HRESULT SVORequestClass::SetStartTick( const SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_StartTick = p_rTick;

	return hrOk;
}

/*
This method returns the start response of this object.
*/
HRESULT SVORequestClass::GetResponse( SVOResponseClass& p_rResponse )
{
	HRESULT hrOk = S_OK;

	p_rResponse = mResponse;

	return hrOk;
}

/*
This method sets the start response of this object.
*/
HRESULT SVORequestClass::SetResponse( SVOResponseClass p_Response )
{
	HRESULT hrOk = S_OK;

	mResponse = p_Response;

	mDMHandle.Assign( mResponse.mDMHandle, mResponse.mDMHandle.GetLockType() );

	SetIsValid( mResponse.IsValid() && -1 < mDMHandle.GetIndex() );

	return hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVORequestClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 15:28:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Apr 2013 13:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 11:57:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Feb 2010 10:58:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 09:24:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2009 10:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to handle buffer transfer and signal handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jan 2003 16:18:54   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Data Manager index attribute to classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:16:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
