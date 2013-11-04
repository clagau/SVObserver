//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGetObjectDequeByTypeVisitor
//* .File Name       : $Workfile:   SVGetObjectDequeByTypeVisitor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:42:16  $
//******************************************************************************

#ifndef SVGETOBJECTDEQUEBYTYPEVISITOR_H
#define SVGETOBJECTDEQUEBYTYPEVISITOR_H

#include <deque>

#include "SVObjectLibrary/SVObjectTypeInfoStruct.h"

class SVObjectClass;

class SVGetObjectDequeByTypeVisitor
{
public:
	typedef std::deque< SVObjectClass* > SVObjectPtrDeque;

	SVGetObjectDequeByTypeVisitor( const SVObjectTypeInfoStruct& p_rObjectInfo );
	virtual ~SVGetObjectDequeByTypeVisitor();

	virtual HRESULT VisitElement( SVObjectClass& p_rElement );

	const SVObjectPtrDeque& GetObjects() const;

protected:
	SVObjectTypeInfoStruct m_ObjectInfo;
	SVObjectPtrDeque m_Objects;

private:
	SVGetObjectDequeByTypeVisitor();

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGetObjectDequeByTypeVisitor.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:42:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jul 2012 08:01:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved visitor object to main SVObserver project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:02:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/