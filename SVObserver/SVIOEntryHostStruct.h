//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vission, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryHostStruct
//* .File Name       : $Workfile:   SVIOEntryHostStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   27 Aug 2014 01:24:38  $
//******************************************************************************

#ifndef SVIOENTRYHOSTSTRUCT_H
#define SVIOENTRYHOSTSTRUCT_H

#include <deque>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"

class SVObjectClass;
class SVValueObjectClass;

enum SVIOObjectType
{
	IO_INVALID_OBJECT	= 0x0000,	// 0000 0000 0000 0000
	IO_DIGITAL_INPUT	= 0x0001,	// 0000 0000 0000 0001
	IO_REMOTE_INPUT		= 0x0004,	// 0000 0000 0000 0100
	IO_DIGITAL_OUTPUT	= 0x0008,	// 0000 0000 0000 1000
	IO_REMOTE_OUTPUT	= 0x0020,	// 0000 0000 0010 0000
	IO_CAMERA_DATA_INPUT = 0x8000,	// 1000 0000 0000 0000
};

struct SVIOEntryHostStruct 
{
	SVIOEntryHostStruct();
	virtual ~SVIOEntryHostStruct();

	void clear();

	bool m_Enabled;
	long m_PPQIndex;

	SVIOObjectType m_ObjectType;

	SVGUID m_IOId;

	bool m_DeleteValueObject;
	SVValueObjectClass* m_pValueObject;
	SVObjectClass* m_pValueParent;

	static bool PtrGreater( SVSharedPtr< SVIOEntryHostStruct > elem1, SVSharedPtr< SVIOEntryHostStruct > elem2 );

private:
	SVIOEntryHostStruct( const SVIOEntryHostStruct& p_rsvObject );

	const SVIOEntryHostStruct& operator=( const SVIOEntryHostStruct& p_rsvObject );
};

typedef SVSharedPtr< SVIOEntryHostStruct > SVIOEntryHostStructPtr;
typedef std::deque< SVIOEntryHostStructPtr > SVIOEntryHostStructPtrList;

#endif

