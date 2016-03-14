//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectSynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectSynchronousCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:54  $
//******************************************************************************

#ifndef SVOBJECTSYNCHRONOUSCOMMANDTEMPLATE_H
#define SVOBJECTSYNCHRONOUSCOMMANDTEMPLATE_H

#include "SVUtilityLibrary/SVGUID.h"
template< typename SVCommandPtr >
class SVObjectSynchronousCommandTemplate
{
public:
	SVObjectSynchronousCommandTemplate( const SVGUID& p_rObjectID, const SVCommandPtr& p_rCommandPtr );

	virtual ~SVObjectSynchronousCommandTemplate();

	HRESULT Execute( DWORD p_TimeoutInMilliseconds );

private:
	SVObjectSynchronousCommandTemplate();

	SVGUID m_ObjectID;
	SVCommandPtr m_CommandPtr;
};

enum 
{
	TWO_MINUTE_CMD_TIMEOUT = 120000
};

#include "SVObjectSynchronousCommandTemplate.inl"

#endif

