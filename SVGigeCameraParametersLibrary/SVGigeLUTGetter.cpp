//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeLUTGetter
//* .File Name       : $Workfile:   SVGigeLUTGetter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:55:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeLutGetter.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"

HRESULT SVGigeLUTGetter::operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
{ 
/*
	HRESULT hr = S_OK;

	if (rValue.vt & VT_ARRAY)
	{
		// Set the Selector
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(selector.GetName().ToString()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t(selector.GetValue().ToString()));

		if (l_Code == SVMEE_STATUS_OK)
		{
			
			try
			{
				VARIANT& var = rValue.GetVARIANT();

				// Get the bounds of the safe array
				long lowerBound(0);
				long upperBound(0);

				hr = ::SafeArrayGetLBound(var.parray, 1, &lowerBound);
				if (hr == S_OK)
				{
					hr = ::SafeArrayGetUBound(var.parray, 1, &upperBound);
				}
				if (hr == S_OK)
				{
					int numElements = upperBound - lowerBound;

					// Get the  SafeArray Dimensions
					for (int i = 0;i < numElements;i++)
					{
						_variant_t value(i);

						// Set LUT Index
						l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), "LUTIndex", SVMatroxDigitizerFeature::SVTypeInt32, value);
						if (l_Code == SVMEE_STATUS_OK)
						{
							_variant_t lutValue;
							lutValue.ChangeType(VT_I4);
							
							// Get Value from the camera
							l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().ToString()), rFeature.GetType(), lutValue);
							if (l_Code == SVMEE_STATUS_OK)
							{
								long index[1];
								index[0] = i + lowerBound;
							
								long saLutValue = lutValue.lVal;
								// Set SafeArray Element
								hr = SafeArrayPutElement(var.parray, index, &saLutValue);
							}
							else
							{
								hr = l_Code;
							}
						}
					}
				}
			}
			catch (...)
			{
				hr = E_FAIL;
			}
		}
		else
		{
			// Not an array - parameter error
			hr = E_INVALIDARG;
		}
	}
	return hr;
*/
	HRESULT hr = S_OK;
	if (rValue.vt & VT_ARRAY)
	{
		// Set the Selector
		const SVGigeFeatureSelector& selector = rFeature.GetSelector();
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(selector.GetName().c_str()), SVMatroxDigitizerFeature::SVTypeStringEnumeration, _variant_t(selector.GetValue().c_str()));
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
					if (hr == S_OK)
					{
						ULONG lNumBands = lBandUBound - lBandLBound + 1;

						VARTYPE vt;
						hr = ::SafeArrayGetVartype(var.parray, &vt);
						if (hr == S_OK)
						{
							if ( vt == VT_I4 || vt == VT_UI4 )
							{
								long lBandSizeLBound(0);
								long lBandSizeUBound(0);
								hr = GetLutBandSizeBounds(var.parray, lBandSizeLBound, lBandSizeUBound);
								
								if (hr == S_OK)
								{
									/* Slow way...
									ULONG lBandSize = lBandSizeUBound - lBandSizeLBound + 1;
									long alDimIndex[2];
									for (unsigned long lBand = 0;lBand < lNumBands && hr == S_OK;lBand++)
									{
										alDimIndex[1] = lBand;
										long lBegin = lBandSizeLBound;
										long lEnd = lBegin + lBandSize;
										for (long l = lBegin; l < lEnd && hr == S_OK; l++)
										{
											_variant_t lutIndex(l - lBandSizeLBound);

											// Set LUT Index
											l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString("LUTIndex"), SVMatroxDigitizerFeature::SVTypeInt32, lutIndex);
											hr = l_Code;

											if (l_Code == SVMEE_STATUS_OK)
											{
												_variant_t lutValue;
												lutValue.ChangeType(VT_I4);

												// Get LUT Value at Index N
												l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().ToString()), rFeature.GetType(), lutValue);
												hr = l_Code;
												if (l_Code == SVMEE_STATUS_OK)
												{
													long saLutValue = lutValue.lVal;
													alDimIndex[0] = l;
													hr = ::SafeArrayPutElement(var.parray, alDimIndex, (void *)&saLutValue);
												}
											}
										}
									}
									*/

									// Faster way...
									unsigned long* pData(NULL);
									hr = ::SafeArrayAccessData(var.parray, (void **)&pData);

									long lBandSize = lBandSizeUBound - lBandSizeLBound + 1;
									for (int i = 0; i < lBandSize && hr == S_OK; i++)
									{
										_variant_t lutIndex(i);

										// Set LUT Index
										l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString("LUTIndex"), SVMatroxDigitizerFeature::SVTypeInt32, lutIndex);
										hr = l_Code;

										if (l_Code == SVMEE_STATUS_OK)
										{
											_variant_t lutValue;
											lutValue.ChangeType(VT_I4);
							
											// // Get LUT Value from camera at Index N
											l_Code = SVMatroxDigitizerInterface::GetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), lutValue);
											hr = l_Code;

											if (l_Code == SVMEE_STATUS_OK)
											{
												pData[1 + i] = lutValue.lVal;
											}
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

HRESULT SVGigeLUTGetter::GetLutBandBounds(SAFEARRAY* parray, long& lBandLBound, long& lBandUBound) const
{
	HRESULT hr = ::SafeArrayGetLBound(parray, 2, &lBandLBound);

	if (hr == S_OK)
	{
		hr = ::SafeArrayGetUBound(parray, 2, &lBandUBound);
	}
	return hr;
}

HRESULT SVGigeLUTGetter::GetLutBandSizeBounds(SAFEARRAY* parray, long& lBandSizeLBound, long& lBandSizeUBound) const
{
	HRESULT hr = ::SafeArrayGetLBound(parray, 1, &lBandSizeLBound);

	if (hr == S_OK)
	{
		hr = ::SafeArrayGetUBound(parray, 1, &lBandSizeUBound);
	}
	return hr;
}

