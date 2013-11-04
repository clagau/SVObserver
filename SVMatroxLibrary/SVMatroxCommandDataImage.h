//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxCommandDataImage
//* .File Name       : $Workfile:   SVMatroxCommandDataImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:01:30  $
//******************************************************************************

#ifndef SVMATROXCOMMANDDATAIMAGE_H
#define SVMATROXCOMMANDDATAIMAGE_H

#include "SVCommandLibrary/SVCommandDataImage.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxBufferCreateStruct.h"



/*
*/
class SVMatroxCommandDataImage : public SVCommandDataImage::SVCommandDataInterface
{
public:
	friend class SVMatroxBlobInterface;
	friend class SVMatroxBufferInterface;
	friend class SVMatroxImageInterface;
	friend class SVMatroxOcrInterface;

	SVMatroxCommandDataImage();
	SVMatroxCommandDataImage( const SVMatroxBufferCreateStruct& p_rCreateData );
	SVMatroxCommandDataImage( const SVMatroxBuffer& p_rBuffer, bool p_ReadOnly = false );
	SVMatroxCommandDataImage( const SVByteVector& p_rData, bool p_ReadOnly = false );

	virtual ~SVMatroxCommandDataImage();

	virtual HRESULT GetData( SVByteVector& p_rData ) const;
	virtual HRESULT SetData( const SVByteVector& p_rData );

protected:
	static HRESULT Convert( HBITMAP hBitmap, SVMatroxBuffer& rImage );
	static HRESULT Convert( const SVMatroxBuffer& rImage, HBITMAP& hBitmap );
	static HRESULT CreateGrayscaleImage( long width, long height, SVMatroxBuffer& rImage );
	static HRESULT ConvertToGrayscale( SVMatroxBuffer& rDstImage, const SVMatroxBuffer& rSrcImage );

	SVMatroxBuffer m_Buffer;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxCommandDataImage.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:01:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:00:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include new command functionality using the matrox library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
