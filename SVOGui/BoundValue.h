//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Bound Value class for use by thye GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>
#include <map>
#include <string>
#include <comdef.h>
#include <guiddef.h>
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class BoundValue
		{
		private:
			GUID m_embeddedID; // maybe use SVObjectTypeInfo instead ?
			GUID m_instanceID;
			SVObjectTypeInfoStruct m_ownerInfo; // to access an object within the heirarchy owned at a certain level
			mutable boost::any m_value;
			bool m_bReadOnly;
			mutable bool m_bModified;

		public:
			BoundValue() : m_embeddedID(GUID_NULL), m_instanceID(GUID_NULL), m_bReadOnly(false), m_bModified(false) {} // for map...
			
			BoundValue(const GUID& rEmbeddedID, bool bReadOnly = false) 
			: m_embeddedID(rEmbeddedID), m_instanceID(GUID_NULL), m_bReadOnly(bReadOnly), m_bModified(false) {}

			BoundValue(const GUID& rEmbeddedID, const SVObjectTypeInfoStruct& ownerInfo, bool bReadOnly = false) 
			: m_embeddedID(rEmbeddedID), m_instanceID(GUID_NULL), m_ownerInfo(ownerInfo), m_bReadOnly(bReadOnly), m_bModified(false) {}

			BoundValue(const GUID& rEmbeddedID, const GUID& rInstanceID, const boost::any& value, const SVObjectTypeInfoStruct& ownerInfo, bool bReadOnly = false) 
			: m_embeddedID(rEmbeddedID), m_instanceID(rInstanceID),  m_value(value), m_ownerInfo(ownerInfo), m_bReadOnly(bReadOnly), m_bModified(false) {}
			
			~BoundValue() { m_value.clear(); }

			const boost::any& GetValue() const { return m_value; }
			void SetValue(const boost::any& value) const { m_value = value; m_bModified = true; }

			const GUID& GetEmbeddedID() const { return m_embeddedID; }
			const GUID& GetObjectID() const { return m_instanceID; }
			const SVObjectTypeInfoStruct& GetOwnerInfo() const { return m_ownerInfo; }

			bool isReadOnly() const { return m_bReadOnly; }
			bool isModified() const { return m_bModified; }
			void ClearModified() { m_bModified = false; }
		};

		class BoundValues
		{
		public:
			typedef std::map<std::string, BoundValue> Container;
			typedef Container::iterator iterator;
			typedef Container::const_iterator const_iterator;
			typedef Container::value_type value_type;
			typedef Container::mapped_type mapped_type;

			BoundValues() : m_inspectionID(GUID_NULL), m_ownerID(GUID_NULL) {}
			BoundValues(const GUID& inspectionID, const GUID& ownerID, const Container& values)
			: m_inspectionID(inspectionID), m_ownerID(ownerID), m_values(values) {}
			~BoundValues() { m_values.clear(); }

			const GUID& GetOwnerID() const { return m_ownerID; }
			const GUID& GetInspectionID() const { return m_inspectionID; }

			void SetValue(const std::string& name, const BoundValue& value) { m_values[name].SetValue(value); }
			void SetValue(const std::string& name, const boost::any& value) { m_values[name].SetValue(value); }

			const boost::any& GetValue(const std::string& name) const 
			{ 
				Container::const_iterator it = m_values.find(name); 
				if (it != m_values.end()) 
				{
					return it->second.GetValue();
				}
				throw std::out_of_range(name.c_str()); 
			}

			GUID GetObjectID(const std::string& name) const 
			{ 
				GUID objectID = GUID_NULL;
				Container::const_iterator it = m_values.find(name);
				if (it != m_values.end())
				{
					objectID = it->second.GetObjectID();
				}
				return objectID;
			}

			iterator begin() { return m_values.begin(); }
			iterator end() { return m_values.end(); }
			const_iterator begin() const { return m_values.begin(); }
			const_iterator end() const { return m_values.end(); }

		private:
			Container m_values;
			GUID m_ownerID;
			GUID m_inspectionID;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;