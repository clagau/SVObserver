///*****************************************************************************
/// \copyright (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
////  class SVSaxErrorHandler
//*****************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SaxErrorHandler.h"
#include "ISaxElementHandler.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvXml
{
	SaxErrorHandler::SaxErrorHandler(void) :
	m_pElementHandler( nullptr )
	{
	}

	SaxErrorHandler::~SaxErrorHandler()
	{
	}

	long STDMETHODCALLTYPE SaxErrorHandler::QueryInterface(const struct _GUID &,void ** )
	{
		// Not implemented as this class is not a COM object.
		return 0;
	}
	
	unsigned long STDMETHODCALLTYPE SaxErrorHandler::AddRef(void)
	{
		// Not implemented as this class is not a COM object.
		return 0;
	}
	
	unsigned long STDMETHODCALLTYPE SaxErrorHandler::Release(void)
	{
		// Not implemented as this class is not a COM object.
		return 0;
	}

	HRESULT STDMETHODCALLTYPE SaxErrorHandler::error( 
		/* [in] */ ISAXLocator  *pLocator,
		/* [in] */ const wchar_t * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
	{
		return ReportError(pLocator, pwchErrorMessage, errCode, false);


	}

	HRESULT STDMETHODCALLTYPE SaxErrorHandler::fatalError( 
		/* [in] */ ISAXLocator  *pLocator,
		/* [in] */ const wchar_t * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
	{
		return ReportError(pLocator, pwchErrorMessage, errCode, true);
	}

	HRESULT STDMETHODCALLTYPE SaxErrorHandler::ignorableWarning( 
		/* [in] */ ISAXLocator* ,
		/* [in] */ const wchar_t* ,
		/* [in] */ HRESULT )
	{
		return S_OK;
	}

	void SaxErrorHandler::AttachElementHandler(ISaxElementHandler* pElementHandler)
	{
		m_pElementHandler  = pElementHandler;
	}

	HRESULT SaxErrorHandler::ReportError( ISAXLocator*  pLocator, const wchar_t* pwchErrorMessage, HRESULT  errCode, bool fatal )
	{
		// Get the line and column where the error occurred.
		int line = 0;
		int column = 0;
		if ( nullptr != pLocator )
		{
			pLocator->getLineNumber(&line);
			pLocator->getColumnNumber(&column);
		}

		if ( nullptr != m_pElementHandler)
		{
			// Delegate error to element handler.
			return m_pElementHandler->OnXMLError(line, column, pwchErrorMessage, errCode, fatal);
		}
		return S_OK;
	}
} //namespace SvXml
