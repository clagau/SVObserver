//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeLUTSetter
//* .File Name       : $Workfile:   SVGigeLUTSetter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:56:58  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeLutSetter.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#pragma endregion Includes

// Note: SafeArray must be allocated to the proper dimension for the camera
// LUTEnable must be set prior to setting the Lut values
// @TODO - Try to speed this up, performance is horrible...
HRESULT SVGigeLUTSetter::operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
{
	HRESULT hr = S_OK;
	if (rValue.vt & VT_ARRAY)
	{
		// Set the Selector
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t(selector.GetValue().c_str()));
		hr = l_Code;

		if (l_Code == SVMEE_STATUS_OK)
		{
			try
			{
				VARIANT& var = const_cast<_variant_t *>(&rValue)->GetVARIANT();
				
				UINT uiDims = ::SafeArrayGetDim(var.parray);
		
				if (uiDims <= 2)
				{
					long lBandLBound(0);
					long lBandUBound(0);
					hr = GetLutBandBounds(var.parray, lBandLBound, lBandUBound);
					if (S_OK == hr)
					{
						ULONG lNumBands = lBandUBound - lBandLBound + 1;

						VARTYPE vt;
						hr = ::SafeArrayGetVartype(var.parray, &vt);
						if (S_OK == hr)
						{
							if ( VT_I4 == vt || VT_UI4 == vt )
							{
								long lBandSizeLBound(0);
								long lBandSizeUBound(0);
								hr = GetLutBandSizeBounds(var.parray, lBandSizeLBound, lBandSizeUBound);
								
								if (S_OK == hr)
								{
									unsigned long* pData(nullptr);
									hr = ::SafeArrayAccessData(var.parray, (void **)&pData);

									long lBandSize = lBandSizeUBound - lBandSizeLBound + 1;
									for (int i = 0; i < lBandSize && S_OK == hr; i++)
									{
										_variant_t lutIndex(i);

										// Set LUT Index
										l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVString("LUTIndex"), SVMatroxDigitizerFeature::SVTypeInt32, lutIndex);
										hr = l_Code;

										if (l_Code == SVMEE_STATUS_OK)
										{
											_variant_t lutValue( pData[ i ] );

											// Set LUT Value at Index N
											l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVString(rFeature.GetName().c_str()), rFeature.GetType(), lutValue);
											hr = l_Code;
										}
									}
									::SafeArrayUnaccessData(var.parray);
								}
							}
							else
							{
								hr = E_INVALIDARG;
							}
						}
					}
				}
				else
				{
					hr = E_INVALIDARG;
				}
			}
			catch(...)
			{
				hr = E_FAIL;
			}
		}
	}
	else
	{
		// Not an array - parameter error
		hr = E_INVALIDARG;
	}
	return hr;
}

HRESULT SVGigeLUTSetter::GetLutBandBounds(SAFEARRAY* parray, long& lBandLBound, long& lBandUBound) const
{
	HRESULT hr = ::SafeArrayGetLBound(parray, 2, &lBandLBound);

	if (S_OK == hr)
	{
		hr = ::SafeArrayGetUBound(parray, 2, &lBandUBound);
	}
	return hr;
}

HRESULT SVGigeLUTSetter::GetLutBandSizeBounds(SAFEARRAY* parray, long& lBandSizeLBound, long& lBandSizeUBound) const
{
	HRESULT hr = ::SafeArrayGetLBound(parray, 1, &lBandSizeLBound);

	if (S_OK == hr)
	{
		hr = ::SafeArrayGetUBound(parray, 1, &lBandSizeUBound);
	}
	return hr;
}

