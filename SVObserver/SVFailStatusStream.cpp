//******************************************************************************
//* COPYRIGHT (c) 2014 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFailStatusStream
//* .File Name       : $Workfile:   SVFailStatusStream.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Feb 2015 10:51:10  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include <comdef.h>
#include "SVFailStatusStream.h"

#include "JsonLib/include/json.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"
#pragma endregion

SVFailStatusStream::SVFailStatusStream()
{
}

SVFailStatusStream::~SVFailStatusStream()
{
}

SVFailStatusStream::SVFailStatusStream(const SVFailStatusStream& rhs)
: m_name(rhs.m_name)
, m_list(rhs.m_list)
{
}

SVFailStatusStream& SVFailStatusStream::operator=(const SVFailStatusStream& rhs)
{
	if (this != &rhs)
	{
		m_name = rhs.m_name;
		m_list = rhs.m_list; 
	}
	return *this;
}

HRESULT SVFailStatusStream::Add(const SVString& rName, const MonitoredObjectList& rList)
{
	HRESULT hr = S_OK;
	m_name = rName;

	SVAutoLockAndReleaseTemplate<SVCriticalSection> l_AutoLock;
	if (l_AutoLock.Assign(&m_CriticalSection))
	{
		// convert guids into object references
		m_list.reserve(rList.size());
		for (MonitoredObjectList::const_iterator it = rList.begin();it != rList.end();++it)
		{
			SVObjectReference objRef(SVObjectManagerClass::Instance().GetObject(it->guid));
			if (objRef.Object())
			{
				m_list.push_back(objRef);
			}
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

void SVFailStatusStream::Clear()
{
	m_name.clear();
	m_list.clear();
}

HRESULT SVFailStatusStream::BuildJsonStream(const SVProductInfoStruct& rData, SVJsonCommandServer& server) const
{
	HRESULT hr = S_OK;

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Array(Json::arrayValue);
	long index = rData.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

	if (0 <= index && !m_list.empty())
	{
		SVAutoLockAndReleaseTemplate<SVCriticalSection> l_AutoLock;
		if (l_AutoLock.Assign(&m_CriticalSection))
		{
			for (SVObjectReferenceVector::const_iterator it = m_list.begin(); it != m_list.end(); ++it)
			{
				const SVObjectReference& rObject = *(it);

				if (rObject.Object() != nullptr)
				{
					SVValueObjectClass* pValue = dynamic_cast<SVValueObjectClass *>(rObject.Object());

					if (pValue != nullptr)
					{
						Json::Value elementObject(Json::objectValue);
						Json::Value arrayObject(Json::arrayValue);
						long triggerCount = rData.ProcessCount();
						HRESULT tempStatus = S_OK;

						std::vector<_variant_t> variantVector;
						pValue->GetValues(index, variantVector);
						for (size_t i = 0; (S_OK == tempStatus) && (i < variantVector.size()); ++i)
						{
							switch (variantVector[i].vt)
							{
								case VT_BSTR:
								{
									arrayObject.append(static_cast<LPCTSTR>(_bstr_t(variantVector[i])));
									break;
								}
								case VT_BOOL:
								{
									arrayObject.append(static_cast<bool>(variantVector[i]));
									break;
								}
								case VT_R4:
								case VT_R8:
								{
									arrayObject.append(static_cast<double>(variantVector[i]));
									break;
								}
								case VT_INT:
								case VT_I1:
								case VT_I2:
								case VT_I4:
								case VT_I8:
								{
									arrayObject.append(static_cast<int>(variantVector[i]));
									break;
								}
								case VT_UINT:
								case VT_UI1:
								case VT_UI2:
								case VT_UI4:
								case VT_UI8:
								{
									arrayObject.append(static_cast<unsigned int>(variantVector[i]));
									break;
								}
								default:
								{
									arrayObject.clear();
									triggerCount = -1;
									tempStatus = E_FAIL;
									break;
								}
							}
						}

						elementObject[_T("Name")] = static_cast<LPCTSTR>(pValue->GetCompleteObjectName());
						elementObject[_T("Array")] = arrayObject;
						elementObject[_T("ProcessCount")] = triggerCount;
						elementObject[_T("Status")] = tempStatus;

						l_Array.append(elementObject);
					}
				}
			}
		}
		{
			hr = E_FAIL;
		}
	}

	l_Object[_T("Notification")] = _T("OutputDataItems");
	l_Object[_T("StreamName")] = GetName();
	l_Object[_T("DataItems")] = l_Array;

	const std::string& JsonResults = l_Writer.write(l_Object).c_str();
	if (!JsonResults.empty())
	{
		hr = server.WriteJson(JsonResults);
	}
	return hr;
}

LPCTSTR SVFailStatusStream::GetName() const
{
	return m_name.c_str();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFailStatusStream.cpp_v  $
 * 
 *    Rev 1.0   18 Feb 2015 10:51:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  975
 * SCR Title:  Add Fail Status Stream (SVO-354)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
