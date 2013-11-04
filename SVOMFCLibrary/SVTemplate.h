// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
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

#ifndef SVTEMPLATE_H
#define SVTEMPLATE_H

#include <vector>

#ifndef SV_INLINE
#define SV_INLINE __forceinline
#endif

// ignore allocators for now
template < class T >
class vector2d : public std::vector< std::vector<T> >
{
public:
	typedef std::vector<T> row_type;
	typedef std::vector< std::vector<T> > base;
	void resize ( int iNewRows, int iNewCols );
	void resize(size_type n, T x = T()) {base::resize(n,x);}
	void fill( T value );
	void fill( int iRow, T value );
	T& at(int iRow, int iCol);
	row_type& row(int iRow);
	int num_rows();
	int num_cols();
private:
	typedef std::vector< std::vector<T> > base;
};

template < class T >
void vector2d<T>::resize( int iNewRows, int iNewCols )
{
	row_type aDefault(iNewCols);
	base::resize(iNewRows, aDefault);

	for ( size_t i=0; i < size(); i++ )
	{
		base::at(i).resize(iNewCols);
	}
}

template < class T >
void vector2d<T>::fill( T value )
{
	for ( size_t i=0; i < size(); i++ )
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
	return size();
}

template < class T >
int vector2d<T>::num_cols()
{
	if ( num_rows() > 0 )
		return row(0).size();
	else
		return 0;
}

template < typename TYPE > class TTemporaryPointerWrapper
{
public:
	TTemporaryPointerWrapper(TYPE*);
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
	if (m_pTemp != NULL)
	{
		delete m_pTemp;
		m_pTemp = NULL;
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


template < typename BASETYPE > class TValueSemantics	// enable value semantics of polymorphic types
                                                        // you should not derive from this class
{
public:
	TValueSemantics();
	TValueSemantics(const BASETYPE* pType);
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
		DERIVED* DerivedValue(DERIVED*& rpDummyArg)
	                            { DERIVED* p = NULL;
	                              if (m_pImpl) {p = dynamic_cast< DERIVED* > ((BASETYPE*) *this);
								              ASSERT(p); }	// if the ASSERT hits, the type is incorrect
	                              return p;}
	template<typename DERIVED>
	    const DERIVED* DerivedValue(const DERIVED*& rpDummyArg) const
	                            { const DERIVED* p = NULL;
	                              if (m_pImpl) { p = dynamic_cast< const DERIVED* > ((const BASETYPE*) *this);
								               ASSERT(p); }  // if the ASSERT hits, the type is incorrect
	                              return p; }
	template<typename DERIVED>
	    HRESULT GetDerivedValue(DERIVED*& rpd)
	                            { if (m_pImpl == NULL) return ERROR_NOT_FOUND; 
	                              else if ((rpd = dynamic_cast< DERIVED* > ((BASETYPE*) *this)) != NULL) return S_OK;
								  else { ASSERT(rpd); return TYPE_E_TYPEMISMATCH;} }  // if the ASSERT hits, the type is incorrect
	/*	// commented out because could not get the const to work properly... compiler problem
	template<typename DERIVED>
	    HRESULT GetDerivedValue(const DERIVED*& rpd) const { return m_pImpl == NULL ? TYPE_E_ELEMENTNOTFOUND : (rpd = dynamic_cast< const DERIVED* > ((const BASETYPE*) *this)) != NULL ? S_OK : TYPE_E_TYPEMISMATCH;}
	*/
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
//    SVAcqLutParam* pParam = NULL;
//    hr = rDevice.mapParameters[ AcqParamLut ].GetDerivedValue( pParam );
//    SVLut lut;
//    if ( pParam != NULL && hr == S_OK )
	//        lut = pParam->lut;


// when calling the following function, you don't need to include the second parameter....
// example of use:
// SVLut lut = DerivedValue< SVAcqLutParam >( rDevice.mapParameters[ AcqParamLut ] )->lut;
// the member function version (see above) is preferred over this method.
template < typename DERIVED, typename DUMMYARG > DERIVED* DerivedValue ( TValueSemantics<DUMMYARG> base )
{
	DERIVED* pd = NULL;
	if ( ((DUMMYARG*) base) != NULL )
	{
		pd = dynamic_cast < DERIVED* > ((DUMMYARG*) base);
		ASSERT( pd );	// if the ASSERT hits, you are telling it the wrong type
	}
	return pd;
}


template <typename BASETYPE>
TValueSemantics<BASETYPE>::TValueSemantics()
{
	m_pImpl = NULL;
}

template <typename BASETYPE>
TValueSemantics<BASETYPE>::TValueSemantics(const BASETYPE* pType)
{
	ASSERT( pType );
	if ( pType )
	{
		// needs an explicit cast because MSVC 6.0 doesn't support covariant return types (Q240862)
		m_pImpl = static_cast<BASETYPE*>( pType->CloneImpl() );
	}
	else
		m_pImpl = NULL;
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
		m_pImpl = NULL;
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
			m_pImpl = NULL;
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
	if ( m_pImpl != NULL )
	{
		delete m_pImpl;
		m_pImpl = NULL;
	}
}

template <typename BASETYPE>
bool TValueSemantics<BASETYPE>::IsValid() const
{
	return ( m_pImpl != NULL );
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






// Singleton and Factory concepts borrowed from Loki
template<class SINGLETON> class TBasicSingletonHolder
{
public:
	typedef SINGLETON InstanceType;
	static SINGLETON& Instance(...);	// the vararg is to prevent inlining
private:
	TBasicSingletonHolder();
	TBasicSingletonHolder(const TBasicSingletonHolder&);
	TBasicSingletonHolder& operator = (const TBasicSingletonHolder&);
	~TBasicSingletonHolder();
};

// the MSVC 6.0 compiler seems to ignore these pragmas
// use the vararg method comes from the documentation of "inline, __inline, __forceinline"
//#pragma auto_inline (off) // do not inline this function!!!
//#pragma inline_depth(0)

template<class SINGLETON>
SINGLETON& TBasicSingletonHolder<SINGLETON>::Instance(...)	// the vararg is to prevent inlining
{
	static SINGLETON obj;
	return obj;
}

//#pragma inline_depth()
//#pragma auto_inline (on) // restore default inlining


#include <map>
#include <sstream>

template <typename TYPEID, class FACTORYBASE> class TFactory
{
public:
	typedef FACTORYBASE* (*CreateFn)();
	bool Register(TYPEID id, CreateFn fn);
	FACTORYBASE* New(TYPEID id);
	bool SetDefault(TYPEID id);
	TFactory();
private:
	typedef std::map<TYPEID, CreateFn> CreateFnMap;
	TYPEID mDefaultType;
	bool mbSetDefault;
	CreateFnMap mapCreateFn;
};

template <typename TYPEID, class FACTORYBASE>
TFactory<TYPEID, FACTORYBASE>::TFactory()
{
	mbSetDefault=false;
}

template <typename TYPEID, class FACTORYBASE>
bool TFactory<TYPEID, FACTORYBASE>::Register(TYPEID id, CreateFn fn)
{
	mapCreateFn[id] = fn;
	return true;
}

template <typename TYPEID, class FACTORYBASE>
bool TFactory<TYPEID, FACTORYBASE>::SetDefault(TYPEID id)
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

template <typename TYPEID, class FACTORYBASE>
FACTORYBASE* TFactory<TYPEID, FACTORYBASE>::New(TYPEID id)
{
	CreateFnMap::const_iterator iter = mapCreateFn.find(id);
	if (iter != mapCreateFn.end())
	{
		if (iter->second == NULL)
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
	return (iter->second)();	// make call to Create function
}


template<typename TYPEID, class FACTORYBASE> class TFactorySingleton
{
public:
	typedef TFactory<TYPEID, FACTORYBASE> factoryclass;
	typedef TBasicSingletonHolder<factoryclass> factory;
};
// example of usage:
// typedef TFactorySingleton<SVDeviceParamEnum, SVDeviceParam>::factory TheDeviceParamFactory;



template <typename TYPEID, typename TYPEID2, class FACTORYBASE> 
class TDoubleFactory
{
public:
	typedef FACTORYBASE* (*CreateFn)();
	bool Register(const TYPEID& id, const TYPEID2& id2, CreateFn fn);
	FACTORYBASE* New(const TYPEID& id);
	FACTORYBASE* New(const TYPEID2& id);
	bool SetDefault(const TYPEID& id);
	TDoubleFactory();

	// give const access to type maps ( the whole point of a 2-type factory )
	typedef std::map<TYPEID, TYPEID2> TypeMapPrimary;
	typedef std::map<TYPEID2, TYPEID> TypeMapSecondary;
	typedef typename TypeMapPrimary::const_iterator const_iterator;
	typedef typename TypeMapSecondary::const_iterator const_iterator2;
	const TypeMapPrimary& PrimaryMap();
	const TypeMapSecondary& SecondaryMap();
private:
	typedef std::map<TYPEID, CreateFn> CreateFnMap;
	TYPEID mDefaultType;
	bool mbSetDefault;
	CreateFnMap mapCreateFn;
	TypeMapPrimary mapPrimaryType;
	TypeMapSecondary mapSecondaryType;
};

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::TDoubleFactory()
{
	mbSetDefault=false;
}

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
	CreateFnMap::const_iterator iter = mapCreateFn.find(id);
	if (iter != mapCreateFn.end())
	{
		if (iter->second == NULL)
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
	return (iter->second)();	// make call to Create function
}

template <typename TYPEID, typename TYPEID2, class FACTORYBASE>
FACTORYBASE* TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE>::New(const TYPEID2& id2)
{
	TypeMapSecondary::const_iterator iter = mapSecondaryType.find(id);
	if (iter == mapSecondaryType.end())
	{
		if ( mbSetDefault )
		{
			// try default type
			iter = mapCreateFn.find(mDefaultType);
		}

		if ( iter != mapCreateFn.end() )	// try test again
		{
			return New(mDefaultType);
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


template<typename TYPEID, typename TYPEID2, class FACTORYBASE> class TDoubleFactorySingleton
{
public:
	typedef TDoubleFactory<TYPEID, TYPEID2, FACTORYBASE> factoryclass;
	typedef TBasicSingletonHolder<factoryclass> factory;
};
// example of usage:
// typedef TFactorySingleton<SVDeviceParamEnum, CString, SVDeviceParam>::factory TheDeviceParamFactory;




#endif // SVTEMPLATE_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVTemplate.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:14:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Feb 2011 09:49:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   07 Dec 2010 16:06:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   15 Oct 2010 15:03:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix casting issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   28 May 2010 14:22:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   24 May 2010 14:20:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   03 Mar 2010 11:30:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove unnecessary containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   20 Jun 2007 14:48:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   01 Aug 2005 14:19:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  469
 * SCR Title:  Add array capability to the Blob Analyzer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed vector2d
 * removed SV2DArrayClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Aug 2005 13:16:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed SVDoubleArrayClass and SVByteArrayClass (no longer necessary)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Aug 2005 13:16:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed SVDoubleArrayClass and SVByteArrayClass (no longer necessary)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Jun 2005 14:58:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added vector2d
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Feb 2005 10:55:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added a double array object to use for bucketizing the edge data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Oct 2004 12:34:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  448
 * SCR Title:  Fix External Tool Change DLL functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added value_type and arg_type to SVArrayClass to make them more compatible with stl
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   03 Feb 2004 17:19:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for named device params
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Dec 2003 12:57:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  397
 * SCR Title:  Blob Analyzer - Allow Max number of blobs to be set.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with 2D array SetSize.  If column size changes it never updates the new size
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Nov 2003 09:26:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Find function to CStdArray
 * added TVennAdapter template
 * made TSet unsupported functions private
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Sep 2003 08:57:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added TValueSemantics::IsValid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Aug 2003 13:12:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem with TFactory::New
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Jul 2003 08:39:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added default capabilities to TFactory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Jul 2003 10:32:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   06 Feb 2003 10:35:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added PVCS header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:22:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
