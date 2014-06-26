//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PictureDisplay
//* .File Name       : $Workfile:   PictureDisplay.cpp  $
//* .Description	 : The class specializes the wrapper for the ActiveX
//*					SVPictureDisplay.  One of the additional functionalities grab
//*					bitmaps from e.g. SVImageClass to set them to the control.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:46:32  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include <afxctl.h>
#include "PictureDisplay.h"
#include "SVImageClass.h"
//for image copy
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVOMFCLibrary\DisplayHelper.h"
#pragma endregion Includes

using namespace Seidenader::SVOMFCLibrary;

#pragma region Contructor
PictureDisplay::~PictureDisplay()
{
	for (unsigned int i = 0; i < m_dibVector.size(); i++)
	{
		m_dibVector[i]->DeleteObject();
		delete m_dibVector[i];
	}
	m_dibVector.clear();
}
#pragma endregion Contructor

#pragma region Public Methods
void PictureDisplay::setImageFromParent( SVImageClass* pImage, long tabNumber )
{
	SVSmartHandlePointer imageHandle;
	if( pImage->GetImageType() == SVImageTypeEnum::SVImageTypePhysical )
	{
		pImage->GetParentImage()->GetImageHandle( imageHandle );
	}
	else
	{
		pImage->GetParentImageHandle( imageHandle );
	}
	setImage( imageHandle, tabNumber );
}

void PictureDisplay::setImage( SVImageClass* pImage, long tabNumber )
{
	SVSmartHandlePointer imageHandle;
	pImage->GetImageHandle( imageHandle );
	setImage( imageHandle, tabNumber );
}

void PictureDisplay::setImage( SVSmartHandlePointer imageHandle, long tabNumber )
{
	ASSERT( static_cast< long >( m_dibVector.size() ) > tabNumber );

	if( !( imageHandle.empty() ) )
	{
		SVBitmapInfo dibInfo = imageHandle->GetBitmapInfo();
		BYTE* pMilBuffer = static_cast< BYTE* >( imageHandle->GetBufferAddress() );

		//copy the image buffer
		SVMatroxBuffer newBuffer;
		SVImageBufferHandleImage milHandle;
		imageHandle->GetData( milHandle );
		SVMatroxBufferInterface::SVStatusCode l_Code = SVMatroxBufferInterface::Create( newBuffer, milHandle.GetBuffer() );
		if( S_OK == l_Code )
		{
			l_Code = SVMatroxBufferInterface::CopyBuffer( newBuffer, milHandle.GetBuffer() );
			l_Code = SVMatroxBufferInterface::GetBitmapInfo( dibInfo, newBuffer );
			l_Code = SVMatroxBufferInterface::GetHostAddress( &pMilBuffer, newBuffer );
		}

		if( nullptr != pMilBuffer && !( dibInfo.empty() ) )
		{
			m_dibVector[tabNumber]->DeleteObject();

			// @WARNING:  Result of LoadDIBitmap is never checked.
			HRESULT hr = m_dibVector[tabNumber]->LoadDIBitmap( dibInfo.GetBitmapInfo(), pMilBuffer );
		}

		//convert the hbitmap to a useable dispatch for the activeX-control.
		CPictureHolder pic;
		BOOL bRet = pic.CreateFromBitmap( static_cast< HBITMAP >( *m_dibVector[ tabNumber ] ) );
		if ( bRet )
		{
			LPDISPATCH pDispatch = pic.GetPictureDispatch();

			//color is ignored when dynamic adjustment of size is active
			COLORREF color = SVColor::WhiteSmoke;

			SetPicture( tabNumber, pDispatch, color );
			pDispatch->Release();
		}
		newBuffer.clear();
	}
}

SCODE PictureDisplay::AddTab( LPCTSTR Text )
{
	long handle =0;
	m_dibVector.push_back(new SVBitmap);
	return CDSVPictureDisplay::AddTab(Text, &handle);
}

SCODE PictureDisplay::AddTab( LPCTSTR Text, LONG *handle )
{
	m_dibVector.push_back(new SVBitmap);
	return CDSVPictureDisplay::AddTab(Text, handle);
}


SCODE PictureDisplay::AddOverlay( long Tab, VARIANT* ParameterList, VARIANT* ParameterValues, long* pHandle )
{
	return CDSVPictureDisplay::AddOverlay( Tab, ParameterList, ParameterValues, pHandle );
}

SCODE PictureDisplay::AddOverlay( long Tab, const LongParamMap& ParMap, long* pHandle )
{
	COleSafeArray saPar, saVal;
	DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
	SCODE retValue = CDSVPictureDisplay::AddOverlay( Tab, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ), pHandle );
	saPar.Destroy();
	saVal.Destroy();
	return retValue;
}

SCODE PictureDisplay::AddOverlay( long Tab, const VariantParamMap& ParMap, long* pHandle )
{
	COleSafeArray saPar, saVal;
	DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
	SCODE retValue = CDSVPictureDisplay::AddOverlay( Tab, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ), pHandle );
	saPar.Destroy();
	saVal.Destroy();
	return retValue;
}

SCODE PictureDisplay::EditOverlay( long Tab, long handle, VARIANT* ParameterList, VARIANT* ParameterValues )
{
	return CDSVPictureDisplay::EditOverlay(Tab, handle, ParameterList, ParameterValues);
}

SCODE PictureDisplay::EditOverlay( long Tab, long handle, LongParamMap ParMap )
{
	COleSafeArray saPar, saVal;
	DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
	SCODE retValue = CDSVPictureDisplay::EditOverlay( Tab, handle, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
	saPar.Destroy();
	saVal.Destroy();
	return retValue;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\PictureDisplay.cpp_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:46:32   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/