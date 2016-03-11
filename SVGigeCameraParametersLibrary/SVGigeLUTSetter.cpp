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

#include "stdafx.h"
#include "SVGigeLutSetter.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"

// Note: SafeArray must be allocated to the proper dimension for the camera
// LUTEnable must be set prior to setting the Lut values
// SEJ - try to speed this up, performance is horrible...
HRESULT SVGigeLUTSetter::operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const
{
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
											_variant_t lutValue( pData[ i ] );

											// Set LUT Value at Index N
											l_Code = SVMatroxDigitizerInterface::SetFeature(*(Digitizer.get()), SVMatroxString(rFeature.GetName().c_str()), rFeature.GetType(), lutValue);
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

	if (hr == S_OK)
	{
		hr = ::SafeArrayGetUBound(parray, 2, &lBandUBound);
	}
	return hr;
}

HRESULT SVGigeLUTSetter::GetLutBandSizeBounds(SAFEARRAY* parray, long& lBandSizeLBound, long& lBandSizeUBound) const
{
	HRESULT hr = ::SafeArrayGetLBound(parray, 1, &lBandSizeLBound);

	if (hr == S_OK)
	{
		hr = ::SafeArrayGetUBound(parray, 1, &lBandSizeUBound);
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeLUTSetter.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:56:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jan 2013 15:49:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed dead code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Jul 2010 16:03:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated LUT Setter to fix indexing problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2010 08:13:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
