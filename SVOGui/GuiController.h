//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the GuiController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include "ObjectInterfaces\NameValueList.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		template <typename Command, typename Model>
		class GuiController : public Command, public boost::noncopyable
		{
			mutable Model m_Data;
		
		public:
			GuiController(const Model& rModel) : m_Data(rModel) {}
			virtual ~GuiController() {}

			virtual HRESULT Init()
			{
				return GetValues(m_Data);
			}
			
			template<typename DataType>
			DataType Get(const std::string& name) const
			{
				return static_cast<DataType>(boost::any_cast<_variant_t>(m_Data.GetValue(name)));
			}

			template<typename DataType>
			void Set(const std::string& name, const DataType& rValue)
			{
				_variant_t v(rValue);
				m_Data.SetValue(name, boost::any(v));
			}

			SvOi::NameValueList GetEnumTypes(const std::string& name) const
			{
				return GetEnums(m_Data.GetInspectionID(), m_Data.GetObjectID(name));
			}

			HRESULT Commit(bool bReset = false)
			{
				return SetValues(m_Data, bReset);
			}
		};
	}
}

namespace SvOg = Seidenader::SVOGui;