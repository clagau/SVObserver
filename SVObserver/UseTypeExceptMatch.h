//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : UseTypeExceptMatch
//* .File Name       : $Workfile:   UseTypeExceptMatch.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:32:22  $
//******************************************************************************

#pragma once

#include "SVLoki/TypeTraits.h"

// this is used to prevent a member function from being declared twice
// e.g. in class SVValueObjectClassImpl<double>, the 
//   GetArrayValues(int iBucket, std::vector<T>& raValues) is instantiated with double.
// therefore we don't want to have another function
//   GetArrayValues(int iBucket, std::vector<double>& raValues)
// this prevents that scenario by changing the function to
//   GetArrayValues(int iBucket, std::vector<Loki::Type2Type<double> >& raValues)
template <typename TYPE, typename EXCEPT> 
struct UseTypeExceptMatch
{
	typedef typename Loki::Select< Loki::IsEqualType<TYPE, EXCEPT>::value, Loki::Type2Type<TYPE>, TYPE >::Result type;
};

