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

#pragma once

//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/tr1/memory.hpp>
//Moved to precompiled header: #include <memory>
#include "SVNullDeleter.h"
///////////////////////////////////////////////////////////////////////
// A Ref counted Pointer class
///////////////////////////////////////////////////////////////////////
template< typename Type >
class SVSharedPtr : public std::shared_ptr< Type > 
{
public:
	typedef typename std::shared_ptr< Type > SVBaseSharedRefPtr;
	typedef typename std::auto_ptr< Type > SVAutoPtr;
	typedef SVSharedPtr< Type > SVSharedRefPtr;
	typedef Type Pointee;

	SVSharedPtr() 
	: SVBaseSharedRefPtr() {}

	SVSharedPtr( SVAutoPtr& p_rAutoPtr ) 
	: SVBaseSharedRefPtr(p_rAutoPtr) {}

	// use this constructor for dynamically allocated variables
	SVSharedPtr( Type* pObject ) 
	: SVBaseSharedRefPtr(pObject) {}

	// use this constructor for statically allocated variables
	template< typename SVDeleter >
	SVSharedPtr( Type* pObject, const SVDeleter& deleter ) 
	: SVBaseSharedRefPtr(pObject, deleter) {}
	
	virtual ~SVSharedPtr() 
	{
#ifdef TRACE_DESTRUCT
		std::cout << "Destruct" << " ADDR: " << get() << " RefCnt: " << use_count() << std::endl;
#endif
	}

	bool empty() const { return nullptr == get(); }

	void clear() { reset(); }

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
	operator std::shared_ptr<T>() const { return boost::dynamic_pointer_cast<T, Type>(this); }
};

