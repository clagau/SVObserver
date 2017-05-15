///*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// SaxEncryptionHandler encapsulate Decryption functuionality for the SAX Parser
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
#include <msxml6.h>
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

namespace SvXml
{
	SaxEncryptionHandler::SaxEncryptionHandler() : m_IsActive(false)
	{
	}

	SaxEncryptionHandler::~SaxEncryptionHandler() 
	{
	}

	bool SaxEncryptionHandler::SaxIsEncrypted() const
	{
		return m_IsActive;
	};

	void SaxEncryptionHandler::DecryptAttribute(SaxTreeElement* pTreeElement)  const
	{
		if(nullptr != pTreeElement && SaxIsEncrypted())
		{
			_bstr_t  bstrString(pTreeElement->GetNameAtt() );
			SVBStr bstrDecryptedString;	
			m_Encryptionclass.DecryptNameAttribute(bstrString.GetBSTR(), &bstrDecryptedString);
			pTreeElement->SetNameAttribute((LPCWSTR) bstrDecryptedString);
			bstrString =  pTreeElement->GetTypeAtt();
			m_Encryptionclass.DecryptString(bstrString.GetBSTR(),&bstrDecryptedString );
			pTreeElement->SetTypeAttribute((LPCWSTR) bstrDecryptedString);
		}
	}

	void SaxEncryptionHandler::DecryptContent(SaxTreeElement* pTreeElement)  const
	{
		if(nullptr != pTreeElement && SaxIsEncrypted())
		{
			_bstr_t  bstrString(pTreeElement->GetContent() );
			SVBStr bstrDecryptedString;	
			m_Encryptionclass.DecryptString(bstrString.GetBSTR(),&bstrDecryptedString );
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
		
		m_IsActive = m_Encryptionclass.LoadEncryptionFromSaxData(pname,pcontent);

		return m_IsActive;		
	}
} //namespace SvXml
