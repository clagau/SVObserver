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
//Moved to precompiled header: #include <afxctl.h>
#include "PictureDisplay.h"
#include "SVColor.h"
//for image copy
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "DisplayHelper.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IMatroxImageData.h"

#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
#pragma region Contructor
		PictureDisplay::~PictureDisplay()
		{
		}
#pragma endregion Contructor

#pragma region Public Methods
		// only called for mask and threshold - @TODO - needs to go!
		void PictureDisplay::setImageFromParent( SvOi::ISVImage* pImage, long tabNumber )
		{
			if (nullptr != pImage)
			{
				SvOi::MatroxImageSmartHandlePtr data;
				if( SVImageTypePhysical == pImage->GetImageType() )
				{
					data = pImage->GetParentImageInterface()->getImageData();
				}
				else
				{
					data = pImage->getParentImageData();
				}
				setImage( data.get(), tabNumber );
			}
		}

		void PictureDisplay::setImage( SvOi::ISVImage* pImage, long tabNumber )
		{
			if (nullptr != pImage)
			{
				SvOi::MatroxImageSmartHandlePtr data = pImage->getImageData();
				setImage( data.get(), tabNumber );
			}
		}

		//@TODO - needs to go!
		void PictureDisplay::setImage( const SvOi::IMatroxImageData *imageData, long tabNumber )
		{
			if( nullptr != imageData && !( imageData->empty() ) )
			{
				SVBitmapInfo dibInfo = imageData->getBitmapInfo();
				BYTE* pMilBuffer = static_cast< BYTE* >( imageData->getBufferAddress() );
				if (nullptr != pMilBuffer)
				{
					//copy the image buffer - because the UnaryImageOperatorList/StdImageOperatorList does not support DIB!
					SVMatroxBuffer newBuffer;
					SVMatroxBuffer oldBuffer;
					imageData->GetBuffer(oldBuffer);

					HRESULT l_Code = SVMatroxBufferInterface::Create(newBuffer, oldBuffer);
					if (S_OK == l_Code)
					{
						l_Code = SVMatroxBufferInterface::CopyBuffer(newBuffer, oldBuffer);
						l_Code = SVMatroxBufferInterface::GetBitmapInfo(dibInfo, newBuffer);
						l_Code = SVMatroxBufferInterface::GetHostAddress(&pMilBuffer, newBuffer);
					}
				
					if (nullptr != pMilBuffer && !dibInfo.empty())
					{
						SVBitmap bitmap;
						HRESULT hr = bitmap.LoadDIBitmap(dibInfo.GetBitmapInfo(), pMilBuffer);
					
						if (S_OK == hr)
						{
							//convert the hbitmap to an IPictureDisp for the activeX-control.
							CPictureHolder pic;
							BOOL bRet = pic.CreateFromBitmap(static_cast<HBITMAP>(bitmap.Detach()));
							if (bRet)
							{
								LPPICTUREDISP pDispatch = pic.GetPictureDispatch();
								setImage(pDispatch, tabNumber);
							}
						}
						newBuffer.clear();
					}
				}
			}
		}

		// This is the preferred method
		void PictureDisplay::setImage(IPictureDisp* pImage, long tabNumber)
		{
			CComPtr<IPictureDisp> pic(pImage);
			//color is ignored when dynamic adjustment of size is active
			COLORREF color = SVColor::WhiteSmoke;
			SetPicture(tabNumber, pic, color);
		}

		SCODE PictureDisplay::AddTab( LPCTSTR Text )
		{
			long handle = 0;
			return CDSVPictureDisplay::AddTab(Text, &handle);
		}

		SCODE PictureDisplay::AddTab( LPCTSTR Text, LONG *handle )
		{
			return CDSVPictureDisplay::AddTab(Text, handle);
		}

		SCODE PictureDisplay::AddOverlay( long Tab, VARIANT* ParameterList, VARIANT* ParameterValues, long* pHandle )
		{
			return CDSVPictureDisplay::AddOverlay( Tab, ParameterList, ParameterValues, pHandle );
		}

		SCODE PictureDisplay::AddOverlay( long Tab, const LongParamMap& ParMap, long* pHandle )
		{
			COleSafeArray saPar, saVal;
			SvOg::DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
			SCODE retValue = CDSVPictureDisplay::AddOverlay( Tab, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ), pHandle );
			saPar.Destroy();
			saVal.Destroy();
			return retValue;
		}

		SCODE PictureDisplay::AddOverlay( long Tab, const VariantParamMap& ParMap, long* pHandle )
		{
			COleSafeArray saPar, saVal;
			SvOg::DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
			SCODE retValue = CDSVPictureDisplay::AddOverlay( Tab, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ), pHandle );
			saPar.Destroy();
			saVal.Destroy();
			return retValue;
		}

		SCODE PictureDisplay::EditOverlay( long Tab, long handle, VARIANT* ParameterList, VARIANT* ParameterValues )
		{
			return CDSVPictureDisplay::EditOverlay(Tab, handle, ParameterList, ParameterValues);
		}

		SCODE PictureDisplay::EditOverlay( long Tab, long handle, const LongParamMap& ParMap )
		{
			COleSafeArray saPar, saVal;
			SvOg::DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
			SCODE retValue = CDSVPictureDisplay::EditOverlay( Tab, handle, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
			saPar.Destroy();
			saVal.Destroy();
			return retValue;
		}

		SCODE PictureDisplay::EditOverlay( long Tab, long handle, const VariantParamMap& ParMap )
		{
			COleSafeArray saPar, saVal;
			SvOg::DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
			SCODE retValue = CDSVPictureDisplay::EditOverlay( Tab, handle, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
			saPar.Destroy();
			saVal.Destroy();
			return retValue;
		}
#pragma endregion Public Methods
	}
}
