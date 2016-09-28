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
#include "SVRemoteControlConstants.h"
#include "JsonLib\include\json.h"
#include "SVSharedMemoryLibrary\SVSharedPPQReader.h"
#include "SVSharedMemoryLibrary\SVMonitorListReader.h"
#include "SVSharedMemoryLibrary\SVShareControlHandler.h"
#include "SVSharedMemoryLibrary\SVSharedConfiguration.h"
#include "SVMessage\SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "RunRejectService.h"
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
static std::map<std::string, ProductPtrPair> g_LastProductMap;  
 /// For last held Reject from GetProduct cmd when product filter is lastReject.Map with ppqnames, (productPtr, Index) 
static std::map<std::string, ProductPtrPair> g_lastRejectProductMap;
// For last held Reject from GetReject cmd. Map with ppqnames, (productPtr, Index) 
static std::map<std::string, ProductPtrPair> g_lastRejectMap; 

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
						std::string filename = std::string(reinterpret_cast<char *>(buf.get()), sz);
						try
						{
							std::ifstream is;
							is.open(filename, std::ios::in | std::ios::binary);
							if (!is.is_open())
							{
								SvStl::MessageContainer MsgCont;
								SVStringArray msgList;
								msgList.push_back( SVString( filename.c_str() ) );
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
							SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
							e.setMessage( rExp.getMessage() );
						}
						catch(std::exception& rExp)
						{
							SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
							SVStringArray msgList;
							msgList.push_back( rExp.what() );
							Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
						}
					}
				}
				else
				{
					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					SVStringArray msgList;
					msgList.push_back( SVString( SvSol::SVSocketError::GetErrorText(SvSol::SVSocketError::GetLastSocketError() ) ) );
					Exception.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_SocketInvalid, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}
		}
	}
	catch( const SvStl::MessageContainer& rExp )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( rExp.getMessage() );
	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		SVStringArray msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	return 0;
}

struct MonitorListCopy 
{
	typedef std::vector<std::string> Strings; // maybe use set instead (to ensure it's sorted)?
	Strings  prodItems;
	Strings  rejctCond;
	Strings  failStats;

	std::string m_name;
	std::string m_ppq;
	int m_rejectDepth;
	bool m_IsActive;
	SvSml::SVProductFilterEnum m_ProductFilter;
};

std::string GetVersionString()
{
	std::string verStr;

	char moduleFilename[512];
	::GetModuleFileNameA(nullptr, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSizeA(moduleFilename, &dwHandle);
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfoA(moduleFilename, 0, size, lpData);
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
			verStr = buf.str();
		}
	}
	delete [] lpData;

	#ifdef _DEBUG
		verStr += "d";        // For debug builds.
	#endif

	return verStr;
}

typedef std::map<std::string, MonitorListCopy> MonitorMapCopy;

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

std::string EncodeImg(const std::string & name)
{
	HANDLE hFile = ::CreateFileA(name.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		SvStl::MessageContainer MsgCont;
		SVStringArray msgList;
		msgList.push_back( SVString( name.c_str() ) );
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
		SVStringArray msgList;
		msgList.push_back( SVString( cmd[SVRC::cmd::name].asString().c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}
	return rsp;
}

typedef SvSml::SVSharedInspectionReader SHMReader;

// the three following functions expect the Json array and add to it value/error/image objects respectively.
Json::Value & GenerateValues(Json::Value & arr, SvSml::InspectionDataPtr data, const MonitorListCopy::Strings& productItemNames)
{
	int count = data->m_TriggerCount;
	for (SvSml::SVSharedValueContainer::const_iterator it = data->m_Values.begin(); it != data->m_Values.end(); ++it)
	{
		const SvSml::SVSharedValueContainer::value_type& sharedValue = *it;
		// Is this lookup really necessary? 
		// The MonitorList should be in sync with SVObserver, so what is written to shared memory would be correct
		if (std::find(productItemNames.begin(), productItemNames.end(), sharedValue.m_ElementName.c_str()) != productItemNames.end())
		{
			Json::Value val(Json::objectValue);
			val[SVRC::vo::name] = sharedValue.m_ElementName.c_str();
			val[SVRC::vo::status] = sharedValue.m_Status;
			val[SVRC::vo::count] = count;
			Json::Value value(Json::arrayValue);
			value.append(sharedValue.m_Result);
			val[SVRC::vo::array] = value;
			arr.append(val);
		}
	}
	return arr;
}

Json::Value & GenerateErrors(Json::Value & arr, const SvSml::InspectionDataPtr & data)
{
	return arr;
}

template<typename API>
void AppendImageContents(Json::Value & val, const std::string & filePath);

template<>
void AppendImageContents<SvSol::UdpApi>(Json::Value & val, const std::string & filePath)
{
	val[SVRC::io::imageFileName] = filePath;
	val[SVRC::io::fetch] = 1;
}

template<>
void AppendImageContents<SvSol::TcpApi>(Json::Value & val, const std::string & filePath)
{
	val[SVRC::io::image] = EncodeImg(filePath);
}

template<typename API>
Json::Value & GenerateImages(Json::Value & arr, SvSml::InspectionDataPtr data, const MonitorListCopy::Strings& productItemNames)
{
	int count = data->m_TriggerCount;
	for (SvSml::SVSharedImageContainer::iterator it = data->m_Images.begin(); it != data->m_Images.end(); ++it)
	{
		const SvSml::SVSharedImageContainer::value_type& sharedImage = *it;
		// Is this lookup really necessary? 
		// The MonitorList should be in sync with SVObserver, so what is written to shared memory would be correct
		if (std::find(productItemNames.begin(), productItemNames.end(), sharedImage.m_ElementName.c_str()) != productItemNames.end())
		{
			Json::Value val(Json::objectValue);
			val[SVRC::io::name] = sharedImage.m_ElementName.c_str();
			val[SVRC::io::status] = sharedImage.m_Status;
			val[SVRC::io::count] = count;
			AppendImageContents<API>(val, sharedImage.m_Filename);
			arr.append(val);
		}
	}
	return arr;
}

template<typename API>
Json::Value WriteProductItems(SvSml::ProductPtr product, const MonitorListCopy::Strings& productItemNames)
{
	Json::Value rslt(Json::objectValue);
	Json::Value values(Json::arrayValue);
	Json::Value images(Json::arrayValue);
	Json::Value errors(Json::arrayValue);
	std::for_each(product->inspections.begin(), product->inspections.end(),
		[&values, &images, &errors, &productItemNames](std::pair<const std::string, SvSml::InspectionDataPtr> & pair) 
		{ 
			GenerateValues(values, pair.second, productItemNames); 
			GenerateImages<API>(images, pair.second, productItemNames); 
			GenerateErrors(errors, pair.second);
		}
	);
	rslt[SVRC::result::values] = values;
	rslt[SVRC::result::images] = images;
	rslt[SVRC::result::errors] = errors;
	return rslt;
}

template<typename API>
Json::Value DispatchCommand(const JsonCmd & cmd, const MonitorMapCopy & mlMap);

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
Json::Value GetLastInspectedProduct(SvSml::SVSharedPPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames, ProductPtrPair& lastProductPtrPair)
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
		rslt = WriteProductItems<SvSol::UdpApi>(product, productItemNames);

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
Json::Value GetRejectedProduct(SvSml::SVSharedPPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames, ProductPtrPair&  lastRejectProduct );

//! for trig == -1 Get the values for the last saved reject   and store the product in lastRejectProduct
//! for trig  > -1 return  product  from lastRejectProduct  if it has the tigger number trig
//! \param rReader [in] reader 
//! \param trig [in]   triggercount
//! \param productItemNames [in] Listof item names 
//! \param lastRejectProduct [in,out] last saved product.
//! \returns Json::Value
template<>
Json::Value GetRejectedProduct<SvSol::TcpApi>(SvSml::SVSharedPPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames, ProductPtrPair&  lastReject )
{
	Json::Value rslt(Json::objectValue);
	
	long idx = -1;
	SvSml::ProductPtr product = rReader.RequestReject(trig, idx);
	rslt = WriteProductItems<SvSol::TcpApi>(product, productItemNames);
	
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
Json::Value GetRejectedProduct<SvSol::UdpApi>(SvSml::SVSharedPPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames, ProductPtrPair&  lastRejectProduct )
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
		rslt = WriteProductItems<SvSol::UdpApi>(product, productItemNames);
	
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
Json::Value DispatchCommand<SvSol::TcpApi>(const JsonCmd & cmd, const MonitorMapCopy & mlMap)
{
	Json::Value rslt(Json::objectValue);
	if (false == cmd.isObject() || false ==  cmd.isMember(SVRC::cmd::name))
	{
		SvStl::MessageContainer MsgCont;
		SVStringArray msgList;
		msgList.push_back( SVString( cmd[SVRC::cmd::name].asString().c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	if (cmd[SVRC::cmd::name] == SVRC::cmdName::getFail)
	{
		const Json::Value & args = cmd[SVRC::cmd::arg];
		std::string ppqName;
		std::string listName;
		MonitorMapCopy::const_iterator mit;

		if (args.isMember(SVRC::arg::listName) 
			&& (mit = mlMap.find(listName = args[SVRC::arg::listName].asString())) != mlMap.end()
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
			SVStringArray msgList;
			msgList.push_back( SVString( listName.c_str() ) );
			MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidArguments, msgList, SvStl::SourceFileParams(StdMessageParams) );
			throw MsgCont;
		}
	}
	else if (cmd[SVRC::cmd::name] == SVRC::cmdName::getRjct)
	{
		const Json::Value & args = cmd[SVRC::cmd::arg];
		std::string ppqName;
		std::string listName;
		MonitorMapCopy::const_iterator mit;

		if (args.isMember(SVRC::arg::trgrCount) 
			&& args.isMember(SVRC::arg::listName) 
			&& (mit = mlMap.find(listName = args[SVRC::arg::listName].asString())) != mlMap.end()
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
			SVStringArray msgList;
			msgList.push_back( SVString( listName.c_str() ) );
			MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidArguments, msgList, SvStl::SourceFileParams(StdMessageParams) );
			throw MsgCont;
		}
	}
	else if (cmd[SVRC::cmd::name] == SVRC::cmdName::getVersion)
	{
		const std::string& rVerStr = GetVersionString();
		Json::Value rslt(Json::objectValue);
		rslt[SVRC::result::SVO_ver] = rVerStr.c_str();
		return rslt;
	}
	
	SvStl::MessageContainer MsgCont;
	SVStringArray msgList;
	msgList.push_back( SVString() );
	MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
	throw MsgCont;
}

template<>
Json::Value DispatchCommand<SvSol::UdpApi>(const JsonCmd & cmd, const MonitorMapCopy & mlMap)
{
	Json::Value rslt(Json::objectValue);
	if ( false == cmd.isObject() || false == cmd.isMember(SVRC::cmd::name))
	{
		SvStl::MessageContainer MsgCont;
		SVStringArray msgList;
		msgList.push_back( SVString() );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	if (cmd[SVRC::cmd::name] != SVRC::cmdName::getProd)
	{
		SvStl::MessageContainer MsgCont;
		SVStringArray msgList;
		msgList.push_back( SVString( cmd[SVRC::cmd::name].asString().c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidCommand, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	const Json::Value & args = cmd[SVRC::cmd::arg];
	std::string ppqName;
	std::string listName;
	MonitorMapCopy::const_iterator mit;

	if (args.isMember(SVRC::arg::listName) && 
		(mit = mlMap.find(listName = args[SVRC::arg::listName].asString())) != mlMap.end()
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
		SVStringArray msgList;
		msgList.push_back( SVString( listName.c_str() ) );
		MsgCont.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_InvalidMonitorlist, msgList, SvStl::SourceFileParams(StdMessageParams) );
		throw MsgCont;
	}

	return rslt;
}

template<typename API>
std::string GenerateResponse(const JsonCmd & cmd, const MonitorMapCopy & mlMap)
{
	Json::Value rsp = NewResponse(cmd);
	try
	{
		const Json::Value& value = DispatchCommand<API>(cmd, mlMap);
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

		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( rExp.getMessage() );
	}
	catch(std::exception& rExp)
	{
		rsp[SVRC::cmd::err] = rExp.what();
		rsp[SVRC::cmd::hr] = Json::Value(E_FAIL);

		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		SVStringArray msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	Json::FastWriter writer;
	return writer.write(rsp);
}

std::string BusyMessage(const JsonCmd & cmd)
{
	Json::Value rsp = NewResponse(cmd);
	rsp[SVRC::cmd::hr] = STG_E_INUSE;
	rsp[SVRC::cmd::err] = "Server busy.";
	Json::FastWriter writer;
	return writer.write(rsp);
}

MonitorListCopy::Strings MakeCopy(const SvSml::StringList & list)
{
	MonitorListCopy::Strings cpy;
	cpy.reserve(list.size());
	std::for_each(list.begin(), list.end(),
		[&cpy](const SvSml::char_string & str)
		{
			cpy.push_back(str.c_str());
		}
	);
	return cpy;
}

MonitorListCopy MakeCopy(const SvSml::SVMonitorListReader & reader, const std::string & name)
{
	const SvSml::SVSharedMonitorList & list = reader[name];
	MonitorListCopy cpy;
	cpy.m_name = list.GetName().c_str();
	cpy.m_ppq = list.GetPPQName().c_str();
	cpy.m_rejectDepth = list.GetRejectDepth();
	cpy.m_ProductFilter = list.GetProductFilter();
	cpy.prodItems = MakeCopy(list.GetProductItems());
	cpy.failStats = MakeCopy(list.GetFailStatus());
	cpy.rejctCond = MakeCopy(list.GetRejectCond());
	cpy.m_IsActive = list.IsActivated();
	return cpy;
}

static bool CheckReadyChanged(long& lastReady, long count, SvSml::SVMonitorListReader& mlReader, MonitorMapCopy& monitorMap)
{
	bool readyChanged = false;
	if (count && count != lastReady) // ready counter has changed - monitor list might have changed
	{
		mlReader.Open();
		monitorMap.clear();
		std::vector<std::string> mlNames = mlReader.GetListNames();
		std::for_each(mlNames.begin(), mlNames.end(),
		[&mlReader, &monitorMap](const std::string & name)
		{
			monitorMap.insert(std::make_pair(name, MakeCopy(mlReader, name)));
		}
		);
		mlReader.Close();
		lastReady = count;
		readyChanged = true;
	}
	return readyChanged;
}

static void CheckProductFilterChanged(long& lastFilterChange, long count, SvSml::SVMonitorListReader& mlReader, MonitorMapCopy& monitorMap)
{
	if (count && count != lastFilterChange) // filter counter has changed - product filter might have changed
	{
		// Update Filter info...
		mlReader.Open();
		std::vector<std::string> mlNames = mlReader.GetListNames();
		std::for_each(mlNames.begin(), mlNames.end(),
		[&mlReader, &monitorMap](const std::string & name)
		{
			monitorMap[name].m_ProductFilter = mlReader[name].GetProductFilter();
		}
		);
		mlReader.Close();
		lastFilterChange = count;
	}
}

template<typename API, typename ServerSocket>
void Handler(ServerSocket& sok, SvSml::SVShareControlHandler& ctrl, SvSml::SVMonitorListReader& mlReader, MonitorMapCopy& monitorMap);

template <>
void Handler<SvSol::UdpApi, UdpServerSocket>(UdpServerSocket& sok, SvSml::SVShareControlHandler& ctrl, SvSml::SVMonitorListReader& mlReader, MonitorMapCopy& monitorMap)
{
	typedef SvSol::UdpApi API;
	typedef UdpSocket Socket;
	static long lastReady = -1;
	static long lastFilterChange = -1;
	sockaddr addr;
	int len = sizeof(sockaddr);
	Socket client = sok.Accept(&addr, &len); // for UDP this never blocks
	if (client.IsValidSocket())
	{
		try
		{
			const JsonCmd& cmd = ReadCommand(client);
			if (!cmd.isNull())
			{
				if (ctrl.IsReady())
				{
					bool readyChanged = CheckReadyChanged(lastReady, ctrl.GetReadyCount(), mlReader, monitorMap);
					if (!readyChanged)
					{
						CheckProductFilterChanged(lastFilterChange, ctrl.GetProductFilterChangeCount(), mlReader, monitorMap);
					}
					else
					{
						ctrl.SetAck();
						ClearHeld<API>();
					}
					const std::string& resp = GenerateResponse<API>(cmd, monitorMap);
					client.Write(resp, SvSol::Traits<API>::needsHeader);
				}
				else
				{
					if (lastReady) // last time we checked control was ready, svo has gone offline since
					{
						lastReady = 0;
						ctrl.SetAck();
					}
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
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( rExp.getMessage() );
		}
		catch(std::exception& rExp)
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			SVStringArray msgList;
			msgList.push_back( rExp.what() );
			Exception.setMessage( SVMSG_RRS_2_STD_EXCEPTION, SvOi::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
	else
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		SVStringArray msgList;
		msgList.push_back( SVString( SvSol::SVSocketError::GetErrorText(SvSol::SVSocketError::GetLastSocketError() ) ) );
		Exception.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_SocketInvalid, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
}

template<>
void Handler<SvSol::TcpApi, TcpServerSocket>(TcpServerSocket& sok, SvSml::SVShareControlHandler& ctrl, SvSml::SVMonitorListReader& mlReader, MonitorMapCopy& monitorMap)
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
					if (ctrl.IsReady())
					{
						bool readyChanged = CheckReadyChanged(lastReady, ctrl.GetReadyCount(), mlReader, monitorMap);
						if (!readyChanged)
						{
							CheckProductFilterChanged(lastFilterChange, ctrl.GetProductFilterChangeCount(), mlReader, monitorMap);
						}
						else
						{
							ctrl.SetAck();
							ClearHeld<API>();
						}
						const std::string& resp = GenerateResponse<API>(cmd, monitorMap);
						client.Write(resp, SvSol::Traits<API>::needsHeader);
						break;
					}
					else
					{
						if (lastReady) // last time we checked control was ready, svo has gone offline since
						{
							lastReady = 0;
							ctrl.SetAck();
						}
						// check for version command
						if (cmd[SVRC::cmd::name] == SVRC::cmdName::getVersion)
						{
							const std::string& resp = GenerateResponse<API>(cmd, monitorMap);
							client.Write(resp, SvSol::Traits<API>::needsHeader);
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
				SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
				e.setMessage( rExp.getMessage() );
			}
			catch(std::exception & rExp)
			{
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				SVStringArray msgList;
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
		SvSml::SVShareControlHandler& ctrl = *reinterpret_cast<SvSml::SVShareControlHandler*> (ctrlPtr);
		SvSml::SVMonitorListReader mlReader;
		MonitorMapCopy monitorMap;
		ServerSocket sok;
		sok.Create();
		sok.SetBlocking();
		sok.Listen(port<API>::number);
		while( WAIT_OBJECT_0 != WaitForSingleObject(g_ServiceStopEvent, 0)  )
		{
			if (sok.ClientConnecting())
			{
				Handler<API>(sok, ctrl, mlReader, monitorMap);
			}
		}
	}
	catch( const SvStl::MessageContainer& rExp )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( rExp.getMessage() );
	}
	catch(std::exception & rExp)
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		SVStringArray msgList;
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
		std::string msg;
		if (STG_E_INSUFFICIENTMEMORY == hr)
		{
			if (!bCheckSizeOverride)
			{
				msg = "Shared Resources - not enough space available.\n";
			}
			else
			{
				hr = S_OK;
			}
		}
		else if (STG_E_PATHNOTFOUND == hr)
		{
			msg = "Shared Resources - Path not found.\n";
		}
		else
		{
			msg = "Shared Resources - missing or inadequate.\n";
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
		previousHandler = signal(SIGSEGV, AccessViolationHandler);

		const std::string& versionStr = GetVersionString();
		std::string title = "Run/Reject Server ";
		title += versionStr;
		SetConsoleTitleA(title.c_str());
		SvSol::SVSocketLibrary::Init();
		SvSml::SVShareControlHandler ctrl;
		DWORD threadIds[3];
		HANDLE threads[3];

		threads[0] = CreateThread(nullptr, 0, servimg, nullptr, 0, &threadIds[0]);
		threads[1] = CreateThread(nullptr, 0, servcmd<SvSol::TcpApi>, (void *)&ctrl, 0, &threadIds[1]);
		threads[2] = CreateThread(nullptr, 0, servcmd<SvSol::UdpApi>, (void *)&ctrl, 0, &threadIds[2]);

		// stop the threads...
		::WaitForMultipleObjects(3, threads, true, INFINITE);

		signal(SIGSEGV, previousHandler);
	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		SVStringArray msgList;
		msgList.push_back( rExp.what() );
		Exception.setMessage( SVMSG_RRS_3_GENERAL_ERROR, SvOi::Tid_FailedtoStart, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	SvSol::SVSocketLibrary::Destroy();
}

int _tmain(int argc, _TCHAR* argv[])
{
	int rc = 0;

	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
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