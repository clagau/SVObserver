//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///  class IXMLElementHandler
///  Interface to handle XML events  
/// Get the events from SaxContentHandler and SaxErrorHandler
//******************************************************************************

#pragma once

#pragma region Includes
#include <msxml6.h>
#pragma endregion Includes

namespace Seidenader
{
	namespace  SVXMLLibrary
	{

		/// /brief wstring Interface to handle XML events  
		/// Get the events from SaxContentHandler and SaxErrorHandler
		///naming convention from msxml6.h is used 
		class ISaxElementHandler
		{
		public:
#pragma region Public Methods
			//************************************
			//! CallBackFunction when the saxparser encounted a new xml Element
			//! \param pwchNamespaceUri [in]
			//! \param cchNamespaceUri [in]
			//! \param pwchName [in]
			//! \param cchName [in]
			//! \param pAttributes [in]
			//! \param depth [in]
			//! \returns HRESULT S_OK or E_FAIL if the parse operation should be aborted.
			//************************************
			virtual HRESULT  OnStartElement( const wchar_t  *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchName,  int cchName,ISAXAttributes *pAttributes,int depth ) = 0;

			//************************************
			//!CallBackFunction when the saxparser encounted a new contend 
			//! \param pwchData [in]
			//! \param cchData [in]
			//! \param depth [in]
			//! //! \returns HRESULT S_OK or E_FAIL if the parse operation should be aborted.
			//************************************
			virtual HRESULT  OnElementData(const wchar_t *pwchData,  int cchData, int depth) = 0;

			//************************************
			//! CallBackFunction when the saxparser encounted the end of an  xml Element
			//! \param pwchNamespaceUri [in]
			//! \param cchNamespaceUri [in]
			//! \param pwchName [in]
			//! \param cchName [in]
			//! \param depth [int]
			//! returns HRESULT S_OK or E_FAIL if the parse operation should be aborted. 
			//************************************
			virtual HRESULT  OnEndElement(const wchar_t  *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchName,  int cchName, int depth ) = 0;


			//************************************
			//! CallBackFunction when the saxparser locates  an error
			//! \param line [in,]
			//! \param column [in]
			//! \param pwchErrorText [in]
			//! \param errorCode [in]
			//! \param fatal [in] true if the error is fatal 
			//! returns HRESULT S_OK or E_FAIL if the parse operation should be aborted. 
			//************************************
			virtual HRESULT  OnXMLError(int line, int column, const wchar_t *pwchErrorText, unsigned long errorCode, bool fatal ) = 0;
#pragma endregion Public Methods
		};

	}
}
namespace SvXml = Seidenader::SVXMLLibrary;
