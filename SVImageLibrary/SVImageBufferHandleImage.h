//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleImage
//* .File Name       : $Workfile:   SVImageBufferHandleImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:36  $
//******************************************************************************

#ifndef SVIMAGEBUFFERHANDLEIMAGE_H
#define SVIMAGEBUFFERHANDLEIMAGE_H

#include "SVImageBufferHandleData.h"
#include "SVImageBufferHandleStruct.h"

class SVImageBufferHandleImage : public SVImageBufferHandleData
{
public:
	SVImageBufferHandleImage();
	virtual ~SVImageBufferHandleImage();

	bool empty() const;
	void clear();

	const SVMatroxBuffer& GetBuffer() const;
	SVMatroxBuffer& GetBuffer();

protected:
	virtual HRESULT SetHandleData( const SVImageBufferHandleStruct& p_rObject );

	SVMatroxBuffer m_Buffer;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageBufferHandleImage.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:44:36   bWalter
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
 *    Rev 1.0   27 Jan 2011 09:54:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

