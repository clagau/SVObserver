//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVector
//* .File Name       : $Workfile:   SVVector.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2013 14:41:50  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <BaseTsd.h>

template< typename SVT_TYPE, typename SVT_ARG_TYPE = const SVT_TYPE& >
class SVVector
{
public:
	typedef std::vector< SVT_TYPE > SVVectorBase;
	typedef typename SVVectorBase::size_type size_type;
	typedef typename SVVectorBase::reference reference;
	typedef typename SVVectorBase::const_reference const_reference;
	typedef typename SVVectorBase::value_type SVType;
	typedef typename SVVectorBase::value_type value_type;
	typedef typename SVVectorBase::iterator iterator;
	typedef typename SVVectorBase::const_iterator const_iterator;
	typedef typename SVVectorBase::reverse_iterator reverse_iterator;
	typedef typename SVVectorBase::const_reverse_iterator const_reverse_iterator;
	typedef SVT_ARG_TYPE SVArgType;
	typedef SVT_ARG_TYPE arg_type;

	SVVector();
	SVVector( size_type p_InitSize );
	SVVector( const SVVector& p_rObject );
	SVVector( const SVVectorBase& p_rObject );

	virtual ~SVVector();

	operator SVVectorBase const & () const;
	operator SVVectorBase & ();

	const SVVector& operator=( const SVVector& p_rObject );
	const SVVector& operator=( const SVVectorBase& p_rObject );

	inline bool empty() const;

	inline size_t size() const;
	inline void resize(size_type p_Newsize) { m_Vector.resize( p_Newsize ); }
	inline void push_back(const SVT_TYPE& element) { m_Vector.push_back(element); }
	inline const_iterator begin() const { return m_Vector.begin(); }
	inline const_iterator end() const { return m_Vector.end(); }
	inline iterator begin() { return m_Vector.begin(); }
	inline iterator end() { return m_Vector.end(); }

	inline const_reverse_iterator rbegin() const { return m_Vector.rbegin(); }
	inline const_reverse_iterator rend() const { return m_Vector.rend(); }
	inline reverse_iterator rbegin() { return m_Vector.rbegin(); }
	inline reverse_iterator rend() { return m_Vector.rend(); }

	inline iterator insert( iterator p_Where, SVArgType p_Value );
	inline iterator erase( iterator p_Where );
	inline iterator erase( const_iterator p_Where );
	inline void clear();

	inline bool operator==(const SVVector<SVT_TYPE, SVT_ARG_TYPE>& rVector) const;
	inline bool operator!=(const SVVector<SVT_TYPE, SVT_ARG_TYPE>& rVector) const;
	inline void swap(SVVector<SVT_TYPE, SVT_ARG_TYPE>& rVector);

	// MFC Style Operators

	virtual int GetSize() const;

	virtual INT_PTR GetUpperBound() const;

	virtual void SetSize( int nNewSize );

	// Add SFINAE to this
	virtual const SVType* GetData() const;
	// Add SFINAE to this
	virtual SVType* GetData();

	virtual SVType GetAt( int nIndex ) const;

	// Add SFINAE to this
	virtual SVType& ElementAt( int nIndex );
	// Add SFINAE to this
	virtual const SVType& ElementAt( int nIndex ) const;

	virtual const SVType& operator[]( size_type nIndex ) const;
	// Add SFINAE to this
	virtual SVType& operator[]( size_type nIndex );

	virtual void Fill( SVArgType fillValue );

	virtual int Add( SVArgType newElement );
	virtual INT_PTR Append( const SVVector& src );

	virtual void InsertAt( int nIndex, SVArgType newElement, int nCount = 1 );
	virtual void InsertAt( int nStartIndex, SVVector* pNewArray );

	virtual void SetAt( int nIndex, SVArgType newElement );
	virtual void SetAtGrow( int nIndex, SVArgType newElement );

	virtual void Copy( const SVVector& src );

	virtual void RemoveAt( int nIndex, int nCount = 1 );
	virtual void RemoveAll();

protected:
	SVVectorBase m_Vector;

};

#include "SVVector.inl"

