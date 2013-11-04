// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIAcqStruct
// * .File Name       : $Workfile:   SVCIAcqStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:00  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCIAcqStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all attribute data to desired defaults.
*/
SVCIAcqStruct::SVCIAcqStruct()
: mhShutdown( NULL ), mhThread( NULL ), m_BufferIndexQueue( 20 )
{
	m_lServerIndex = -1;

	m_hCorServer = CORHANDLE_NULL;
	m_hCorAcq = CORHANDLE_NULL;
	m_hCorLut = CORHANDLE_NULL;
	m_hCorCam = CORHANDLE_NULL;
	m_hCorVic = CORHANDLE_NULL;
	m_hCorXfer = CORHANDLE_NULL;

	m_ulWidth = 640;
	m_ulHeight = 480;
	m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_MONO8;
	m_ulType = CORBUFFER_VAL_TYPE_CONTIGUOUS;

	m_ulLutEntries = 256;
	m_ulLutFormat = CORLUT_VAL_FORMAT_MONO8;

	m_ulCycleMode = CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH;
	m_UsesTrashBuffer = true;

	m_lIsStarted = 0;
	m_lIsTriggerRegistered = 0;

	m_pBufferInterface = NULL;

	m_StartIsTrash = true;
	m_StartIndex = -1;
	m_StartFrameTimeStamp = 0;

	mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );
}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVCIAcqStruct::SVCIAcqStruct( const SVCIAcqStruct& p_rValue)
: mhShutdown( NULL ), mhThread( NULL ), m_BufferIndexQueue( 20 )
{
	mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );

	*this = p_rValue;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVCIAcqStruct::~SVCIAcqStruct()
{
	if( mhShutdown != NULL )
	{
		::SetEvent( mhShutdown );
		::CloseHandle( mhShutdown );
		mhShutdown = NULL;
	}

	if( mhThread != NULL )
	{
		::CloseHandle( mhThread );
		mhThread = NULL;
	}
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVCIAcqStruct& SVCIAcqStruct::operator=( const SVCIAcqStruct& p_rValue )
{
	m_lServerIndex = p_rValue.m_lServerIndex;

	m_csName = (LPCTSTR)p_rValue.m_csName;
	m_csFullName = (LPCTSTR)p_rValue.m_csFullName;

	m_hCorServer = p_rValue.m_hCorServer;
	m_hCorAcq = p_rValue.m_hCorAcq;
	m_hCorLut = p_rValue.m_hCorLut;
	m_hCorCam = p_rValue.m_hCorCam;
	m_hCorVic = p_rValue.m_hCorVic;
	m_hCorXfer = p_rValue.m_hCorXfer;

	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;
	m_ulFormat = p_rValue.m_ulFormat;
	m_ulType = p_rValue.m_ulType;

	m_ulLutEntries = p_rValue.m_ulLutEntries;
	m_ulLutFormat = p_rValue.m_ulLutFormat;

	m_ulCycleMode = p_rValue.m_ulCycleMode;
	m_UsesTrashBuffer = p_rValue.m_UsesTrashBuffer;

	m_lIsStarted = p_rValue.m_lIsStarted;

	m_caBuffers.RemoveAll();
	m_caBuffers.Copy( p_rValue.m_caBuffers );

	m_lIsTriggerRegistered = p_rValue.m_lIsTriggerRegistered;

	m_svTriggerCallbacks.RemoveAll();

	SVCallbackListClass *l_pList = (SVCallbackListClass *)(&(p_rValue.m_svTriggerCallbacks)); 

	m_svTriggerCallbacks.AddTail( l_pList );

	m_pBufferInterface = p_rValue.m_pBufferInterface;

	m_StartIsTrash = p_rValue.m_StartIsTrash;
	m_StartIndex = p_rValue.m_StartIndex;
	m_StartFrameTimeStamp = p_rValue.m_StartFrameTimeStamp;

	m_BufferIndexQueue.clear();

	return *this;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCIAcqStruct.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:59:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Jun 2011 10:48:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use original functionality for acquisition and dynamic loading of the DLL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 May 2011 15:30:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added ring buffer to DLL to remove locks in Sapera callback threads.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 May 2011 09:50:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the Dalsa/Coreco libraries directly, change the acquisition mode to CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH, and change the buffer allocation to CORBUFFER_VAL_TYPE_SCATTER_GATHER.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 08:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jul 2009 13:04:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new methodology to aquire a buffer from SVObserver and copy/convert data from the acquisition buffer into the SVObserver buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/