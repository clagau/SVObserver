///*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// SaxEncryptionHandler encapsulate Decryption functuionality for the SAX Parser
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
#import <msxml6.dll> // MSXML
#include "SaxEncryptionHandler.h"
#include "SVXMLEncryptionClass.h"
#include "SaxTreeElement.h"
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

	SaxEncryptionHandler::SaxEncryptionHandler() : m_IsActive(false)
	{
		m_pEncryptionclass =  new SVXMLEncryptionClass;
	}

	SaxEncryptionHandler::~SaxEncryptionHandler() 
	{
		delete m_pEncryptionclass;

	}

	bool SaxEncryptionHandler::SaxIsEncrypted() const
	{
		return m_IsActive && nullptr != m_pEncryptionclass;
	};



	void SaxEncryptionHandler::DecryptAttribute(SaxTreeElement* pTreeElement)  const
	{
		if(nullptr != pTreeElement && SaxIsEncrypted())
		{
			_bstr_t  bstrString(pTreeElement->GetNameAtt() );
			SVBStr bstrDecryptedString;	
			m_pEncryptionclass->DecryptNameAttribute(bstrString.Detach(), &bstrDecryptedString);
			pTreeElement->SetNameAttribute((LPCWSTR) bstrDecryptedString);
			bstrString =  pTreeElement->GetTypeAtt();
			m_pEncryptionclass->DecryptString(bstrString.Detach(),&bstrDecryptedString );
			pTreeElement->SetTypeAttribute((LPCWSTR) bstrDecryptedString);

		}

	}

	void SaxEncryptionHandler::DecryptContent(SaxTreeElement* pTreeElement)  const
	{
		if(nullptr != pTreeElement && SaxIsEncrypted())
		{
			_bstr_t  bstrString(pTreeElement->GetContent() );
			SVBStr bstrDecryptedString;	
			m_pEncryptionclass->DecryptString(bstrString.Detach(),&bstrDecryptedString );
			pTreeElement->SetContent((LPCWSTR) bstrDecryptedString);
		}

	}

	void SaxEncryptionHandler::DisableEncryption()
	{
		m_IsActive = false;
	}

	bool  SaxEncryptionHandler::SaxSetEncryption(LPCWSTR pname , LPCWSTR pcontent)
	{
		m_IsActive = false;
		

		if(m_pEncryptionclass)
		{
			m_IsActive = m_pEncryptionclass->LoadEncryptionFromSaxData(pname,pcontent);
		}
		return m_IsActive;		
	}

}
}