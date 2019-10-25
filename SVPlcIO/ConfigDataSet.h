//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file ConfigDataSet.h
//*****************************************************************************
/// The header of the configuration data set
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#pragma endregion Includes

#pragma pack(push, 1)
struct ConfigDataSet
{
public:
	ConfigDataSet() = default;
	ConfigDataSet(uint8_t mode, uint16_t dataType, uint16_t startByte, uint16_t byteSize)
		: m_mode{mode} , m_dataType{dataType} , m_startByte{startByte}, m_byteSize{byteSize} {};
	~ConfigDataSet() = default;
	explicit ConfigDataSet(const ConfigDataSet& rRhs) = default;
	explicit ConfigDataSet(ConfigDataSet&& rRhs) = default;
	ConfigDataSet& operator=(const ConfigDataSet& rRhs) = default;
	ConfigDataSet& operator=(ConfigDataSet&& rRhs) = default;

#pragma region Member Variables
public:
	uint8_t m_mode {0};						//Config data mode
	uint16_t m_dataType {0};				//The data type defined in the interface protocol
	uint16_t m_startByte {0};				//The byte start offset of the data
	uint16_t m_byteSize {0};				//The size of the data

#pragma endregion Member Variables
};

#pragma pack(pop)
