//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/Color.h"
//for image copy
#include "DisplayHelper.h"
#pragma endregion Includes

namespace SvOgu
{
#pragma region Contructor
	PictureDisplay::~PictureDisplay()
	{
	}
#pragma endregion Contructor

#pragma region Public Methods
	void PictureDisplay::setImage(HBITMAP pHBitmap, long tabNumber)
	{
		if (nullptr != pHBitmap)
		{
			//convert the hbitmap to an IPictureDisp for the activeX-control.
			CPictureHolder pic;
			BOOL bRet = pic.CreateFromBitmap(pHBitmap, 0, true);
			if (bRet)
			{

				LPPICTUREDISP pDispatch = pic.GetPictureDispatch();
				setImage(pDispatch, tabNumber);
			}
		}
	}

	// This is the preferred method
	void PictureDisplay::setImage(IPictureDisp* pImage, long tabNumber)
	{
		CComPtr<IPictureDisp> pic(pImage);
		//color is ignored when dynamic adjustment of size is active
		COLORREF color = SvDef::WhiteSmoke;
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

	SCODE PictureDisplay::EditOverlay( long Tab, long handle, const LongParamMap& ParMap )
	{
		COleSafeArray saPar, saVal;
		DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
		SCODE retValue = CDSVPictureDisplay::EditOverlay( Tab, handle, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
		saPar.Destroy();
		saVal.Destroy();
		return retValue;
	}

	SCODE PictureDisplay::EditOverlay( long Tab, long handle, const VariantParamMap& ParMap )
	{
		COleSafeArray saPar, saVal;
		DisplayHelper::CreateSafeArrayFromMap( ParMap, saPar, saVal );
		SCODE retValue = CDSVPictureDisplay::EditOverlay( Tab, handle, static_cast< LPVARIANT >( saPar ), static_cast< LPVARIANT >( saVal ) );
		saPar.Destroy();
		saVal.Destroy();
		return retValue;
	}
#pragma endregion Public Methods
}  //namespace SvOgu
