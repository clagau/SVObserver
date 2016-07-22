//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

struct SVObjectNameInfo
{
	typedef std::deque< SVString > SVNameDeque;

	static HRESULT ParseObjectName( SVObjectNameInfo& p_rNameInfo, const SVString& p_rObjectName );

	SVObjectNameInfo();
	SVObjectNameInfo( const SVNameDeque& p_rNameArray );
	SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex );
	SVObjectNameInfo( const SVNameDeque& p_rNameArray, const SVString& p_rIndex, const SVString& p_rDefaultValue );
	SVObjectNameInfo( const SVObjectNameInfo& p_rObject );

	void clear();

	HRESULT ParseObjectName( const SVString& p_rObjectName );

	SVString GetObjectName( size_t p_StartIndex = 0 ) const;
	SVString GetObjectArrayName( size_t p_StartIndex = 0 ) const;

	void RemoveTopName();
	void RemoveBottomName();
	long GetIndexValue() const;
	
	//************************************
	//! Set function for m_Index
	//! \param Index [in]
	//! \returns void
	//************************************
	void SetIndex(const SVString& Index ) {m_Index = Index;}
	
	//************************************
	//! Get Funtionn for m_Index 
	//! \returns const SVString&
	//************************************
	const SVString&      GetIndex() const {return m_Index;} 

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
	
	const SVString&      GetDefaultValue() const {return m_DefaultValue;} 
	void SetDefaultValue(const SVString& DefaultValue ) {m_DefaultValue = DefaultValue;}

	SVNameDeque m_NameArray;

private:
	bool m_IndexPresent; /// true for arrays 
	SVString m_Index; /// one based index or empty for non array names or whole arrays 

	bool m_DefaultValuePresent;
	SVString m_DefaultValue;
};

typedef std::set< SVObjectNameInfo > SVObjectNameInfoSet;
