//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectReference
//* .File Name       : $Workfile:   SVValueObjectReference.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:40  $
//******************************************************************************

#ifndef SVVALUEOBJECTREFERENCE_H_
#define SVVALUEOBJECTREFERENCE_H_

#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "SVValueObjectImpl.h"

class SVValueObjectReference : public SVCheckedObjectReference<SVValueObjectClass>
{
public:
	typedef SVCheckedObjectReference<SVValueObjectClass> base;
	// forwarding constructors...
	SVValueObjectReference() : base() {}
	SVValueObjectReference( SVObjectClass* pObject, long lArrayIndex, CString strDefaultValue = CString() ) : base(pObject, lArrayIndex, strDefaultValue) {}
	SVValueObjectReference( SVObjectClass* pObject, const SVObjectNameInfo& p_rNameInfo) : base(pObject, p_rNameInfo) {}
	SVValueObjectReference( SVObjectClass* pObject ) : base(pObject) {}
	SVValueObjectReference( const SVObjectReference& rhs ) : base(rhs) {}
	const SVValueObjectReference& operator = ( const SVObjectReference& rhs ) { base::operator = (rhs); return *this; }

	//interface into value object using internal array index
	template <typename T>
	__forceinline HRESULT SetValue(int iBucket, T value)
	{
//		return Object()->SetValue(iBucket, m_lArrayIndex, value);
		return (static_cast<SVValueObjectClass*>(m_pObject))->SetValueAtConvert(iBucket, ::atol( m_NameInfo.m_Index.c_str() ), value);
	}

	template <typename T>
	__forceinline HRESULT GetValue(T& rValue) const
	{
//		return Object()->GetValue(Object()->GetLastSetIndex(), m_lArrayIndex, rValue);
		return (static_cast<SVValueObjectClass*>(m_pObject))->GetValueAt((static_cast<SVValueObjectClass*>(m_pObject))->m_iLastSetIndex, ::atol( m_NameInfo.m_Index.c_str() ), rValue);
	}

	template <typename T>
	__forceinline HRESULT GetValue(int iBucket, T& rValue) const
	{
//		return Object()->GetValue(iBucket, m_lArrayIndex, rValue);
		return (static_cast<SVValueObjectClass*>(m_pObject))->GetValueAt(iBucket, ArrayIndex() > -1 ? ArrayIndex() : 0, rValue);
	}

	__forceinline HRESULT GetValues(std::vector< _variant_t >&  rValue) const
	{
		return (static_cast<SVValueObjectClass*>(m_pObject))->GetValues(rValue);
	}

	template<typename T, typename Iter>
	__forceinline HRESULT SetArrayValues(int iBucket, Iter begin, Iter end)
	{
		typedef SVValueObjectClassImpl<T> VOType;
		VOType * obj = dynamic_cast<VOType *>(m_pObject);
		if (obj)
		{
			HRESULT hr = obj->SetArraySize(static_cast<int>(end - begin));
			if (S_OK != hr)
			{
				return hr;
			}
			else
			{
				return obj->SetArrayValues(iBucket, begin, end);
			}
		}
		else
		{
			return TYPE_E_TYPEMISMATCH;
		}
	}

	template<typename Cont>
	__forceinline HRESULT SetArrayValues(int iBucket, const Cont & cont)
	{
		typedef typename Cont::value_type Type;
		typedef typename Cont::const_iterator Iter;
		return SetArrayValues<Type, Iter>(iBucket, cont.begin(), cont.end());
	}
};

typedef std::vector<SVValueObjectReference> SVValueObjectReferenceVector;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVValueObjectReference.h_v  $
 * 
 *    Rev 1.1   02 Oct 2013 08:24:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:18:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Feb 2013 14:22:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new framework for the remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   26 Jul 2012 11:15:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed GetValue to use ArrayIndex() instead of "atol( m_NameInfo.m_Index...."
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Jun 2012 14:29:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 Oct 2010 14:27:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 13:31:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 Jan 2009 11:49:42   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  640
 * SCR Title:  Update Archive Tool to expose Selected List to remote access.
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added SetArrayValues templates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Aug 2005 14:06:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   make direct calls to protected SVValueObject member functions
 * use member variable directly instead of Object function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Aug 2005 08:52:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Jul 2005 13:21:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added constructor taking SVObjectNameParseInfoStruct
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Jun 2005 15:12:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jun 2005 09:34:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVValueObjectReference is now typedef'd to the template SVCheckedObjectReference instead of being its own class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jun 2005 14:42:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
