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
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVVariantList.h"
#include "SVUtilityLibrary/XMLwriter.h"
#include "SVObjectWriter.h"
#pragma endregion Includes

#pragma region Declarations
typedef std::tr1::shared_ptr<xml::writer> XMLWriterPtr;
typedef std::tr1::shared_ptr<xml::element> XMLElementPtr;
typedef std::deque<XMLElementPtr> Elements;
#pragma endregion Declarations

class SVObjectXMLWriter : public SVObjectWriter
{
public:
	SVObjectXMLWriter(std::ostream& os);
	~SVObjectXMLWriter();

	void WriteAttribute(LPCTSTR rName, const _variant_t& value) override;
	void WriteAttribute(LPCTSTR rName, const SVVariantList& rValues) override;

	void StartElement(LPCTSTR rName) override;
	void EndElement();
	void EndAllElements();
	void ElementAttribute(LPCTSTR rAttrName, const _variant_t& value);

	void WriteRootElement(LPCTSTR rName);
	void WriteSchema();

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

private:
	XMLWriterPtr m_pWriter;
	Elements m_elements;

};


