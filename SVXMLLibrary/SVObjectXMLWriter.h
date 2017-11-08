//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
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
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <ostream>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/tr1/memory.hpp>

#include "SVUtilityLibrary/SVVariantList.h"
#include "SVUtilityLibrary/XMLwriter.h"
#include "SVObjectLibrary/SVObjectWriter.h"
#pragma endregion Includes

#pragma region Declarations
typedef std::shared_ptr<xml::writer> XMLWriterPtr;
typedef std::shared_ptr<xml::element> XMLElementPtr;
typedef std::deque<XMLElementPtr> Elements;
#pragma endregion Declarations

namespace  SvXml
{
	class SVXMLEncryptionClass;
	
	class SVObjectXMLWriter : public SVObjectWriter
	{
	public:
		SVObjectXMLWriter(std::ostream& os);
		~SVObjectXMLWriter();

		///Write <DATA ...>
		void WriteAttribute(LPCTSTR Name, const _variant_t& value) override;
	
		///Write <DATA ...>
		void WriteAttribute(LPCTSTR Name, const SVVariantList& rValues) override;

	



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
		Elements m_elements;
		SVXMLEncryptionClass*  m_pEncryption;

	};
} //namespace SvXml


