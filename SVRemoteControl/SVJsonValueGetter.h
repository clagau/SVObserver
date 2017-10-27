//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonValueGetter
//* .File Name       : $Workfile:   SVJsonValueGetter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.15  $
//* .Check In Date   : $Date:   09 Dec 2014 07:10:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <exception>
#include <string>
#include <set>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <comdef.h>
#include <Shlwapi.h>

#include "SVControlResponse.h"
#include "SVProductItems.h"
#include "SVRemoteControl.h"
#include "SVRemoteControlConstants.h"
#include "SVValueObject.h"
#include "SVValueObjectList.h"
#include "SVImageObject.h"
#include "SVErrorObject.h"
#include "SVErrorObjectList.h"
#include "SVDataDefObject.h"
#include "SVDataDefObjectList.h"
#pragma endregion Includes


template<typename T>
struct SVJsonValueGetter
{
	
	SVJsonValueGetter(SVControlCommands*  pControlCommands)
	{
		m_pControlCommands = pControlCommands;
	}
	T operator()(const Json::Value & val) const 
	{ 
		throw std::invalid_argument(std::string("Don't know how to convert the type: ") + typeid(T).name()); 
	}
	

	SVControlCommands*  m_pControlCommands;
};

#define SPECIALIZE_GETTER_FOR(type, jsonValue, jsonMethod) \
template<> \
type SVJsonValueGetter<type>::operator ()(const Json::Value &val) const\
{\
	if (val.isConvertibleTo(jsonValue))\
	{\
		return val.jsonMethod();\
	}\
	throw std::invalid_argument("Cannot convert " #jsonValue " to " #type ".");\
}\

SPECIALIZE_GETTER_FOR(int, Json::intValue, asInt)

SPECIALIZE_GETTER_FOR(bool, Json::booleanValue, asBool)

SPECIALIZE_GETTER_FOR(double, Json::realValue, asDouble)

SPECIALIZE_GETTER_FOR(std::string, Json::stringValue, asString)

typedef unsigned int u_int;

SPECIALIZE_GETTER_FOR(u_int, Json::uintValue, asUInt)

// specializations for objects

template<>
CComBSTR SVJsonValueGetter<CComBSTR>::operator()(const Json::Value & val) const
{
	return SVStringConversions::to_utf16(val.asString()).c_str();
}

template<>
CComVariant SVJsonValueGetter<CComVariant>::operator ()(const Json::Value &val) const
{
	CComVariant var;
	switch(val.type())
	{
	case Json::intValue:
		{
			var.intVal = val.asInt();
			var.vt = VT_I4;
			return var;
		}
	case Json::uintValue:
		{
			var.uintVal = val.asUInt();
			var.vt = VT_UI4;
			return var;
		}
	case Json::realValue:
		{
			var.dblVal = val.asDouble();
			var.vt = VT_R8;
			return var;
		}
	case Json::booleanValue:
		{
			var.boolVal = static_cast<BOOL>(val.asBool());
			var.vt = VT_BOOL;
			return var;
		}
	case Json::stringValue:
		{
			_bstr_t bstr(SVStringConversions::to_utf16(val.asString()).c_str());
			var.bstrVal = bstr.Detach();
			var.vt = VT_BSTR;
			return var;
		}
	case Json::arrayValue:
		{
			u_int sz = val.size();
			SAFEARRAY * sa = SafeArrayCreateVector(VT_BSTR, 0, sz);
			BSTR * bstrs = NULL;
			SafeArrayAccessData(sa, reinterpret_cast<void**>(&bstrs));
			for (u_int i = 0; i < sz; ++i)
			{
				bstrs[i] = _bstr_t(SVStringConversions::to_utf16(val[i].asString()).c_str()).Detach();
			}
			SafeArrayUnaccessData(sa);
			var.vt = VT_ARRAY | VT_BSTR;
			var.parray = sa;
			return var;
		}
	}
	throw std::invalid_argument("Cannot convert to VARIANT.");
}
typedef std::set<std::string> string_set;

bool hasA_rec(const Json::Value & val, const string_set & members, string_set::const_iterator it)
{
	return (it == members.end())?true:(val.isMember(*it) && hasA_rec(val, members, ++it));
}

bool hasA(const Json::Value & val, const string_set & members)
{
	return val.isObject() && hasA_rec(val, members, members.begin());
}

typedef CComPtr<ISVValueObject> ValueObjectPtr;

template<>
ValueObjectPtr SVJsonValueGetter<ValueObjectPtr>::operator ()(const Json::Value &val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::vo::name)(SVRC::vo::status)(SVRC::vo::count)(SVRC::vo::array)))
	{
		SVJsonValueGetter<int> intGetter(nullptr);
		SVJsonValueGetter<CComBSTR> stringGetter(nullptr);
		typedef CComObject<SVValueObject> ValueObject;
		ValueObject * vo = 0;
		ValueObject::CreateInstance(&vo);
		ValueObjectPtr pvo(vo);
		vo->put_Name(stringGetter(val[SVRC::vo::name]));
		vo->put_Status(static_cast<LONG>(intGetter(val[SVRC::vo::status])));
		vo->put_TriggerCount(static_cast<LONG>(intGetter(val[SVRC::vo::count])));
		const Json::Value & arr = val[SVRC::vo::array];
		if (arr.isArray())
		{
			SVJsonValueGetter<CComVariant> varGetter(nullptr);
			for (Json::Value::const_iterator it = arr.begin(); it != arr.end(); ++it)
			{
				vo->Add(varGetter(*it));
			}
		}
		return pvo;
	}
	throw std::invalid_argument("Ill-formed value object.");
}

typedef CComPtr<ISVErrorObject> ErrorObjectPtr;

template<>
ErrorObjectPtr SVJsonValueGetter<ErrorObjectPtr>::operator ()(const Json::Value &val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::error::name)(SVRC::error::status)))
	{
		SVJsonValueGetter<int> intGetter(nullptr);
		SVJsonValueGetter<std::string> stringGetter(nullptr);
		typedef CComObject<SVErrorObject> ErrorObject;
		ErrorObject * eo = 0;
		ErrorObject::CreateInstance(&eo);
		ErrorObjectPtr peo(eo);
		eo->put_Name(CComBSTR(stringGetter(val[SVRC::error::name]).c_str()));
		eo->put_Status(static_cast<LONG>(intGetter(val[SVRC::error::status])));
		return peo;
	}
	throw std::invalid_argument("Ill-formed error object.");
}

typedef CComPtr<ISVDataDefObject> DataDefObjectPtr;

template<>
DataDefObjectPtr SVJsonValueGetter<DataDefObjectPtr>::operator ()(const Json::Value &val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::ddlo::name)(SVRC::ddlo::writable)(SVRC::ddlo::published)(SVRC::ddlo::type)(SVRC::ddlo::add_info)))
	{
		SVJsonValueGetter<int> intGetter(nullptr);
		SVJsonValueGetter<std::string> stringGetter(nullptr);
		typedef CComObject<SVDataDefObject> DataDefObject;
		DataDefObject * ddo = 0;
		DataDefObject::CreateInstance(&ddo);
		DataDefObjectPtr pddo(ddo);
		ddo->put_Name(CComBSTR(stringGetter(val[SVRC::ddlo::name]).c_str()));
		ddo->put_Writable(static_cast<VARIANT_BOOL>(intGetter(val[SVRC::ddlo::writable])));
		ddo->put_Published(static_cast<VARIANT_BOOL>(intGetter(val[SVRC::ddlo::published])));
		ddo->put_DataType(CComBSTR(stringGetter(val[SVRC::ddlo::type]).c_str()));
		const Json::Value & arr = val[SVRC::ddlo::add_info];
		if (arr.isArray())
		{
			SVJsonValueGetter<CComVariant> varGetter(nullptr);
			for (Json::Value::const_iterator it = arr.begin(); it != arr.end(); ++it)
			{
				ddo->Add(varGetter(*it));
			}
		}
		return pddo;
	}
	throw std::invalid_argument("Ill-formed data definiton object.");

}

typedef std::pair<ULONG, bytes> ub_pair;

template<>
ub_pair SVJsonValueGetter<ub_pair>::operator()(const Json::Value & val) const
{
	if( val.isString() )
	{
		const char * chars = val.asCString();
		int str_sz = static_cast<int>(strlen(chars));
		int len = ::Base64DecodeGetRequiredLength(str_sz);
		BYTE * buff = new BYTE[len + 1];
		buff[len] = 0;
		bytes arr(buff);
		// base64 decode
		::Base64Decode(chars, str_sz, buff, &len);
		return std::make_pair(static_cast<ULONG>(len), arr);
	}

	return ub_pair( 0, bytes() );
}

typedef CComPtr<ISVImageObject> ImageObjectPtr;

template<>
ImageObjectPtr  SVJsonValueGetter<ImageObjectPtr>::operator ()(const Json::Value & val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::io::name)(SVRC::io::status)(SVRC::io::count)))
	{
		SVJsonValueGetter<int> intGetter(nullptr);
		SVJsonValueGetter<CComBSTR> bstrGetter(nullptr);
		SVJsonValueGetter<std::string> stringGetter(nullptr);
		SVJsonValueGetter<ub_pair> imageGetter(nullptr);

		typedef CComObject<SVImageObject> ImageObject;


		ImageObject * io = 0;
		ImageObject::CreateInstance(&io);
		ImageObjectPtr pio(io);
		
		io->SetImageSok(m_pControlCommands->GetImageSok());
		io->put_Name(bstrGetter(val[SVRC::io::name]));
		io->put_Status(static_cast<LONG>(intGetter(val[SVRC::io::status])));
		io->put_TriggerCount(static_cast<LONG>(intGetter(val[SVRC::io::count])));
		if (hasA(val, boost::assign::list_of(SVRC::io::image)))
		{
			ub_pair p = imageGetter(val[SVRC::io::image]);
			io->SetDIB(p.second);
			io->SetLen(p.first);
		}
		else
		{
			io->SetLen(0);
			if (hasA(val, boost::assign::list_of(SVRC::io::imageFileName)))
			{
				io->SetUrl(stringGetter(val[SVRC::io::imageFileName]));
			}
			if (hasA(val, boost::assign::list_of(SVRC::io::fetch)))
			{
				if ( !SUCCEEDED( io->put_ImageFormat( fetch ) ) )
				{
					throw std::invalid_argument("Problem setting image format.");
				}
			}
		}
		return pio;
	}
	throw std::invalid_argument("Ill-formed image object.");
}

typedef CComPtr<ISVErrorObjectList> ErrorListPtr;

template<>
ErrorListPtr SVJsonValueGetter<ErrorListPtr>::operator ()(const Json::Value &val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::result::errors)))
	{
		typedef CComObject<SVErrorObjectList> ErrorList;
		ErrorList * pi = 0;
		ErrorList::CreateInstance(&pi);
		ErrorListPtr ppi(pi);
		const Json::Value & errors = val[SVRC::result::errors];
		if (errors.isArray())
		{
			SVJsonValueGetter<ErrorObjectPtr> eoGetter(nullptr);
			for (Json::Value::const_iterator it = errors.begin(); it != errors.end(); ++it)
			{
				pi->Add(eoGetter(*it));
			}
		}
		return ppi;
		
	}
	throw std::invalid_argument("Ill-formed ErrorObjectList object.");
}
typedef CComPtr<ISVProductItems> ProductPtr;

template<>
ProductPtr SVJsonValueGetter<ProductPtr>::operator ()(const Json::Value &val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::result::values)(SVRC::result::images)(SVRC::result::errors)))
	{
		typedef CComObject<SVProductItems> Product;
		Product * pi = 0;
		Product::CreateInstance(&pi);
		ProductPtr ppi(pi);
		const Json::Value & values = val[SVRC::result::values];
		if (values.isArray())
		{
			SVJsonValueGetter<ValueObjectPtr> voGetter(nullptr);
			for (Json::Value::const_iterator it = values.begin(); it != values.end(); ++it)
			{
				pi->AddValue(voGetter(*it));
			}
		}
		const Json::Value & images = val[SVRC::result::images];
		if (images.isArray())
		{
			SVJsonValueGetter<ImageObjectPtr> ioGetter(m_pControlCommands);
			for (Json::Value::const_iterator it = images.begin(); it != images.end(); ++it)
			{
				pi->AddImage(ioGetter(*it));
			}
		}
		const Json::Value & errors = val[SVRC::result::errors];
		if (errors.isArray())
		{
			SVJsonValueGetter<ErrorObjectPtr> eoGetter(m_pControlCommands);
			for (Json::Value::const_iterator it = errors.begin(); it != errors.end(); ++it)
			{
				pi->AddError(eoGetter(*it));
			}
		}
		return ppi;
		
	}
	throw std::invalid_argument("Ill-formed ProductItems object.");
}

typedef CComPtr<ISVDataDefObjectList> DataDefListPtr;

template<>
DataDefListPtr SVJsonValueGetter<DataDefListPtr>::operator ()(const Json::Value &val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::result::entries)))
	{
		typedef CComObject<SVDataDefObjectList> DataDefList;
		DataDefList * pi = 0;
		DataDefList::CreateInstance(&pi);
		DataDefListPtr ppi(pi);
		const Json::Value & entries = val[SVRC::result::entries];
		if (entries.isArray())
		{
			SVJsonValueGetter<DataDefObjectPtr> ddoGetter(nullptr);
			for (Json::Value::const_iterator it = entries.begin(); it != entries.end(); ++it)
			{
				pi->Add(ddoGetter(*it));
			}
		}
		return ppi;
	}
	throw std::invalid_argument("Ill-formed DataDefObjectList object.");
}

typedef CComPtr<ISVValueObjectList> ValueObjectListPtr;
typedef CComSafeArray<VARIANT> FailList;

template<>
ValueObjectListPtr SVJsonValueGetter<ValueObjectListPtr>::operator()(const Json::Value & val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::result::entries)))
	{
		CComObject<SVValueObjectList> * vl = 0;
		CComObject<SVValueObjectList>::CreateInstance(&vl);
		ValueObjectListPtr vlp(vl);
		const Json::Value & entries = val[SVRC::result::entries];
		if (entries.isArray())
		{
			SVJsonValueGetter<ValueObjectPtr> voGetter(nullptr);
			for (Json::Value::const_iterator it = entries.begin(); it != entries.end(); ++it)
			{
				vl->Add(voGetter(*it));
			}
		}
		return vlp;
	}
	throw std::invalid_argument("Ill-formed FailStatus list.");
}

template<>
FailList SVJsonValueGetter<FailList>::operator ()(const Json::Value &val) const
{
	if (hasA(val, boost::assign::list_of(SVRC::result::values)))
	{
		FailList list;
		const Json::Value & entries = val[SVRC::result::values];
		
		if (entries.isArray())
		{
			SVJsonValueGetter<ValueObjectListPtr> vlGetter(nullptr);
			int i( 0 );
			list.Create(entries.size(), 0);
			for (Json::Value::const_iterator it = entries.begin(); it != entries.end(); ++it)
			{
				list[i++] = CComVariant(vlGetter(*it));
			}
		}
		return list;
	}
	throw std::invalid_argument("Ill-formed FailStatus list.");
}

struct response_error: std::invalid_argument
{
	response_error(): std::invalid_argument("Json value not a valid SVControlResponse object") {}
};

template<>
struct SVJsonValueGetter<SVControlResponse>
{
	typedef boost::function<void (const Json::Value & val)> resultSetter;
	typedef std::map<std::string, resultSetter> dispMap;

	void SetContents(const Json::Value & val) const
	{
		SVJsonValueGetter<std::string> stringGetter(nullptr);
		if (val.isMember(SVRC::result::contents))
		{
			CComVariant var;
			const std::string& rValue = stringGetter(val[SVRC::result::contents]);
			if (!rValue.empty())
			{
				var.vt = VT_I1|VT_BYREF;
				var.pcVal = (CHAR *)::_strdup( rValue.c_str() );
			}
			resp.results[SVRC::result::contents] = var;
		}
		else
		{
			throw error;
		}
	}

	template<typename T>
	void SetResults(const string_set & args, const Json::Value & val) const
	{
		if (hasA(val, args))
		{
			SVJsonValueGetter<T> getter(m_pControlCommands);
			for (string_set::const_iterator it = args.begin(); it != args.end(); ++ it)
			{
				resp.results[*it] = getter(val[*it]);
			}
		}
		else
		{
			throw error;
		}
	}

	void SetResultsForGetMonitorListProperties(const Json::Value & val) const
	{
		if(!val.isMember(SVRC::result::ppqName) || !val.isMember(SVRC::result::active) || !val.isMember(SVRC::result::rejectDepth))
		{
			throw error;
		}
	
		SVJsonValueGetter<std::string> stringGetter(nullptr);
		SVJsonValueGetter<int> intGetter(nullptr);
		SVJsonValueGetter<bool> boolGetter(nullptr);
		std::string ppQname = stringGetter(val[SVRC::result::ppqName]);
		
		resp.results[SVRC::result::ppqName] =  ppQname.c_str();
		resp.results[SVRC::result::active] = boolGetter(val[SVRC::result::active]);
		resp.results[SVRC::result::rejectDepth] = intGetter(val[SVRC::result::rejectDepth]);

	}




	SVJsonValueGetter(SVControlCommands* pControlCommands) : m_dispatcher(
		boost::assign::map_list_of<std::string, resultSetter>
		(SVRC::cmdName::getItems, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<ProductPtr>,
			this, boost::assign::list_of(SVRC::result::items), _1))
			(SVRC::cmdName::getConfig, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetContents, this, _1))
		(SVRC::cmdName::beginGetCfg, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetContents, this, _1))
		(SVRC::cmdName::getVPName, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
			this, boost::assign::list_of(SVRC::result::name), _1))
			(SVRC::cmdName::getMode, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<int>,
				this, boost::assign::list_of(SVRC::result::mode), _1))
				(SVRC::cmdName::setItems, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<ErrorListPtr>,
					this, boost::assign::list_of(SVRC::result::faults), _1))
					(SVRC::cmdName::regMon, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<ErrorListPtr>,
						this, boost::assign::list_of(SVRC::result::faults), _1))
						(SVRC::cmdName::getState, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<int>,
							this, boost::assign::list_of(SVRC::result::state), _1))
							(SVRC::cmdName::getDefList, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<DataDefListPtr>,
								this, boost::assign::list_of(SVRC::result::items), _1))
								(SVRC::cmdName::getOLCount, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<int>,
									this, boost::assign::list_of(SVRC::result::count), _1))
									(SVRC::cmdName::getReport, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
										this, boost::assign::list_of(SVRC::result::report), _1))
										(SVRC::cmdName::getVersion, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
											this, boost::assign::list_of(SVRC::result::SVO_ver), _1))
											(SVRC::cmdName::qryProd, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
												this, boost::assign::list_of(SVRC::result::names), _1))
												(SVRC::cmdName::qryRjct, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
													this, boost::assign::list_of(SVRC::result::names), _1))
													(SVRC::cmdName::qryFail, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
														this, boost::assign::list_of(SVRC::result::names), _1))
														(SVRC::cmdName::getProd, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<ProductPtr>,
															this, boost::assign::list_of(SVRC::result::items), _1))
															(SVRC::cmdName::getRjct, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<ProductPtr>,
																this, boost::assign::list_of(SVRC::result::items), _1))
		//(SVRC::cmdName::actvMonList, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<int>, 
			//this, boost::assign::list_of(SVRC::result::state), _1))
		//(SVRC::cmdName::deactvList, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>, 
			//this, boost::assign::list_of(SVRC::result::items), _1))
																(SVRC::cmdName::getFail, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<FailList>,
																	this, boost::assign::list_of(SVRC::result::items), _1))
																	(SVRC::cmdName::shutdownSVIM, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<int>,
																		this, boost::assign::list_of(SVRC::result::state), _1))
																		(SVRC::cmdName::qryMonListNames, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
																			this, boost::assign::list_of(SVRC::result::names), _1))
																			(SVRC::cmdName::getInspectionNames, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<CComVariant>,
																				this, boost::assign::list_of(SVRC::result::names), _1))
																				(SVRC::cmdName::getProductFilter, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<int>,
																					this, boost::assign::list_of(SVRC::result::filter), _1))
																					(SVRC::cmdName::getMonitorListPropCmd, boost::bind(&SVJsonValueGetter::SetResultsForGetMonitorListProperties, this, _1))
		(SVRC::cmdName::getMaxRejectDeptCmd, boost::bind(&SVJsonValueGetter<SVControlResponse>::SetResults<int>,
			this, boost::assign::list_of(SVRC::result::maxRejectDepth), _1)).convert_to_container<dispMap>()
			)
	{
		m_pControlCommands = pControlCommands;
	}

	SVControlResponse operator ()(const Json::Value &val) const
	{
		SVJsonValueGetter<int> intGetter(nullptr);
		SVJsonValueGetter<std::string> stringGetter(nullptr);
		if (hasA(val, boost::assign::list_of(SVRC::cmd::id)(SVRC::cmd::name)(SVRC::cmd::hr)))
		{
			resp.cmd_id = intGetter(val[SVRC::cmd::id]);
			resp.cmd_name = stringGetter(val[SVRC::cmd::name]);
			resp.m_Status.errorText = SVStringConversions::to_utf16(stringGetter(val[SVRC::cmd::err]));
			resp.m_Status.hResult = intGetter(val[SVRC::cmd::hr]);
			if (SUCCEEDED(resp.m_Status.hResult) && val.isMember(SVRC::cmd::reslts))
			{
				const Json::Value & res = val[SVRC::cmd::reslts];

				dispMap::const_iterator it = m_dispatcher.find(resp.cmd_name);
				if (it != m_dispatcher.end())
				{
					(it->second)(res);
				}
			}
		}
		else
		{
			throw error;
		}
		return resp;
	}

	private:
		response_error error;
		mutable SVControlResponse resp;
		dispMap m_dispatcher;
		SVControlCommands*  m_pControlCommands;
};

typedef CComPtr<ISVValueObjectList> ValueListPtr;
typedef CComPtr<ISVImageObjectList> ImageListPtr;
