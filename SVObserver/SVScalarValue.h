//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVScalarValue
//* .File Name       : $Workfile:   SVScalarValue.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:02:34  $
//******************************************************************************

#ifndef SVSCALARVALUE_H
#define SVSCALARVALUE_H

#include "SVObjectLibrary/SVObjectReference.h"

class SVValueObjectClass;

class SVScalarValue
{
public:
	CString            strName;
	CString            strValue;
	CString            strType;
	SVObjectReference  object;
	unsigned long      status;

	SVScalarValue() : status(0) {}
	explicit SVScalarValue( SVObjectReference& rhs ) : status(0) { object = rhs; strName = object.GetCompleteObjectName(); }
	SVScalarValue(const CString& p_rstrName, const CString& p_rstrValue, const SVObjectReference& p_Object = SVObjectReference(), const CString& p_rstrType=CString())
		: strName(p_rstrName), strValue(p_rstrValue), strType(p_rstrType), object(p_Object), status(0) {}
	void clear() { strName.Empty(); strValue.Empty(); strType.Empty(); object=SVObjectReference(); status=0; }
};

typedef std::map    <CString, SVScalarValue>  SVScalarValueMapType;
typedef std::vector <SVScalarValue>           SVScalarValueVectorType;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVScalarValue.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:02:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Jan 2006 14:17:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Conditional History bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:20:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
