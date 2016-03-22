//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGetObjectListVisitor
//* .File Name       : $Workfile:   SVGetObjectListVisitor.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:42  $
//******************************************************************************

#pragma once

class SVObjectClass;

template<typename Filter, typename Insertor>
class SVGetObjectListVisitor
{
public:
	SVGetObjectListVisitor(Filter p_Allow, Insertor p_Insertor );
	~SVGetObjectListVisitor();

	HRESULT VisitElement( SVObjectClass& p_rElement );

private:
	Filter m_Allow;
	Insertor m_insertor;
};

#include "SVGetObjectListVisitor.inl"

