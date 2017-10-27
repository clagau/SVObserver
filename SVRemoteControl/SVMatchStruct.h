//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatchStruct
//* .File Name       : $Workfile:   SVMatchStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:39:06  $
//******************************************************************************
#pragma once
#pragma region Includes
#include <deque>
#include <string>
#pragma endregion Includes


struct SVMatchStruct
{
	std::wstring matchString;
	long transactionID;

	SVMatchStruct(const std::wstring& p_MatchString, long p_TransactionID)
		: matchString(p_MatchString), transactionID(p_TransactionID) {}
};
typedef std::deque<SVMatchStruct> SVMatchList;

