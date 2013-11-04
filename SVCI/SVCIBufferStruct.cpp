// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIBufferStruct
// * .File Name       : $Workfile:   SVCIBufferStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:02  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCIBufferStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVCIBufferStruct::SVCIBufferStruct()
{
	m_hCorServer = CORHANDLE_NULL;

	m_ulWidth = 640;
	m_ulHeight = 480;
	m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_MONO8;
	m_ulType = CORBUFFER_VAL_TYPE_SCATTER_GATHER;

	m_pAddress = NULL;
	m_Pitch = 0;

	m_hCorBuffer = CORHANDLE_NULL;

	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;

}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVCIBufferStruct::SVCIBufferStruct( const SVCIBufferStruct &p_rValue )
{
	*this = p_rValue;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVCIBufferStruct::~SVCIBufferStruct()
{
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVCIBufferStruct &SVCIBufferStruct::operator=( const SVCIBufferStruct &p_rValue )
{
	m_hCorServer = p_rValue.m_hCorServer;

	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;
	m_ulFormat = p_rValue.m_ulFormat;
	m_ulType = p_rValue.m_ulType;

	m_pAddress = p_rValue.m_pAddress;
	m_Pitch = p_rValue.m_Pitch;

	m_hCorBuffer = p_rValue.m_hCorBuffer;

	m_StartTimeStamp = p_rValue.m_StartTimeStamp;
	m_EndTimeStamp = p_rValue.m_EndTimeStamp;

	return *this;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCI\SVCIBufferStruct.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:59:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 May 2011 09:50:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the Dalsa/Coreco libraries directly, change the acquisition mode to CORXFER_VAL_CYCLE_MODE_SYNCHRONOUS_WITH_TRASH, and change the buffer allocation to CORBUFFER_VAL_TYPE_SCATTER_GATHER.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2010 08:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jul 2009 13:04:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new methodology to aquire a buffer from SVObserver and copy/convert data from the acquisition buffer into the SVObserver buffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/