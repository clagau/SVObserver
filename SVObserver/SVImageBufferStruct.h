//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferStruct
//* .File Name       : $Workfile:   SVImageBufferStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:49:48  $
//******************************************************************************

#ifndef SVIMAGEBUFFERSTRUCT_H_
#define SVIMAGEBUFFERSTRUCT_H_

#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVImageBuffer.h"	// SVImageOverlayClass

#include "SVLibrary/SVReferenceCountingTemplate.h"
#include "SVScalarValue.h"

struct SVImageBufferStruct
{
	SVSmartHandlePointer       handle;
	SVImageInfoClass           info;
	CString                    strName;
	SVObjectReference          object;
	SVImageOverlayClass        overlay;

	SVImageBufferStruct() {}
	SVImageBufferStruct( const SVImageBufferStruct& rhs ) : handle(rhs.handle), info(rhs.info), strName(rhs.strName), object(rhs.object), overlay(rhs.overlay) {}	// needs to be defined to avoid infinite recursion
	explicit SVImageBufferStruct( const SVScalarValue& rhs );
	operator SVScalarValue() const;
	const SVImageBufferStruct& operator = ( const SVImageBufferStruct& rhs );

	HRESULT CreateBuffer();
	HRESULT DestroyBuffer();
};

// Resource Manager policy class for SVReferenceCountingTemplate
struct SVImageBufferStructResourceManager
{
	static void      DefaultCreate ( SVImageBufferStruct* ) {};
	static HRESULT Create        ( SVImageBufferStruct* );
	static HRESULT Destroy       ( SVImageBufferStruct* );
	static void      Clear         ( SVImageBufferStruct& );
};

typedef SVReferenceCountingTemplate < SVImageBufferStruct, SVImageBufferStructResourceManager > SVRefCountImageBufferStruct;

typedef std::vector <SVImageBufferStruct>  SVImageBufferStructVectorType;
typedef std::vector <SVRefCountImageBufferStruct> SVRefCountImageBufferStructVectorType;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImageBufferStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:49:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Feb 2011 14:45:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Oct 2010 13:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 11:00:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:20:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
