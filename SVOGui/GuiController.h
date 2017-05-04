//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the GuiController class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/any.hpp>
//Moved to precompiled header: #include <boost/noncopyable.hpp>
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\NameValueList.h"
#include "SVStatusLibrary\MessageContainer.h"
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

			HRESULT Init()
			{
				return GetValues(m_Data);
			}
			
			template<typename DataType>
			DataType Get(const SVString& rName) const
			{
				return static_cast<DataType>(boost::any_cast<_variant_t>(m_Data.GetValue(rName)));
			}

			template<typename DataType>
			void Set(const SVString& rName, const DataType& rValue)
			{
				_variant_t v(rValue);
				m_Data.SetValue(rName, boost::any(v));
			}

			SvOi::NameValueList GetEnumTypes(const SVString& rName) const
			{
				return GetEnums(m_Data.GetInspectionID(), m_Data.GetObjectID(rName));
			}

			SVString GetName(const SVString& rName) const
			{
				return GetObjectName(m_Data.GetInspectionID(), m_Data.GetObjectID(rName));
			}

			HRESULT Commit(bool bReset = false)
			{
				return SetValues(m_Data, bReset);
			}

			SvStl::MessageContainerVector getCommitErrorList()
			{
				return getSetFailedMessageList();
			}

			const GUID& GetOwnerID() const { return m_Data.GetOwnerID(); };
		};
	}
}

namespace SvOg = Seidenader::SVOGui;