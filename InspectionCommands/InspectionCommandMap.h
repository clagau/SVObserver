//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/09/04,2020/09/04 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file InspectionCommandMap.h
/// // InspectionCommandMap is a map with all commands, which are used by InspectionCommands()
/// The single commands can be defined by an functor  or by lambda function.
/// The map holds the information which function is called, in which context.
/// The map holds the message number and a tupel of three fuctionPointers
//******************************************************************************
#pragma once
namespace SvPb
{
	class InspectionCmdRequest;
	class InspectionCmdResponse;
}
namespace  SvCmd
{
	enum class ThreadPref;
	using  MessageCaseFktPtr = DWORD(*)();
	using ThreadPrefFktPtr = ThreadPref(*)();
	using  TimeoutFktPtr = std::chrono::seconds(*)();
	using  ExecuteFktPtr = SvPb::InspectionCmdResponse(*)(const SvPb::InspectionCmdRequest& rRequest);
	enum FunctionPtrSelect { ThFkt = 0, TiFkt = 1, ExFkt = 2 };

	using FktPtrs = std::tuple< ThreadPrefFktPtr, TimeoutFktPtr, ExecuteFktPtr>;
	extern std::map<DWORD, FktPtrs > InspectionCommandMap;

}


