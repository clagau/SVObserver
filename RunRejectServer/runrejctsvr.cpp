//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : runrejctsvr
//* .File Name       : $Workfile:   runrejctsvr.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   22 Aug 2014 11:01:30  $
//******************************************************************************

#include "stdafx.h"
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

using Seidenader::Socket::header;
using Seidenader::Socket::Traits;

const std::string mock_rsp_begin = 
"{\
\"ID\" : 0,\
\"Name\" : \"GetProduct\",\
\"HResult\" : 0,\
\"Results\" :\
{\
\"items\" :\
{\
\"Images\" :\
[\
";
const std::string mock_rsp_end =
"],\
\"Values\" : [],\
\"Errors\" : []\
}\
}\
}";

void servimg(LPVOID)
{
	try
	{
		typedef Seidenader::Socket::SVServerSocket<UdpApi> ServerSocket;
		typedef Seidenader::Socket::SVSocket<UdpApi> Socket;
		typedef Seidenader::Socket::header header;
		ServerSocket sok;
		sok.Create();
		sok.SetBufferSize(1024*1024);
		sok.Listen(imgPort);
		const size_t buflen = 1024*48;
		boost::scoped_array<u_char> arr(new u_char[buflen]);
		long cnt = 0;
		while(true)
		{
			sockaddr addr;
			u_char *buf = arr.get();
			int len = sizeof(sockaddr);
			Socket client = sok.Accept(&addr, &len);
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
						//std::cout << "writing " << sz << " bytes...";
						client.Write(&vec[0], sz, true);
						//std::cout << filename << " written\n";
					}
					catch(std::exception & ex)
					{
						std::cout << ex.what() << std::endl;
						break;
					}
				}
			}
			//client.Destroy();
		}
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
	}	
	_endthread();
}

typedef Seidenader::Socket::SVSocket<TcpApi> TcpSocket;
typedef Seidenader::Socket::SVSocket<UdpApi> UdpSocket;

typedef SeidenaderVision::SVShareControlHandler ShareControl;
typedef SeidenaderVision::SVMonitorListReader MonitorListReader;
typedef SeidenaderVision::SVSharedPPQReader PPQReader;

struct MonitorListCopy 
{
	typedef std::vector<std::string> Strings;
	Strings  prodItems;
	Strings  rejctCond;
	Strings  failStats;

	std::string m_name;
	std::string m_ppq;
	int m_rejectDepth;
};

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
		std::string cmd = reinterpret_cast<char *>(arr.get());
		//::OutputDebugStringA(cmd.c_str());
		//::OutputDebugStringA("\n");
		Json::Reader reader;
		reader.parse(cmd, val, false); // don't care for the actual command for now
	}
	else
	{
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
	//const std::string quot = "\"";
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
Json::Value & GenerateValues(Json::Value & arr, InspectionDataPtr data)
{
	int count = data->m_TriggerCount;
	//std::for_each(data->m_Values.begin(), data->m_Values.end(),
	for (SVSharedValueMap::const_iterator it = data->m_Values.begin(); it != data->m_Values.end(); ++it)
		//[&arr, count](const SVSharedValuePair & pair)
		{
			const SVSharedValueMap::value_type & pair = *it;
			Json::Value val(Json::objectValue);
			val[SVRC::vo::name] = pair.first.c_str();
			val[SVRC::vo::status] = pair.second.m_Status;
			val[SVRC::vo::count] = count;
			Json::Value value(Json::arrayValue);
			value.append(pair.second.m_Result.c_str());
			val[SVRC::vo::array] = value;
			arr.append(val);
		}
	//);
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
Json::Value & GenerateImages(Json::Value & arr, InspectionDataPtr data)
{
	int count = data->m_TriggerCount;
	//std::for_each(data->m_Images.begin(), data->m_Images.end(),
		//[&arr, count](SVSharedImagePair & pair)
	for (SVSharedImageMap::iterator it = data->m_Images.begin(); it != data->m_Images.end(); ++it)
		{
			SVSharedImageMap::value_type & pair = *it;
			Json::Value val(Json::objectValue);
			val[SVRC::io::name] = pair.first.c_str();
			val[SVRC::io::status] = pair.second.m_Status;
			val[SVRC::io::count] = count;
			AppendImageContents<API>(val, pair.second.m_Filename.c_str());
			arr.append(val);
		}
	//);
	return arr;
}

typedef SeidenaderVision::ProductPtr ProductPtr;

template<typename API>
Json::Value WriteProductItems(ProductPtr product)
{
	Json::Value rslt(Json::objectValue);
	Json::Value values(Json::arrayValue);
	Json::Value images(Json::arrayValue);
	Json::Value errors(Json::arrayValue);
	std::for_each(product->inspections.begin(), product->inspections.end(),
		[&values, &images, &errors](std::pair<const std::string, InspectionDataPtr> & pair) 
		{ 
			GenerateValues(values, pair.second); 
			GenerateImages<API>(images, pair.second); 
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
					vo[SVRC::vo::array] = value.value;
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

template<>
Json::Value DispatchCommand<TcpApi>(const JsonCmd & cmd, const MonitorMapCopy & mlMap)
{
	static std::pair<ProductPtr, long> lastReject(ProductPtr(nullptr), -1);
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
				long idx;
				const ProductPtr product = reader.RequestReject(trig, idx);
				rslt = WriteProductItems<TcpApi>(product);
				reader.ReleaseReject(lastReject.first, lastReject.second);
				lastReject = std::make_pair(product, idx);
				return rslt;
			}
			else
			{
				throw std::exception("Invalid arguments");
			}
		}
	}
	throw std::exception("Invalid command name.");
}

template<>
Json::Value DispatchCommand<UdpApi>(const JsonCmd & cmd, const MonitorMapCopy & mlMap)
{
	static std::pair<ProductPtr, long> lastProduct(ProductPtr(nullptr), -1);
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
				long idx;
				ProductPtr product;
				if (args.isMember(SVRC::arg::trgrCount) && (idx = args[SVRC::arg::trgrCount].asInt()) >= 0) // request locked product by trigger count
				{
					if (lastProduct.second < 0 || lastProduct.first->product.m_TriggerCount != idx)
					{
						throw std::exception("No product for this trigger count.");
					}
					product = lastProduct.first;
					idx = lastProduct.second;
				}
				else // request next product
				{
					product = reader.RequestNextProduct(idx);
				
					rslt = WriteProductItems<UdpApi>(product);
					if (lastProduct.second >= 0)
					{
						reader.ReleaseProduct(lastProduct.first, lastProduct.second);
					}
					lastProduct = std::make_pair(product, idx);
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
	Json::Value rslt(Json::objectValue);
	try
	{
		rslt[SVRC::result::items] = DispatchCommand<API>(cmd, mlMap);
		rsp[SVRC::cmd::reslts] = rslt;
	}
	catch(std::exception & ex)
	{
		rsp[SVRC::cmd::err] = ex.what();
		rsp[SVRC::cmd::hr] = Json::Value(E_FAIL);
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
	cpy.prodItems = MakeCopy(list.GetProductItems());
	cpy.failStats = MakeCopy(list.GetFailStatus());
	cpy.rejctCond = MakeCopy(list.GetRejectCond());
	return cpy;
}

template<typename API>
void servcmd(LPVOID ctrlPtr)
{
	try
	{
		typedef typename Seidenader::Socket::SVServerSocket<API> ServerSocket;
		typedef typename Seidenader::Socket::SVSocket<API> Socket;
		bool lastReady = false;
		ShareControl & ctrl = *reinterpret_cast<ShareControl *>(ctrlPtr);
		MonitorListReader mlReader;
		MonitorMapCopy monitorMap;
		ServerSocket sok;
		sok.Create();
		sok.SetBlocking();
		sok.Listen(port<API>::number);
		while(true)
		{
			sockaddr addr;
			int len = sizeof(sockaddr);
			Socket client = sok.Accept(&addr, &len);
			if (client.IsValidSocket())
			{
				try
				{
					JsonCmd cmd = ReadCommand(client);
					if (!cmd.isNull())
					{
						if (ctrl.IsReady())
						{
							if (!lastReady)
							{
								ctrl.SetAck();
								lastReady = true;
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
									lastReady = true;
							}
							std::string resp = GenerateResponse<API>(cmd, monitorMap);
							client.Write(resp, Traits<API>::needsHeader);
						}
						else
						{
							if (lastReady)
							{
								ctrl.SetAck();
								lastReady = false;
							}
							client.Write(BusyMessage(cmd), Traits<API>::needsHeader);
							Sleep(100);
						}
					}
					else
					{
						std::cout << "null cmd \n";
					}
				}
				catch(std::exception & ex)
				{
					std::cout << ex.what() << std::endl;
				}
			}
			else
			{
				std::cout << "invalid socket\n";
			}
		}
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
	}	
	_endthread();
}

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
		//::ZeroMemory(&version, sizeof(VS_FIXEDFILEINFO));
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

int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr = SeidenaderVision::SVSharedConfiguration::SharedResourcesOk();
	if (hr != S_OK)
	{
		std::string msg;
		if (hr == STG_E_INSUFFICIENTMEMORY)
		{
			msg = "Shared Resources - not enough space available.\n";
		}
		else if (hr == STG_E_PATHNOTFOUND)
		{
			msg = "Shared Resources - Path not found.\n";
		}
		else
		{
			msg = "Shared Resources - missing or inadequate.\n";
		}
		::OutputDebugStringA(msg.c_str());
		std::cout << msg;
		// Messagebox ?
		return -1;
	}
	try
	{
		const std::string& versionStr = GetVersionString();
		std::string title = "Run/Reject Server ";
		title += versionStr;
		SetConsoleTitleA(title.c_str());
		SVSocketLibrary::Init();
		ShareControl ctrl;
		HANDLE threads[3];
		threads[0] = (HANDLE)_beginthread(servimg, 0, nullptr);
		threads[1] = (HANDLE)_beginthread(servcmd<TcpApi>, 0, (void *)&ctrl);
		threads[2] = (HANDLE)_beginthread(servcmd<UdpApi>, 0, (void *)&ctrl);
		::MsgWaitForMultipleObjects(3, threads, FALSE, INFINITE, QS_KEY);
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
