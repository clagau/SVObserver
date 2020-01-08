//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file PlcDataVersion.h
//*****************************************************************************
/// The header of the PLC data version
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#pragma endregion Includes

namespace SvPlc
{
#pragma pack(push, 1)
struct PlcDataVersion
{
public:
	PlcDataVersion() = default;
	~PlcDataVersion() = default;
	explicit PlcDataVersion(const PlcDataVersion& rRhs) = default;
	explicit PlcDataVersion(PlcDataVersion&& rRhs) = default;
	PlcDataVersion& operator=(const PlcDataVersion& rRhs) = default;
	PlcDataVersion& operator=(PlcDataVersion&& rRhs) = default;

#pragma region Member Variables
public:
	uint8_t m_major {0};				//Major version number
	uint8_t m_minor {0};				//Minor version number
#pragma endregion Member Variables
};
#pragma pack(pop)

} //namespace SvPlc
