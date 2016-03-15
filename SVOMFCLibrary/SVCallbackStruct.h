// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackStruct
// * .File Name       : $Workfile:   SVCallbackStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:01:00  $
// ******************************************************************************

#ifndef _SVCALLBACKSTRUCT_H
#define _SVCALLBACKSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif

typedef HRESULT ( CALLBACK *SVCallbackPtr )( void *,   /* Owner */
                                             void * ); /* Data */ 

struct SVCallbackStruct  
{
	SVCallbackStruct();
	SVCallbackStruct( const SVCallbackStruct &p_rOriginal );
	virtual ~SVCallbackStruct();

	const SVCallbackStruct &operator=( const SVCallbackStruct &p_rOriginal );

	SVCallbackPtr m_pCallback;

	void *m_pOwner;
	void *m_pData;
};

bool operator==( const SVCallbackStruct &p_rLeft,
                 const SVCallbackStruct &p_rRight );

#endif // _SVCALLBACKSTRUCT_H

