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

