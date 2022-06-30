// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTemplate
// * .File Name       : $Workfile:   SVTemplate.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:14:48  $
// ******************************************************************************

//
//////////////////////////////////////////////////////////////////////
// SVTemplate.h: interface for the SVArrayClass class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <sstream>
#pragma endregion Includes

// ignore allocators for now
template<typename T>
class vector2d : public std::vector<std::vector<T>>
{
public:
	using row_type = std::vector<typename T>;
	using base = std::vector<row_type> ;
	void resize ( int iNewRows, int iNewCols );
	void resize(typename base::size_type n, T x = T()) {base::resize(n,x);}
	void fill( T value );
	void fill( int iRow, T value );
	T& at(int iRow, int iCol);
	row_type& row(int iRow);
	int num_rows();
	int num_cols();
private:
	typedef std::vector< std::vector<T> > base;
};

template<typename T>
void vector2d<T>::resize( int iNewRows, int iNewCols )
{
	row_type aDefault(iNewCols);
	base::resize(iNewRows, aDefault);

	for ( size_t i=0; i < this->size(); i++ )
	{
		base::at(i).resize(iNewCols);
	}
}

template <typename T>
void vector2d<T>::fill( T value )
{
	for ( size_t i=0; i < this->size(); i++ )
	{
		std::fill(base::at(i).begin(), base::at(i).end(), value);
	}
}

template < class T >
void vector2d<T>::fill( int iRow, T value )
{
	std::fill(base::at(iRow).begin(), base::at(iRow).end(), value);
}

template < class T >
std::vector<T>& vector2d<T>::row( int iRow )
{
	return base::at(iRow);
}

template < class T >
T& vector2d<T>::at( int iRow, int iCol )
{
	return base::at(iRow).at(iCol);
}

template < class T >
int vector2d<T>::num_rows()
{
	return this->size();
}

template < class T >
int vector2d<T>::num_cols()
{
	if ( num_rows() > 0 )
		return row(0).size();
	else
		return 0;
}

template < typename TYPE >
class TTemporaryPointerWrapper
{
public:
	explicit TTemporaryPointerWrapper(TYPE*);
	~TTemporaryPointerWrapper();
	operator const TYPE*();
private:
	TYPE* m_pTemp;
};

template <typename TYPE>
TTemporaryPointerWrapper<TYPE>::TTemporaryPointerWrapper(TYPE* pT)
{
	m_pTemp = pT;
}

template <typename TYPE>
TTemporaryPointerWrapper<TYPE>::~TTemporaryPointerWrapper()
{
	if (nullptr != m_pTemp)
	{
		delete m_pTemp;
		m_pTemp = nullptr;
	}
}

template <typename TYPE>
TTemporaryPointerWrapper<TYPE>::operator const TYPE*()
{
	return m_pTemp;
}



class SVClonable
{
	// because covariant return types aren't supported(see Q240862),
	// we call this function CloneImpl, otherwise it really should be called Clone
	// but for now as a workaround, the base class that inherits from this can
	// create a public function called Clone that performs a cast to the appropriate type.
public:
	virtual SVClonable* CloneImpl() const = 0;
};


template < typename BASETYPE >
class TValueSemantics	// enable value semantics of polymorphic types
                                                        // you should not derive from this class
{
public:
	TValueSemantics();
	// cppcheck-suppress has a constructor with 1 argument that is not explicit.
	TValueSemantics(const BASETYPE* pType);
	// cppcheck-suppress has a constructor with 1 argument that is not explicit.
	TValueSemantics(const BASETYPE& rType);
	TValueSemantics(const TValueSemantics<BASETYPE>& rhs);
	~TValueSemantics();// this class should not be derived from, therefore not virtual
	const TValueSemantics<BASETYPE>& operator = (const TValueSemantics<BASETYPE>& rhs);
	operator BASETYPE*();
	BASETYPE* operator -> ();
	operator const BASETYPE*() const;
	const BASETYPE* operator -> () const;

	void Clear();
	bool IsValid() const;

	// the following template functions are defined in-class because out-of-class is not supported yet (Q241949)
	template<typename DERIVED>
	    //DERIVED* DerivedValue(const DERIVED*& rpDummyArg)
		DERIVED* DerivedValue(DERIVED*& )
	                            { DERIVED* p = nullptr;
	                              if (m_pImpl) {p = dynamic_cast< DERIVED* > ((BASETYPE*) *this);
								              assert(p); }	// if the assert hits, the type is incorrect
	                              return p;}
	template<typename DERIVED>
	    const DERIVED* DerivedValue(const DERIVED*& ) const
	                            { const DERIVED* p = nullptr;
	                              if (m_pImpl) { p = dynamic_cast< const DERIVED* > ((const BASETYPE*) *this);
								               assert(p); }  // if the assert hits, the type is incorrect
	                              return p; }
	template<typename DERIVED>
	    HRESULT GetDerivedValue(DERIVED*& rpd)
	                            { if (nullptr == m_pImpl) return ERROR_NOT_FOUND; 
	                              else if (nullptr != (rpd = dynamic_cast< DERIVED* > ((BASETYPE*) *this))) return S_OK;
								  else { assert(rpd); return TYPE_E_TYPEMISMATCH;} }  // if the assert hits, the type is incorrect
private:
	BASETYPE* m_pImpl;
};
// an example of how to use TValueSemantics::DerivedValue:
//    SVAcqLutParam* pParam = rDevice.mapParameters[ AcqParamLut ].DerivedValue( pParam );
//    SVLut lut;
//    if ( pParam )
//        lut = pParam->lut;

// an example of how to use TValueSemantics::GetDerivedValue:
//    HRESULT hr=S_OK;
//    SVAcqLutParam* pParam = nullptr;
//    hr = rDevice.mapParameters[ AcqParamLut ].GetDerivedValue( pParam );
//    SVLut lut;
//    if ( nullptr != pParam && S_OK == hr )
	//        lut = pParam->lut;


// when calling the following function, you don't need to include the second parameter....
// example of use:
// SVLut lut = DerivedValue< SVAcqLutParam >( rDevice.mapParameters[ AcqParamLut ] )->lut;
// the member function version (see above) is preferred over this method.
template < typename DERIVED, typename DUMMYARG > DERIVED* DerivedValue ( TValueSemantics<DUMMYARG> base )
{
	DERIVED* pd = nullptr;
	if ( nullptr != ((DUMMYARG*) base) )
	{
		pd = dynamic_cast < DERIVED* > ((DUMMYARG*) base);
		assert( pd );	// if the assert hits, you are telling it the wrong type
	}
	return pd;
}


template <typename BASETYPE>
TValueSemantics<BASETYPE>::TValueSemantics()
{
	m_pImpl = nullptr;
}

template <typename BASETYPE>
TValueSemantics<BASETYPE>::TValueSemantics(const BASETYPE* pType)
{
	assert( pType );
	if ( pType )
	{
		// needs an explicit cast because MSVC 6.0 doesn't support covariant return types (Q240862)
		m_pImpl = static_cast<BASETYPE*>( pType->CloneImpl() );
	}
	else
		m_pImpl = nullptr;
}

template <typename BASETYPE>
TValueSemantics<BASETYPE>::TValueSemantics(const BASETYPE& rType)
{
	// needs an explicit cast because MSVC 6.0 doesn't support covariant return types (Q240862)
	m_pImpl = static_cast<BASETYPE*>( rType.CloneImpl() );
}

template <typename BASETYPE>
TValueSemantics<BASETYPE>::TValueSemantics(const TValueSemantics<BASETYPE>& rhs)
{
	// needs an explicit cast because MSVC 6.0 doesn't support covariant return types (Q240862)
	if ( rhs.m_pImpl )
	{
		// needs an explicit cast because MSVC 6.0 doesn't support covariant return types (Q240862)
		m_pImpl = static_cast<BASETYPE*>( rhs.m_pImpl->CloneImpl() );
	}
	else
	{
		m_pImpl = nullptr;
	}
}

template <typename BASETYPE>
TValueSemantics<BASETYPE>::~TValueSemantics()
{
	Clear();
}

template <typename BASETYPE>
const TValueSemantics<BASETYPE>& TValueSemantics<BASETYPE>::operator = (const TValueSemantics<BASETYPE>& rhs)
{
	if ( m_pImpl != rhs.m_pImpl )
	{
		if ( m_pImpl )
		{
			delete m_pImpl;
			m_pImpl = nullptr;
		}
		if ( rhs.m_pImpl )
		{
			// needs an explicit cast because MSVC 6.0 doesn't support covariant return types (Q240862)
			m_pImpl = static_cast<BASETYPE*>( rhs.m_pImpl->CloneImpl() );
		}
	}
	return *this;
}

template <typename BASETYPE>
void TValueSemantics<BASETYPE>::Clear()
{
	if ( nullptr != m_pImpl )
	{
		delete m_pImpl;
		m_pImpl = nullptr;
	}
}

template <typename BASETYPE>
bool TValueSemantics<BASETYPE>::IsValid() const
{
	return ( nullptr != m_pImpl );
}

template <typename BASETYPE>
TValueSemantics<BASETYPE>::operator BASETYPE*()
{
	return m_pImpl;
}

template <typename BASETYPE>
BASETYPE* TValueSemantics<BASETYPE>::operator -> ()
{
	return m_pImpl;
}

template <typename BASETYPE>
TValueSemantics<BASETYPE>::operator const BASETYPE*() const
{
	return m_pImpl;
}

template <typename BASETYPE>
const BASETYPE* TValueSemantics<BASETYPE>::operator -> () const
{
	return m_pImpl;
}

template <typename TYPEID, typename TYPEID2, class FACTORYBASE> 
class TDoubleFactory
{
public:
	static TDoubleFactory& Instance()
	{
		static TDoubleFactory object;

		return object;
	}

	typedef FACTORYBASE* (*CreateFn)(TYPEID typeEnum);
	bool Register(const TYPEID& id, const TYPEID2& id2, CreateFn fn);
	FACTORYBASE* New(const TYPEID& id);
	FACTORYBASE* New(const TYPEID2& id);
	bool SetDefault(const TYPEID& id);
	TDoubleFactory() = default;

	// give const access to type maps ( the whole point of a 2-type factory )
	typedef std::map<TYPEID, TYPEID2> TypeMapPrimary;
	typedef std::map<TYPEID2, TYPEID> TypeMapSecondary;
	typedef typename TypeMapPrimary::const_iterator const_iterator;
	typedef typename TypeMapSecondary::const_iterator const_iterator2;
	const TypeMapPrimary& PrimaryMap();
	const TypeMapSecondary& SecondaryMap();
private:
	typedef std::map<TYPEID, CreateFn> CreateFnMap;
	TYPEID mDefaultType {};
	bool mbSetDefault {false};
	CreateFnMap mapCreateFn;
	TypeMapPrimary mapPrimaryType;
	TypeMapSecondary mapSecondaryType;
};

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
bool TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::Register(const TYPEID& id, const TYPEID2& id2, CreateFn fn)
{
	mapCreateFn[id] = fn;
	mapPrimaryType[id] = id2;
	mapSecondaryType[id2] = id;
	return true;
}

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
bool TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::SetDefault(const TYPEID& id)
{
	if ( mbSetDefault == false )
	{
		mDefaultType = id;
		mbSetDefault = true;
		return true;
	}
	else
		return false;
}

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
FACTORYBASE* TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::New(const TYPEID& id)
{
	typename CreateFnMap::const_iterator iter = mapCreateFn.find(id);
	if (iter != mapCreateFn.end())
	{
		if (nullptr == iter->second)
		{
			std::string sError;
			std::stringstream stream;
			stream << "Create function for type " << id << " is NULL";
			sError = stream.str();
			throw std::runtime_error(sError);
		}
	}
	else	// can't find type
	{
		if ( mbSetDefault )
		{
			// try default type
			iter = mapCreateFn.find(mDefaultType);
		}

		if ( iter != mapCreateFn.end() )	// try test again
		{
		}
		else	// if still not found
		{
			// id not found in map; unknown type
			std::string sError;
			std::stringstream stream;
			stream << "Unknown Type: " << id;
			sError = stream.str();
			throw std::runtime_error(sError);
		}
	}
	return (iter->second)(id);	// make call to Create function
}

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
FACTORYBASE* TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::New(const TYPEID2& id2)
{
	typename TypeMapSecondary::const_iterator iter = mapSecondaryType.find(id2);
	if (iter == mapSecondaryType.end())
	{
		auto iterCreateFn {mapCreateFn.end()};
		if ( mbSetDefault )
		{
			// try default type
			iterCreateFn = mapCreateFn.find(mDefaultType);
		}

		if (mapCreateFn.end() != iterCreateFn)
		{
			return New(mDefaultType);
		}
		else	// if still not found
		{
			// id not found in map; unknown type
			std::string sError;
			std::stringstream stream;
			stream << "Unknown Type: " << id2;
			sError = stream.str();
			throw std::runtime_error(sError);
		}
	}
	return New(iter->second);
}

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
const typename TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::TypeMapPrimary& TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::PrimaryMap()
{
	return mapPrimaryType;
}

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
const typename TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::TypeMapSecondary& TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::SecondaryMap()
{
	return mapSecondaryType;
}

