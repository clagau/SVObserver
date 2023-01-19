//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "VariantHelper.h"
#include "SVVariantCustomTypes.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMessage/SVMessage.h"
#include "SVVariantConverter.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvXml
{
	static std::map<std::wstring, VARTYPE>  map_wstring2Type
	{
		{L"VT_EMPTY",	static_cast<VARTYPE> (VT_EMPTY)},
		{L"VT_NULL",	static_cast<VARTYPE> (VT_NULL)},
		{L"VT_BOOL",	static_cast<VARTYPE> (VT_BOOL)},
		{L"VT_INT",		static_cast<VARTYPE> (VT_INT)},
		{L"VT_I4",		static_cast<VARTYPE> (VT_I4)},
		{L"VT_UINT",	static_cast<VARTYPE> (VT_UINT)},
		{L"VT_UI4",		static_cast<VARTYPE> (VT_UI4)},
		{L"VT_R4",		static_cast<VARTYPE> (VT_R4)},
		{L"VT_R8",		static_cast<VARTYPE> (VT_R8)},
		{L"VT_I8",		static_cast<VARTYPE> (VT_I8)},
		{L"VT_UI8",		static_cast<VARTYPE> (VT_UI8)},
		{L"VT_UI1",		static_cast<VARTYPE> (VT_UI1)},
		{L"VT_I1",		static_cast<VARTYPE> (VT_I1)},
		{L"VT_I2",		static_cast<VARTYPE> (VT_I2)},
		{L"VT_UI2",		static_cast<VARTYPE> (VT_UI2)},
		{L"VT_BSTR",	static_cast<VARTYPE> (VT_BSTR)},
		{L"VT_ARRAY",	static_cast<VARTYPE> (VT_ARRAY)},
		{L"SV_ARRAYNODE", static_cast<VARTYPE> (VT_SVARRAYNODE)},  /// not used ?
		{L"SV_NODEWITHDATA", static_cast<VARTYPE> (VT_SVNODEWITHDATA)}, /// not used ?
		{L"SV_SVBASENODE", static_cast<VARTYPE> (VT_SVBASENODE)}
	};


	VARTYPE  VariantHelper::WCHAR2Type(const wchar_t *ptype )
	{
		auto it =  map_wstring2Type.find(ptype);
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
			SvDef::StringVector messageList;
			messageList.push_back(std::format(_T("{:d}"), hres));
			SvStl::MessageContainer Exception;
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR, SvStl::Tid_RestoreVariantError, messageList, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}

	}

	const  WCHAR* VariantHelper::pWhitespace = L"\a\b\f\n\r\t\v ";

	void  VariantHelper::ToVariant(VARTYPE vartype, const wchar_t* pVal, _variant_t* pVar)
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
			///Set vt to VT_EMPTY otherwise ::VariantClear in SetString causes a crash!
			pVar->vt = VT_EMPTY;
			pVar->SetString(SvUl::createStdString(pVal).c_str());
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
				//This is when the float value is evaluated to infinity and saved into the configuration
				else if (wcscmp(pVal, L"inf") == 0)
				{
					pVar->fltVal = std::numeric_limits<float>::infinity();
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
				//This is when the double value is evaluated to infinity and saved into the configuration
				else if (wcscmp(pVal, L"inf") == 0)
				{
					pVar->dblVal = std::numeric_limits<double>::infinity();
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
				bInvalidArgument = true;
				break;
			}

		}

		if(bInvalidArgument)
		{
			SvStl::MessageContainer Exception;
			Exception.setMessage(SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR,SvStl::Tid_ToVariant_InvalidArgument, SvStl::SourceFileParams(StdMessageParams));
			throw Exception;
		}

	}
} //namespace SvXml
