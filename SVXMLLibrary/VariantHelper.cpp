//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
////  class VariantHelper
//*****************************************************************************
#pragma region Includes
#include "StdAfx.h"
//Moved to precompiled header: #include <comutil.h>
//Moved to precompiled header: #include <iosfwd>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #include <boost/assign/std/map.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
#include "VariantHelper.h"
#include "SVVariantCustomTypes.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMessage/SVMessage.h"
#include "SVVariantConverter.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace  SVXMLLibrary
{
	static std::map<std::wstring, UINT>  map_wstring2Type = boost::assign::map_list_of<>
		(L"VT_EMPTY",		(UINT)VT_EMPTY)
		(L"VT_NULL",		(UINT)VT_NULL)
		(L"VT_BOOL",		(UINT)VT_BOOL)
		(L"VT_INT",		(UINT)VT_INT)
		(L"VT_I4",		(UINT)VT_I4)
		(L"VT_UINT",		(UINT)VT_UINT)
		(L"VT_UI4",		(UINT)VT_UI4)
		(L"VT_R4",		(UINT)VT_R4)
		(L"VT_R8",		(UINT)VT_R8)
		(L"VT_I8",		(UINT)VT_I8)
		(L"VT_UI8",		(UINT)VT_UI8)
		(L"VT_UI1",		(UINT)VT_UI1)
		(L"VT_I1",		(UINT)VT_I1)
		(L"VT_I2",		(UINT)VT_I2)
		(L"VT_UI2",		(UINT)VT_UI2)
		(L"VT_BSTR",	(UINT)VT_BSTR)
		(L"VT_ARRAY",	(UINT)VT_ARRAY)
		(L"SV_ARRAYNODE", VT_SVARRAYNODE)  /// not used ?
		(L"SV_NODEWITHDATA", VT_SVNODEWITHDATA) /// not used ?
		(L"SV_SVBASENODE", VT_SVBASENODE)
		;


	UINT  VariantHelper::WCHAR2Type(const wchar_t *ptype )
	{
		std::map<std::wstring, UINT>::iterator it =  map_wstring2Type.find(ptype);
		if(it != map_wstring2Type.end())
		{
			return it->second;
		}
		else return VT_UNKNOWN;
	}

	void VariantHelper::ToVariant(const wchar_t *ptype, const wchar_t  *pValue,  _variant_t*  pVar )
	{
		_bstr_t Value =pValue;
		_bstr_t Type = ptype; 

		HRESULT hres = SVVariantConverter::RestoreVariant(Value.GetBSTR(), Type.GetBSTR() ,pVar);
		if (S_OK != hres)
		{
			SVStringVector messageList;
			messageList.push_back(SvUl_SF::Format(_T("%d"), hres));
			SvStl::MessageContainer Exception;
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvOi::Tid_RestoreVariantError, messageList, SvStl::SourceFileParams(StdMessageParams),SvOi::Err_16064_RESTOREVARIANT  );
			throw Exception;
		}

	}

	const  WCHAR* VariantHelper::pWhitespace = L"\a\b\f\n\r\t\v ";

	void  VariantHelper::ToVariant( unsigned vartype, const wchar_t  *pVal, _variant_t* pVar)
	{
		bool bInvalidArgument(false);

		if( nullptr == pVal || nullptr == pVar )
		{
			return;
		}

		pVar->Clear();
		pVar->vt = vartype;
		pVar->lVal =0 ;
		const wchar_t *pNumericVal =  pVal +  wcsspn (pVal, pWhitespace);
		wchar_t *pNumericValEnd(nullptr); 
		switch(vartype)
		{
		case VT_NULL:
		case VT_EMPTY:
		case VT_SVBASENODE:
		case VT_SVARRAYNODE:
		case VT_ARRAY:
			pVar->lVal =0 ;
			break;
		case VT_BSTR:
			pVar->bstrVal = SysAllocStringLen(pVal, (UINT)wcslen(pVal)) ; 
			break;
		case VT_BOOL:
			{
				if(nullptr == pVal)
				{
					bInvalidArgument = true;
				}
				else if( nullptr != pVal && (!wcscmp(pVal, L"TRUE" ) || !wcscmp(pVal, L"YES" )) )
				{
					pVar->boolVal = VARIANT_TRUE;
				}
				else if(!wcscmp(pVal, L"FALSE" ) || !wcscmp(pVal, L"NO" )) 
				{
					pVar->boolVal = VARIANT_FALSE;
				}
				else 
				{
					if (iswdigit (*pNumericVal))
					{
						//-				Convert strings to longs (including hex representations)
						pVar->boolVal = (short) wcstol (pNumericVal, &pNumericValEnd, 0);
					}
					else
					{
						bInvalidArgument = true;
					}

				}
				break;
			}
		case VT_INT:
			{
				if(nullptr == pNumericVal)
				{
					bInvalidArgument = true;
				}
				if ((iswdigit (*pNumericVal)) || ((*pNumericVal == '-') && iswdigit (*(pNumericVal + 1))))
				{
					//-			Convert strings to longs (including hex representations)
					pVar->intVal = wcstol (pNumericVal, &pNumericValEnd, 0);
				}
				else
				{
					bInvalidArgument = true;


				}

				break;

			}
		case VT_I4:
			{
				if(nullptr == pNumericVal)
				{
					bInvalidArgument = true;
				}

				else if ( iswdigit(*pNumericVal) || ((*pNumericVal == '-') && iswdigit (*(pNumericVal + 1))))
				{
					//-			Convert strings to longs (including hex representations)
					pVar->lVal = wcstol (pNumericVal, &pNumericValEnd, 0);
				}
				else
				{
					bInvalidArgument = true;

				}
				break;
			}
		case VT_UINT:
			{
				if (nullptr != pNumericVal &&  iswdigit (*pNumericVal))
				{
					//-			Convert strings to longs (including hex representations)
					pVar->uintVal = wcstol (pNumericVal,  &pNumericValEnd, 0);
				}
				else
				{
					bInvalidArgument = true;
				}

				break;
			}

		case VT_UI4:
			{

				if (nullptr != pNumericVal &&  iswdigit (*pNumericVal))
				{
					//-			Convert strings to longs (including hex representations)
					pVar->ulVal = wcstol (pNumericVal,  &pNumericValEnd, 0);
				}
				else
				{
					bInvalidArgument = true;
				}
				break;
			}

		case VT_R4:
			{

				if (nullptr == pNumericVal)
				{
					bInvalidArgument = true;
				}
				else if ((iswdigit (*pNumericVal)) || ((*pNumericVal == '-') && iswdigit (*(pNumericVal + 1))))
				{
					pVar->fltVal = static_cast <float> (wcstod (pNumericVal, nullptr));
				}
				else
				{
					bInvalidArgument = true;
				}

				break;
			}
		case VT_R8:
			{

				if (nullptr == pNumericVal)
				{
					bInvalidArgument = true;
				}
				else if ((iswdigit (*pNumericVal)) || ((*pNumericVal == '-') && iswdigit (*(pNumericVal + 1))))
				{
					pVar->dblVal = static_cast <double> (wcstod (pNumericVal, nullptr));
				}
				else
				{
					bInvalidArgument = true;
				}

				break;
			}
		case VT_I8:
			{

				if (nullptr == pNumericVal)
				{
					bInvalidArgument = true;
				}

				else if (iswdigit (*pNumericVal))
				{
					//-			Convert strings to int64 (including hex representations)
					pVar->llVal = _wcstoi64 (pNumericVal, &pNumericValEnd, 0);
				}
				else
				{
					bInvalidArgument = true;
				}

				break;
			}
		case VT_UI8:
			{

				if (nullptr == pNumericVal)
				{
					bInvalidArgument = true;
				}

				else if (iswdigit (*pNumericVal))
				{
					//-			Convert strings to int64 (including hex representations)
					pVar->ullVal = _wcstoui64 (pNumericVal, &pNumericValEnd, 0);
				}
				else
				{
					bInvalidArgument = true;
				}

				break;
			}
		case  VT_UI1:
		case  VT_I1:
			{

				if (nullptr == pNumericVal)
				{
					bInvalidArgument = true;
				}
				else if ((iswdigit (*pNumericVal)) || (iswxdigit (*pNumericVal)))
				{
					//-			Convert string to byte (including hex representations)
					swscanf(pNumericVal, L"%hhX", &(pVar->bVal));
				}
				else
				{
					bInvalidArgument = true;
				}

				break;
			}
		default:
			{
				break;
				bInvalidArgument = true;
			}

		}

		if(bInvalidArgument)
		{
			SvStl::MessageContainer Exception;
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR,SvOi::Tid_ToVariant_InvalidArgument, SvStl::SourceFileParams(StdMessageParams),SvOi::Err_16066_TOVARIANT  );
			throw Exception;
		}

	}
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */