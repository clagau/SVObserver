//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeFeatureStringEnum
//* .File Name       : $Workfile:   SVGigeFeatureStringEnumList.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:55:02  $
//******************************************************************************

#include "SVGigeFeatureStringEnumList.h"

template< typename _Iter>
SVGigeFeatureStringEnumList::SVGigeFeatureStringEnumList(_Iter first, _Iter last)
{
	for (; first != last; ++first)
		m_stringEnums.insert(*first);
}

