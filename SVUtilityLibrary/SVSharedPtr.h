//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedPtr
//* .File Name       : $Workfile:   SVSharedPtr.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:24:42  $
//******************************************************************************

#ifndef SVSHAREDPTR_H
#define SVSHAREDPTR_H

#include <boost/config.hpp> // put this first to suppress some VC++ warnings
#include <boost/tr1/memory.hpp>
#include <memory>
#include "SVNullDeleter.h"

///////////////////////////////////////////////////////////////////////
// A Ref counted Pointer class
///////////////////////////////////////////////////////////////////////
template< typename Type >
class SVSharedPtr : public std::tr1::shared_ptr< Type > 
{
public:
	typedef typename std::tr1::shared_ptr< Type > SVBaseSharedRefPtr;
	typedef typename std::auto_ptr< Type > SVAutoPtr;
	typedef SVSharedPtr< Type > SVSharedRefPtr;
	typedef Type Pointee;

	SVSharedPtr() 
	: SVBaseSharedRefPtr() {}

	SVSharedPtr( SVAutoPtr& p_rAutoPtr ) 
	: SVBaseSharedRefPtr(p_rAutoPtr) {}

	// use this constructor for dynamicallly allocated varisbles
	SVSharedPtr( Type* pObject ) 
	: SVBaseSharedRefPtr(pObject) {}

	// use this constructor for statically allocated varisbles
	template< typename SVDeleter >
	SVSharedPtr( Type* pObject, const SVDeleter& deleter ) 
	: SVBaseSharedRefPtr(pObject, deleter) {}
	
	virtual ~SVSharedPtr() 
	{
#ifdef TRACE_DESTRUCT
		std::cout << "Destruct" << " ADDR: " << get() << " RefCnt: " << use_count() << std::endl;
#endif
	}

	virtual bool empty() const { return get() == NULL; }

	virtual void clear() { reset(); }

	SVSharedPtr& operator=(const SVSharedPtr& rRefPtr) 
	{
		if( &rRefPtr != this )
		{
			SVBaseSharedRefPtr::operator=(rRefPtr);
		}

		return *this;
	}

	// Try to provide casting operator...
	template<typename T>
	operator std::tr1::shared_ptr<T>() const { return boost::dynamic_pointer_cast<T, Type>(this); }
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVSharedPtr.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:24:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 May 2010 14:44:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for a generic delete function object for shared pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 09:40:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:29:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
