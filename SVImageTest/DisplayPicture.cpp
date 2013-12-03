// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : DisplayPicture.cpp
// * .File Name       : $Workfile:   DisplayPicture.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 08:25:50  $
// ******************************************************************************

#include "stdafx.h"
#include "DisplayPicture.h"
#include "SVImageLibrary/SVImageBufferInterface.h"
#include "SVDisplayImageBufferClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDisplayPicture::CDisplayPicture()
{
	m_BufferInUse = 0;
	m_pImage = new SVDisplayImageBufferClass;
}

CDisplayPicture::~CDisplayPicture()
{
}

BEGIN_MESSAGE_MAP(CDisplayPicture, CStatic)
	//{{AFX_MSG_MAP(CDisplayPicture)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CDisplayPicture::UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat )
{
	SVDisplayImageBufferClass* l_pImage = dynamic_cast< SVDisplayImageBufferClass* >( m_pImage.get() );
	bool l_Status = l_pImage != NULL;

	if( l_Status )
	{
		l_Status = l_pImage->UpdateDisplayBufferInfo( bufWidth, bufHeight, iFormat );
	}

	return l_Status;
}

void CDisplayPicture::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	SVDisplayImageBufferClass* l_pImage = dynamic_cast< SVDisplayImageBufferClass* >( m_pImage.get() );

	if( l_pImage != NULL && !( l_pImage->GetBitmapInfo().empty() ) && l_pImage->m_pucImageData != NULL )
	{
		CRect rec;	// Rect from IDC_PICT
		GetClientRect(rec);

		StretchDIBits( dc.m_hDC,
		               0, 0, rec.Width(), rec.Height(), 
									 0, 0, abs( l_pImage->GetBitmapInfo().GetWidth() ), abs( l_pImage->GetBitmapInfo().GetHeight() ),
									 l_pImage->m_pucImageData, l_pImage->GetBitmapInfo().GetBitmapInfo(),
									 DIB_RGB_COLORS, SRCCOPY);
	}
	else
	{
		char *pStr = "Camera has no valid settings!";
		dc.TextOut( 0, 0, pStr, static_cast< int >( strlen( pStr ) ) );
	}

	::InterlockedExchange( &m_BufferInUse, 0 );
	// Do not call CStatic::OnPaint() for painting messages
}

HRESULT CDisplayPicture::GetNextBuffer( SVImageBufferInterface& p_rBuffer )
{
	HRESULT l_Status = S_OK;

	if( ::InterlockedExchange( &m_BufferInUse, 1 ) == 0 )
	{
		p_rBuffer.Assign( m_pImage, SVDataManagerHandle() );
	}
	else
	{
		l_Status = E_FAIL;

		p_rBuffer.clear();
	}
	
	return l_Status;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\DisplayPicture.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 08:25:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add X64 Platform
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 11:07:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jan 2011 09:57:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 11:42:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jul 2009 15:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update image test program to use the new acquisition methodology for transferring the buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jun 2009 08:24:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised OnPaint method to set stretchblitmode.
 * Revised OnPaint method to check for valid host buffer address.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Nov 2006 12:22:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added revision header block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
