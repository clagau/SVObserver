// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCICallbackStruct
// * .File Name       : $Workfile:   SVCICallbackStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:04  $
// ******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"

/*
This class is used to manage callback information for each buffer.
*/
struct SVCICallbackStruct  
{
public:
	SVCICallbackStruct();
	virtual ~SVCICallbackStruct();

	//This attribute holds the index to the associated digitizer.
	unsigned long m_ulDigitizerIndex;
	//This attribute holds the index to the associated buffer.
	unsigned long m_ulBufferIndex;

	bool m_IsTrash;

};

/*
This specialization of the CArray template creates a container object to hold pointers to the SVCICallbackStruct objects.
*/
typedef SVVector< SVCICallbackStruct * > SVCICallbackStructPtrCArray;


