//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferInterface
//* .File Name       : $Workfile:   SVImageBufferInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 20:14:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageBufferInterface.h"

SVImageBufferInterface::SVImageBufferInterface()
:	m_StartTimeStamp( 0 ),
	m_EndTimeStamp( 0 )
{
}

SVImageBufferInterface::SVImageBufferInterface( const SVImageBufferInterface& p_rObject )
:	m_ImageHandle( p_rObject.m_ImageHandle ),
	m_StartTimeStamp( p_rObject.m_StartTimeStamp ),
	m_EndTimeStamp( p_rObject.m_EndTimeStamp )
{
	m_IndexHandle.Assign( p_rObject.m_IndexHandle, p_rObject.m_IndexHandle.GetLockType() );
}

SVImageBufferInterface::SVImageBufferInterface( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle )
:	m_ImageHandle( p_ImageHandle ),
	m_StartTimeStamp( 0 ),
	m_EndTimeStamp( 0 )
{
	m_IndexHandle.Assign( p_rIndexHandle, p_rIndexHandle.GetLockType() );
}

SVImageBufferInterface::~SVImageBufferInterface()
{
}

const SVImageBufferInterface& SVImageBufferInterface::operator=( const SVImageBufferInterface& p_rObject )
{
	if( this != &p_rObject )
	{
		m_ImageHandle = p_rObject.m_ImageHandle;
		m_IndexHandle.Assign( p_rObject.m_IndexHandle, p_rObject.m_IndexHandle.GetLockType() );
		m_StartTimeStamp = p_rObject.m_StartTimeStamp;
		m_EndTimeStamp = p_rObject.m_EndTimeStamp;
	}

	return *this;
}

void SVImageBufferInterface::clear()
{
	m_ImageHandle.clear();
	m_IndexHandle.clear();
	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;
}

HRESULT SVImageBufferInterface::Assign( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle )
{
	HRESULT l_Status = S_OK;

	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;

	m_ImageHandle = p_ImageHandle;

	l_Status = m_IndexHandle.Assign( p_rIndexHandle, p_rIndexHandle.GetLockType() );

	return l_Status;
}

unsigned char* SVImageBufferInterface::GetBufferAddress() const
{
	unsigned char* l_pBuffer = NULL;

	if( ! m_ImageHandle.empty() )
	{
		l_pBuffer = reinterpret_cast< unsigned char* >( m_ImageHandle->GetBufferAddress() );
	}

	return l_pBuffer;
}

const SVClock::SVTimeStamp& SVImageBufferInterface::GetStartFrameTimeStamp() const
{
	return m_StartTimeStamp;
}

void SVImageBufferInterface::SetStartFrameTimeStamp()
{
	m_StartTimeStamp = SVClock::GetTimeStamp();
}

void SVImageBufferInterface::SetStartFrameTimeStamp( const SVClock::SVTimeStamp& p_rTimeStamp )
{
	m_StartTimeStamp = p_rTimeStamp;
}

const SVClock::SVTimeStamp& SVImageBufferInterface::GetEndFrameTimeStamp() const
{
	return m_EndTimeStamp;
}

void SVImageBufferInterface::SetEndFrameTimeStamp()
{
	m_EndTimeStamp = SVClock::GetTimeStamp();
}

void SVImageBufferInterface::SetEndFrameTimeStamp( const SVClock::SVTimeStamp& p_rTimeStamp )
{
	m_EndTimeStamp = p_rTimeStamp;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVImageLibrary\SVImageBufferInterface.cpp_v  $
 * 
 *    Rev 1.1   07 May 2013 20:14:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 12:34:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:53:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2009 13:26:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image library to include the new data manager objects to track the indexes.  Updated DLL interface to use the new buffer transfer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
