//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************
#pragma once

namespace SvUl
{
	/*
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	#define SetBits(nData, bits, bOn) if (bOn) (nData) |= (bits); else (nData) &= ~(bits);
	// sets the specified bits of the input value to either on or off
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	*/
	template <typename DATATYPE, typename BITSTYPE>
	inline void SetBits(DATATYPE& rDest, BITSTYPE bits, bool bOnOff)
	{
		if (bOnOff)
		{
			rDest |= bits;
		}
		else
		{
			rDest &= ~bits;
		}
	}
} // namespace SvUl
