//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ManagedIndexArrayList.h
//* .File Name       : $Workfile:   ManagedIndexArrayList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:42  $
//******************************************************************************

#ifndef _INC_MANAGEDINDEXARRAYLIST_3C8E8E400133_INCLUDED
#define _INC_MANAGEDINDEXARRAYLIST_3C8E8E400133_INCLUDED

#include "SVContainerLibrary/SVVector.h"

class SVManagedIndexArray;

class SVManagedIndexArrayList :
	public SVVector< SVManagedIndexArray* >
{
public:
	SVManagedIndexArrayList();

	virtual ~SVManagedIndexArrayList();

};

#endif /* _INC_MANAGEDINDEXARRAYLIST_3C8E8E400133_INCLUDED */

