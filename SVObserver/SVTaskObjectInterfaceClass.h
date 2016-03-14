//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceClass
//* .File Name       : $Workfile:   SVTaskObjectInterfaceClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:25:48  $
//******************************************************************************

#ifndef SVTASKOBJECTINTERFACECLASS_H
#define SVTASKOBJECTINTERFACECLASS_H

#include "SVTaskObjectInterfaceInputRequestStruct.h"
#include "SVTaskObjectValueInterface.h"
#include "SVValueObjectReference.h"

class SVTaskObjectClass;
class SVToolClass;

class SVTaskObjectInterfaceClass : public SVTaskObjectValueInterface
{
public:
	SVTaskObjectInterfaceClass();
	SVTaskObjectInterfaceClass( SVTaskObjectClass *p_psvTaskObject );
	virtual ~SVTaskObjectInterfaceClass();

	virtual HRESULT SetInspectionData() = 0;
};

#endif

