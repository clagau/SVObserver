//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFailStatusStream
//* .File Name       : $Workfile:   SVFailStatusStream.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   20 Jan 2015 17:37:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <atlsafe.h>
#include "SVFailStatusStream.h"
#include "SVStartStopStreamJsonBuilder.h"
#include "SVRemoteControlConstants.h"
#include "JsonDataHelper.h"
#include "SVProductItems.h"
#include "SVRemoteCtrl.h"
#include "SVStringConversions.h"
#include "Logging.h"
#pragma endregion Includes

using namespace SeidenaderVision;

static const int milliseconds = 1000;

SVFailStatusStream::SVFailStatusStream()
: m_RemoteCtrl(nullptr)
, m_TimeoutInSeconds(5)
, m_ServerAddress()
, m_ServerPort(svr::failStatusPort)
, m_ClientSocket()
, m_Command()
{
	m_ClientSocket.SetConnectionStatusCallback(boost::bind(&SVFailStatusStream::OnConnectionStatus, this, _1));
	m_ClientSocket.SetDataReceivedCallback(boost::bind(&SVFailStatusStream::OnControlDataReceived, this, _1));

	HKEY Key = nullptr;

	if (ERROR_SUCCESS == ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Seidenader\\SVFailStatusStream", NULL, KEY_READ, &Key))
	{
		DWORD Type = 0;
		DWORD Value = 0;
		DWORD Size = sizeof(Value);
		
		if (ERROR_SUCCESS == ::RegQueryValueExW(Key, L"CommandPort", NULL, &Type, reinterpret_cast<BYTE*>(&Value), &Size))
		{
			m_ServerPort = static_cast<unsigned short>(Value);
		}
	}
}

HRESULT SVFailStatusStream::FinalConstruct()
{
	return S_OK;
}

void SVFailStatusStream::FinalRelease()
{
	ClearRemoteControl();
}



STDMETHODIMP SVFailStatusStream::get_TimeoutInSeconds(long* pVal)
{
	HRESULT hr = S_OK;
	*pVal = m_TimeoutInSeconds;
	return hr;
}

STDMETHODIMP SVFailStatusStream::put_TimeoutInSeconds(long Val)
{
	HRESULT hr = S_OK;
	m_TimeoutInSeconds = Val;
	return hr;
}



HRESULT SVFailStatusStream::AssignRemoteControl(ISVRemoteCtrl* newVal)
{
	HRESULT hr = S_OK;

	ClearRemoteControl();

	if (nullptr != newVal)
	{
		m_RemoteCtrl = newVal;
		hr = CheckSocketConnection();
	}
	return hr;
}

void SVFailStatusStream::ClearRemoteControl()
{
	m_ClientSocket.Disconnect();
	m_RemoteCtrl.Release();
}

HRESULT SVFailStatusStream::CheckSocketConnection()
{
	HRESULT hr = S_OK;

	if (nullptr != m_RemoteCtrl.p)
	{
		_bstr_t Addr;

		hr = m_RemoteCtrl->get_VPAddress(Addr.GetAddress());
		SVLOG(hr);

		if (S_OK == hr)
		{
			if (Addr != m_ServerAddress || !m_ClientSocket.IsConnected())
			{
				SvSol::SVSocketError::ErrorEnum err = m_ClientSocket.BuildConnection(Addr, m_ServerPort, m_TimeoutInSeconds * milliseconds);
				if (SvSol::SVSocketError::Success == err)
				{
					m_ServerAddress = Addr;
				}
				else
				{
					hr = SvSol::SVSocketError::HrFromSocketError(err);
					SVLOG(hr);
				}
			}
		}
	}
	else
	{
		hr = E_NOINTERFACE;
		SVLOG(hr);
	}
	return hr;
}

HRESULT SVFailStatusStream::WriteToSocket(const std::string& data)
{
	HRESULT hr = CheckSocketConnection();
	SVLOG(hr);

	if (S_OK == hr)
	{
			hr = SvSol::SVSocketError::HrFromSocketError(m_ClientSocket.Write(data));
			SVLOG(hr);
	}
	return hr;
}

void SVFailStatusStream::OnConnectionStatus(SvSol::ConnectionState state)
{
}

void SVFailStatusStream::OnControlDataReceived(std::istream& is)
{
	// parse the json first!
	Json::Reader Reader;
	Json::Value JsonValues;

	if (Reader.parse(is, JsonValues, false))
	{
		if (JsonDataHelper::IsNotification(JsonValues))
		{
			if (nullptr != m_RemoteCtrl.p)
			{
				HRESULT hr = SendFailStatusStreamData(JsonValues);
			}
		}
		else if (JsonDataHelper::IsResponse(JsonValues))
		{
			std::string JsonCommand;

			if (S_OK == m_Command.GetJsonCommand(JsonCommand, INFINITE))
			{
				Json::Value JsonCommandValues;

				if (Reader.parse(JsonCommand, JsonCommandValues, false))
				{
					bool bNotify = JsonDataHelper::DoesCommandMatchResponse(JsonCommandValues, JsonValues);
					if (bNotify)
					{
						bNotify = (S_OK == m_Command.SetJsonResults(JsonValues));
					}
					if (bNotify)
					{
						m_Command.NotifyRequestComplete();
					}
				}
			}
		}
	}
}

HRESULT SVFailStatusStream::SendFailStatusStreamData(const Json::Value& rJsonValues)
{
	IConnectionPointContainer* pConnectionPointContainer = nullptr;
	HRESULT hr = m_RemoteCtrl.QueryInterface<IConnectionPointContainer>(&pConnectionPointContainer);
	SVLOG(hr);

	if (nullptr != pConnectionPointContainer)
	{
		CComPtr<ISVProductItems> productPtr;

		hr = productPtr.CoCreateInstance(__uuidof(SVProductItems));
		SVLOG(hr);
		if (S_OK == hr && nullptr != productPtr.p)
		{
			hr = BuildDataStream(productPtr, rJsonValues);
			SVLOG(hr);
			if (S_OK == hr)
			{
				Fire_OnDataArrive(productPtr);
			}
		}
		pConnectionPointContainer->Release();
	}
	return hr;
}

HRESULT SVFailStatusStream::BuildDataStream(ISVProductItems* pProductItems, const Json::Value& rJsonValue)
{
	HRESULT hr = S_OK;

	if (nullptr != pProductItems)
	{
		CComPtr<ISVValueObjectList> valueObjectListPtr;
		
		hr = valueObjectListPtr.CoCreateInstance(__uuidof(SVValueObjectList));
		SVLOG(hr);

		if (S_OK == hr && nullptr != valueObjectListPtr.p)
		{
			const Json::Value& dataItems = rJsonValue[SVRC::stream::dataItems];

			if (dataItems.isArray())
			{
				for (Json::Value::const_iterator it = dataItems.begin();it != dataItems.end();++it)
				{
					const Json::Value& item = (*it);
					if (item.isObject())
					{
						CComPtr<ISVValueObject> valueObject;
						HRESULT tempHR = valueObject.CoCreateInstance(__uuidof(SVValueObject));
						if (S_OK == tempHR && nullptr != valueObject.p)
						{
							const Json::Value& nameValue = item[SVRC::vo::name];
							if (nameValue.isString())
							{
								_bstr_t name(SVStringConversions::to_utf16(nameValue.asString()).c_str());
								valueObject->put_Name(name.GetBSTR());
							}
							else
							{
								tempHR = E_INVALIDARG;
							}

							if (S_OK == tempHR)
							{
								const Json::Value& arrayValue = item[SVRC::vo::array];
								if (arrayValue.isArray())
								{
									for (unsigned int i = 0;S_OK == tempHR && i < arrayValue.size();++i)
									{
										_variant_t variantValue;
										const Json::Value& arrayItem = arrayValue[i];

										if (arrayItem.isString())
										{
											variantValue = SVStringConversions::to_utf16(arrayItem.asString()).c_str();
										}
										else if (arrayItem.isInt())
										{
											variantValue = arrayItem.asInt();
										}
										else if (arrayItem.isBool())
										{
											variantValue = arrayItem.asBool();
										}
										else if (arrayItem.isDouble())
										{
											variantValue = arrayItem.asDouble();
										}
										else
										{
											tempHR = E_INVALIDARG;
										}
										valueObject->Add(variantValue.Detach());
									}
								}
								else
								{
									tempHR = E_INVALIDARG;
								}

								const Json::Value& productCountValue = item[SVRC::vo::count];
								if (S_OK == tempHR && productCountValue.isNumeric())
								{
									valueObject->put_TriggerCount(productCountValue.asInt());
								}
								else
								{
									valueObject->put_TriggerCount(-1);

									if (S_OK == tempHR)
									{
										tempHR = E_INVALIDARG;
									}
								}

								const Json::Value& statusValue = item[SVRC::vo::status];
								if (S_OK == tempHR && statusValue.isNumeric())
								{
									tempHR = statusValue.asInt();
								}
								else
								{
									if (S_OK == tempHR)
									{
										tempHR = E_INVALIDARG;
									}
								}

								SVLOG(tempHR);
								valueObject->put_Status(tempHR);
								valueObjectListPtr->Add(valueObject);
							}
						}
						else
						{
							if (S_OK == hr)
							{
								hr = E_NOINTERFACE;
								SVLOG(hr);
							}
						}
					}
					else
					{
						hr = E_INVALIDARG;
						SVLOG(hr);
					}
				}
			}
			else
			{
				hr = S_FALSE;
			}
		}
		else
		{
			if (S_OK == hr)
			{
				hr = E_NOINTERFACE;
				SVLOG(hr);
			}
		}

		if (S_OK == hr)
		{
			pProductItems->put_Values(valueObjectListPtr.p);
		}
	}
	else
	{
		hr = E_NOINTERFACE;
		SVLOG(hr);
	}
	return hr;
}
