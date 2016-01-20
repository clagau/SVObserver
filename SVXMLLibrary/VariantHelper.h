#pragma once
//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///  class VariantHelper
//******************************************************************************

namespace Seidenader
{
	namespace  SVXMLLibrary
	{
		
		class VariantHelper
		{
		public:
			//************************************
			//! Convert a string to a Variant Type
			//! \param ptype [in]
			//! \returns UINT
			//************************************
			static  UINT WCHAR2Type(const wchar_t *ptype );
			//************************************
			//! convert variant to a value and type string 
			//! \param Var [in]
			//! \param rtype [out]
			//! \param rValue [out]
			//! \returns void
			//************************************
			static void FromVariant( VARIANT  const *Var, std::wstring  &rtype, std::wstring  &rValue );
			
			//************************************
			//! Convert a type and value string to a variant 
			//! \param ptype [in]
			//! \param pValue [in]
			//! \param pVar [out]
			//! \returns void
			//************************************
			static void ToVariant(const wchar_t *ptype , const wchar_t  *pValue,  VARIANT*  pVar );
			
			
			//************************************
			//!Convert a type and value string to a variant 
			//! \param vartype [in,out]
			//! \param pVal [in,out]
			//! \param pVar [in,out]
			//! \returns void
			//************************************
			static void ToVariant( unsigned vartype, const wchar_t  *pVal, VARIANT* pVar);
		private:
			const static WCHAR*  pWhitespace;
		};

	}
}
namespace SvXml = Seidenader::SVXMLLibrary;
