//******************************************************************************
/// \copyright (c) 2018 by Seidenader Maschinenbau GmbH
/// \file Overlay.h
/// All Rights Reserved
//******************************************************************************
/// Include-wrapper to simplify suppressing compiler warnings.
//******************************************************************************

#pragma once

#include <comutil.h>
#pragma warning( push )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4244 )
#include "Overlay.pb.h"
#pragma warning( pop )


namespace SvPb
{
	template <class T>
	void setValueObject(const T& rSVOValueObject, ValueObject& rPbValueObject, bool setTrPos=false)
	{
		rPbValueObject.set_objectid(rSVOValueObject.getObjectId());
		double var = 0;
		rSVOValueObject.getValue(var);
		rPbValueObject.set_value(var);
		if (setTrPos)
		{
			rPbValueObject.set_trpos(rSVOValueObject.getTrPos() + 1);
		}
	}
}