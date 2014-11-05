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

#include "stdafx.h"
#include <conio.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <process.h>
#include <string>
#include <fstream>
#include <list>
#include <boost/scoped_array.hpp>
#include <boost/any.hpp>
#include <atlenc.h>
#include "SVSocketLibrary.h"
#include "SVServerSocket.h"
#include "SVRemoteControlConstants.h"
#include "..\JsonLib\include\json.h"
#include "SVSharedPPQReader.h"
#include "SVMonitorListReader.h"
#include "SVShareControlHandler.h"
#include "SVSharedConfiguration.h"

#pragma comment (lib, "version.lib")

bool g_bQuit = false;

DWORD threadIds[3] = {0};
HANDLE threads[3] = {0};

const u_short imgPort = 28963;

template<typename API>
struct port;

template<>
struct port<TcpApi>
{
	enum : u_short { number = 28965 };
};

template<>
struct port<UdpApi>
{
	enum : u_short { number = 28967 };
};

typedef Json::Value JsonCmd;

typedef SeidenaderVision::SVShareControlHandler ShareControl;
typedef SeidenaderVision::SVMonitorListReader MonitorListReader;
typedef SeidenaderVision::SVSharedPPQReader PPQReader;

typedef Seidenader::Socket::SVServerSocket<UdpApi> UdpServerSocket;
typedef Seidenader::Socket::SVSocket<UdpApi> UdpSocket;

typedef Seidenader::Socket::SVServerSocket<TcpApi> TcpServerSocket;
typedef Seidenader::Socket::SVSocket<TcpApi> TcpSocket;

typedef SeidenaderVision::ProductPtr ProductPtr;

typedef std::pair<ProductPtr, long> ProductPtrPair;
static ProductPtrPair g_lastProduct(ProductPtr(nullptr), -1); // For last held product from GetProduct cmd
static ProductPtrPair g_lastRejectProduct(ProductPtr(nullptr), -1); // For last held Reject from GetProduct cmd when product filter is lastReject
static ProductPtrPair g_lastReject(ProductPtr(nullptr), -1); // For last held Reject from GetReject cmd

using Seidenader::Socket::header;
using Seidenader::Socket::Traits;
using SeidenaderVision::SVSharedConfiguration;

DWORD WINAPI servimg(LPVOID)
{
	try
	{
		typedef UdpServerSocket ServerSocket;
		typedef UdpSocket Socket;
		typedef Seidenader::Socket::header header;
		ServerSocket sok;
		sok.Create();
		sok.SetBufferSize(1024*1024);
		sok.Listen(imgPort);
		const size_t buflen = 1024*48;
		boost::scoped_array<u_char> arr(new u_char[buflen]);
		long cnt = 0;
		while (!g_bQuit)
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
					if (client.ReadAll(buf, sz, true) == SVSocketError::Success)
					{
						std::string filename = std::string(reinterpret_cast<char *>(buf.get()), sz);
						try
						{
							std::ifstream is;
							is.open(filename, std::ios::in | std::ios::binary);
							if (!is.is_open())
							{
								throw std::runtime_error("Cannot open " + filename);
							}
							is.seekg(0, std::ios::end);
							std::streamsize sz = is.tellg();
							is.seekg(0, std::ios::beg);
							std::vector<u_char> vec(sz);
							is.read(reinterpret_cast<char *>(&vec[0]), sz);
							client.Write(&vec[0], sz, true);
						}
						catch(std::exception & ex)
						{
							std::cout << ex.what() << std::endl;
							SVSharedConfiguration::Log(ex.what());
							break;
						}
					}
				}
				else
				{
					SVSharedConfiguration::Log(client.Log(SVSocketError::GetErrorText(SVSocketError::GetLastSocketError()), true));
				}
			}
		}
	}
	catch (std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		SVSharedConfiguration::Log(ex.what());
	}	
	return 0;
}

struct MonitorListCopy 
{
	typedef std::vector<std::string> Strings;
	Strings  prodItems;
	Strings  rejctCond;
	Strings  failStats;

	std::string m_name;
	std::string m_ppq;
	int m_rejectDepth;
	SVProductFilterEnum m_ProductFilter;
};

std::string GetVersionString()
{
	std::string verStr;

	char moduleFilename[512];
	::GetModuleFileNameA(NULL, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSizeA(moduleFilename, &dwHandle);
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfoA(moduleFilename, NULL, size, lpData);
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = NULL;
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
	SVSocketError::ErrorEnum error = SVSocketError::Success;
	if (SVSocketError::Success == (error = sok.ReadAll(arr, buflen, false)))
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
		if (SVSocketError::ConnectionReset == error)
		{
			sok.Destroy();
		}
		::OutputDebugStringA(SVSocketError::GetErrorText(error));
		::OutputDebugStringA("\n");
	}
	return val;
}

JsonCmd ReadCommand(UdpSocket & sok)
{
	Json::Value val(Json::nullValue);
	bytes buff;
	u_long sz;
	if (SVSocketError::Success == sok.ReadAll(buff, sz, true))
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
	HANDLE hFile = ::CreateFileA(name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw std::exception("Invalid file handle");
	}
	else
	{
		DWORD file_sz;
		file_sz = ::GetFileSize(hFile, NULL);

		HANDLE hMapping = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, file_sz, NULL);
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
		throw std::exception("Invalid command received.");
	}
	return rsp;
}

typedef SeidenaderVision::SVSharedInspectionReader SHMReader;

// the three following functions expect the Json array and add to it value/error/image objects respectively.
Json::Value & GenerateValues(Json::Value & arr, InspectionDataPtr data, const MonitorListCopy::Strings& productItemNames)
{
	int count = data->m_TriggerCount;
	for (SVSharedValueMap::const_iterator it = data->m_Values.begin(); it != data->m_Values.end(); ++it)
	{
		const SVSharedValueMap::value_type & pair = *it;
		if (std::find(productItemNames.begin(), productItemNames.end(), pair.first.c_str()) != productItemNames.end())
		{
			Json::Value val(Json::objectValue);
			val[SVRC::vo::name] = pair.first.c_str();
			val[SVRC::vo::status] = pair.second.m_Status;
			val[SVRC::vo::count] = count;
			Json::Value value(Json::arrayValue);
			value.append(pair.second.m_Result.c_str());
			val[SVRC::vo::array] = value;
			arr.append(val);
		}
	}
	return arr;
}

Json::Value & GenerateErrors(Json::Value & arr, const InspectionDataPtr & data)
{
	return arr;
}

template<typename API>
void AppendImageContents(Json::Value & val, const std::string & filePath);

template<>
void AppendImageContents<UdpApi>(Json::Value & val, const std::string & filePath)
{
	val[SVRC::io::imageFileName] = filePath;
	val[SVRC::io::fetch] = 1;
}

template<>
void AppendImageContents<TcpApi>(Json::Value & val, const std::string & filePath)
{
	val[SVRC::io::image] = EncodeImg(filePath);
}

template<typename API>
Json::Value & GenerateImages(Json::Value & arr, InspectionDataPtr data, const MonitorListCopy::Strings& productItemNames)
{
	int count = data->m_TriggerCount;
	for (SVSharedImageMap::iterator it = data->m_Images.begin(); it != data->m_Images.end(); ++it)
	{
		SVSharedImageMap::value_type & pair = *it;
		if (std::find(productItemNames.begin(), productItemNames.end(), pair.first.c_str()) != productItemNames.end())
		{
			Json::Value val(Json::objectValue);
			val[SVRC::io::name] = pair.first.c_str();
			val[SVRC::io::status] = pair.second.m_Status;
			val[SVRC::io::count] = count;
			AppendImageContents<API>(val, pair.second.m_Filename.c_str());
			arr.append(val);
		}
	}
	return arr;
}

template<typename API>
Json::Value WriteProductItems(ProductPtr product, const MonitorListCopy::Strings& productItemNames)
{
	Json::Value rslt(Json::objectValue);
	Json::Value values(Json::arrayValue);
	Json::Value images(Json::arrayValue);
	Json::Value errors(Json::arrayValue);
	std::for_each(product->inspections.begin(), product->inspections.end(),
		[&values, &images, &errors, &productItemNames](std::pair<const std::string, InspectionDataPtr> & pair) 
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

using SeidenaderVision::FailStatusMap;
using SeidenaderVision::Values;
using SeidenaderVision::Value;

Json::Value WriteFailStatus(const FailStatusMap & fsMap)
{
	Json::Value rslt(Json::objectValue);
	Json::Value failList(Json::arrayValue);
	std::for_each(fsMap.begin(), fsMap.end(),
		[&failList](const FailStatusMap::value_type & entry)
		{
			const Values & values = entry.second;
			Json::Value voList(Json::objectValue);
			Json::Value entries(Json::arrayValue);
			std::for_each(values.begin(), values.end(),
				[&entries](const Value & value)
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

Json::Value GetLastInspectedProduct(PPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames)
{
	static Json::Value lastResult(Json::objectValue);
	Json::Value rslt(Json::objectValue);
	if (trig >= 0)
	{
		if (g_lastProduct.second < 0 || trig != g_lastProduct.first->product.m_TriggerCount)
		{
			throw(std::exception("Product Not Found"));
		}
		else
		{
			rslt = lastResult;
		}
	}
	else
	{
		long idx = -1;
		const ProductPtr product = rReader.RequestNextProduct(idx);
		rslt = WriteProductItems<UdpApi>(product, productItemNames);

		if (g_lastProduct.second >= 0)
		{
			rReader.ReleaseProduct(g_lastProduct.first, g_lastProduct.second);
		}
		g_lastProduct = std::make_pair(product, idx);
		lastResult = rslt;
	}
	return rslt;
}

template<typename API>
void ClearHeld();

template<>
void ClearHeld<TcpApi>()
{
	g_lastReject = std::make_pair(ProductPtr(nullptr), -1);
}

template<>
void ClearHeld<UdpApi>()
{
	g_lastProduct = std::make_pair(ProductPtr(nullptr), -1);
	g_lastRejectProduct = std::make_pair(ProductPtr(nullptr), -1);
}

template<typename API>
Json::Value GetRejectedProduct(PPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames);

template<>
Json::Value GetRejectedProduct<TcpApi>(PPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames)
{
	Json::Value rslt(Json::objectValue);
	
	long idx = -1;
	ProductPtr product = rReader.RequestReject(trig, idx);
	rslt = WriteProductItems<TcpApi>(product, productItemNames);
	
	// Decide which product to keep and which to release...
	if (g_lastReject.second >= 0)
	{
		rReader.ReleaseReject(g_lastReject.first, g_lastReject.second);
	}
	g_lastReject = std::make_pair(product, idx);
	return rslt;
}

template<>
Json::Value GetRejectedProduct<UdpApi>(PPQReader& rReader, long trig, const MonitorListCopy::Strings& productItemNames)
{
	static Json::Value lastResult(Json::objectValue);
	Json::Value rslt(Json::objectValue);
	if (trig >= 0)
	{
		if (g_lastRejectProduct.second < 0 || trig != g_lastRejectProduct.first->product.m_TriggerCount)
		{
			throw(std::exception("Reject Not Found"));
		}
		else
		{
			rslt = lastResult;
		}
	}
	else
	{
		long idx = -1;
		const ProductPtr product = rReader.RequestReject(trig, idx);
		rslt = WriteProductItems<UdpApi>(product, productItemNames);
	
		// Decide which product to keep and which to release...
		if (g_lastRejectProduct.second >= 0)
		{
			rReader.ReleaseReject(g_lastRejectProduct.first, g_lastRejectProduct.second);
		}
		g_lastRejectProduct = std::make_pair(product, idx);
		lastResult = rslt;
	}
	return rslt;
}

template<>
Json::Value DispatchCommand<TcpApi>(const JsonCmd & cmd, const MonitorMapCopy & mlMap)
{
	Json::Value rslt(Json::objectValue);
	if (cmd.isObject() && cmd.isMember(SVRC::cmd::name))
	{
		if (cmd[SVRC::cmd::name] == SVRC::cmdName::getFail)
		{
			const Json::Value & args = cmd[SVRC::cmd::arg];
			std::string ppqName;
			std::string listName;
			MonitorMapCopy::const_iterator mit;
			
			if (args.isMember(SVRC::arg::listName) 
				&& (mit = mlMap.find(listName = args[SVRC::arg::listName].asString())) != mlMap.end()) 
			{
				// for now let's skip checking if the list is activated - there's only one list.
				ppqName = mit->second.m_ppq;
				PPQReader reader;
				reader.Open(ppqName);
				FailStatusMap fsMap = reader.GetFailStatus(mit->second.failStats);
				if (fsMap.size() > mit->second.m_rejectDepth)
				{
					size_t sz = fsMap.size();
					FailStatusMap::iterator it = fsMap.begin();
					std::advance(it, sz - mit->second.m_rejectDepth);
					fsMap.erase(fsMap.begin(), it);
				}
				rslt = WriteFailStatus(fsMap);
				return rslt;
			}
			else
			{
				throw std::exception("Invalid arguments");
			}
		}
		else
		if (cmd[SVRC::cmd::name] == SVRC::cmdName::getRjct)
		{
			const Json::Value & args = cmd[SVRC::cmd::arg];
			std::string ppqName;
			std::string listName;
			MonitorMapCopy::const_iterator mit;
			
			if (args.isMember(SVRC::arg::trgrCount) 
				&& args.isMember(SVRC::arg::listName) 
				&& (mit = mlMap.find(listName = args[SVRC::arg::listName].asString())) != mlMap.end()) 
			{
				// for now let's skip checking if the list is activated - there's only one list.
				ppqName = mit->second.m_ppq;
				PPQReader reader;
				reader.Open(ppqName);
				long trig = args[SVRC::arg::trgrCount].asInt();
				rslt = GetRejectedProduct<TcpApi>(reader, trig, mit->second.prodItems);
				return rslt;
			}
			else
			{
				throw std::exception("Invalid arguments");
			}
		}
		else if (cmd[SVRC::cmd::name] == SVRC::cmdName::getVersion)
		{
			const std::string& rVerStr = GetVersionString();
			Json::Value rslt(Json::objectValue);
			rslt[SVRC::result::SVO_ver] = rVerStr.c_str();
			return rslt;
		}
	}
	throw std::exception("Invalid command name.");
}

template<>
Json::Value DispatchCommand<UdpApi>(const JsonCmd & cmd, const MonitorMapCopy & mlMap)
{
	Json::Value rslt(Json::objectValue);
	if (cmd.isObject() && cmd.isMember(SVRC::cmd::name))
	{
		if (cmd[SVRC::cmd::name] == SVRC::cmdName::getProd)
		{
			const Json::Value & args = cmd[SVRC::cmd::arg];
			std::string ppqName;
			std::string listName;
			MonitorMapCopy::const_iterator mit;
			
			if (args.isMember(SVRC::arg::listName) && 
				(mit = mlMap.find(listName = args[SVRC::arg::listName].asString())) != mlMap.end()) // does the list exist?
			{
				// for now let's skip checking if the list is activated - there's only one list.
				ppqName = mit->second.m_ppq;
				PPQReader reader;
				reader.Open(ppqName);

				long trig = -1;
				if (args.isMember(SVRC::arg::trgrCount))
				{
					trig = args[SVRC::arg::trgrCount].asInt();
				}
				if (LastInspectedFilter == mit->second.m_ProductFilter)
				{
					rslt = GetLastInspectedProduct(reader, trig, mit->second.prodItems);
				}
				else
				{
					// Check if there are rejects, if so get the rejected propduct, else get the last inspected product.
					try
					{
						rslt = GetRejectedProduct<UdpApi>(reader, trig, mit->second.prodItems);
					}
					catch (std::exception& e)
					{
						::OutputDebugStringA(e.what());
						rslt = GetLastInspectedProduct(reader, trig, mit->second.prodItems);
					}
				}
			}
			else
			{
				throw std::exception("Invalid monitor list name.");
			}
		}
		else
		{
			throw std::exception("Invalid command name.");
		}
	}
	else
	{
		throw std::exception("Invalid command packet.");
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
	catch(std::exception & ex)
	{
		rsp[SVRC::cmd::err] = ex.what();
		rsp[SVRC::cmd::hr] = Json::Value(E_FAIL);
		SVSharedConfiguration::Log(Traits<API>::ApiName() + ": " + ex.what());
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

MonitorListCopy::Strings MakeCopy(const SeidenaderVision::StringList & list)
{
	MonitorListCopy::Strings cpy;
	cpy.reserve(list.size());
	std::for_each(list.begin(), list.end(),
		[&cpy](const char_string & str)
		{
			cpy.push_back(str.c_str());
		}
	);
	return cpy;
}

MonitorListCopy MakeCopy(const MonitorListReader & reader, const std::string & name)
{
	const SeidenaderVision::SVSharedMonitorList & list = reader[name];
	MonitorListCopy cpy;
	cpy.m_name = list.GetName().c_str();
	cpy.m_ppq = list.GetPPQName().c_str();
	cpy.m_rejectDepth = list.GetRejectDepth();
	cpy.m_ProductFilter = list.GetProductFilter();
	cpy.prodItems = MakeCopy(list.GetProductItems());
	cpy.failStats = MakeCopy(list.GetFailStatus());
	cpy.rejctCond = MakeCopy(list.GetRejectCond());
	return cpy;
}

static bool CheckReadyChanged(long& lastReady, long count, MonitorListReader& mlReader, MonitorMapCopy& monitorMap)
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

static void CheckProductFilterChanged(long& lastFilterChange, long count, MonitorListReader& mlReader, MonitorMapCopy& monitorMap)
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
void Handler(ServerSocket& sok, ShareControl& ctrl, MonitorListReader& mlReader, MonitorMapCopy& monitorMap);

template <>
void Handler<UdpApi, UdpServerSocket>(UdpServerSocket& sok, ShareControl& ctrl, MonitorListReader& mlReader, MonitorMapCopy& monitorMap)
{
	typedef UdpApi API;
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
					client.Write(resp, Traits<API>::needsHeader);
				}
				else
				{
					if (lastReady) // last time we checked control was ready, svo has gone offline since
					{
						lastReady = 0;
						ctrl.SetAck();
					}
					client.Write(BusyMessage(cmd), Traits<API>::needsHeader);
				}
			}
			else
			{
				std::cout << "null cmd \n";
			}
		}
		catch(std::exception & ex)
		{
			SVSharedConfiguration::Log(Traits<API>::ApiName() + ": " + ex.what());
			std::cout << ex.what() << std::endl;
		}
	}
	else
	{
		std::cout << "invalid socket\n";
		SVSharedConfiguration::Log(client.Log(SVSocketError::GetErrorText(SVSocketError::GetLastSocketError()), true));
	}
}

template<>
void Handler<TcpApi, TcpServerSocket>(TcpServerSocket& sok, ShareControl& ctrl, MonitorListReader& mlReader, MonitorMapCopy& monitorMap)
{
	typedef TcpApi API;
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

	while (!g_bQuit && client.IsValidSocket() && client.IsConnected())
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
						client.Write(resp, Traits<API>::needsHeader);
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
							client.Write(resp, Traits<API>::needsHeader);
						}
						else
						{
							client.Write(BusyMessage(cmd), Traits<API>::needsHeader);
						}
					}
				}
			}
			catch(std::exception & ex)
			{
				SVSharedConfiguration::Log(Traits<API>::ApiName() + ": " + ex.what());
				std::cout << ex.what() << std::endl;
			}
		}
	}
}

template <typename API>
DWORD WINAPI servcmd(LPVOID ctrlPtr)
{
	try
	{
		typedef Seidenader::Socket::SVServerSocket<API> ServerSocket;
		ShareControl& ctrl = *reinterpret_cast<ShareControl *>(ctrlPtr);
		MonitorListReader mlReader;
		MonitorMapCopy monitorMap;
		ServerSocket sok;
		sok.Create();
		sok.SetBlocking();
		sok.Listen(port<API>::number);
		while (!g_bQuit)
		{
			if (sok.ClientConnecting())
			{
				Handler<API>(sok, ctrl, mlReader, monitorMap);
			}
		}
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
	}	
	return 0;
}

void RunConsole()
{
	bool paused = false;
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	WORD wOldColorAttrs = csbiInfo.wAttributes;

	// Set the text attributes to draw green text on black background. 
    SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	// Turn off the line input and echo input modes 
	DWORD fdwOldMode;
    GetConsoleMode(hStdin, &fdwOldMode);
    
    DWORD fdwMode = fdwOldMode &  ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT); 
    SetConsoleMode(hStdin, fdwMode);
    	
    LPTSTR lpszPrompt = _T("Type q to quit: ");
	DWORD cWritten;
	COORD coordScreen = { 0, 0 };
	while (!g_bQuit) 
    { 
		coordScreen.X = 0;
		coordScreen.Y = 23;
		SetConsoleCursorPosition( hStdout, coordScreen);
		
		if (paused)
		{
			WriteConsole(hStdout, _T("*** PAUSED ***"), 14, &cWritten, nullptr);
		}
		else
		{
			WriteConsole(hStdout, _T("              "), 14, &cWritten, nullptr);
		}
		coordScreen.Y = 24;
		SetConsoleCursorPosition( hStdout, coordScreen);
		WriteConsole(hStdout, lpszPrompt, static_cast<DWORD>(_tcslen(lpszPrompt)), &cWritten, nullptr);

		DWORD NumberOfEventsRead = 0;
		INPUT_RECORD buff[128];
		BOOL rc = PeekConsoleInput(hStdin, buff, sizeof(buff), &NumberOfEventsRead);
		if (rc && NumberOfEventsRead)
		{
			rc = ReadConsoleInput(hStdin, buff, sizeof(buff), &NumberOfEventsRead);
			if (rc && NumberOfEventsRead)
			{           
				for (DWORD i = 0; i < NumberOfEventsRead; i++) 
				{
					switch(buff[i].EventType) 
					{ 
						case KEY_EVENT: // keyboard input 
						{
							if (buff[i].Event.KeyEvent.uChar.AsciiChar == 'q' || buff[i].Event.KeyEvent.uChar.AsciiChar == 'Q' )
							{
								g_bQuit = true;
							}
							break; 
						}
	 
						case MOUSE_EVENT: // mouse input 
						//MouseEventProc(irInBuf[i].Event.MouseEvent); 
						break; 
	 
						case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
						//ResizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent); 
						break; 
	 
						case FOCUS_EVENT:  // disregard focus events 
						case MENU_EVENT:   // disregard menu events 
						break; 
	 
						default: 
						break; 
					} 
				}
			}
		}
		Sleep(250);
	}
	// Restore the original console mode. 
    SetConsoleMode(hStdin, fdwOldMode);

    // Restore the original text colors. 
    SetConsoleTextAttribute(hStdout, wOldColorAttrs);
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

bool IsProcessRunning(const TCHAR* processName,DWORD dwProcess)
{
    bool exists = false;
    PROCESSENTRY32 entry;
	memset(&entry, 0, sizeof(entry));
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != snapshot)
	{
		if (Process32First(snapshot, &entry))
		{
			while (Process32Next(snapshot, &entry))
			{
				if (!_tcsicmp(entry.szExeFile, processName))
				{
					if (entry.th32ProcessID != dwProcess)
					{
						exists = true;
					}
				}
			}
		}
		CloseHandle(snapshot);
	}
    return exists;
}

// Command Line arguments: /nocheck /show
// /nocheck means to ignore the 2 GiG size requirement
// /show means show console window (it is hidden by default)
int _tmain(int argc, _TCHAR* argv[])
{
	 DWORD pid = GetCurrentProcessId();

	if ( IsProcessRunning(_T("RunRejectServer.exe"),pid) )
	{
		return -1;
	}
	// check command line args - if /nocheck is specified - ignore the < 2 Gig error
	bool bCheckSizeOverride = CheckCommandLineArgs(argc, argv, _T("/nocheck"));
	bool bShowConsole = CheckCommandLineArgs(argc, argv, _T("/show"));
	
	HRESULT hr = SeidenaderVision::SVSharedConfiguration::SharedResourcesOk();
	if (hr != S_OK)
	{
		std::string msg;
		if (hr == STG_E_INSUFFICIENTMEMORY)
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
		else if (hr == STG_E_PATHNOTFOUND)
		{
			msg = "Shared Resources - Path not found.\n";
		}
		else
		{
			msg = "Shared Resources - missing or inadequate.\n";
		}
		if (S_OK != hr)
		{
			::OutputDebugStringA(msg.c_str());
			std::cout << msg;
			// Messagebox ?
			return -1;
		}
	}
	try
	{
		const std::string& versionStr = GetVersionString();
		std::string title = "Run/Reject Server ";
		title += versionStr;
		SetConsoleTitleA(title.c_str());
		if (!bShowConsole)
		{
			HWND hWnd = GetConsoleWindow();
			ShowWindow(hWnd, SW_HIDE);
		}
		SVSocketLibrary::Init();
		ShareControl ctrl;
		DWORD threadIds[3];
		HANDLE threads[3];
		
		threads[0] = CreateThread(nullptr, 0, servimg, nullptr, 0, &threadIds[0]);
		threads[1] = CreateThread(nullptr, 0, servcmd<TcpApi>, (void *)&ctrl, 0, &threadIds[1]);
		threads[2] = CreateThread(nullptr, 0, servcmd<UdpApi>, (void *)&ctrl, 0, &threadIds[2]);
	
		RunConsole();

		// stop the threads...
		::WaitForMultipleObjects(3, threads, true, INFINITE);
	}
	catch (std::exception & ex)
	{
		std::cout << "Failed to start: " << ex.what() << std::endl;
	}
	SVSocketLibrary::Destroy();
	return 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\RunRejectServer\runrejctsvr.cpp_v  $
 * 
 *    Rev 1.16   28 Oct 2014 10:45:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  952
 * SCR Title:  Run/Reject Server Should Respond to the GetVersion Command
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised TCP handler to check for version command even if the Control Share indicates not ready (No Monitor share).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   27 Oct 2014 18:18:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Handler for TCP to disable the nagle delay
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   27 Oct 2014 09:48:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Tcp specialized Handler to set keepalive values.
 * Revised IsProcessRunning to check for valid handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Oct 2014 14:21:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  966
 * SCR Title:  Limit the RunRejectServer to only one instance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made the check to see if the process is running to be the first operation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   22 Oct 2014 12:11:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  966
 * SCR Title:  Limit the RunRejectServer to only one instance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method IsProcessingRunning to only allow 1 instance of the RunRejectServer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   20 Oct 2014 11:28:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  953
 * SCR Title:  Refactor Design for Socket Used by SVRC
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised ReadCommand to destroy the clinet socket on a Connection Rest error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Oct 2014 13:02:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  954
 * SCR Title:  Fix Issues with Run/Reject Server and Shared Memory Synchronization
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the lastReady and lastFilterChange variables in the Handler templated functions to be static to correct an issue where the shared mempry slot weren't being released.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   14 Oct 2014 18:31:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  963
 * SCR Title:  Missing End Group when saving and loading tool group
 * Checked in by:  una;  <<Unassigned>>
 * Change Description:  
 *   Revised servimg to call ClientConnecting
 * Revised servcmd to call a templated handler, one for UDP and one for TCP
 * Add Handler templated method for UDP.
 * Add Handler templated method for TCP.
 * Added console
 * Revised to use CreateThread.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   06 Oct 2014 17:01:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  952
 * SCR Title:  Run/Reject Server Should Respond to the GetVersion Command
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to handle GetVersion Command
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Oct 2014 09:10:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  954
 * SCR Title:  Fix Issues with Run/Reject Server and Shared Memory Synchronization
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Aded CheckReadyChanged function.
 * Added CheckProductFilterChanged function.
 * Added ClearHeld function.
 * Revised servcmd function.
 * Revised GetLastInspectedProduct
 * Revised GetRejectedProduct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Sep 2014 14:12:50   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Enabled logging.
 * Added logic to save JSON for last product.
 * Changes for Product Filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Aug 2014 17:35:56   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for product filter, bug fixes, refactoring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Aug 2014 17:16:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added command line option to override the 2 Gig minimum size requirement.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Aug 2014 11:01:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised to correct merge issue (missing ending brace)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Aug 2014 09:55:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added PVCS header/footer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
