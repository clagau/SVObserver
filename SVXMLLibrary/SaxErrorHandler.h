//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// class  class SVSaxErrorHandler implements ISAXErrorHandler
//******************************************************************************
#pragma once
#pragma region Includes
#include <msxml6.h>
#include "ISaxElementHandler.h"
#pragma endregion Includes


namespace Seidenader
{
	namespace  SVXMLLibrary
	{

		/// /brief Implement ISAXerrorHandler
		/// events are delegated to IXMLElementHandler
		///naming convention from msxml6.h is used 
		class SaxErrorHandler : public ISAXErrorHandler
		{
		public:
#pragma region Constructor
			SaxErrorHandler(void);
#pragma endregion Constructor

#pragma region Public Methods
			virtual long STDMETHODCALLTYPE          QueryInterface(const struct _GUID &,void ** );
			virtual unsigned long STDMETHODCALLTYPE AddRef(void);
			virtual unsigned long STDMETHODCALLTYPE Release(void);

			virtual HRESULT STDMETHODCALLTYPE error( 
				/* [in] */ ISAXLocator *pLocator,
				/* [in] */ const wchar_t *pwchErrorMessage,
				/* [in] */ HRESULT hrErrorCode);

			virtual HRESULT STDMETHODCALLTYPE fatalError( 
				/* [in] */ ISAXLocator *pLocator,
				/* [in] */ const wchar_t *pwchErrorMessage,
				/* [in] */ HRESULT hrErrorCode);

			virtual HRESULT STDMETHODCALLTYPE ignorableWarning( 
				/* [in] */ ISAXLocator *pLocator,
				/* [in] */ const wchar_t *pwchErrorMessage,
				/* [in] */ HRESULT hrErrorCode);
			
			//************************************
			//! / Attach/XML events handler.
			//! \param pElementHandler [in]
			//! \returns void
			//************************************
			void AttachElementHandler(ISaxElementHandler*  pElementHandler);
#pragma endregion Public Methods
		private:
#pragma region Private Methods
			//************************************
			//! Callbackfunction is called when an XML error occurs
			//! \param pLocator [in]
			//! \param pwchErrorMessage [in]
			//! \param errCode [in]
			//! \param isfatal [in]
			//! \returns HRESULT if not S_OK parsing will be stopped even the error is not fatal
			//************************************
			HRESULT ReportError( ISAXLocator*    pLocator, const wchar_t  * pwchErrorMessage, HRESULT  errCode, bool isfatal );
#pragma endregion Private Methods

			ISaxElementHandler* m_ElementHandler; 
		};

	}
}