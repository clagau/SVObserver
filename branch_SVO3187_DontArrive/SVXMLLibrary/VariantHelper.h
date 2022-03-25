//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
///  class VariantHelper
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvXml
{
	class VariantHelper
	{
	#pragma region Public Methods
	public:
		//************************************
		//! Convert a string to a Variant Type
		//! \param ptype [in]
		//! \returns VARTYPE
		//************************************
		static VARTYPE WCHAR2Type(const wchar_t *ptype );

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
		static void ToVariant(VARTYPE vartype, const wchar_t  *pVal, _variant_t* pVar);
	#pragma endregion Public Methods
	
	#pragma region Member variables
	private:
		const static WCHAR*  pWhitespace;
	#pragma endregion Member variables
	};
} //namespace SvXml

