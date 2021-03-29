//******************************************************************************
/// \copyright (c) 2018 by Seidenader Maschinenbau GmbH
/// \file SVO-Enum.h
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
#pragma warning( disable : 5054 )
#include "SVO-Enum.pb.h"
#pragma warning( pop )

inline constexpr SvPb::EmbeddedIdEnum operator+(SvPb::EmbeddedIdEnum lhs, int rhs)
{
	return static_cast<SvPb::EmbeddedIdEnum>(static_cast<int>(lhs) + rhs);
}

inline constexpr SvPb::EmbeddedIdEnum operator+(SvPb::EmbeddedIdEnum lhs, long rhs)
{
	return static_cast<SvPb::EmbeddedIdEnum>(static_cast<int>(lhs) + rhs);
}

inline constexpr SvPb::EmbeddedIdEnum operator+(SvPb::EmbeddedIdEnum lhs, size_t rhs)
{
	return static_cast<SvPb::EmbeddedIdEnum>(static_cast<size_t>(lhs) + rhs);
}