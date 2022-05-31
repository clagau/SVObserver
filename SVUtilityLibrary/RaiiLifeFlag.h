//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file RaiiLifeFlag.h
//*****************************************************************************
/// RAII flag to set and reset a bool flag
//******************************************************************************
#pragma once

namespace SvDef
{
#pragma region Declarations
//RAII class to set and reset flag
struct RaiiLifeFlag
{
	explicit RaiiLifeFlag(bool& flag) :
		m_rFlag(flag)
	{
		m_rFlag = true;
	}
	~RaiiLifeFlag()
	{
		m_rFlag = false;
	}
private:
	bool& m_rFlag;
};
#pragma endregion Declarations
} //namespace SvDef
