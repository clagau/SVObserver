//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectXMLWriter
//* .File Name       : $Workfile:   SVObjectXMLWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:19:56  $
//******************************************************************************
#ifndef INCL_SVOBJECTXMLWRITER_H
#define INCL_SVOBJECTXMLWRITER_H

#include <deque>
#include <ostream>
#include <boost/function.hpp>
#include <boost/tr1/memory.hpp>
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVVariantList.h"
#include "SVUtilityLibrary/XMLwriter.h"
#include "SVObjectWriter.h"

typedef std::tr1::shared_ptr<xml::writer> XMLWriterPtr;
typedef std::tr1::shared_ptr<xml::element> XMLElementPtr;
typedef std::deque<XMLElementPtr> Elements;

class SVObjectXMLWriter : public SVObjectWriter
{
private:
	XMLWriterPtr m_pWriter;
	Elements m_elements;

public:
	SVObjectXMLWriter(std::ostream& os);
	~SVObjectXMLWriter();

	void WriteAttribute(const SVString& rName, const _variant_t& value);
	void WriteAttribute(const SVString& rName, const SVVariantList& rValues);

	void StartElement(const SVString& rName);
	void EndElement();
	void ElementAttribute(const SVString& rAttrName, const _variant_t& value);

	void WriteRootElement(const SVString& rName);
	void WriteSchema();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectXMLWriter.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:19:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 2012 11:54:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated type definitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Sep 2012 13:16:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in of new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/