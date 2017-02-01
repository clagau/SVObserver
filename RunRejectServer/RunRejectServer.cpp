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
#include "SVSocketLibrary\SVSocketLibrary.h"
#include "SVSocketLibrary\SVServerSocket.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVRemoteControlConstants.h"
#include "JsonLib\include\json.h"
#include "SVSharedMemoryLibrary\SVSharedPPQReader.h"
#include "SVSharedMemoryLibrary\SVMonitorListReader.h"
#include "SVSharedMemoryLibrary\SVSharedConfiguration.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "RunRejectService.h"
#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "SVSharedMemoryLibrary\SVSharedMonitorList.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"
#pragma endregion Includes

#pragma region Declarations
DWORD threadIds[3] = {0};
HANDLE threads[3] = {0};

const u_short imgPort = 28963;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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



typedef std::pair<SvSml::ProductPtr, long> ProductPtrPair;

/// For last held product from GetProduct cmd. Map with ppqnames, (productPtr, Index) 
static std::map<SVString, ProductPtrPair> g_LastProductMap;  
/// For last held Reject from GetProduct cmd when product filter is lastReject.Map with ppqnames, (productPtr, Index) 
static std::map<SVString, ProductPtrPair> g_lastRejectProductMap;
// For last held Reject from GetReject cmd. Map with ppqnames, (productPtr, Index) 
static std::map<SVString, ProductPtrPair> g_lastRejectMap; 

typedef void (*SignalHandlerPointer)(int);
SignalHandlerPointer previousHandler(nullptr);

using SvSml::SVSharedConfiguration;

void AccessViolationHandler(int signal)
{
	SVSharedConfiguration::Log("Access Violation");
	throw std::exception("Access Violation");
}

DWORD WINAPI servimg(LPVOID)
{
	try
	{
		typedef UdpServerSocket ServerSocket;
		typedef UdpSocket Socket;
		typedef SvSol::header header;
		ServerSocket sok;
		sok.Create();
		sok.SetBufferSize(1024*1024);
		sok.Listen(imgPort);
		const size_t buflen = 1024*48;
		boost::scoped_array<u_char> arr(new u_char[buflen]);
		long cnt = 0;
		while( WAIT_OBJECT_0 != WaitForSingleObject(g_ServiceStopEvent, 0) )
		{
			if (sok.ClientConnecting())
			{
				sockaddr addr;
				u_char *buf = arr.get();
				int len = sizeof(sockaddr);
				Socket client = sok.Accept(&addr, &len); // this never blocks for udp...
				if (client.IsValidSocket())
				{
					// read file name
					bytes buf;
					u_long sz;
					if (SvSol::SVSocketError::Success == client.ReadAll(buf, sz, true))
					{
						SVString filename = SVString(reinterpret_cast<char *>(buf.get()), sz);
						try
						{
							std::ifstream is;
							is.open(filename, std::ios::in | std::ios::binary);
							if (!is.is_open())
							{
								SvStl::MessageContainer MsgCont;
								SVStringVector msgList;
								msgList.push_back( filename );
								MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_Error_CannotOpenFile, msgList, SvStl::SourceFileParams(StdMessageParams) );
								throw MsgCont;
							}
							is.seekg(0, std::ios::end);
							std::streamsize sz = is.tellg();
							is.seekg(0, std::ios::beg);
							std::vector<u_char> vec(static_cast<unsigned int>(sz));
							is.read(reinterpret_cast<char *>(&vec[0]), sz);
							client.Write(&vec[0], static_cast<size_t>(sz), true);
						}
						catch( const SvStl::MessageContainer& rExp )
						{
							SvStl::MessageMgrStd e( SvStl::LogOnly );
							e.setMessage( rExp.getMessage() );
						}
						catch(std::exception& rExp)
						{
							SvStl::MessageMgrStd Exception( SvStl::LogOnly );
							SVStringVector msgList;
							msgList.push_back( rExp.what() );
							Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
						}
					}
				}
				else
				{
					SvStl::MessageMgrStd Exception( SvStl::LogOnly );
					SVStringVector msgList;
					msgList.push_back( SVString( SvSol::SVSocketError::GetErrorText(SvSol::SVSocketError::GetLastSocketError() ) ) );
					Exception.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_SocketInvalid, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}
		}
	}
	catch( const SvStl::MessageContainer& rExp )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( rExp.getMessage() );
	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		SVStringVector msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	return 0;
}


SVString GetVersionString()
{
	SVString Result;

	TCHAR moduleFilename[512];
	::GetModuleFileName( nullptr, moduleFilename, static_cast<DWORD> (_tcslen(moduleFilename)) );

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSize( moduleFilename, &dwHandle );
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfo(moduleFilename, 0, size, lpData);
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = nullptr;
		UINT Len = 0;
		if (::VerQueryValueA(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&Len)) 
		{
			std::stringstream buf;

			buf << HIWORD(pFileInfo->dwFileVersionMS);
			buf << ".";
			buf << std::setfill('0') << std::setw(2) << LOWORD(pFileInfo->dwFileVersionMS);

			if( HIWORD(pFileInfo->dwFileVersionLS) < 255 )
			{
				buf << "b" << HIWORD(pFileInfo->dwFileVersionLS);
			}
			Result = buf.str();
		}
	}
	delete [] lpData;

#ifdef _DEBUG
	Result += "d";        // For debug builds.
#endif

	return Result;
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

SVString EncodeImg(const SVString& rName)
{
	HANDLE hFile = ::CreateFile( rName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
	if (hFile == INVALID_HANDLE_VALUE)
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back( rName );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_Error_CannotOpenFile, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}
	else
	{
		DWORD file_sz;
		file_sz = ::GetFileSize(hFile, nullptr);

		HANDLE hMapping = ::CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, file_sz, nullptr);
		BYTE * buff = (BYTE *)::MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, file_sz);
		int enc_len = ::Base64EncodeGetRequiredLength(file_sz, ATL_BASE64_FLAG_NOCRLF);
		boost::scoped_array<char>  enc_buff( new char[enc_len + 1]);
		enc_buff[enc_len] = '\0';
		::Base64Encode(buff, file_sz, enc_buff.get(), &enc_len, ATL_BASE64_FLAG_NOCRLF);
		::UnmapViewOfFile(buff);
		::CloseHandle(hMapping);
		::CloseHandle(hFile);
		return enc_buff.get();
	}
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
		msgList.push_back( SVString( cmd[SVRC::cmd::name].asString().c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}
	return rsp;
}

typedef SvSml::SVSharedInspectionReader SHMReader;

// the three following functions expect the Json array and add to it value/error/image objects respectively.
Json::Value & GenerateValues(Json::Value & rarr, SvSml::InspectionDataPtr data, const SvSml::MonitorEntryVector& rProductItemNames)
{
	int count = data->m_TriggerCount;
	for (SvSml::SVSharedValueContainer::const_iterator it = data->m_Values.begin(); it != data->m_Values.end(); ++it)
	{
		const SvSml::SVSharedValueContainer::value_type& sharedValue = *it;
		//@TODO[MEC][7.50][30.01.2017]
		// Is this lookup really necessary? 
		// The MonitorList should be in sync with SVObserver, so what is written to shared memory would be correct
		if(SvSml::SVSharedMonitorList::IsInMoListVector( sharedValue.m_ElementName.c_str(), rProductItemNames))
		{
			Json::Value val(Json::objectValue);
			val[SVRC::vo::name] = sharedValue.m_ElementName.c_str();
			val[SVRC::vo::status] = sharedValue.m_Status;
			val[SVRC::vo::count] = count;
			Json::Value value(Json::arrayValue);
			value.append(sharedValue.m_Result);
			val[SVRC::vo::array] = value;
			rarr.append(val);
		}
	}
	return rarr;
}

Json::Value & GenerateErrors(Json::Value & arr, const SvSml::InspectionDataPtr & data)
{
	return arr;
}

template<typename API>
void AppendImageContents( Json::Value& rValue, const SVString& rFilePath );

template<>
void AppendImageContents<SvSol::UdpApi>( Json::Value& rValue, const SVString& rFilePath )
{
	rValue[SVRC::io::imageFileName] = rFilePath;
	rValue[SVRC::io::fetch] = 1;
}

template<>
void AppendImageContents<SvSol::TcpApi>( Json::Value& rValue, const SVString& rFilePath )
{
	rValue[SVRC::io::image] = EncodeImg(rFilePath);
}

template<typename API>
Json::Value & GenerateImages(Json::Value & rArr, SvSml::InspectionDataPtr data, const SvSml::MonitorEntryVector& rProductItemNames)
{
	int count = data->m_TriggerCount;
	for (SvSml::SVSharedImageContainer::iterator it = data->m_Images.begin(); it != data->m_Images.end(); ++it)
	{
		const SvSml::SVSharedImageContainer::value_type& sharedImage = *it;
		
		//@TODO[MEC][7.50][30.01.2017] 
		// Is this lookup really necessary? 		
		// The MonitorList should be in sync with SVObserver, so what is written to shared memory would be correct
		
		if(SvSml::SVSharedMonitorList::IsInMoListVector( sharedImage.m_ElementName.c_str(), rProductItemNames))
		{
			Json::Value val(Json::objectValue);
			val[SVRC::io::name] = sharedImage.m_ElementName.c_str();
			val[SVRC::io::status] = sharedImage.m_Status;
			val[SVRC::io::count] = count;
			AppendImageContents<API>(val, sharedImage.m_Filename);
			rArr.append(val);
		}
	}
	return rArr;
}

template<typename API>
Json::Value WriteProductItems(SvSml::ProductPtr product, const SvSml::MonitorEntryVector& rProductItemNames)
{
	Json::Value rslt(Json::objectValue);
	Json::Value values(Json::arrayValue);
	Json::Value images(Json::arrayValue);
	Json::Value errors(Json::arrayValue);
	std::for_each(product->inspections.begin(), product->inspections.end(),
		[&values, &images, &errors, &rProductItemNames](std::pair<const SVString, SvSml::InspectionDataPtr>& pair) 
	{ 
		GenerateValues(values, pair.second, rProductItemNames); 
		GenerateImages<API>(images, pair.second, rProductItemNames); 
		GenerateErrors(errors, pair.second);
	}
	);
	rslt[SVRC::result::values] = values;
	rslt[SVRC::result::images] = images;
	rslt[SVRC::result::errors] = errors;
	return rslt;
}

template<typename API>
Json::Value DispatchCommand(const JsonCmd & cmd, const SvSml::MonitorListCpyMap & rmlMap);

Json::Value WriteFailStatus(const SvSml::FailStatusMap & fsMap)
{
	Json::Value rslt(Json::objectValue);
	Json::Value failList(Json::arrayValue);
	std::for_each(fsMap.begin(), fsMap.end(),
		[&failList](const SvSml::FailStatusMap::value_type & entry)
	{
		const SvSml::Values & values = entry.second;
		Json::Value voList(Json::objectValue);
		Json::Value entries(Json::arrayValue);
		std::for_each(values.begin(), values.end(),
			[&entries](const SvSml::Value & value)
		{
			Json::Value vo(Json::objectValue);
			vo[SVRC::vo::name] = value.name;
			vo[SVRC::vo::status] = value.status;
			vo[SVRC::vo::count] = value.trigger;
			Json::Value valueArray(Json::arrayValue);
			valueArray.append(value.value);
			vo[SVRC::vo::array] = valueArray;
			entries.append(vo);
		}
		);
		voList[SVRC::result::items] = 0; // workaround for a svrc bug, to be fixed in the next svrc beta/release
		voList[SVRC::result::entries] = entries;
		failList.append(voList);
	}
	);

	rslt[SVRC::result::values] = failList;
	return rslt;
}

/// for trig == -1 Get the values for the last saved product  and store the product in lastProductPtrPair
/// for trig  > -1 return  product  from lastProductPtrPair  if it has the tigger number trig
//! \param rReader [in] reader 
//! \param trig [in]   triggercount
//! \param productItemNames [in] Listof item names 
//! \param lastRejectProduct [in,out] last saved product.
//! \returns Json::Value
Json::Value GetLastInspectedProduct(SvSml::SVSharedPPQReader& rReader, long trig, const SvSml::MonitorEntryVector& rProductItemNames, ProductPtrPair& lastProductPtrPair)
{
	static Json::Value lastResult(Json::objectValue);
	Json::Value rslt(Json::objectValue);
	if (trig >= 0)
	{
		if (lastProductPtrPair.second < 0 || trig != lastProductPtrPair.first->product.m_TriggerCount)
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_RRS_4_GENERAL_WARNING, SvOi::Tid_ProductNotFound, SvStl::SourceFileParams(StdMessageParams) );
			throw MsgCont;
		}
		else
		{
			rslt = lastResult;
		}
	}
	else
	{
		long idx = -1;
		const SvSml::ProductPtr product = rReader.RequestNextProduct(idx);
		rslt = WriteProductItems<SvSol::UdpApi>(product, rProductItemNames);

		ProductPtrPair prevProduct = lastProductPtrPair;
		lastProductPtrPair = std::make_pair(product, idx);
		lastResult = rslt;
		if (prevProduct.second >= 0)
		{
			rReader.ReleaseProduct(prevProduct.first, prevProduct.second);
		}
	}
	return rslt;
}

template<typename API>
void ClearHeld();

template<>
void ClearHeld<SvSol::TcpApi>()
{
	g_lastRejectMap.clear();
}

template<>
void ClearHeld<SvSol::UdpApi>()
{
	g_LastProductMap.clear();
	g_lastRejectProductMap.clear();
}

//! for trig == -1 Get the values for the last saved reject   and store the product in lastRejectProduct
//! for trig  > -1 return  product  from lastRejectProduct  if it has the tigger number trig
//! \param rReader [in] reader 
//! \param trig [in]   triggercount
//! \param productItemNames [in] Listof item names 
//! \param lastRejectProduct [in,out] last saved product.
//! \returns Json::Value
template<typename API>
Json::Value GetRejectedProduct(SvSml::SVSharedPPQReader& rReader, long trig,  const SvSml::MonitorEntryVector& rproductItemNames, ProductPtrPair&  rlastRejectProduct );

//! for trig == -1 Get the values for the last saved reject   and store the product in lastRejectProduct
//! for trig  > -1 return  product  from lastRejectProduct  if it has the tigger number trig
//! \param rReader [in] reader 
//! \param trig [in]   triggercount
//! \param productItemNames [in] Listof item names 
//! \param lastRejectProduct [in,out] last saved product.
//! \returns Json::Value
template<>
Json::Value GetRejectedProduct<SvSol::TcpApi>(SvSml::SVSharedPPQReader& rReader, long trig,  const SvSml::MonitorEntryVector& rProductItemNames, ProductPtrPair&  lastReject )
{
	Json::Value rslt(Json::objectValue);

	long idx = -1;
	SvSml::ProductPtr product = rReader.RequestReject(trig, idx);
	rslt = WriteProductItems<SvSol::TcpApi>(product, rProductItemNames);

	ProductPtrPair prevReject = lastReject;
	lastReject = std::make_pair(product, idx);
	// Decide which product to keep and which to release...
	if (prevReject.second >= 0)
	{
		rReader.ReleaseReject(prevReject.first, prevReject.second);
	}

	return rslt;
}

//! for trig == -1 Get the values for the last saved reject   and store the product in lastRejectProduct
//! for trig  > -1 return  product  from lastRejectProduct  if it has the tigger number trig
//! \param rReader [in] reader 
//! \param trig [in]   triggercount
//! \param productItemNames [in] Listof item names 
//! \param lastRejectProduct [in,out] last saved product.
//! \returns Json::Value
template<>
Json::Value GetRejectedProduct<SvSol::UdpApi>(SvSml::SVSharedPPQReader& rReader, long trig, const SvSml::MonitorEntryVector& rProductItemNames, ProductPtrPair&  lastRejectProduct )
{
	static Json::Value lastResult(Json::objectValue);
	Json::Value rslt(Json::objectValue);
	if (trig >= 0)
	{
		if (lastRejectProduct.second < 0 || trig != lastRejectProduct.first->product.m_TriggerCount)
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_RRS_4_GENERAL_WARNING, SvOi::Tid_RejectNotFound, SvStl::SourceFileParams(StdMessageParams) );
			throw MsgCont;
		}
		else
		{
			rslt = lastResult;
		}
	}
	else
	{
		long idx = -1;
		const SvSml::ProductPtr product = rReader.RequestReject(trig, idx);
		rslt = WriteProductItems<SvSol::UdpApi>(product, rProductItemNames);

		ProductPtrPair prevReject = lastRejectProduct;

		lastRejectProduct = std::make_pair(product, idx);
		lastResult = rslt;
		// Decide which product to keep and which to release...
		if (prevReject.second >= 0)
		{
			rReader.ReleaseReject(prevReject.first, prevReject.second);
		}
	}
	return rslt;
}

template<>
Json::Value DispatchCommand<SvSol::TcpApi>(const JsonCmd & cmd, const SvSml::MonitorListCpyMap & rmlMap)
{
	Json::Value rslt(Json::objectValue);
	if (false == cmd.isObject() || false ==  cmd.isMember(SVRC::cmd::name))
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back( SVString( cmd[SVRC::cmd::name].asString().c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	if (cmd[SVRC::cmd::name] == SVRC::cmdName::getFail)
	{
		const Json::Value & args = cmd[SVRC::cmd::arg];
		SVString ppqName;
		SVString listName;
		SvSml::MonitorListCpyMap::const_iterator mit;

		if (args.isMember(SVRC::arg::listName) 
			&& (mit = rmlMap.find(listName = args[SVRC::arg::listName].asString())) != rmlMap.end()
			&& true == mit->second.m_IsActive 
			) 
		{
			ppqName = mit->second.m_ppq;
			SvSml::SVSharedPPQReader reader;
			if (false == reader.Open(ppqName))
			{
				SvStl::MessageContainer MsgCont;
				MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_CannotOpenReader, SvStl::SourceFileParams(StdMessageParams) );
				throw MsgCont;
			}
			SvSml::FailStatusMap fsMap = reader.GetFailStatus(mit->second.failStats);
			if (static_cast<int>(fsMap.size()) > mit->second.m_rejectDepth)
			{
				size_t sz = fsMap.size();
				SvSml::FailStatusMap::iterator it = fsMap.begin();
				std::advance(it, sz - mit->second.m_rejectDepth);
				fsMap.erase(fsMap.begin(), it);
			}
			rslt = WriteFailStatus(fsMap);
			return rslt;
		}
		else
		{
			SvStl::MessageContainer MsgCont;
			SVStringVector msgList;
			msgList.push_back( SVString( listName.c_str() ) );
			MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidArguments, msgList, SvStl::SourceFileParams(StdMessageParams) );
			throw MsgCont;
		}
	}
	else if (cmd[SVRC::cmd::name] == SVRC::cmdName::getRjct)
	{
		const Json::Value & args = cmd[SVRC::cmd::arg];
		SVString ppqName;
		SVString listName;
		SvSml::MonitorListCpyMap::const_iterator mit;

		if (args.isMember(SVRC::arg::trgrCount) 
			&& args.isMember(SVRC::arg::listName) 
			&& (mit = rmlMap.find(listName = args[SVRC::arg::listName].asString())) != rmlMap.end()
			&& true == mit->second.m_IsActive 
			) 
		{
			ppqName = mit->second.m_ppq;
			SvSml::SVSharedPPQReader reader;
			if(false == reader.Open(ppqName))
			{
				SvStl::MessageContainer MsgCont;
				MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_CannotOpenReader, SvStl::SourceFileParams(StdMessageParams) );
				throw MsgCont;
			}
			long trig = args[SVRC::arg::trgrCount].asInt();
			rslt = GetRejectedProduct<SvSol::TcpApi>(reader, trig, mit->second.prodItems,g_lastRejectMap[ppqName] );
			return rslt;
		}
		else
		{
			SvStl::MessageContainer MsgCont;
			SVStringVector msgList;
			msgList.push_back( SVString( listName.c_str() ) );
			MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidArguments, msgList, SvStl::SourceFileParams(StdMessageParams) );
			throw MsgCont;
		}
	}
	else if (cmd[SVRC::cmd::name] == SVRC::cmdName::getVersion)
	{
		const SVString& rVerStr = GetVersionString();
		Json::Value rslt(Json::objectValue);
		rslt[SVRC::result::SVO_ver] = rVerStr.c_str();
		return rslt;
	}

	SvStl::MessageContainer MsgCont;
	SVStringVector msgList;
	msgList.push_back( SVString() );
	MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
	throw MsgCont;
}

template<>
Json::Value DispatchCommand<SvSol::UdpApi>(const JsonCmd & cmd, const SvSml::MonitorListCpyMap & rmlMap)
{
	Json::Value rslt(Json::objectValue);
	if ( false == cmd.isObject() || false == cmd.isMember(SVRC::cmd::name))
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back( SVString() );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	if (cmd[SVRC::cmd::name] != SVRC::cmdName::getProd)
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back( SVString( cmd[SVRC::cmd::name].asString().c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	const Json::Value & args = cmd[SVRC::cmd::arg];
	SVString ppqName;
	SVString listName;
	SvSml::MonitorListCpyMap::const_iterator mit;

	if (args.isMember(SVRC::arg::listName) && 
		(mit = rmlMap.find(listName = args[SVRC::arg::listName].asString())) != rmlMap.end()
		&& true == mit->second.m_IsActive 
		) // does the list exist?
	{
		ppqName = mit->second.m_ppq;
		SvSml::SVSharedPPQReader reader;
		if (false == reader.Open(ppqName))
		{
			SvStl::MessageContainer MsgCont;
			MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_CannotOpenReader, SvStl::SourceFileParams(StdMessageParams) );
			throw MsgCont;
		}

		long trig = -1;
		if (args.isMember(SVRC::arg::trgrCount))
		{
			trig = args[SVRC::arg::trgrCount].asInt();
		}
		if (SvSml::LastInspectedFilter == mit->second.m_ProductFilter)
		{
			rslt = GetLastInspectedProduct(reader, trig, mit->second.prodItems, g_LastProductMap[ppqName]);
		}
		else
		{
			// Check if there are rejects, if so get the rejected product, else get the last inspected product.
			try
			{
				rslt = GetRejectedProduct<SvSol::UdpApi>(reader, trig, mit->second.prodItems, g_lastRejectProductMap[ppqName]);
			}
			catch( const SvStl::MessageContainer& rExp )
			{
				UNREFERENCED_PARAMETER(rExp);
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				::OutputDebugStringA(rExp.what());
#endif
				rslt = GetLastInspectedProduct(reader, trig, mit->second.prodItems, g_LastProductMap[ppqName]);
			}
		}
	}
	else
	{
		SvStl::MessageContainer MsgCont;
		SVStringVector msgList;
		msgList.push_back( SVString( listName.c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidMonitorlist, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	return rslt;
}

template<typename API>
std::string GenerateResponse(const JsonCmd & cmd, const SvSml::MonitorListCpyMap & rmlMap)
{
	Json::Value rsp = NewResponse(cmd);
	try
	{
		const Json::Value& value = DispatchCommand<API>(cmd, rmlMap);
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
	catch( const SvStl::MessageContainer& rExp )
	{
		rsp[SVRC::cmd::err] = rExp.what();
		rsp[SVRC::cmd::hr] = Json::Value(E_FAIL);

		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( rExp.getMessage() );
	}
	catch(std::exception& rExp)
	{
		rsp[SVRC::cmd::err] = rExp.what();
		rsp[SVRC::cmd::hr] = Json::Value(E_FAIL);

		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		SVStringVector msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	Json::FastWriter writer;
	return SVString( writer.write(rsp) );
}

SVString BusyMessage(const JsonCmd & cmd)
{
	Json::Value rsp = NewResponse(cmd);
	rsp[SVRC::cmd::hr] = STG_E_INUSE;
	rsp[SVRC::cmd::err] = "Server busy.";
	Json::FastWriter writer;
	return SVString( writer.write(rsp) );
}


template<typename API, typename ServerSocket>
void Handler(ServerSocket& sok, SvSml::SVMonitorListReader& mlReader, SvSml::MonitorListCpyMap& rmonitorMap);

template <>
void Handler<SvSol::UdpApi, UdpServerSocket>(UdpServerSocket& sok,  SvSml::SVMonitorListReader& mlReader, SvSml::MonitorListCpyMap& rmonitorMap)
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
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		SVStringVector msgList;
		msgList.push_back( SVString( SvSol::SVSocketError::GetErrorText(SvSol::SVSocketError::GetLastSocketError() ) ) );
		Exception.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_SocketInvalid, msgList, SvStl::SourceFileParams(StdMessageParams) );
		return;
	}

	try
	{
		const JsonCmd& cmd = ReadCommand(client);
		if (!cmd.isNull())
		{

			bool isready = SvSml::ShareEvents::GetInstance().GetIsReady(); 
			if(isready)
			{
				if(!SvSml::ShareEvents::GetInstance().GetIsInit())
				{
					SvSml::MonitorListCpyHelper::ReloadMonitorMap(mlReader, rmonitorMap);
					SvSml::ShareEvents::GetInstance().SetIsInit();
					ClearHeld<API>();
				}
					const SVString& rResponse = GenerateResponse<API>(cmd, rmonitorMap);
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
	catch( const SvStl::MessageContainer& rExp )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( rExp.getMessage() );
	}
	catch(std::exception& rExp)
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		SVStringVector msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}


}

template<>
void Handler<SvSol::TcpApi, TcpServerSocket>(TcpServerSocket& sok,/* SvSml::SVShareControlHandler& ctrl,*/ SvSml::SVMonitorListReader& mlReader, SvSml::MonitorListCpyMap& rmonitorMap)
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

	while( WAIT_OBJECT_0 != WaitForSingleObject(g_ServiceStopEvent, 0) && client.IsValidSocket() && client.IsConnected())
	{
		if (client.DataAvailable())
		{
			try
			{
				const JsonCmd& cmd = ReadCommand(client);
				if (!cmd.isNull())
				{
					bool isready = SvSml::ShareEvents::GetInstance().GetIsReady(); 
					if(isready)
					{
						if(!SvSml::ShareEvents::GetInstance().GetIsInit())
						{
							SvSml::MonitorListCpyHelper::ReloadMonitorMap(mlReader, rmonitorMap);
							SvSml::ShareEvents::GetInstance().SetIsInit();
							ClearHeld<API>();
						}
						const SVString& rResponse = GenerateResponse<API>(cmd, rmonitorMap);
						client.Write(rResponse, SvSol::Traits<API>::needsHeader);
						break;

					}
					else
					{
						// check for version command
						if (cmd[SVRC::cmd::name] == SVRC::cmdName::getVersion)
						{
							const SVString& rResponse = GenerateResponse<API>(cmd, rmonitorMap);
							client.Write(rResponse, SvSol::Traits<API>::needsHeader);
						}
						else
						{
							client.Write(BusyMessage(cmd), SvSol::Traits<API>::needsHeader);
						}
					}

				}
			}
			catch( const SvStl::MessageContainer& rExp )
			{
				SvStl::MessageMgrStd e( SvStl::LogOnly );
				e.setMessage( rExp.getMessage() );
			}
			catch(std::exception & rExp)
			{
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				SVStringVector msgList;
				msgList.push_back( rExp.what() );
				Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
			}
		}
	}
}

template <typename API>
DWORD WINAPI servcmd(LPVOID ctrlPtr)
{
	try
	{
		typedef SvSol::SVServerSocket<API> ServerSocket;
		
		SvSml::SVMonitorListReader mlReader;
		SvSml::MonitorListCpyMap monitorMap;
		ServerSocket sok;
		sok.Create();
		sok.SetBlocking();
		sok.Listen(port<API>::number);
		while( WAIT_OBJECT_0 != WaitForSingleObject(g_ServiceStopEvent, 0)  )
		{
			if (sok.ClientConnecting())
			{
				Handler<API>(sok,  mlReader, monitorMap);
			}
		}
	}
	catch( const SvStl::MessageContainer& rExp )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( rExp.getMessage() );
	}
	catch(std::exception & rExp)
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		SVStringVector msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	return 0;
}

bool CheckCommandLineArgs(int argc, _TCHAR* argv[], LPCTSTR option)
{
	bool bFound = false;
	if (argc > 1)
	{
		for (int i = 1;i < argc && !bFound;i++)
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
void StartThreads( DWORD argc, LPTSTR  *argv )
{
	// check command line args - if /nocheck is specified - ignore the < 2 Gig error
	bool bCheckSizeOverride = CheckCommandLineArgs(argc, argv, _T("/nocheck"));

	HRESULT hr = SvSml::SVSharedConfiguration::SharedResourcesOk();
	if (S_OK != hr )
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
			// Messagebox ?
			return;
		}
	}
	try
	{

		///Start watching ReadyEvent
		SvSml::ShareEvents::GetInstance().StartWatch();

		previousHandler = signal(SIGSEGV, AccessViolationHandler);

		const SVString& rVersionStr = GetVersionString();
		SVString title = _T("Run/Reject Server ");
		title += rVersionStr;
		SetConsoleTitle( title.c_str() );
		SvSol::SVSocketLibrary::Init();
		DWORD threadIds[3];
		HANDLE threads[3];

		threads[0] = CreateThread(nullptr, 0, servimg, nullptr, 0, &threadIds[0]);
		threads[1] = CreateThread(nullptr, 0, servcmd<SvSol::TcpApi>, NULL, 0, &threadIds[1]);
		threads[2] = CreateThread(nullptr, 0, servcmd<SvSol::UdpApi>, NULL, 0, &threadIds[2]);

		// stop the threads...
		::WaitForMultipleObjects(3, threads, true, INFINITE);

		signal(SIGSEGV, previousHandler);
	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		SVStringVector msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_FailedtoStart, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	SvSol::SVSocketLibrary::Destroy();
}

int _tmain(int argc, _TCHAR* argv[])
{
	int rc = 0;

	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_RRS_0_STARTED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );

	//Function pointer for starting the threads
	gp_StartThreads = &StartThreads;

	SERVICE_TABLE_ENTRY ServiceTable[] = 
	{
		{ cServiceName, (LPSERVICE_MAIN_FUNCTION) ServiceMain },
		{ nullptr, nullptr}
	};

	if( !StartServiceCtrlDispatcher( ServiceTable ) )
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

	Exception.setMessage( SVMSG_RRS_1_STOPPED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );

	return rc;
}