//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : runrejctsvr
//* .File Name       : $Workfile:   runrejctsvr.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.16  $
//* .Check In Date   : $Date:   28 Oct 2014 10:45:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <conio.h>
//Moved to precompiled header: #include <Psapi.h>
//Moved to precompiled header: #include <tlhelp32.h>
//Moved to precompiled header: #include <process.h>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <list>
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/scoped_array.hpp>
//Moved to precompiled header: #include <boost/any.hpp>
//Moved to precompiled header: #include <boost/multi_index_container.hpp>				//Used by static library
//Moved to precompiled header: #include <boost/multi_index/member.hpp>					//Used by static library
//Moved to precompiled header:  #include <boost/multi_index/ordered_index.hpp>			//Used by static library

#include "SVSocketLibrary\SVSocketLibrary.h"
#include "SVSocketLibrary\SVServerSocket.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVRemoteControlConstants.h"
#include "JsonLib\include\json.h"
#include "SVSharedMemoryLibrary\SVMonitorListReader.h"
#include "SVSharedMemoryLibrary\SVSharedConfiguration.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "RunRejectService.h"
#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "SVSharedMemoryLibrary\SVSharedMonitorList.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"
#include "RRSVersionString.h"
#include "SVSharedMemoryLibrary\MLCpyContainer.h"
#include "SVSharedMemoryLibrary\SharedMemReader.h"
#include <mil.h>
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVSharedMemoryLibrary\MLProduct.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma endregion Declarations

const u_short imgPort = 28963;
const DWORD Timeout_Thread_Functions = 0;

template<typename API>
struct port;

template<>
struct port<SvSol::TcpApi>
{
	enum : u_short { number = 28965 };
};

template<>
struct port<SvSol::UdpApi>
{
	enum : u_short { number = 28967 };
};

typedef Json::Value JsonCmd;
typedef SvSol::SVServerSocket<SvSol::UdpApi> UdpServerSocket;
typedef SvSol::SVSocket<SvSol::UdpApi> UdpSocket;
typedef SvSol::SVServerSocket<SvSol::TcpApi> TcpServerSocket;
typedef SvSol::SVSocket<SvSol::TcpApi> TcpSocket;
typedef void(*SignalHandlerPointer)(int);


/// For last held product from GetProduct map with monitorlistname
///Map with monitorlistname 
static std::map<SVString, SvSml::productPointer> g_LastProduct;
static std::map<SVString, SvSml::productPointer> g_LastReject;
static std::map<SVString, SvSml::FailstatusPointer> g_LastFailstatus;
template<typename API>
void ClearHeld();

template<>
void ClearHeld<SvSol::TcpApi>()
{
	g_LastFailstatus.clear();
	g_LastReject.clear();
}

template<>
void ClearHeld<SvSol::UdpApi>()
{
	g_LastProduct.clear();
}

SignalHandlerPointer previousHandler(nullptr);

static DWORD g_threadIds[3] = { 0 };
static HANDLE g_threads[3] = { 0 };
static SvSml::SharedMemReader   g_MemReader;

bool EventHandler(DWORD event)
{
	bool res(false);
	switch (event)
	{
	case SvSml::ShareEvents::Change:
		try
		{
			Sleep(SvSml::ShareEvents::Delay_Before_ClearShare);
			g_MemReader.Clear();
			res = true;
		}
		catch (std::exception& rExp)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			SVStringVector msgList;
			msgList.push_back(rExp.what());
			Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
		break;
	case SvSml::ShareEvents::Ready:
	{
		try
		{
			DWORD version = SvSml::ShareEvents::GetInstance().GetReadyCounter();
			g_MemReader.Reload(version);
			ClearHeld<SvSol::TcpApi>();
			ClearHeld<SvSol::UdpApi>();
			res = true;
		}
		catch (std::exception& rExp)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			SVStringVector msgList;
			msgList.push_back(rExp.what());
			Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
		break;
	}
	default:
		break;
	}
	return res;
}


void AccessViolationHandler(int signal)
{
	SvSml::SVSharedConfiguration::Log("Access Violation");
	throw std::exception("Access Violation");
}

void  ScanImageFileName(LPCTSTR IndexString, DWORD& ImageIndex, DWORD& ImageStoreIndex, DWORD& SlotIndex)
{
	_stscanf(IndexString, _T("%u %u  %u"), &ImageStoreIndex, &ImageIndex, &SlotIndex);
}

DWORD WINAPI servimg(LPVOID ptr)
{
	try
	{
		const DWORD DefaultImageBufferSize = 0xA00000;
		const DWORD SocketBufferSize = 0x100000;

		SVByteVector ByteVector;
		ByteVector.reserve(DefaultImageBufferSize);

		SvSml::SharedMemReader*  pSharedMemReader = static_cast<SvSml::SharedMemReader*>(ptr);
		UdpServerSocket sok;
		sok.Create();
		sok.SetBufferSize(SocketBufferSize);
		sok.Listen(imgPort);
		long cnt = 0;
		while (WAIT_OBJECT_0 != WaitForSingleObject(g_ServiceStopEvent, Timeout_Thread_Functions))
		{
			if (sok.ClientConnecting())
			{
				sockaddr addr;
				int len = sizeof(sockaddr);
				UdpSocket client = sok.Accept(&addr, &len); // this never blocks for udp...
				if (client.IsValidSocket())
				{
					// read file name
					bytes buf;
					u_long sz;
					if (SvSol::SVSocketError::Success == client.ReadAll(buf, sz, true))
					{
						try
						{
							bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
							if (isready)
							{
								SVString filename = SVString(reinterpret_cast<char *>(buf.get()), sz);
								DWORD ImageIndex, ImageStoreIndex, SlotIndex;
								ScanImageFileName(filename.c_str(), ImageIndex, ImageStoreIndex, SlotIndex);
								DWORD offset = sizeof(SvSol::header);
								SVMatroxBufferInterface::CopyBufferToFileDIB(ByteVector, pSharedMemReader->m_DataContainer.GetImageBuffer(SlotIndex, ImageStoreIndex, ImageIndex), offset, true);
								client.WriteWithHeader(&ByteVector[0], ByteVector.size());
							}
						}
						catch (const SvStl::MessageContainer& rExp)
						{
							SvStl::MessageMgrStd e(SvStl::LogOnly);
							e.setMessage(rExp.getMessage());
						}
						catch (std::exception& rExp)
						{
							SvStl::MessageMgrStd Exception(SvStl::LogOnly);
							SVStringVector msgList;
							msgList.push_back(rExp.what());
							Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
						}
					}
				}
				else
				{
					SvStl::MessageMgrStd Exception(SvStl::LogOnly);
					SVStringVector msgList;
					msgList.push_back(SVString(SvSol::SVSocketError::GetErrorText(SvSol::SVSocketError::GetLastSocketError())));
					Exception.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_SocketInvalid, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
			}
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(rExp.getMessage());
	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		SVStringVector msgList;
		msgList.push_back(rExp.what());
		Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}

	return 0;
}

JsonCmd ReadCommand(TcpSocket & sok)
{
	Json::Value val(Json::nullValue);
	u_long buflen = 0;
	bytes arr;
	// read command from socket
	SvSol::SVSocketError::ErrorEnum error = SvSol::SVSocketError::Success;
	if (SvSol::SVSocketError::Success == (error = sok.ReadAll(arr, buflen, false)))
	{
		if (arr.get())
		{
			std::string cmd = reinterpret_cast<char *>(arr.get());
			Json::Reader reader;
			reader.parse(cmd, val, false); // don't care for the actual command for now
		}
	}
	else
	{
		if (SvSol::SVSocketError::ConnectionReset == error)
		{
			sok.Destroy();
		}
#if defined (TRACE_FAILURE) || defined (TRACE_OTHER)
		::OutputDebugStringA(SvSol::SVSocketError::GetErrorText(error));
		::OutputDebugStringA("\n");
#endif
	}
	return val;
}

JsonCmd ReadCommand(UdpSocket & sok)
{
	Json::Value val(Json::nullValue);
	bytes buff;
	u_long sz;
	if (SvSol::SVSocketError::Success == sok.ReadAll(buff, sz, true))
	{
		const char * beg = reinterpret_cast<char *>(buff.get());
		const char * end = beg + sz;
		Json::Reader reader;
		reader.parse(beg, end, val, false);
	}
	return val;
}

//for performance reason Bytevector is static. 
//Function must not be called from more then one Thread
SVString EncodeImg(SVMatroxBuffer& rMatroxBuffer)
{
	static SVByteVector G_ByteVector;
	SVMatroxBufferInterface::CopyBufferToFileDIB(G_ByteVector, rMatroxBuffer);
	DWORD len = static_cast<DWORD>(G_ByteVector.size());
	int enc_len = ::Base64EncodeGetRequiredLength(len, ATL_BASE64_FLAG_NOCRLF);
	boost::scoped_array<char>  enc_buff(new char[enc_len + 1]);
	enc_buff[enc_len] = '\0';
	::Base64Encode(&(G_ByteVector[0]), len, enc_buff.get(), &enc_len, ATL_BASE64_FLAG_NOCRLF);
	return enc_buff.get();
}

Json::Value NewResponse(const JsonCmd & cmd)
{
	Json::Value rsp(Json::objectValue);
	if (cmd.isObject() && cmd.isMember(SVRC::cmd::id) && cmd.isMember(SVRC::cmd::name))
	{
		rsp[SVRC::cmd::id] = cmd[SVRC::cmd::id];
		rsp[SVRC::cmd::name] = cmd[SVRC::cmd::name];
		rsp[SVRC::cmd::hr] = S_OK;
	}
	else
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString(cmd[SVRC::cmd::name].asString().c_str()));
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	return rsp;
}


Json::Value& GenerateValues(Json::Value & rarr, SvSml::MLProduct* pProduct)
{
	if (nullptr == pProduct)
		return rarr;
	for (int i = 0; i < pProduct->m_data.size(); i++)
	{
		Json::Value val(Json::objectValue);
		val[SVRC::vo::name] = pProduct->m_dataEntries[i]->name.c_str();
		val[SVRC::vo::status] = (int)pProduct->m_status;
		val[SVRC::vo::count] = (int)pProduct->m_trigger;
		Json::Value value(Json::arrayValue);
		value.append(pProduct->m_data[i]->c_str());
		val[SVRC::vo::array] = value;
		rarr.append(val);
	}
	return rarr;
}
Json::Value & GenerateErrors(Json::Value & arr, SvSml::MLProduct* pProduct)
{
	return arr;
}

template<typename API>
Json::Value & GenerateImages(Json::Value & rArr, const SvSml::MLProduct* pProduct, SvSml::SharedMemReader  *pMemReader);

template<>
Json::Value & GenerateImages<SvSol::UdpApi>(Json::Value & rArr, const SvSml::MLProduct* pProduct, SvSml::SharedMemReader  *pMemReader)
{
	if (nullptr == pProduct)
		return rArr;
	int count = pProduct->m_trigger;
	int slot = pProduct->m_slot;
	for (auto& MeP : pProduct->m_ImageEntries)
	{
		if (!MeP.get())
			break;
		int StoreIndex = MeP->data.InspectionStoreId;
		int ImageIndex = MeP->data.ItemId;
		SVString filename = SvUl::StringFunctions::Format(_T("%i %i  %i"), StoreIndex, ImageIndex, slot);
		Json::Value val(Json::objectValue);
		val[SVRC::vo::name] = MeP->name.c_str();
		val[SVRC::vo::status] = pProduct->m_status;
		val[SVRC::vo::count] = count;
		val[SVRC::io::imageFileName] = filename;
		val[SVRC::io::fetch] = 1;
		rArr.append(val);
	}
	return rArr;
}

template<>
Json::Value & GenerateImages<SvSol::TcpApi>(Json::Value & rArr, const SvSml::MLProduct* pProd, SvSml::SharedMemReader  *pMemReader)
{
	if (nullptr == pProd)
		return rArr;

	int count = pProd->m_trigger;
	int slot = pProd->m_slot;
	for (auto& MeP : pProd->m_ImageEntries)
	{
		if (!MeP.get())
			break;
		int StoreIndex = MeP->data.InspectionStoreId;
		int ImageIndex = MeP->data.ItemId;
		
		Json::Value val(Json::objectValue);
		val[SVRC::vo::name] = MeP->name.c_str();
		val[SVRC::vo::status] = pProd->m_status;
		val[SVRC::vo::count] = count;
		val[SVRC::io::image] = EncodeImg(pMemReader->GetImageBuffer(slot, StoreIndex, ImageIndex));
		rArr.append(val);
	}
	return rArr;
}

template<typename API>
Json::Value WriteProduct(SvSml::MLProduct* pProduct, SvSml::SharedMemReader  *pMemReader)
{
	Json::Value rslt(Json::objectValue);
	Json::Value values(Json::arrayValue);
	Json::Value images(Json::arrayValue);
	Json::Value errors(Json::arrayValue);
	GenerateValues(values, pProduct);
	GenerateImages<API>(images, pProduct, pMemReader);
	GenerateErrors(errors, pProduct);

	rslt[SVRC::result::values] = values;
	rslt[SVRC::result::images] = images;
	rslt[SVRC::result::errors] = errors;
	return rslt;
}
Json::Value WriteFailStatus(SvSml::Failstatus * pFailstatus)
{
	Json::Value rslt(Json::objectValue);
	Json::Value failList(Json::arrayValue);
	if (nullptr == pFailstatus)
		return rslt;
	for (SvSml::productPointer& pProduct : *pFailstatus)
	{
		Json::Value voList(Json::objectValue);
		Json::Value entries(Json::arrayValue);
		if (pProduct.get())
		{
			for (int i = 0; i < pProduct->m_data.size(); i++)
			{
				Json::Value vo(Json::objectValue);
				vo[SVRC::vo::name] = pProduct->m_dataEntries[i]->name.c_str();
				vo[SVRC::vo::status] = (int)pProduct->m_status;
				vo[SVRC::vo::count] = (int)pProduct->m_trigger;
				Json::Value valueArray(Json::arrayValue);
				valueArray.append(pProduct->m_data[i]->c_str());
				vo[SVRC::vo::array] = valueArray;
				entries.append(vo);
			}
		}
		voList[SVRC::result::items] = 0; // workaround for a svrc bug 
		voList[SVRC::result::entries] = entries;
		failList.append(voList);
	}
	rslt[SVRC::result::values] = failList;
	return rslt;
}

template<typename API>
Json::Value DispatchCommand(const JsonCmd & cmd, SvSml::SharedMemReader* pMemReader);

//FOR TCAPI
Json::Value  DispatchCmdGetFailstatus(const JsonCmd & cmd, SvSml::SharedMemReader  *pMemReader)
{
	Json::Value rslt(Json::objectValue);
	const Json::Value &args = cmd[SVRC::cmd::arg];
	SVString listName;
	bool validArgs(false);
	if (args.isMember(SVRC::arg::listName))
	{
		listName = args[SVRC::arg::listName].asString();
		validArgs = pMemReader->IsActiveMonitorList(listName);
	}
	if (validArgs == false)
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString(listName.c_str()));
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidMonitorlist, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	SvSml::Failstatus* pLastFailstatus(nullptr);
	SvSml::FailstatusPointer  new_FailstatusPtr(new SvSml::Failstatus);
	auto& it = g_LastFailstatus.find(listName);
	if (it != g_LastFailstatus.end())
	{
		pLastFailstatus = it->second.get();
	}

	SvSml::SharedMemReader::retvalues ret =
		pMemReader->GetFailstatus(listName.c_str(), new_FailstatusPtr.get(), pLastFailstatus);

	switch (ret)
	{
	case SvSml::SharedMemReader::sucess:
	{
		rslt = WriteFailStatus(new_FailstatusPtr.get());
		g_LastFailstatus[listName] = std::move(new_FailstatusPtr);
	}
	break;
	case SvSml::SharedMemReader::last:
	{
		rslt = WriteFailStatus(pLastFailstatus);

	}
	break;
	case SvSml::SharedMemReader::fail:
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_RRS_4_GENERAL_WARNING, SvStl::Tid_FailstatusNotFound, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	break;
	}
	return rslt;
}


//FOR TCAPI
Json::Value  DispatchCmdGetReject(const JsonCmd & cmd, SvSml::SharedMemReader  *pMemReader)
{
	Json::Value rslt(Json::objectValue);
	const Json::Value &args = cmd[SVRC::cmd::arg];
	SVString listName;
	bool validArgs(false);
	if (args.isMember(SVRC::arg::listName))
	{
		listName = args[SVRC::arg::listName].asString();
		validArgs = pMemReader->IsActiveMonitorList(listName);
	}
	if (validArgs == false)
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString(listName.c_str()));
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidMonitorlist, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	int trig = -1;
	if (args.isMember(SVRC::arg::trgrCount))
	{
		trig = args[SVRC::arg::trgrCount].asInt();
	}

	SvSml::MLProduct* pLastReject(nullptr);
	SvSml::productPointer new_productPtr(new SvSml::MLProduct());
	auto& it = g_LastReject.find(listName);
	if (it != g_LastReject.end())
	{
		pLastReject = it->second.get();
	}

	SvSml::SharedMemReader::retvalues ret =	pMemReader->GetRejectData(listName.c_str(), trig, new_productPtr.get(), pLastReject, false);
	switch (ret)
	{
	case SvSml::SharedMemReader::sucess:
	{
		DWORD index = pMemReader->GetSlotManagerIndexForMonitorList(listName.c_str());
		if (nullptr != pLastReject)
		{
			///release Readerslot for last reject
			if (index < 0)
			{
				SvStl::MessageContainer MsgCont;
				MsgCont.setMessage(SVMSG_RRS_4_GENERAL_WARNING, SvStl::Tid_RejectNotFound, SvStl::SourceFileParams(StdMessageParams));
				throw MsgCont;
			}
			pMemReader->GetSlotManager(index)->ReleaseReaderSlot(pLastReject->m_slot);
		}

		rslt = WriteProduct<SvSol::TcpApi>(new_productPtr.get(), pMemReader);
		///now the last is the newone
		g_LastReject[listName] = std::move(new_productPtr);
	}
	break;
	case SvSml::SharedMemReader::last:
	{
		rslt = WriteProduct<SvSol::TcpApi>(pLastReject, pMemReader);

	}
	break;
	case SvSml::SharedMemReader::fail:
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_RRS_4_GENERAL_WARNING, SvStl::Tid_ProductNotFound, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	break;
	}
	return rslt;
}


template<>
Json::Value DispatchCommand<SvSol::TcpApi>(const JsonCmd & cmd, SvSml::SharedMemReader  *pMemReader)
{
	Json::Value rslt(Json::objectValue);
	if (false == cmd.isObject() || false == cmd.isMember(SVRC::cmd::name))
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString(cmd[SVRC::cmd::name].asString().c_str()));
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}

	if (cmd[SVRC::cmd::name] == SVRC::cmdName::getFail)
	{
		return DispatchCmdGetFailstatus(cmd, pMemReader);

	}
	else if (cmd[SVRC::cmd::name] == SVRC::cmdName::getRjct)
	{
		return DispatchCmdGetReject(cmd, pMemReader);
	}
	else if (cmd[SVRC::cmd::name] == SVRC::cmdName::getVersion)
	{
		Json::Value rslt(Json::objectValue);
		rslt[SVRC::result::SVO_ver] = RRSVersionString::Get().c_str();
		return rslt;
	}
	else
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString());
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	return rslt;
}

template<>
Json::Value DispatchCommand<SvSol::UdpApi>(const JsonCmd & cmd, SvSml::SharedMemReader*  pMemReader)
{
	Json::Value rslt(Json::objectValue);
	if (false == cmd.isObject() || false == cmd.isMember(SVRC::cmd::name))
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString());
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}

	if (cmd[SVRC::cmd::name] != SVRC::cmdName::getProd)
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString(cmd[SVRC::cmd::name].asString().c_str()));
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}

	const Json::Value & args = cmd[SVRC::cmd::arg];
	SVString listName;
	bool validArgs(false);

	if (args.isMember(SVRC::arg::listName))
	{
		listName = args[SVRC::arg::listName].asString();
		validArgs = pMemReader->IsActiveMonitorList(listName);
	}
	if (validArgs == false)
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back(SVString(listName.c_str()));
		MsgCont.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_InvalidMonitorlist, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	long trig = -1;
	if (args.isMember(SVRC::arg::trgrCount))
	{
		trig = args[SVRC::arg::trgrCount].asInt();
	}

	///cmd GetProduct 
	SvSml::MLProduct* pLastProduct(nullptr);
	SvSml::productPointer new_productPtr(new SvSml::MLProduct());
	auto& it = g_LastProduct.find(listName);
	if (it != g_LastProduct.end())
	{
		pLastProduct = it->second.get();
	}

	SvSml::SharedMemReader::retvalues ret =
		pMemReader->GetProductData(listName.c_str(), trig, new_productPtr.get(), pLastProduct, false);
	switch (ret)
	{
	case SvSml::SharedMemReader::sucess:
	{
		DWORD index = pMemReader->GetSlotManagerIndexForMonitorList(listName.c_str());
		if (nullptr != pLastProduct)
		{
			///release Readerslot for last product
			if (index < 0)
			{
				SvStl::MessageContainer MsgCont;
				MsgCont.setMessage(SVMSG_RRS_4_GENERAL_WARNING, SvStl::Tid_ProductNotFound, SvStl::SourceFileParams(StdMessageParams));
				throw MsgCont;
			}
			pMemReader->GetSlotManager(index)->ReleaseReaderSlot(pLastProduct->m_slot);
		}
		///the last become the new one 
		rslt = WriteProduct<SvSol::UdpApi>(new_productPtr.get(), pMemReader);
		g_LastProduct[listName] = std::move(new_productPtr);
	}
	break;
	case SvSml::SharedMemReader::last:
	{
		rslt = WriteProduct<SvSol::UdpApi>(pLastProduct, pMemReader);

	}
	break;
	case SvSml::SharedMemReader::fail:
	{
		SvStl::MessageContainer MsgCont;
		MsgCont.setMessage(SVMSG_RRS_4_GENERAL_WARNING, SvStl::Tid_ProductNotFound, SvStl::SourceFileParams(StdMessageParams));
		throw MsgCont;
	}
	break;
	}
	return rslt;
}

template<typename API>
std::string GenerateResponse(const JsonCmd & cmd, SvSml::SharedMemReader* pMemReader)
{
	Json::Value rsp = NewResponse(cmd);
	try
	{
		const Json::Value& value = DispatchCommand<API>(cmd, pMemReader);
		if (cmd[SVRC::cmd::name] != SVRC::cmdName::getVersion)
		{
			Json::Value rslt(Json::objectValue);
			rslt[SVRC::result::items] = value;
			rsp[SVRC::cmd::reslts] = rslt;
		}
		else
		{
			rsp[SVRC::cmd::reslts] = value;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		rsp[SVRC::cmd::err] = rExp.what();
		rsp[SVRC::cmd::hr] = Json::Value(E_FAIL);

		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(rExp.getMessage());
	}
	catch (std::exception& rExp)
	{
		rsp[SVRC::cmd::err] = rExp.what();
		rsp[SVRC::cmd::hr] = Json::Value(E_FAIL);

		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		SVStringVector msgList;
		msgList.push_back(rExp.what());
		Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	Json::FastWriter writer;
	return SVString(writer.write(rsp));
}

SVString BusyMessage(const JsonCmd & cmd)
{
	Json::Value rsp = NewResponse(cmd);
	rsp[SVRC::cmd::hr] = STG_E_INUSE;
	rsp[SVRC::cmd::err] = "Server busy.";
	Json::FastWriter writer;
	return SVString(writer.write(rsp));
}


template<typename API, typename ServerSocket>
void Handler(ServerSocket& sok, SvSml::SharedMemReader* pSharedMemReader);

template <>
void Handler<SvSol::UdpApi, UdpServerSocket>(UdpServerSocket& sok, SvSml::SharedMemReader* pMemRead)
{
	typedef SvSol::UdpApi API;
	typedef UdpSocket Socket;
	static long lastReady = -1;
	static long lastFilterChange = -1;
	sockaddr addr;
	int len = sizeof(sockaddr);
	Socket client = sok.Accept(&addr, &len); // for UDP this never blocks
	if (FALSE == client.IsValidSocket())
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		SVStringVector msgList;
		msgList.push_back(SVString(SvSol::SVSocketError::GetErrorText(SvSol::SVSocketError::GetLastSocketError())));
		Exception.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_SocketInvalid, msgList, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	try
	{
		const JsonCmd& cmd = ReadCommand(client);
		if (!cmd.isNull())
		{
			bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();
			if (isready)
			{
				SVString rResponse = GenerateResponse<API>(cmd, pMemRead);
				client.Write(rResponse, SvSol::Traits<API>::needsHeader);
			}
			else
			{
				client.Write(BusyMessage(cmd), SvSol::Traits<API>::needsHeader);
			}

		}
		else
		{
			std::cout << "null cmd \n";
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(rExp.getMessage());
	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		SVStringVector msgList;
		msgList.push_back(rExp.what());
		Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}


}

template<>
void Handler<SvSol::TcpApi, TcpServerSocket>(TcpServerSocket& sok, SvSml::SharedMemReader* pMemRead)
{
	typedef SvSol::TcpApi API;
	typedef TcpSocket Socket;
	static long lastReady = -1;
	static long lastFilterChange = -1;
	sockaddr addr;
	int len = sizeof(sockaddr);
	SOCKET s = sok.Accept(&addr, &len); // this blocks until a client tries to connect
	Socket client((Socket::InvalidSock != s), true, s); // create a socket
	if (client.IsValidSocket())
	{
		client.DisableDelay(); // turn off nagle
	}

	while (WAIT_OBJECT_0 != WaitForSingleObject(g_ServiceStopEvent, Timeout_Thread_Functions) && client.IsValidSocket() && client.IsConnected())
	{
		if (client.DataAvailable())
		{
			try
			{
				const JsonCmd& cmd = ReadCommand(client);
				if (!cmd.isNull())
				{
					bool isready = SvSml::ShareEvents::GetInstance().GetIsReady();

					if (isready)
					{

						SVString  rResponse = GenerateResponse<API>(cmd, pMemRead);
						client.Write(rResponse, SvSol::Traits<API>::needsHeader);
						break;

					}
					else
					{
						// check for version command
						if (cmd[SVRC::cmd::name] == SVRC::cmdName::getVersion)
						{
							SVString rResponse = GenerateResponse<API>(cmd, pMemRead);
							client.Write(rResponse, SvSol::Traits<API>::needsHeader);
						}
						else
						{
							client.Write(BusyMessage(cmd), SvSol::Traits<API>::needsHeader);
						}
					}

				}
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvStl::MessageMgrStd e(SvStl::LogOnly);
				e.setMessage(rExp.getMessage());
			}
			catch (std::exception & rExp)
			{
				SvStl::MessageMgrStd Exception(SvStl::LogOnly);
				SVStringVector msgList;
				msgList.push_back(rExp.what());
				Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
}

template <typename API>
DWORD WINAPI servcmd(LPVOID ptr)
{
	try
	{
		typedef SvSol::SVServerSocket<API> ServerSocket;

		SvSml::SharedMemReader*  pMemRead = static_cast<SvSml::SharedMemReader*>(ptr);

		ServerSocket sok;
		sok.Create();
		sok.SetBlocking();
		sok.Listen(port<API>::number);
		while (WAIT_OBJECT_0 != WaitForSingleObject(g_ServiceStopEvent, Timeout_Thread_Functions))
		{
			if (sok.ClientConnecting())
			{
				Handler<API>(sok, pMemRead);
			}

		}
	}

	catch (const SvStl::MessageContainer& rExp)
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(rExp.getMessage());
	}
	catch (std::exception & rExp)
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		SVStringVector msgList;
		msgList.push_back(rExp.what());
		Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return 0;
}

bool CheckCommandLineArgs(int argc, _TCHAR* argv[], LPCTSTR option)
{
	bool bFound = false;
	if (argc > 1)
	{
		for (int i = 1; i < argc && !bFound; i++)
		{
			if (_tcsicmp(argv[i], option) == 0)
			{
				bFound = true;
			}
		}
	}
	return bFound;
}

// Command Line arguments: /nocheck
// /nocheck means to ignore the 2 GiG size requirement
void StartThreads(DWORD argc, LPTSTR  *argv)
{


	/// Allocate MilSystem
	MIL_ID AppId = MappAlloc(M_DEFAULT, M_NULL);
	if (AppId == M_NULL)
	{
		std::cout << "MapAlloc failed";
		return;
	}
	MappControl(M_ERROR, M_PRINT_DISABLE);

	// check command line args - if /nocheck is specified - ignore the < 2 Gig error
	bool bCheckSizeOverride = CheckCommandLineArgs(argc, argv, _T("/nocheck"));

	HRESULT hr = SvSml::SVSharedConfiguration::SharedResourcesOk();
	if (S_OK != hr)
	{
		SVString msg;
		if (STG_E_INSUFFICIENTMEMORY == hr)
		{
			if (!bCheckSizeOverride)
			{
				msg = _T("Shared Resources - not enough space available.\n");
			}
			else
			{
				hr = S_OK;
			}
		}
		else if (STG_E_PATHNOTFOUND == hr)
		{
			msg = _T("Shared Resources - Path not found.\n");
		}
		else
		{
			msg = _T("Shared Resources - missing or inadequate.\n");
		}
		if (S_OK != hr)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			::OutputDebugStringA(msg.c_str());
#endif
			std::cout << msg;
			return;
		}
	}
	try
	{
		SvSml::ShareEvents::GetInstance().SetCallbackFunction(EventHandler);
		SvSml::ShareEvents::GetInstance().StartWatch();
		previousHandler = signal(SIGSEGV, AccessViolationHandler);

		SVString title = _T("Run/Reject Server ");
		title += RRSVersionString::Get();
		SetConsoleTitle(title.c_str());
		SvSol::SVSocketLibrary::Init();

		g_threads[0] = CreateThread(nullptr, 0, servimg, LPVOID(&g_MemReader), 0, &g_threadIds[0]); ///TODO  
		g_threads[1] = CreateThread(nullptr, 0, servcmd<SvSol::TcpApi>, LPVOID(&g_MemReader), 0, &g_threadIds[1]);
		g_threads[2] = CreateThread(nullptr, 0, servcmd<SvSol::UdpApi>, LPVOID(&g_MemReader), 0, &g_threadIds[2]);

		// stop the threads...
		::WaitForMultipleObjects(3, g_threads, true, INFINITE);
		signal(SIGSEGV, previousHandler);

	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		SVStringVector msgList;
		msgList.push_back(rExp.what());
		Exception.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_FailedtoStart, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	SvSol::SVSocketLibrary::Destroy();
	MappFree(AppId);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int rc = 0;

	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_RRS_0_STARTED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	//Function pointer for starting the threads
	gp_StartThreads = &StartThreads;

	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ cServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ nullptr, nullptr}
	};

	if (!StartServiceCtrlDispatcher(ServiceTable))
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)	
		OutputDebugString(_T("StartServiceCtrlDispatcher returned error"));
#endif		
		rc = GetLastError();
		// running as console (for Debug?)
		if (ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == rc)
		{
			StartThreads(argc, argv);
		}
	}

	Exception.setMessage(SVMSG_RRS_1_STOPPED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	return rc;
}
