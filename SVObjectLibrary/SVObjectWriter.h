//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectWriter
//* .File Name       : $Workfile:   SVObjectWriter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:14  $
//******************************************************************************
#ifndef INCL_SVOBJECTWRITER_H
#define INCL_SVOBJECTWRITER_H

#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVVariantList.h"
#include "SVUtilityLibrary/SVNameVariantList.h"

class SVObjectWriter
{
public:
	SVObjectWriter() {}
	virtual ~SVObjectWriter() {}

	virtual void WriteAttribute(const SVString& rName, const _variant_t& value)=0;
	virtual void WriteAttribute(const SVString& rName, const SVVariantList& rValues)=0;

	virtual void StartElement(const SVString& rName)=0;
	virtual void EndElement()=0;
	virtual void ElementAttribute(const SVString& rAttrName, const _variant_t& value)=0;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectWriter.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:00:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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