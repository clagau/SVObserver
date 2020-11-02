//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : DisplayHelper
//* .File Name       : $Workfile:   Displayhelper.cpp  $
//* .Description     : Helper class for the SVPictureDisplay to convert between std::map and Variants
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 11:38:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DisplayHelper.h"
#pragma warning (push ,2)
#include "SVProtoBuf\BasicStructure.pb.h"
#pragma warning(pop)
#pragma endregion Includes

namespace SvOg
{
	#pragma region Public Methods
	long DisplayHelper::CreateSafeArrayFromMap(const LongParamMap& map, COleSafeArray& sPar, COleSafeArray& saVal)
	{
		LongParamMap::const_iterator it = map.begin();
		long size = static_cast< long >( map.size() );
		long *pa = new long[size];
		VARIANT *val = new VARIANT[size];

		int i = 0;
		VARIANT variantVal;
		variantVal.vt = VT_I4;
		while(it != map.end())
		{
			pa[i] = it->first;
			variantVal.lVal = it->second;
			val[i] = variantVal;
			++it;
			i++;
		}
		sPar.CreateOneDim(VT_I4, i, pa);
		saVal.CreateOneDim(VT_VARIANT, i, val);
		delete[] pa;
		delete[] val;
		return size;
	}

	long DisplayHelper::CreateSafeArrayFromMap(const VariantParamMap& map, COleSafeArray& sPar, COleSafeArray& saVal)
	{
		VariantParamMap::const_iterator it = map.begin();
		long size = static_cast< long >( map.size() );
		long *pa = new long[size];
		VARIANT *val = new VARIANT[size];

		int i = 0;
		while(it != map.end())
		{
			pa[i] = it->first;
			_variant_t varTTmp = it->second;
			val[i] = varTTmp.Detach();
			++it;
			i++;
		}
		sPar.CreateOneDim(VT_I4, i, pa);
		saVal.CreateOneDim(VT_VARIANT, i, val);
		delete[] pa;
		delete[] val;
		return size;
	}

	long DisplayHelper::FillParameterMap(VariantParamMap& pamap, VARIANT* ParameterList, VARIANT* ParameterValue)
	{
		if (V_VT(ParameterList) != (VT_ARRAY | VT_I4) || V_VT(ParameterValue) != (VT_ARRAY | VT_VARIANT) )
		{
			return -1;
		}
		COleSafeArray saParameterList(ParameterList);
		COleSafeArray saParameterValue(ParameterValue);

		// Check that array is 1 dimensional
		if (saParameterList.GetDim() != 1 || saParameterValue.GetDim() !=1 )
		{
			return -1;
		}

		// Get array bounds.
		long size = saParameterList.GetOneDimSize();
		if(saParameterList.GetOneDimSize() != saParameterValue.GetOneDimSize())
		{
			return -1;
		}
		long min;
		saParameterList.GetLBound(1, &min);
		assert( min == 0 );

		LONG *pParameterList( nullptr );
		VARIANT *pParameterValue( nullptr );
		// Get a pointer to the elements of the array
		// and increment the lock count on the array.
		saParameterList.AccessData( reinterpret_cast< LPVOID* >( &pParameterList ) );
		saParameterValue.AccessData( reinterpret_cast< LPVOID* >( &pParameterValue ) );

		for (int i = 0; i < size; i++)
		{
			pamap[pParameterList[i]].Attach(pParameterValue[i]);
		}

		//decrement lock count
		saParameterList.UnaccessData();
		saParameterValue.UnaccessData();

		return static_cast< long >( pamap.size() );
	}

	HRESULT DisplayHelper::convertPBImageToIPictureDisp(const SvPb::Image& rImage, long &rWidth, long &rHeight, IPictureDisp** idisp)
	{
		HRESULT hr = S_OK;
		rWidth = rImage.width();
		rHeight = rImage.height();

		size_t len = rImage.rgbdata().length();
		HGLOBAL hg = ::GlobalAlloc(GHND, len);
		if (hg)
		{
			memcpy(::GlobalLock(hg), rImage.rgbdata().data(), len);
			::GlobalUnlock(hg);
			CComPtr<IStream> stream;
			hr = ::CreateStreamOnHGlobal(hg, TRUE, &stream);
			// Create IPictureDisp from IStream
			if (SUCCEEDED(hr))
			{
				hr = ::OleLoadPicture(stream, static_cast<LONG>(len), false, __uuidof(IPictureDisp), (LPVOID *)idisp);
			}
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
		return hr;
	}

	void DisplayHelper::setIconListToGrid(CImageList& rImageList, CBitmap& rDownArrowBitmap, SvGcl::GridCtrl& rGrid)
	{
		// Put the Down Arrow on the Button
		rDownArrowBitmap.LoadOEMBitmap(OBM_DNARROW);

		rImageList.Create(16, 16, ILC_COLOR8, 0, 1);
		rImageList.Add(&rDownArrowBitmap, nullptr);
		rGrid.SetImageList(&rImageList);

		//The cell must at least 16 (Icon-Size) + 2 * margin, that the arrow-icon is 
		int minHeight = 16 + (rGrid.GetDefCellMargin() * 2);
		rGrid.SetDefCellHeight(std::max(minHeight, rGrid.GetDefCellHeight()));
	}
	#pragma endregion Public Methods

} //namespace SvOg
