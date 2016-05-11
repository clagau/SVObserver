//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantConverter
//* .File Name       : $Workfile:   SVVariantConverter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:48:02  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <atlbase.h>
//Moved to precompiled header: #include <comdef.h>
#include "SVVariantConverter.h"
#include "SVVariantCustomTypes.h"
#include "SVLibrary/SVBStr.h"

static const WCHAR*	g_wcsBaseNode = L"SV_BASENODE";

HRESULT SVVariantConverter::TranslateVariant(VARIANT* avpValue, BSTR* abstrpValue, BSTR* abstrpType)
{
	USES_CONVERSION;

	HRESULT hr = 0;
	long lIsArray = 0;
	SVBStr bstrType;
	WCHAR wszTempBuildBuffer[500];

	while (1)
	{
		if (nullptr == avpValue)
		{
			hr = 2;
			break;
		}

		if (avpValue->vt & VT_ARRAY)
		{
//-		It is understood that standard variants which reference SAFEARRAYs
//-	   will also reference the type of elements within the SAFEARRAY.  
//-		This function looses that information at this time.
			swprintf (wszTempBuildBuffer, A2W("%p"), avpValue->parray);
			*abstrpValue = SysAllocString (wszTempBuildBuffer);
			bstrType = "VT_ARRAY";
			*abstrpType = SysAllocString (bstrType);
		}
		else if (avpValue->vt == VT_SVARRAYNODE)
		{
//-		It is understood that standard variants which reference SAFEARRAYs
//-	   will also reference the type of elements within the SAFEARRAY.  
//-		This function looses that information at this time.

			swprintf (wszTempBuildBuffer, A2W("%p"), avpValue->parray);
			*abstrpValue = SysAllocString (wszTempBuildBuffer);
			bstrType = "SV_ARRAYNODE";
			*abstrpType = SysAllocString (bstrType);
		}
		else
		{
			switch (avpValue->vt)
			{
			case VT_SVBASENODE:
				{
					*abstrpValue = nullptr;
					bstrType = g_wcsBaseNode;
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_SVNODEWITHDATA:
				{
					*abstrpValue = nullptr;
					bstrType = "SV_NODEWITHDATA";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_NULL:
				{
					*abstrpValue = nullptr;
					bstrType = "VT_NULL";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_EMPTY:
				{
					swprintf (wszTempBuildBuffer, L"");
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_EMPTY";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_BSTR:
				{
					*abstrpValue = SysAllocString (avpValue->bstrVal);
					bstrType = "VT_BSTR";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_BOOL:
				{
					if (avpValue->boolVal == 0)
					{
						*abstrpValue = SysAllocString (L"FALSE");
					}
					else
					{
						*abstrpValue = SysAllocString (L"TRUE");
					}

					bstrType = "VT_BOOL";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_INT:
				{
					swprintf (wszTempBuildBuffer, L"%d", avpValue->intVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_INT";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_I4:
				{
					swprintf (wszTempBuildBuffer, L"%d", avpValue->lVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_I4";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_R4:
				{
					swprintf (wszTempBuildBuffer, L"%f", avpValue->fltVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_R4";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_R8:
				{
					swprintf (wszTempBuildBuffer, L"%lf", avpValue->dblVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_R8";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_UINT:
				{
					swprintf (wszTempBuildBuffer, L"%lu", avpValue->uintVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_UINT";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_UI4:
				{
					swprintf (wszTempBuildBuffer, L"%lu", avpValue->ulVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_UI4";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_I8:
				{
					swprintf (wszTempBuildBuffer, L"%I64d", avpValue->llVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_I8";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_UI8:
				{
					swprintf (wszTempBuildBuffer, L"%I64u", avpValue->ullVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_UI8";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_UI1:
				{
					swprintf (wszTempBuildBuffer, L"%02X", avpValue->bVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_UI1";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			case VT_I1:
				{
					swprintf (wszTempBuildBuffer, L"%02X", avpValue->bVal);
					*abstrpValue = SysAllocString (wszTempBuildBuffer);
					bstrType = "VT_I1";
					*abstrpType = SysAllocString (bstrType);
					break;
				}
			default:
				{
					hr = -1676;
					break;
				}
			} // switch (avpVariant->vt)
		} // if (avpValue->vt & VT_ARRAY) else...
		
		break;
	} // while (1)

	return hr;
}
	
HRESULT SVVariantConverter::RestoreVariant(BSTR abstrValue, BSTR abstrType, VARIANT* avpValue)
{
	USES_CONVERSION;

	HRESULT hr = 0;

	WCHAR wszWhiteSpace[500];
	WCHAR* wcpPntr1;
	WCHAR* wcpPntr2;

	long l_lStringLength;

	VARIANT vRestoredValue;

	vRestoredValue.vt = VT_EMPTY;
	vRestoredValue.lVal = 0;

	while (1)
	{
		//--- Yes, I am treating a comma as white space.
      wcscpy (wszWhiteSpace, L"\a\b\f\n\r\t\v ");

//--- START - DONE FOR CONVERSION TO NUMBER. ------------------------------\/
		wcpPntr1 = abstrValue;

//-	If the value is numeric, we will return the same value that is 
//-   present.
//-   Move passed any white space.
		if (nullptr != wcpPntr1)
		{
			wcpPntr1 = wcpPntr1 + wcsspn (wcpPntr1, wszWhiteSpace);
		}
//--- END - DONE FOR CONVERSION TO NUMBER. --------------------------------/\
		
		if (nullptr == abstrType)
		{
			vRestoredValue.vt = VT_NULL;
			vRestoredValue.lVal = 0;
		}
		else
//-	Insensitive wide character compare with number of characters input.
		if (!_wcsnicmp (abstrType, g_wcsBaseNode, SysStringLen (abstrType)))
		{
			vRestoredValue.vt = VT_SVBASENODE;
			vRestoredValue.lVal = 0;
		}
		else if (!_wcsnicmp (abstrType, L"VT_ARRAY", SysStringLen (abstrType)))
		{
			vRestoredValue.vt = VT_ARRAY;
			vRestoredValue.lVal = 0;
		}
		else if (!_wcsnicmp (abstrType, L"VT_EMPTY", SysStringLen (abstrType)))
		{
			vRestoredValue.vt = VT_EMPTY;
			vRestoredValue.lVal = 0;
		}
		else if (!_wcsnicmp (abstrType, L"SV_ARRAYNODE", SysStringLen (abstrType)))
		{
			vRestoredValue.vt = VT_SVARRAYNODE;
			vRestoredValue.lVal = 0;
		}
		else if (!_wcsnicmp (abstrType, L"VT_BSTR", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1802;
				break;
			}

			vRestoredValue.vt = VT_BSTR;
			l_lStringLength = SysStringLen (abstrValue);
			vRestoredValue.bstrVal = SysAllocStringLen (abstrValue, l_lStringLength);
		}
		else if (!_wcsnicmp (abstrType, L"VT_BOOL", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1803;
				break;
			}
//-      For BOOL values we will accept TRUE/FALSE or 1/0.
			vRestoredValue.vt = VT_BOOL;

			if (nullptr == abstrValue)
			{
				hr = -1773;
				break;
			}

			if ((!_wcsnicmp (abstrValue, L"TRUE", SysStringLen (abstrValue))) ||	// true
				 (!_wcsnicmp (abstrValue, L"YES", SysStringLen (abstrValue))))    // yes
			{
				vRestoredValue.boolVal = VARIANT_TRUE;
			}
			else if ((!_wcsnicmp (abstrValue, L"FALSE", SysStringLen (abstrValue))) ||// false
				 (!_wcsnicmp (abstrValue, L"NO", SysStringLen (abstrValue))))		// no
			{
				vRestoredValue.boolVal = VARIANT_FALSE;
			}
			else
			{
				if (iswdigit (*wcpPntr1))
				{
//-				Convert strings to longs (including hex representations)
					vRestoredValue.boolVal = (short) wcstol (wcpPntr1, &wcpPntr2, 0);
				}
				else
				{
					hr = -1774;
					break;
				}

			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_INT", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1804;
				break;
			}

			vRestoredValue.vt = VT_INT;

			if ((iswdigit (*wcpPntr1)) ||
				 ((*wcpPntr1 == '-') && iswdigit (*(wcpPntr1 + 1))))
			{
//-			Convert strings to longs (including hex representations)
				vRestoredValue.intVal = wcstol (wcpPntr1, &wcpPntr2, 0);
			}
			else
			{
				hr = -1775;
				break;
			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_I4", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1805;
				break;
			}

			vRestoredValue.vt = VT_I4;

			if ((iswdigit (*wcpPntr1)) ||
				 ((*wcpPntr1 == '-') && iswdigit (*(wcpPntr1 + 1))))
			{
//-			Convert strings to longs (including hex representations)
				vRestoredValue.lVal = wcstol (wcpPntr1, &wcpPntr2, 0);
			}
			else
			{
				hr = -1776;
				break;
			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_UINT", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1806;
				break;
			}

			vRestoredValue.vt = VT_UINT;
			if (iswdigit (*wcpPntr1))
			{
//-			Convert strings to longs (including hex representations)
				vRestoredValue.uintVal = wcstol (wcpPntr1, &wcpPntr2, 0);
			}
			else
			{
				hr = -1777;
				break;
			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_UI4", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1807;
				break;
			}


			vRestoredValue.vt = VT_UI4;
			if (iswdigit (*wcpPntr1))
			{
//-			Convert strings to longs (including hex representations)
				vRestoredValue.ulVal = wcstol (wcpPntr1, &wcpPntr2, 0);
			}
			else
			{
				hr = -1778;
				break;
			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_R4", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1791;
				break;
			}

			vRestoredValue.vt = VT_R4;

			if ((iswdigit (*wcpPntr1)) ||
				 ((*wcpPntr1 == '-') && iswdigit (*(wcpPntr1 + 1))))
			{
				vRestoredValue.fltVal = static_cast <float> (wcstod (wcpPntr1, nullptr));
			}
			else
			{
				hr = -1811;
				break;
			}
		}// ...else if (!_wcsnicmp (abstrType, L"VT_R4", SysStringLen (abstrType)))
		else if (!_wcsnicmp (abstrType, L"VT_R8", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1812;
				break;
			}

			vRestoredValue.vt = VT_R8;

			if ((iswdigit (*wcpPntr1)) ||
				 ((*wcpPntr1 == '-') && iswdigit (*(wcpPntr1 + 1))))
			{
				vRestoredValue.dblVal = wcstod (wcpPntr1, nullptr);
			}
			else
			{
				hr = -1813;
				break;
			}
		} // ...else if (!_wcsnicmp (abstrType, L"VT_R8", SysStringLen (abstrType)))
		else if (!_wcsnicmp (abstrType, L"VT_I8", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1814;
				break;
			}

			vRestoredValue.vt = VT_I8;

			if (iswdigit (*wcpPntr1))
			{
//-			Convert strings to int64 (including hex representations)
				vRestoredValue.llVal = _wcstoi64 (wcpPntr1, &wcpPntr2, 0);
			}
			else
			{
				hr = -1815;
				break;
			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_UI8", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1816;
				break;
			}

			vRestoredValue.vt = VT_UI8;

			if (iswdigit (*wcpPntr1))
			{
//-			Convert strings to uint64 (including hex representations)
				vRestoredValue.ullVal = _wcstoui64 (wcpPntr1, &wcpPntr2, 0);
			}
			else
			{
				hr = -1817;
				break;
			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_UI1", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1818;
				break;
			}

			vRestoredValue.vt = VT_UI1;

			if ((iswdigit (*wcpPntr1)) || (iswxdigit (*wcpPntr1)))
			{
//-			Convert string to byte (including hex representations)
				swscanf(wcpPntr1, L"%02X", &vRestoredValue.bVal);
			}
			else
			{
				hr = -1819;
				break;
			}
		}
		else if (!_wcsnicmp (abstrType, L"VT_I1", SysStringLen (abstrType)))
		{
			if (nullptr == wcpPntr1)
			{
				hr = -1820;
				break;
			}

			vRestoredValue.vt = VT_I1;

			if ((iswdigit (*wcpPntr1)) || (iswxdigit (*wcpPntr1)))
			{
//-			Convert string to char (byte actually)
				swscanf(wcpPntr1, L"%02X", &vRestoredValue.bVal);
			}
			else
			{
				hr = -1821;
				break;
			}
		}
		else
		{
			hr = -1771;
			break;
		}
		break;
	} // while (1)

	if (hr & 0xc000)
	{
	}
	else
	{
//-	Will not copy the actual BSTR (if one exists).  
//-	Do not VariantClear () vRestoredValue.
		*avpValue = vRestoredValue;
	}
	return hr;
}

