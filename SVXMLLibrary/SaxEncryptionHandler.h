//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// SaxEncryptionHandler encapsulate Decryption functuionality for the SAX Parser
//******************************************************************************
#pragma once


#pragma region Includes
#include "SVXMLEncryptionClass.h"
#pragma endregion Includes

namespace Seidenader { namespace  SVXMLLibrary
{
	
	class SaxTreeElement;
	/// \brief Class encapsulate Decryptionfunctuionality for the SAX Parser
	class SaxEncryptionHandler
	{
	public:
		SaxEncryptionHandler();
		~SaxEncryptionHandler();
		//************************************
		//! returns true if encryption is active
		//! \returns bool
		//************************************
		bool SaxIsEncrypted() const;
		//************************************
		//! Set the parameter for the encryption 
		//! \param pname [in]
		//! \param pcontent [in]
		//! \returns bool
		//************************************
		bool SaxSetEncryption(LPCWSTR pname , LPCWSTR pcontent);
		//************************************
		//! Disable the encryption
		//! \returns void
		//************************************
		void DisableEncryption();

		//************************************
		//! Decrypt name and type attribute in SAXtreeelement
		//! \param pTreeElement [in, out]
		//! \returns void
		//************************************
		void DecryptAttribute(SaxTreeElement* pTreeElement) const; 
		//************************************
		//! Decrypt content in SAXTreeelemnet
		//! \param pTreeElement [in,out]
		//! \returns void
		//************************************
		void DecryptContent(SaxTreeElement* pTreeElement) const; 
	private:
		bool m_IsActive;
		SVXMLEncryptionClass* m_pEncryptionclass;

	};
}
}
namespace SvXml = Seidenader::SVXMLLibrary;
