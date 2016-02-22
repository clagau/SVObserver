//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///  class VariantHelper
//******************************************************************************

#pragma once

#pragma region Includes
#include "comutil.h"
#pragma endregion Includes

namespace Seidenader { namespace  SVXMLLibrary
{
	class VariantHelper
	{
	#pragma region Public Methods
	public:
		//************************************
		//! Convert a string to a Variant Type
		//! \param ptype [in]
		//! \returns UINT
		//************************************
		static  UINT WCHAR2Type(const wchar_t *ptype );

		//************************************
		//! convert variant to a value and type string 
		//! \param rVar [in]
		//! \param rtype [out]
		//! \param rValue [out]
		//! \returns void
		//************************************
		static void FromVariant( const _variant_t& rVar, std::wstring  &rtype, std::wstring  &rValue );
			
		//************************************
		//! Convert a type and value string to a variant 
		//! \param ptype [in]
		//! \param pValue [in]
		//! \param pVar [out]
		//! \returns void
		//************************************
		static void ToVariant(const wchar_t *ptype , const wchar_t  *pValue,  _variant_t*  pVar );
			
		//************************************
		//!Convert a type and value string to a variant 
		//! \param vartype [in,out]
		//! \param pVal [in,out]
		//! \param pVar [in,out]
		//! \returns void
		//************************************
		static void ToVariant( unsigned vartype, const wchar_t  *pVal, _variant_t* pVar);
	#pragma endregion Public Methods
	
	#pragma region Member variables
	private:
		const static WCHAR*  pWhitespace;
	#pragma endregion Member variables
	};
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */

namespace SvXml = Seidenader::SVXMLLibrary;
