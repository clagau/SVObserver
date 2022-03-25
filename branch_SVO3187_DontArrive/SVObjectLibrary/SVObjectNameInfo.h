//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectNameInfo
//* .File Name       : $Workfile:   SVObjectNameInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Mar 2014 14:19:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <set>
#pragma endregion Includes

struct SVObjectNameInfo
{
	typedef std::vector<std::string> SVNameVector;

	static HRESULT ParseObjectName(SVObjectNameInfo& rNameInfo, const std::string& p_rObjectName);

	SVObjectNameInfo();
	explicit SVObjectNameInfo(const SVNameVector& rNameArray);
	SVObjectNameInfo(const SVNameVector& rNameArray, const std::string& rIndex);
	SVObjectNameInfo(const SVNameVector& rNameArray, const std::string& rIndex, const std::string& rDefaultValue);
	SVObjectNameInfo(const SVObjectNameInfo& rNameInfo);

	~SVObjectNameInfo();

	void clear();

	HRESULT ParseObjectName(const std::string& rObjectName);

	std::string GetObjectName(size_t p_StartIndex = 0) const;
	std::string GetObjectArrayName(size_t p_StartIndex = 0) const;

	void RemoveTopName();
	void RemoveBottomName();
	long GetIndexValue() const;
	
	//************************************
	//! Set function for m_Index
	//! \param Index [in]
	//! \returns void
	//************************************
	void SetIndex(const std::string& Index ) {m_Index = Index;}
	
	//************************************
	//! Get function for m_Index 
	//! \returns const std::string&
	//************************************
	const std::string&      GetIndex() const {return m_Index;} 

	//************************************
	//! Get function for m_IndexPresent
	//! \returns bool
	//************************************
	bool IsIndexPresent() const  {return m_IndexPresent; } ;
	
	//************************************
	//! Set function for m_IndexPresent
	//! \param ispresent [in]
	//! \returns void
	//************************************
	void SetIsIndexPresent(bool ispresent) {m_IndexPresent = ispresent;}
	
	//************************************
	//! Clears the index variable
	//! \returns void
	//************************************
	void ClearIndex() {m_Index.clear();};
	
	//************************************
	//! true if m_index is empty 
	//! \returns bool
	//************************************
	bool IsIndexEmpty()  const {return m_Index.empty() ;}

	bool IsDefaultValuePresent() const  {return m_DefaultValuePresent; } ;
	void SetIsDefaultValuePresent(bool ispresent) {m_DefaultValuePresent = ispresent;}
	
	const std::string&      GetDefaultValue() const {return m_DefaultValue;} 
	void SetDefaultValue(const std::string& DefaultValue ) {m_DefaultValue = DefaultValue;}

	SVNameVector m_NameArray;

private:
	bool m_IndexPresent; /// true for arrays 
	std::string m_Index; /// one based index or empty for non array names or whole arrays 

	bool m_DefaultValuePresent;
	std::string m_DefaultValue;
};

typedef std::set< SVObjectNameInfo > SVObjectNameInfoSet;
