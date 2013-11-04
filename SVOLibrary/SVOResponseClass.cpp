// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOResponseClass
// * .File Name       : $Workfile:   SVOResponseClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVOResponseClass.h"

/*
This constructor initializes the local attributes and calls the Reset method.
*/
SVOResponseClass::SVOResponseClass()
{
	mpvOwner = NULL;

	Reset();
}

/*
This constructor initializes the local attributes, calls the Reset method, and calls the assignment operator.
*/
SVOResponseClass::SVOResponseClass(const SVOResponseClass &rResponse)
{
	mpvOwner = NULL;

	Reset();

	*this = rResponse;
}

/*
This destructor is a placeholder and performs no operations.
*/
SVOResponseClass::~SVOResponseClass()
{
}

/*
This method returns the current validity state of this object.
*/
bool SVOResponseClass::IsValid() const
{
	return mbIsValid;
}

/*
This method returns the current completion state of this object.
*/
bool SVOResponseClass::IsComplete() const
{
	return mbIsComplete;
}

/*
This operator assigns the data from the provided object into this object.
*/
const SVOResponseClass &SVOResponseClass::operator=(const SVOResponseClass &rResponse)
{
	mbIsValid = rResponse.mbIsValid;
	mbIsComplete = rResponse.mbIsComplete;
	mDMHandle.Assign( rResponse.mDMHandle, rResponse.mDMHandle.GetLockType() );
	mpvOwner = rResponse.mpvOwner;
	mExtraData = rResponse.mExtraData;
	m_StartTick = rResponse.m_StartTick;
	m_EndTick = rResponse.m_EndTick;

	return *this;
}

/*
This method sets all the local attributes to the desired defaults.
*/
HRESULT SVOResponseClass::Reset()
{
	HRESULT hrOk = S_OK;

	mbIsValid = FALSE;
	mbIsComplete = FALSE;
	mDMHandle.clear();
	mExtraData.swap(boost::any());

	m_StartTick = SVClock::GetTimeStamp();
	m_EndTick = SVClock::GetTimeStamp();

	return hrOk;
}

/*
This method returns the pointer of the owner object of this object.
*/
HRESULT SVOResponseClass::GetOwner( void **ppvOwner )
{
	HRESULT hrOk = S_OK;

	*ppvOwner = mpvOwner;

	return hrOk;
}

/*
This method sets the pointer of the owner object of this object.
*/
HRESULT SVOResponseClass::SetOwner( void *pvOwner )
{
	HRESULT hrOk = S_OK;

	mpvOwner = pvOwner;

	return hrOk;
}

/*
This method returns the start time stamp of this object.
*/
HRESULT SVOResponseClass::GetStartTick( SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	p_rTick = m_StartTick;

	return hrOk;
}

/*
This method sets the start time stamp of this object.
*/
HRESULT SVOResponseClass::SetStartTick( const SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_StartTick = p_rTick;

	return hrOk;
}

/*
This method returns the end time stamp of this object.
*/
HRESULT SVOResponseClass::GetEndTick( SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	p_rTick = m_EndTick;

	return hrOk;
}

/*
This method sets the end time stamp of this object.
*/
HRESULT SVOResponseClass::SetEndTick( const SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_EndTick = p_rTick;

	return hrOk;
}

/*
This method returns the state of the validity attribute.
*/
HRESULT SVOResponseClass::GetIsValid( bool &rbIsValid ) const
{
	HRESULT hrOk = S_OK;

	rbIsValid = mbIsValid;

	return hrOk;
}

/*
This method sets the state of the validity attribute.
*/
HRESULT SVOResponseClass::SetIsValid( bool bIsValid )
{
	HRESULT hrOk = S_OK;

	mbIsValid = bIsValid;

	return hrOk;
}

/*
This method returns the state of the completion attribute.
*/
HRESULT SVOResponseClass::GetIsComplete( bool &rbIsComplete )
{
	HRESULT hrOk = S_OK;

	rbIsComplete = mbIsComplete;

	return hrOk;
}

/*
This method sets the state of the completion attribute.
*/
HRESULT SVOResponseClass::SetIsComplete( bool bIsComplete )
{
	HRESULT hrOk = S_OK;

	mbIsComplete = bIsComplete;

	return hrOk;
}

/*
This method returns the context data of this object.
*/
HRESULT SVOResponseClass::GetExtraData( boost::any& p_rExtraData )
{
	HRESULT hrOk = S_OK;

	p_rExtraData = mExtraData;

	return hrOk;
}

/*
This method sets the context data of this object.
*/
HRESULT SVOResponseClass::SetExtraData( const boost::any& p_rExtraData )
{
	HRESULT hrOk = S_OK;

	mExtraData = p_rExtraData;

	return hrOk;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVOResponseClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 15:28:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Apr 2013 13:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 11:57:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jan 2013 10:02:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use boost::any for the extra data
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
 *    Rev 1.3   03 Sep 2009 09:24:02   jspila
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
