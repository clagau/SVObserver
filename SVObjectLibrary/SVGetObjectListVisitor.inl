//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGetObjectListVisitor
//* .File Name       : $Workfile:   SVGetObjectListVisitor.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:48  $
//******************************************************************************

template<typename Filter, typename Insertor>
SVGetObjectListVisitor<Filter, Insertor>::SVGetObjectListVisitor(Filter p_Allow, Insertor p_Insertor)
: m_Allow(p_Allow)
, m_insertor(p_Insertor)
{
}

template<typename Filter, typename Insertor>
SVGetObjectListVisitor<Filter, Insertor>::~SVGetObjectListVisitor()
{
}

template<typename Filter, typename Insertor>
HRESULT SVGetObjectListVisitor<Filter, Insertor>::VisitElement(SVObjectClass& p_rElement)
{
	if (m_Allow(p_rElement))
	{
		*m_insertor = p_rElement;
	}
	return S_OK;
}

