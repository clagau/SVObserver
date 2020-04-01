// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataItemManagerTemplate
// * .File Name       : $Workfile:   SVDataItemManagerTemplate.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   02 Oct 2013 08:55:16  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>

template< typename SVItemData, typename SVIndexType = int >
class SVDataItemManagerTemplate
{
public:
	typedef boost::function< DWORD_PTR ( SVIndexType ) > SVGetItemDataFunctor;
	typedef boost::function< BOOL ( SVIndexType, DWORD_PTR ) > SVSetItemDataFunctor;
	typedef std::unordered_map< unsigned long, SVItemData > SVItemDataMap;
	typedef typename SVItemDataMap::iterator iterator;
	typedef typename SVItemDataMap::const_iterator const_iterator;
	typedef typename SVItemDataMap::key_type key_type;
	typedef typename SVItemDataMap::size_type size_type;

	SVDataItemManagerTemplate( SVGetItemDataFunctor GetFunctor, SVSetItemDataFunctor SetFunctor );
	SVDataItemManagerTemplate( const SVDataItemManagerTemplate& p_rObject );
	virtual ~SVDataItemManagerTemplate();

	bool empty() const;
	void clear();

	iterator end();
	const_iterator end() const;

	iterator find( const key_type& p_Keyval );
	const_iterator find( const key_type& p_Keyval ) const;

	void erase( iterator p_Where );
	size_type erase( const key_type& p_Keyval );

	iterator GetItemData( const SVIndexType& p_IndexVal );
	const_iterator GetItemData( const SVIndexType& p_IndexVal ) const;
	bool SetItemData( const SVIndexType& p_IndexVal, const SVItemData& p_rItem );
	void ClearItemData( const SVIndexType& p_IndexVal );

protected:
	SVDataItemManagerTemplate();

	SVGetItemDataFunctor m_GetFunctor;
	SVSetItemDataFunctor m_SetFunctor;

	unsigned long m_LastUsedIdentifier;
	SVItemDataMap m_Items;
};

#include "SVDataItemManagerTemplate.inl"

