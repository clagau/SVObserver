//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// SaxEncryptionHandler encapsulate Decryption functuionality for the SAX Parser
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVXMLEncryptionClass.h"
#pragma endregion Includes

namespace SvXml
{
	class SaxTreeElement;
	/// \brief Class encapsulate Decryption functionality for the SAX Parser
	class SaxEncryptionHandler
	{
	#pragma region Constructor
	public:
		SaxEncryptionHandler();
		virtual ~SaxEncryptionHandler();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
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
	#pragma endregion Public Methods

	#pragma region Member variables
	private:
		bool m_IsActive;
		mutable SVXMLEncryptionClass m_Encryptionclass;
	#pragma endregion Member variables
	};
} //namespace SvXml
