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
#pragma endregion Includes

namespace Seidenader { namespace SVOGui
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
		ASSERT( min == 0 );

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
	#pragma endregion Public Methods

} /* namespace SVOGui */ } /* namespace Seidenader */
