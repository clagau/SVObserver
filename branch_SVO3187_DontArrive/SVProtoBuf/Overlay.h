//******************************************************************************
/// \copyright (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Overlay.h
/// All Rights Reserved
//******************************************************************************
/// Include-wrapper to simplify suppressing compiler warnings.
//******************************************************************************

#pragma once

#pragma warning( push )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4800 )
#pragma warning( disable : 5054 )
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

	unsigned char* generateBmpString(SvPb::OverlayShape& rShape, const RECT& rec);
	unsigned char* generateBmpString(SvPb::Image& image, long width, long height);
}