//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectClassImpl
//* .File Name       : $Workfile:   SVValueObjectClassImpl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:44  $
//******************************************************************************

#ifndef SVVALUEOBJECTCLASSIMPL_H
#define SVVALUEOBJECTCLASSIMPL_H

#include "UseTypeExceptMatch.h"
#include "SVValueObjectClass.h"

// implement default behavior for most of the types
// derived classes should override the virtuals needed for type-specific behavior.
template <typename T>
class SVValueObjectClassImpl : public SVValueObjectClass
{
public:
	virtual ~SVValueObjectClassImpl(); // See C++ Coding standards page 90 avoid memory leaks. public virtual destructor.
	typedef SVValueObjectClassImpl<T>  this_type;
	typedef typename T                          value_type;
	typedef std::vector <value_type>   array_type;
	typedef std::vector <value_type>   bucket_type_scalar;
	typedef std::vector <array_type>   bucket_type_array;
	typedef bucket_type_array          bucket_type;
	typedef value_type*                bucket_pointer_type;

	array_type m_ScalarBuckets;
	value_type m_DefaultValue;



	const this_type& operator = (const this_type& rhs);
	void swap( SVValueObjectClassImpl<T>& rhs );	// provide an efficient swap; derived classes with extra data should redefine.

	virtual BOOL      SetObjectDepth ( int iNewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );
	virtual HRESULT SetArraySize   ( int iSize );
	virtual HRESULT CopyValue      ( int src, int dest );
	
	HRESULT SetDefaultValue( T value, bool bResetAll );
	HRESULT GetDefaultValue( T& rValue ) const;

	template <typename ELEMENT_TYPE>	// enable any allowable (implicit) conversions if using derived class interface
	HRESULT GetValues(std::vector<ELEMENT_TYPE>& raValues) const {return GetArrayValuesImpl(GetLastSetIndex(), raValues);}

	template <typename ELEMENT_TYPE>	// enable any allowable (implicit) conversions if using derived class interface
	HRESULT GetValues(int iBucket, std::vector<ELEMENT_TYPE>& raValues) const {return GetArrayValuesImpl(iBucket, raValues);}

	// implements copying the specified vector to the value object
	template <typename ELEMENT_TYPE>
	HRESULT SetArrayValues(int iBucket, std::vector<ELEMENT_TYPE>& raValues)
	{
		return SetArrayValues(iBucket, raValues.begin(), raValues.end());
	}

	// implements copying the specified data to the value object
	// source data specified like STL ranges: e.g. myvector.begin(), myvector.end()
	template <class InIterator>
	HRESULT SetArrayValues(int iBucket, InIterator begin, InIterator end)//std::vector<ELEMENT_TYPE>& raValues)
	{
		HRESULT hr = ValidateIndexes(iBucket,0);
		if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
		{
			int iSize = static_cast< int >( std::distance(begin, end));
			ASSERT( iSize <= ArraySize() );
			if ( iSize <= ArraySize() )
			{
				SetResultSize(iBucket, iSize);
				if ( iSize > 0 )
				{
					if ( ArraySize() == 1 )
						ScalarBucket(iBucket) = *begin;
					else
						std::copy( begin, end, Array(iBucket).begin() );
				}
				m_iLastSetIndex = iBucket;
				hr = S_OK;
			}
			else
			{
				hr = SVMSG_SVO_33_OBJECT_INDEX_INVALID;
			}
		}

		return hr;
	}


protected:
	typedef SVValueObjectClassImpl<T> base;	// to be used only by derived class!

	SVValueObjectClassImpl() : SVValueObjectClass() {Init();}
	SVValueObjectClassImpl(LPCTSTR lpszObjectName) : SVValueObjectClass(lpszObjectName) {Init();}
	SVValueObjectClassImpl(SVObjectClass* pOwner, int StringResourceID) : SVValueObjectClass(pOwner, StringResourceID) {Init();}

	virtual HRESULT CreateBuckets();
	virtual HRESULT SetValueAt( int iBucket, int iIndex, T value );
	virtual HRESULT GetValueAt( int iBucket, int iIndex, T& rValue ) const;

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );

	virtual HRESULT GetArrayValues(std::vector<T>& raValues) const;	// allow copy of native type
	virtual HRESULT GetArrayValues(int iBucket, std::vector<T>& raValues) const;	// allow copy of native type
	// implement <double> for all value object types (that can convert to double) to support the equation array capability:
	typedef typename UseTypeExceptMatch<std::vector<double>, std::vector<T> >::type DoubleVectorType;
	typedef typename UseTypeExceptMatch<double, T>::type DoubleType;
	virtual HRESULT GetArrayValues(int iBucket, DoubleVectorType& raValues) const;	// allow copy to vector<double>
	virtual HRESULT GetArrayValuesAsVariant( int iBucket, VARIANT& rValue ) const;	// allow copy to VARIANT SAFEARRAY
	virtual HRESULT GetArrayValuesAsVariantVector(int iBucket, std::vector<_variant_t>& raValues) const;	// allow copy to vector<_variant_t>

	// implements copying the specified array to a vector of any implicitly convertible type
	template <typename ELEMENT_TYPE>
	HRESULT GetArrayValuesImpl(int iBucket, std::vector<ELEMENT_TYPE>& raValues) const
	{
		HRESULT hrOk = S_FALSE;
		ASSERT( iBucket >= 0 && iBucket < m_iNumberOfBuckets );
		if ( iBucket >= 0 && iBucket < m_iNumberOfBuckets )
		{
			int iResultSize = GetResultSize(iBucket);
			ASSERT( iResultSize <= ArraySize() );
			raValues.resize( iResultSize );
			if ( ArraySize() == 1 )
				raValues[0] = static_cast< ELEMENT_TYPE >(ScalarBucket(iBucket));
			else
			{
				array_type::const_iterator l_FromIter( Array(iBucket).begin() );

				std::vector<ELEMENT_TYPE>::iterator l_ToIter( raValues.begin() );

				while( l_ToIter != raValues.end() &&
					l_FromIter != Array(iBucket).end() && 
					l_FromIter != Array(iBucket).begin() + iResultSize )
				{
					*l_ToIter = static_cast< ELEMENT_TYPE >( *l_FromIter );

					++l_FromIter;
					++l_ToIter;
				}
			}
			hrOk = S_OK;
		}
		return hrOk;
	}

	// Comments on m_ScalarBuckets vs. m_pBuckets:
	// Because of cache hits, having a two-dimensional array for all value objects caused
	// approx. 20% slowdown per inspection on a large (100 tools) configuration.
	// therefore, since 99.9% of all value objects are single array element, the optimization
	// was put in to implement that case as a single dimentional array, the way the pre-array value objects worked.
	// In most cases, the derived classes can use the Element function which will work correctly regardless
	// of which mode is being used. However, in certain cases, the derived classes need to be aware of which mode
	// the object is operating in, and use the correct accessor function ( Buckets() and Array() vs. ScalarBucket() )

	__forceinline value_type& Element(int iBucket, int iArray)
		{if (ArraySize() == 1) return m_ScalarBuckets[iBucket]; else return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket][iArray];}
	__forceinline const value_type& Element(int iBucket, int iArray) const
		{if (ArraySize() == 1) return m_ScalarBuckets[iBucket]; else return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket][iArray];}

	__forceinline value_type& ScalarBucket(int iBucket)
		{ASSERT(ArraySize() == 1); return m_ScalarBuckets[iBucket];}
	__forceinline const value_type& ScalarBucket(int iBucket) const
		{ASSERT(ArraySize() == 1); return m_ScalarBuckets[iBucket];}

	__forceinline array_type& Array(int iBucket)
		{ASSERT(ArraySize() != 1); return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket];}
	__forceinline const array_type& Array(int iBucket) const
		{ASSERT(ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array[iBucket];}

	__forceinline bucket_type& Buckets()
		{ASSERT(ArraySize() != 1); return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	__forceinline const bucket_type& Buckets() const
		{ASSERT(ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	__forceinline bucket_type& BucketsNoAssert()
		{return (static_cast<SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	__forceinline const bucket_type& BucketsNoAssert() const
		{return (static_cast<const SVArrayValueHolder<array_type>*> (m_pBuckets))->m_array;}
	static const bucket_type& Buckets(const this_type& rhs)
		{ASSERT(rhs.ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (rhs.m_pBuckets))->m_array;}
	static const bucket_type& Buckets(const SVArrayValueHolderBase* pArrayHolder)
		{ASSERT(rhs.ArraySize() != 1); return (static_cast<const SVArrayValueHolder<array_type>*> (pArrayHolder))->m_array;}

	__forceinline value_type& DefaultValue()
		{return m_DefaultValue;}
	__forceinline const value_type& DefaultValue() const
		{return m_DefaultValue;}
	static const value_type& DefaultValue(const this_type& rhs)
		{return rhs.m_DefaultValue;}
	static const value_type& DefaultValue(const SVArrayValueHolderBase* pArrayHolder)
		{return rhs.m_DefaultValue;}

	// helper structs for GetArrayValues(int iBucket, std::vector< UseTypeExceptMatch<double, T>::result >& raValues)
	struct conversion_exists
	{
		HRESULT call(const this_type* pThis, int iBucket, std::vector<double>& raValues)
		{return pThis->GetArrayValuesImpl(iBucket, raValues);}
	};
	struct conversion_doesnt_exist
	{
		HRESULT call(const this_type* pThis, int iBucket, DoubleVectorType& raValues)
		{return S_FALSE;}
	};
	friend struct conversion_exists;
	friend struct conversion_doesnt_exist;

	// used by SetObjectValue; one set per template type
	static CString m_sLegacyScriptDefaultName;
	static CString m_sLegacyScriptArrayName;

private:
	void Init();
};// end SVValueObjectClassImpl

template <typename T>
CString SVValueObjectClassImpl<T>::m_sLegacyScriptDefaultName;
template <typename T>
CString SVValueObjectClassImpl<T>::m_sLegacyScriptArrayName;

// we don't want to use the default std::swap because it will invoke copy constructors and destructors.
// Make the efficient member function available to algorithms:
// according to C++ Templates: the complete guide( Vandevoorde/Josuttis ) section 11.4,
// these swap functions will succeed when passed a derived class

template < typename T >
inline void swap ( SVValueObjectClassImpl<T>& lhs, SVValueObjectClassImpl<T>& rhs )
{
	lhs.swap(rhs);
}

// should we provide this????
/*
namespace std
{
	template < typename T >
	inline void swap( SVValueObjectClassImpl<T>& lhs, SVValueObjectClassImpl<T>& rhs )
	{
		lhs.swap(rhs);
	}
};
//*/

#include "SVValueObjectClassImpl.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVValueObjectClassImpl.h_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:24:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:17:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Feb 2013 13:08:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Value Object classes to include new GetValues methods that take VARIANT as the value parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jan 2013 10:59:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jan 2013 12:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Feb 2012 13:04:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated target windows version to XP (_WIN32_WINNT 0x0501).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 May 2011 08:38:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  706
 * SCR Title:  Fix Histogram Drawing Problem in Linear Analyzer Dialogs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated set array values to set the last set index when setting the array data at a particular bucket.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 14:27:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2008 14:59:14   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source to split the value objects into individual files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   07 Mar 2008 12:58:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added TypeName to value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   31 Oct 2007 08:36:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added virtual destructor See C++ Coding standards page 90 avoid memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   21 Jun 2007 15:07:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   17 Jan 2006 14:56:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetNormalizedValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   17 Jan 2006 10:13:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetValue(SVScalarValue) and swap
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   11 Nov 2005 14:27:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  525
 * SCR Title:  Fix problem with Shape Mask editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CompareWithCurrentValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   14 Sep 2005 13:20:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new method to get the array data without have to present the last set bucket.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   13 Sep 2005 14:24:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed bug in setting array with 0 elements
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   08 Sep 2005 11:47:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed problematic ASSERTs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   07 Sep 2005 13:26:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added comments describing the optimization
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   07 Sep 2005 12:56:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ASSERTS to catch misuse of functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   02 Sep 2005 14:46:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized for speed
 * - only store multidimentional buckets if array size != 1
 * - inlined functions (ArraySize) etc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   29 Aug 2005 14:06:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made SVValueObjectReference a friend class of SVValueObject so it can make direct calls to protected member functions.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   26 Aug 2005 15:06:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made slight speed improvements to ValueObject. Really need /Og to get huge speed increases.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   08 Aug 2005 15:16:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Split Value Object base functionality into own file and moved all other value objects to another single file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   02 Aug 2005 10:42:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   be less strict on SetArrayValues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   01 Aug 2005 12:02:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   replaced SVVectorObject with SVValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   29 Jul 2005 13:37:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new attributes and methods to track required reset functionality when values are changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   19 Jul 2005 14:44:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   14 Jul 2005 12:38:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to bool on SetDefaultValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   30 Jun 2005 15:12:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   22 Jun 2005 08:46:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implement array value object
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   17 Feb 2005 14:32:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support the new createObject and reset Methodology.
 * Added ResetObject(, CreateArrays(, DestroyArrays(
 * Removed SetObjectDepth(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   16 Feb 2005 15:25:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   27 Jul 2004 15:18:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added long GetDefaultType() to SVVariantValueObjectClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   07 Jul 2004 10:15:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up clutter
 * added SVValueObjectUnitTest
 * added support for Variant
 * added support for char
 * added GetLastSetIndex()
 * made CopyLastSetValue, GetLastSetIndex(int&) non-virtual
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   02 Apr 2004 12:06:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  411
 * SCR Title:  Don't print Calculated values of Line Analyzers in Configuration Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new Set & Get methods for bool's
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   04 Dec 2003 13:17:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   17 Sep 2003 13:02:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   17 Jun 2003 15:13:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated the new disable method code. Also, made Tool.Enabled accessible through the ActiveX.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   22 Apr 2003 17:29:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   10 Apr 2003 10:14:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  271
 * SCR Title:  Adding timer features for Tool Execution times.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a new SetValue and new GetValue with the type being __int64
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   14 Jan 2003 14:11:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a SetValue that takes a double value, to the SVLongValueObjectClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Dec 2002 15:22:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   typedef SVArrayClass <SVValueObjectClass*, SVValueObjectClass*> SVValueObjectArray;
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   13 Nov 2002 12:32:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  289, 272, 226
 * SCR Title:  Extending the SVValueObjectClass Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   The following apply to the class SVValueObject and its derivatives, SVBoolValueObjectClass, SVByteValueObjectClass, SVDWordValueObjectClass, SVLongValueObjectClass, SVDoubleValueObjectClass, SVPointValueObjectClass, SVStringValueObjectClass, SVFileNameValueObjectClass.
 * 
 * The SetValue(type) functions were changed to SetValue(bucket, type)
 * 
 * The SetValue(bucket, type&) functions were changed to be aware of the buckets.
 * 
 * The GetValue(type&) functions were changed to call GetValue(m_nLastSetIndex, type&)
 * 
 * The GetValue(bucket, type&) functions were changed to be aware of the buckets.
 * 
 * Extra GetValue functions were added in each class to support uniform conversions of type.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   13 Jun 2001 10:44:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SVStringValueObjectClass::SetValue, 
 * SVStringValueObjectClass::SetObjectValue, both SVFileNameValueObjectClass::SVFileNameValueObjectClass constructors, 
 * ~SVFileNameValueObjectClass, SVFileNameValueObjectClass::SetValue,
 * SVFileNameValueObjectClass::SetDefaultValue, SVFileNameValueObjectClass::SetObjectValue,
 * SVFileNameValueObjectClass::GetObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2001 20:20:32   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Jan 2001 17:37:38   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Modified SVStringValueObjectClass to use the SVFileNameClass and the SVFileNameManagerClass is the object refers to a filename.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   23 Mar 2000 16:52:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Fixed bugs in SVByteValueObjectClass.
 * Fixed SVValueObjectClass to handle Byte Value Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 14:00:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   13 Sep 1999 13:43:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetDefaultValue Method for all classes
 * derived from SVValueObjectClass.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Sep 1999 08:35:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to set defaults in the PPQDataLinkInfoStruct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Aug 1999 16:40:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Overridden GetValue() for CString& and double&.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
