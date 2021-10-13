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

#pragma region Includes
#include "stdafx.h"
#include "DisplayPicture.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "TriggerRecordController/Image.h"
#include "Definitions/SVImageFormatEnum.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DisplayPicture::DisplayPicture()
{
}

DisplayPicture::~DisplayPicture()
{
}

BEGIN_MESSAGE_MAP(DisplayPicture, CStatic)
	//{{AFX_MSG_MAP(DisplayPicture)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SvOi::ITRCImagePtr DisplayPicture::GetNextBuffer()
{
	return m_pImage;
}

bool DisplayPicture::UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat )
{
	SVMatroxBufferCreateStruct create;
	create.m_lSizeX = bufWidth;
	create.m_lSizeY = bufHeight;

	switch (iFormat)
	{
		case  SvDef::SVImageFormatMono8:
		{
			create.m_eAttribute = SVBufAttImageProcDib;
			SetImageDepth(create, 8);
			create.m_lSizeBand = 1;
			break;
		}

		case SvDef::SVImageFormatBGR888:
		case SvDef::SVImageFormatBGR888X:
		{
			create.m_eAttribute = SVBufAttImageProcPackedOffBoardDibPagedBgr32;
			SetImageDepth(create, 8);
			create.m_lSizeBand = 3;
			break;
		}

		default:
			break;
	}

	SVMatroxBuffer newBuffer;
	SVMatroxBufferInterface::Create(newBuffer, create);

	SvOi::SVImageBufferHandlePtr pImagePtr = SvOi::SVImageBufferHandlePtr {new SVImageBufferHandleImage{newBuffer} };
	m_pImage = std::make_shared<SvTrc::Image>(pImagePtr, 0, SvTrc::Image::cLocalTmpImagePos, false, false);
	return true;
}

void DisplayPicture::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	bool imageValid{false};

	if(nullptr != m_pImage.get())
	{
		SVImageBufferHandleImage* pImage = dynamic_cast<SVImageBufferHandleImage*>(m_pImage.get()->getHandle().get());
		if( nullptr != pImage && !( pImage->GetBitmapInfo().empty() ))
		{
			CRect rec;	// Rect from IDC_PICT
			GetClientRect(rec);

			StretchDIBits( dc.m_hDC,
						   0, 0, rec.Width(), rec.Height(), 
										 0, 0, abs( pImage->GetBitmapInfo().GetWidth() ), abs( pImage->GetBitmapInfo().GetHeight() ),
										 pImage->GetBufferAddress(), pImage->GetBitmapInfo().GetBitmapInfo(),
										 DIB_RGB_COLORS, SRCCOPY);
			imageValid = true;
		}
	}

	if(!imageValid)
	{
		constexpr const char* pStr = "Camera has no valid settings!";
		dc.TextOut( 0, 0, pStr, static_cast<int>(strlen(pStr)));
	}
}
