//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputBroker
//* .File Name       : $Workfile:   SVOutputBroker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   16 Jan 2015 10:35:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOutputBroker.h"
#include "SVOutputBrokerJsonBuilder.h"
#include "SVProductItems.h"
#include "SVRemoteCtrl.h"
#include "SVStringConversions.h"
#include "JsonDataHelper.h"
#include "Logging.h"
#include "SVRemoteControlConstants.h"
#pragma endregion Includes

using namespace SeidenaderVision;

static const int milliseconds = 1000;

SVOutputBroker::SVOutputBroker()
: m_RemoteCtrl(nullptr)
, m_TimeoutInSeconds(5)
, m_ServerAddress()
, m_ServerPort(svr::notifyPort)
, m_ClientSocket()
, m_Command()
{
	m_ClientSocket.SetConnectionStatusCallback(boost::bind(&SVOutputBroker::OnConnectionStatus, this, _1));
	m_ClientSocket.SetDataReceivedCallback(boost::bind(&SVOutputBroker::OnControlDataReceived, this, _1));

	HKEY Key = nullptr;

	if (ERROR_SUCCESS == ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Seidenader\\SVOutputBroker", NULL, KEY_READ, &Key))
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

HRESULT SVOutputBroker::FinalConstruct()
{
	return S_OK;
}

void SVOutputBroker::FinalRelease()
{
	ClearRemoteControl();
}

STDMETHODIMP SVOutputBroker::get_RemoteCtrl(ISVRemoteCtrl** ppVal)
{
	HRESULT hr = S_OK;

	if (nullptr!= ppVal)
	{
		if (nullptr != *ppVal)
		{
			(*ppVal)->Release();
			*ppVal = nullptr;
		}

		if (nullptr != m_RemoteCtrl.p)
		{
			hr = m_RemoteCtrl.QueryInterface(ppVal);
			SVLOG(hr);
		}
	}
	else
	{
		hr = E_INVALIDARG;
		SVLOG(hr);
	}
	return hr;
}

STDMETHODIMP SVOutputBroker::put_RemoteCtrl(ISVRemoteCtrl* newVal)
{
	HRESULT hr = AssignRemoteControl(newVal);
	SVLOG(hr);

	return hr;
}

STDMETHODIMP SVOutputBroker::get_TimeoutInSeconds(long* pVal)
{
	HRESULT hr = S_OK;

	*pVal = m_TimeoutInSeconds;

	return hr;
}

STDMETHODIMP SVOutputBroker::put_TimeoutInSeconds(long Val)
{
	HRESULT hr = S_OK;

	m_TimeoutInSeconds = Val;

	return hr;
}

STDMETHODIMP SVOutputBroker::Start(BSTR ListName)
{
	HRESULT hr = S_OK;

	_bstr_t remoteOutputListName = ListName;

	if (remoteOutputListName.length() > 0)
	{
		if (m_Command.IsJsonCommandEmpty())
		{
			const std::string& JsonCommand = SVOutputBrokerJsonBuilder::BuildStartCommand(static_cast<wchar_t*>(remoteOutputListName));
			if (!JsonCommand.empty())
			{
				hr = m_Command.SetJsonCommand(JsonCommand, m_TimeoutInSeconds * milliseconds);
				SVLOG(hr);

				if (S_OK == hr)
				{
					hr = WriteToSocket(JsonCommand);
					SVLOG(hr);

					if (S_OK == hr)
					{
						hr = m_Command.WaitForRequest(m_TimeoutInSeconds * milliseconds);
						SVLOG(hr);
					}
					if (S_OK == hr)
					{
						// check for errors...
						hr = m_Command.ExtractStatus(SVRC::stream::status);
					}
					m_Command.ClearJsonCommand();
				}
			}
			else
			{
				hr = INET_E_INVALID_REQUEST;
				SVLOG(hr);
			}
		}
		else
		{
			hr = INET_E_INVALID_REQUEST;
			SVLOG(hr);
		}
	}
	else
	{
		hr = E_INVALIDARG;
		SVLOG(hr);
	}
	return hr;
}

static HRESULT CheckErrors()
{
	HRESULT hr = S_OK;
	return hr;
}

STDMETHODIMP SVOutputBroker::Stop(BSTR ListName)
{
	HRESULT hr = S_OK;

	_bstr_t remoteOutputListName = ListName;

	if (remoteOutputListName.length() > 0)
	{
		if (m_Command.IsJsonCommandEmpty())
		{
			const std::string& JsonCommand = SVOutputBrokerJsonBuilder::BuildStopCommand(static_cast<const wchar_t*>(remoteOutputListName));
			if (!JsonCommand.empty())
			{
				hr = m_Command.SetJsonCommand(JsonCommand, m_TimeoutInSeconds * milliseconds);
				SVLOG(hr);

				if (S_OK == hr)
				{
					hr = WriteToSocket(JsonCommand);
					SVLOG(hr);

					if (S_OK == hr)
					{
						hr = m_Command.WaitForRequest(m_TimeoutInSeconds * milliseconds);
						SVLOG(hr);
					}
					if (S_OK == hr)
					{
						// check for errors...
						hr = m_Command.ExtractStatus(SVRC::stream::status);
					}
					m_Command.ClearJsonCommand();
				}
			}
			else
			{
				hr = INET_E_INVALID_REQUEST;
				SVLOG(hr);
			}
		}
		else
		{
			hr = INET_E_INVALID_REQUEST;
			SVLOG(hr);
		}
	}
	else
	{
		hr = E_INVALIDARG;
		SVLOG(hr);
	}
	return hr;
}

STDMETHODIMP SVOutputBroker::QueryListNames(SAFEARRAY** ppNames)
{
	HRESULT hr = S_OK;

	if (nullptr != ppNames)
	{
		CComSafeArrayBound bounds(0, 1);
		CComSafeArray<BSTR> nameArray(bounds);

		if (nullptr != *ppNames)
		{
			::SafeArrayDestroy(*ppNames);
			*ppNames = nullptr;
		}

		if (m_Command.IsJsonCommandEmpty())
		{
			const std::string& JsonCommand = SVOutputBrokerJsonBuilder::BuildQueryListNamesCommand();
			if (!JsonCommand.empty())
			{
				hr = m_Command.SetJsonCommand(JsonCommand, m_TimeoutInSeconds * milliseconds);
				SVLOG(hr);

				if (S_OK == hr)
				{
					hr = WriteToSocket(JsonCommand);
					SVLOG(hr);

					if (S_OK == hr)
					{
						hr = m_Command.WaitForRequest(m_TimeoutInSeconds * milliseconds);
						SVLOG(hr);
					}

					if (S_OK == hr)
					{
						const Json::Value& JsonValues = m_Command.GetJsonResults();
						if (!JsonValues.empty() && JsonValues.isObject())
						{
							const Json::Value& JsonDataItems = JsonValues[SVRC::iobroker::streamNames];

							if (JsonDataItems.isArray())
							{
								for (Json::Value::iterator it = JsonDataItems.begin(); it != JsonDataItems.end();++it)
								{
									HRESULT tempHR = S_OK;

									const Json::Value& JsonItem = (*it);

									if (JsonItem.isString())
									{
										_bstr_t Value = SVStringConversions::to_utf16(JsonItem.asString()).c_str();
										tempHR = nameArray.Add(Value.Detach());
									}
									else
									{
										tempHR = E_INVALIDARG;
									}

									if (S_OK == hr)
									{
										hr = tempHR;
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
						else
						{
							hr = INET_E_INVALID_REQUEST;
							SVLOG(hr);
						}
					}
					m_Command.ClearJsonCommand();
				}
			}
			else
			{
				hr = INET_E_INVALID_REQUEST;
				SVLOG(hr);
			}
		}
		else
		{
			hr = INET_E_INVALID_REQUEST;
			SVLOG(hr);
		}
		*ppNames = nameArray.Detach();
	}
	else
	{
		hr = E_INVALIDARG;
		SVLOG(hr);
	}
	return hr;
}

STDMETHODIMP SVOutputBroker::QueryListItems(BSTR ListName, SAFEARRAY** ppNames)
{
	HRESULT hr = S_OK;

	_bstr_t remoteOutputListName = ListName;

	if (nullptr != ppNames)
	{
		CComSafeArrayBound bounds(0, 1);
		CComSafeArray<BSTR> itemArray(bounds);

		if (nullptr != *ppNames)
		{
			::SafeArrayDestroy(*ppNames);
			*ppNames = nullptr;
		}
		
		if (remoteOutputListName.length() > 0)
		{
			if (m_Command.IsJsonCommandEmpty())
			{
				const std::string& JsonCommand = SVOutputBrokerJsonBuilder::BuildQueryListItemsCommand(static_cast<const wchar_t*>(remoteOutputListName));
				if (!JsonCommand.empty())
				{
					hr = m_Command.SetJsonCommand(JsonCommand, m_TimeoutInSeconds * milliseconds);
					SVLOG(hr);

					if (S_OK == hr)
					{
						hr = WriteToSocket(JsonCommand);
						SVLOG(hr);

						if (S_OK == hr)
						{
							hr = m_Command.WaitForRequest(m_TimeoutInSeconds * milliseconds);
							SVLOG(hr);
						}

						if (S_OK == hr)
						{
							// check for errors...
							hr = m_Command.ExtractStatus(SVRC::stream::status);

							const Json::Value& JsonValues = m_Command.GetJsonResults();
							if (!JsonValues.empty() && JsonValues.isObject())
							{
								const Json::Value& JsonDataItems = JsonValues[SVRC::stream::dataItems];

								if (JsonDataItems.isArray())
								{
									for (Json::Value::const_iterator it = JsonDataItems.begin();it != JsonDataItems.end();++it)
									{
										HRESULT tempHR = S_OK;

										const Json::Value& JsonItem = (*it);

										if (JsonItem.isString())
										{
											_bstr_t Value = SVStringConversions::to_utf16(JsonItem.asString()).c_str();
											tempHR = itemArray.Add(Value.Detach());
										}
										else
										{
											tempHR = E_INVALIDARG;
										}

										if (S_OK == hr)
										{
											hr = tempHR;
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
							else
							{
								hr = INET_E_INVALID_REQUEST;
								SVLOG(hr);
							}
						}
						m_Command.ClearJsonCommand();
					}
				}
				else
				{
					hr = INET_E_INVALID_REQUEST;
					SVLOG(hr);
				}
			}
			else
			{
				hr = INET_E_INVALID_REQUEST;
				SVLOG(hr);
			}
		}
		else
		{
			hr = E_INVALIDARG;
			SVLOG(hr);
		}
		*ppNames = itemArray.Detach();
	}
	else
	{
		hr = E_INVALIDARG;
		SVLOG(hr);
	}

	return hr;
}

HRESULT SVOutputBroker::AssignRemoteControl(ISVRemoteCtrl* newVal)
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

void SVOutputBroker::ClearRemoteControl()
{
	m_ClientSocket.Disconnect();

	m_RemoteCtrl.Release();
}

HRESULT SVOutputBroker::CheckSocketConnection()
{
	HRESULT hr = S_OK;

	if (nullptr != m_RemoteCtrl.p)
	{
		_bstr_t addr;

		hr = m_RemoteCtrl->get_VPAddress(addr.GetAddress());
		SVLOG(hr);

		if (S_OK == hr)
		{
			if (addr != m_ServerAddress || !m_ClientSocket.IsConnected())
			{
				SvSol::SVSocketError::ErrorEnum err = m_ClientSocket.BuildConnection(addr, m_ServerPort, (m_TimeoutInSeconds * milliseconds));
				if (SvSol::SVSocketError::Success == err)
				{
					m_ServerAddress = addr;
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

HRESULT SVOutputBroker::WriteToSocket(const std::string& data)
{
	HRESULT hr = CheckSocketConnection();
	SVLOG(hr);

	if (S_OK == hr)
	{
		hr = SvSol::SVSocketError::HrFromSocketError(m_ClientSocket.Write(data));
		SVLOG(hr);
	}
	else
	{
		hr = E_NOINTERFACE;
		SVLOG(hr);
	}
	return hr;
}

void SVOutputBroker::OnConnectionStatus(SvSol::ConnectionState state)
{
}

void SVOutputBroker::OnControlDataReceived(std::istream& is)
{
	// parse the json first!
	Json::Reader Reader;
	Json::Value JsonValues;

	if (Reader.parse(is, JsonValues, false))
	{
		if (JsonDataHelper::IsNotification(JsonValues))
		{
			if (nullptr!= m_RemoteCtrl.p)
			{
				HRESULT hr = SendOutputStreamData(JsonValues);
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

HRESULT SVOutputBroker::SendOutputStreamData(const Json::Value& rJsonValues)
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

HRESULT SVOutputBroker::BuildDataStream(ISVProductItems* pProductItems, const Json::Value& rJsonValue)
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
										const Json::Value& arrayItem  = arrayValue[i];

										if (arrayItem.isString())
										{
											variantValue = SVStringConversions::to_utf16(arrayItem .asString()).c_str();
										}
										else if (arrayItem .isInt())
										{
											variantValue = arrayItem .asInt();
										}
										else if (arrayItem.isBool())
										{
											variantValue = arrayItem .asBool();
										}
										else if (arrayItem.isDouble())
										{
											variantValue = arrayItem .asDouble();
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

								// In the first release of the OutputBroker the Tag was "TriggerCount".
								// It was changed to "ProcessCount" for consistentcy with the various other populations of a ValueObject in JSON.
								// Could be "TriggerCount" (old) or "ProcessCount" (new).
								const Json::Value& triggerCountValue = item[SVRC::iobroker::triggerCount];
								const Json::Value& countValue = item[SVRC::vo::count];
								if (S_OK == tempHR && triggerCountValue.isNumeric())
								{
									valueObject->put_TriggerCount(triggerCountValue.asInt());
								}
								else if (S_OK == tempHR && countValue.isNumeric())
								{
									valueObject->put_TriggerCount(countValue.asInt());
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
									tempHR = statusValue.asUInt();
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
