// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalStruct
// * .File Name       : $Workfile:   SVIODigitalStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:52  $
// ******************************************************************************

#ifndef _SVIODIGITALSTRUCT_H
#define _SVIODIGITALSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct SVIODigitalStruct  
{
	SVIODigitalStruct();
	SVIODigitalStruct( const SVIODigitalStruct &p_rOriginal );
	virtual ~SVIODigitalStruct();

	const SVIODigitalStruct &operator=( const SVIODigitalStruct &p_rOriginal );

	bool m_bIsInverted;
	bool m_bIsForced;
	bool m_bForcedValue;
	bool m_bValue;
};

#endif // _SVIODIGITALSTRUCT_H

