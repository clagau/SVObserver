//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************
#pragma once

namespace SvUl
{
template <typename T>
inline void SetBit(T& rDest, int bitNumber, bool state)
{
	constexpr cMaxBitNr {sizeof(T) * 8};
	if (bitNumber < cMaxBitNr)
	{
		T bitMask {1 << bitNumber};
		rDest = state ? (rDest | bitMask) : (rDest & ~bitMask);
	}
}

template <typename T>
inline bool GetBit(const T& rSource, int bitNumber)
{
	bool result {false};
	constexpr cMaxBitNr {sizeof(T) * 8};
	if (bitNumber < cMaxBitNr)
	{
		T bitMask {1 << bitNumber};
		result = (rSource & bitMask) == bitMask;
	}
}
} // namespace SvUl
