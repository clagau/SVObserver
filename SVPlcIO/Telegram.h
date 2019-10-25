//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file Telegram.h
//*****************************************************************************
/// The main telegram which is sent or received by the PLC
//******************************************************************************
#pragma once


#pragma region Includes
#include <stdint.h>
#pragma endregion Includes

#pragma region Declarations
constexpr uint8_t c_ReservedTelegram = 7;
#pragma endregion Declarations

#pragma pack(push, 1)

enum TelegramType : uint8_t
{
	NoneType = 0,
	LifeCheck = 1,
	Request = 2,
	Response = 3,
	Status = 4,
};

enum TelegramContent : uint8_t
{
	NoneContent = 0,
	VersionData = 1,
	ConfigurationData = 2,
	OperationData = 3,
	TimeSyncData = 4,
};

enum TelegramLayout : uint8_t
{
	NoneLayout = 0,
	Layout1 = 1,
	Layout2 = 2,
};

enum SystemStatus : uint8_t
{
	SystemDown = 0,
	Startup = 1,
	ComReady = 2,		//Communication ready
	AppReady = 3,		//Application ready
};

struct Telegram
{
public:
    Telegram() = default;
	~Telegram() = default;
	explicit Telegram(const Telegram& rRhs) = default;
	explicit Telegram(Telegram&& rRhs) = default;
	Telegram& operator=(const Telegram& rRhs) = default;
	Telegram& operator=(Telegram&& rRhs) = default;

#pragma region Member Variables
public:
	uint16_t m_contentID {0};								//Content ID is incremented every time it is sent
	uint16_t m_referenceID {0};								//Optional reference ID for the telegram
	TelegramType m_type {TelegramType::NoneType};			//Telegram type
	TelegramContent m_content {TelegramContent::NoneContent};//Telegram content
	TelegramLayout m_layout {TelegramLayout::NoneLayout};	//Telegram layout
	uint8_t m_error;										//Error number
	uint16_t m_errorInfoA;									//Addition info A to error
	uint16_t m_errorInfoB;									//Addition info B to error
	SystemStatus m_systemStatus {SystemStatus::SystemDown};	//System status
	uint8_t m_reserved[c_ReservedTelegram] {0, 0, 0, 0, 0, 0, 0};	//Reserved data
#pragma endregion Member Variables
};

#pragma pack(pop)
