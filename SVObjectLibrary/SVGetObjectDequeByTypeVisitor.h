//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGetObjectDequeByTypeVisitor
//* .File Name       : $Workfile:   SVGetObjectDequeByTypeVisitor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:42:16  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <deque>
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"

class SVObjectClass;

class SVGetObjectDequeByTypeVisitor
{
public:
	typedef std::deque< SVObjectClass* > SVObjectPtrDeque;

	SVGetObjectDequeByTypeVisitor( const SVObjectTypeInfoStruct& p_rObjectInfo );
	virtual ~SVGetObjectDequeByTypeVisitor();

	virtual HRESULT VisitElement( SVObjectClass& p_rElement );

	const SVObjectPtrDeque& GetObjects() const;

protected:
	SVObjectTypeInfoStruct m_ObjectInfo;
	SVObjectPtrDeque m_Objects;

private:
	SVGetObjectDequeByTypeVisitor();

};

