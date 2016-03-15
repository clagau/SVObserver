// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIServerStruct
// * .File Name       : $Workfile:   SVCIServerStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:08  $
// ******************************************************************************

#ifndef SVCISERVERSTRUCT_H
#define SVCISERVERSTRUCT_H

#include <corapi.h>
#include "SVContainerLibrary/SVVector.h"

/*
This class is the data class for each server available for use.
*/
struct SVCIServerStruct  
{
public:
	SVCIServerStruct();
	SVCIServerStruct( const SVCIServerStruct &p_rValue );
	virtual ~SVCIServerStruct();

	const SVCIServerStruct &operator=( const SVCIServerStruct &p_rValue );

	//This attribute holds the server name.
	CString m_csName;

	//This attribute holds the Coreco server handle.
	CORSERVER m_hCorServer;
};

/*
This specialization of the CArray template creates a container object to hold the SVCIServerStruct objects.
*/
typedef SVVector< SVCIServerStruct > SVCIServerStructCArray;

#endif

