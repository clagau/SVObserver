//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectXMLWriter
//* .File Name       : $Workfile:   SVObjectXMLWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 02:47:46  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLLibrary/XMLwriter.h"
#include "ObjectInterfaces/IObjectWriter.h"
#pragma endregion Includes

#pragma region Declarations
typedef std::shared_ptr<xml::writer> XMLWriterPtr;
typedef std::shared_ptr<xml::element> XMLElementPtr;
typedef std::deque<XMLElementPtr> cElements;
#pragma endregion Declarations

namespace  SvXml
{
	class SVXMLEncryptionClass;
	
	class SVObjectXMLWriter : public SvOi::IObjectWriter
	{
	public:
		explicit SVObjectXMLWriter(std::ostream& os);
		virtual ~SVObjectXMLWriter();

		///Write <DATA ...>
		void WriteAttribute(LPCTSTR Name, const _variant_t& value) override;
	
		///Write <DATA ...>
		void WriteAttribute(LPCTSTR Name, const std::vector<_variant_t>& rValues) override;

	



		///StartNode <Node  Name =  
		void StartElement(LPCTSTR Name) override;
		///EndNode  
		void EndElement();
		///EndAllNode
		void EndAllElements();
		void ElementAttribute(LPCTSTR AttrName, const _variant_t& value);

		void WriteRootElement(LPCTSTR Name);
		void WriteSchema();

	
		//! Set Encryption pointer
		//! \param m_pEncryption [in]
		//! \returns void
		void SetEncryption(SVXMLEncryptionClass*  m_pEncryption);

		//! Writes Encryption Section 
		//! \returns void
		void WriteEncryption();

		//************************************
		/// Write the start of the node base
		//************************************
		void WriteStartOfBase();

		///  Write the Environment-Element only with the version 
		/// \param version [in] SVO version number
		void WriteShortEvirmonment(DWORD version);

		//************************************
		/// Write the revision history node.
		/// \param formatVersionValue [in] The version number.
		/// \param revisionValue [in] The revision value.
		//************************************
		void WriteRevisionHistory(const _variant_t formatVersionValue, const _variant_t revisionValue);

		//************************************
		/// Sets if a new line character is inserted for each line inserted in the file
		/// \param NewLine [in] true if a new line character should be inserted
		//************************************
		void setNewLine( bool NewLine );


		void setHeader(LPCTSTR header);
	private:
		XMLWriterPtr m_pWriter;
		cElements m_elements;
		SVXMLEncryptionClass*  m_pEncryption;

	};
} //namespace SvXml


