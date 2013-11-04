//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleImage
//* .File Name       : $Workfile:   SVImageBufferHandleImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageBufferHandleImage.h"

SVImageBufferHandleImage::SVImageBufferHandleImage()
: SVImageBufferHandleData(), m_Buffer()
{
}

SVImageBufferHandleImage::~SVImageBufferHandleImage()
{
	clear();
}

bool SVImageBufferHandleImage::empty() const
{
	return m_Buffer.empty();
}

void SVImageBufferHandleImage::clear()
{
	m_Buffer.clear();
}

const SVMatroxBuffer& SVImageBufferHandleImage::GetBuffer() const
{
	return m_Buffer;
}

SVMatroxBuffer& SVImageBufferHandleImage::GetBuffer()
{
	return m_Buffer;
}

HRESULT SVImageBufferHandleImage::SetHandleData( const SVImageBufferHandleStruct& p_rObject )
{
	m_Buffer = p_rObject.milImage;

	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageBufferHandleImage.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:44:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Feb 2011 16:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 09:54:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

