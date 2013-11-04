//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistory
//* .File Name       : $Workfile:   SVConditionalHistory.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVConditionalHistory.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVPPQObject.h"
#include "SVObserver.h"
#include "SVConfigurationObject.h"
#include "SVImageClass.h"

LPCTSTR const PPQCH_Size      = (_T("Size"));
LPCTSTR const PPQCH_Overwrite = (_T("Overwrite"));

namespace local
{
	struct case_insensitive
	{
		case_insensitive( const CString& strCompare ) : m_strCompare(strCompare) {}
		bool operator() (const std::pair<CString, SVScalarValue>& key)
		{
			return key.second.strValue.CompareNoCase(m_strCompare) == 0;
		}
		CString m_strCompare;
	};
}

void SVConditionalHistoryData::swap( SVConditionalHistoryData& rhs )
{
	using std::swap;

	if ( this != &rhs )
	{
		vecHistory.swap( rhs.vecHistory );

		vecConditionals.swap( rhs.vecConditionals );
		vecValues.swap( rhs.vecValues );
		vecImages.swap( rhs.vecImages );

		bvoOverwrite.swap( rhs.bvoOverwrite );
		lvoSize.swap( rhs.lvoSize );
	}
}


SVConditionalHistory::SVConditionalHistory()
: SVObjectClass()
{
	m_bvoOverwrite.SetDefaultValue( TRUE, TRUE );
	m_lvoSize.SetDefaultValue( 0, TRUE );
}

void SVConditionalHistory::SetInspection( SVInspectionProcess* pInspection )
{
	m_pInspection = pInspection;
}

std::pair<SVObjectReference, SVInspectionProcess*>
SVConditionalHistory::ParseName( const CString& strName )
{
	int iPos = strName.Find('.');
	if ( iPos > 0 )
	{
		CString strInspectionName = strName.Left(iPos);
		
		SVConfigurationObject* l_pConfig = NULL;
		SVObjectManagerClass::Instance().GetConfigurationObject( l_pConfig );

		SVInspectionProcess* pInspection = NULL;
		if ( l_pConfig->GetInspectionObject( strInspectionName, &pInspection ) )
		{
			SVObjectReference object;
			HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( strName ), object );
			if ( hr == S_OK )
			{
				return std::pair<SVObjectReference, SVInspectionProcess*>(object, pInspection);
			}
		}
	}
	return std::pair<SVObjectReference, SVInspectionProcess*>(SVObjectReference(), reinterpret_cast<SVInspectionProcess*>(NULL) );
}

// Validate that the object exists and store the object reference
HRESULT SVConditionalHistory::Validate( std::vector <SVScalarValue>& p_rvec )
{
	HRESULT hr = S_OK;

	std::vector<SVScalarValue>::iterator iter;
	for ( iter = p_rvec.begin(); iter != p_rvec.end(); ++iter )
	{
		SVScalarValue& rValue = *iter;
		std::pair<SVObjectReference, SVInspectionProcess*> retval = ParseName( rValue.strName );
		SVInspectionProcess* pInspection = retval.second;
		if ( pInspection == m_pInspection )
		{
			rValue.object = retval.first;	// assign into passed in vector
			if ( rValue.object.Object() == NULL )	// error: object does not exist
			{
				rValue.status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
				hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			}
		}
		else	//!! error: object not on this inspection
		{
			rValue.status = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST;
			rValue.object = SVObjectReference();// empty object
			hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
	}// end for ( iter = p_rvec.begin(); iter != p_rvec.end(); ++iter )

	return hr;
}

HRESULT SVConditionalHistory::Validate( std::vector <SVScalarValue>& rvecValues, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals )
{
	HRESULT hr = S_OK;

	KeepPrevError( hr, Validate( rvecConditionals ) );
	KeepPrevError( hr, Validate( rvecValues ) );
	KeepPrevError( hr, Validate( rvecImages ) );

	return hr;
}

namespace local
{
	template < typename T >
	struct IsInvalid
	{
		bool operator () ( const T& rhs )
			{ return rhs.object.Object() == NULL; }
	};

	template < typename CONTAINER >
	void RemoveInvalidItems( CONTAINER& rvec )
	{
		CONTAINER::iterator iter = std::remove_if( rvec.begin(), rvec.end(), local::IsInvalid<CONTAINER::value_type>() );
		if ( iter != rvec.end() )
			rvec.erase( iter, rvec.end() );
	}
};

HRESULT SVConditionalHistory::ResetObject()
{
	HRESULT hr = S_OK;

	// WORK ON A COPY OF THE HISTORY, swap atomically later 
	// This copy needs to be an atomic operation, too??!!!
	// Not necessary because there is no dynamic allocation at runtime.
	// The data that could be changing doesn't matter to this function

	// **UNNECESSARY (and undesirable) now because CH is completely managed and accessed from the inspection thread.
	//SVConditionalHistoryData l_Data( m_Data );  
	SVConditionalHistoryData& l_Data = m_Data;    // INSTEAD OF A COPY JUST USE A REFERENCE  


	// CLEAR OBJECT ATTRIBUTES FOR CURRENTLY SELECTED OBJECTS
	{// block scope
		size_t i;
		for ( i=0; i < l_Data.vecConditionals.size(); ++i )
			SetBits( l_Data.vecConditionals.at(i).object.ObjectAttributesSetRef(), SV_CH_CONDITIONAL, false );
		for ( i=0; i < l_Data.vecValues.size(); ++i )
			SetBits( l_Data.vecValues.at(i).object.ObjectAttributesSetRef(), SV_CH_VALUE, false );
		for ( i=0; i < l_Data.vecImages.size(); ++i )
			SetBits( l_Data.vecImages.at(i).object.ObjectAttributesSetRef(), SV_CH_IMAGE, false );
	}

	// COPY INPUTS TO HISTORY DATA
	l_Data.bvoOverwrite = m_bvoOverwrite;
	l_Data.lvoSize = m_lvoSize;
	l_Data.vecConditionals = m_vecConditionals;
	l_Data.vecValues = m_vecValues;
	l_Data.vecImages = m_vecImages;

	// VALIDATE AND GET OBJECTS AS SPECIFIED IN THE INPUTS

	// copy image buffer struct vector temporarily to ScalarValue vector
	std::vector<SVScalarValue> l_vecImages;
	std::copy( l_Data.vecImages.begin(), l_Data.vecImages.end(), std::inserter( l_vecImages, l_vecImages.begin() ) );

	hr = Validate( l_Data.vecValues, l_vecImages, l_Data.vecConditionals );

	// GET THE IMAGE INFO
	{// block
		for ( size_t i = 0; i < l_Data.vecImages.size(); ++i )
		{
			SVImageBufferStruct& rImage = l_Data.vecImages.at(i);
			SVScalarValue& rValidatedImage = l_vecImages.at(i);
			rImage.object = rValidatedImage.object;	// copy info back from the Validate function
			if ( rImage.object.Object() )
			{
				SVImageClass* pImage = dynamic_cast <SVImageClass*> ( rImage.object.Object() );
				if ( pImage )
					rImage.info = pImage->GetImageInfo();
				else
					rImage.object = SVObjectReference();
			}
		}
	}// end block

	local::RemoveInvalidItems( l_Data.vecValues );
	local::RemoveInvalidItems( l_Data.vecImages );
	local::RemoveInvalidItems( l_Data.vecConditionals );


	// SET OBJECT ATTRIBUTES FOR NEWLY SELECTED OBJECTS
	{// block scope
		size_t i;
		for ( i=0; i < l_Data.vecConditionals.size(); ++i )
			SetBits( l_Data.vecConditionals.at(i).object.ObjectAttributesSetRef(), SV_CH_CONDITIONAL, true );
		for ( i=0; i < l_Data.vecValues.size(); ++i )
			SetBits( l_Data.vecValues.at(i).object.ObjectAttributesSetRef(), SV_CH_VALUE, true );
		for ( i=0; i < l_Data.vecImages.size(); ++i )
			SetBits( l_Data.vecImages.at(i).object.ObjectAttributesSetRef(), SV_CH_IMAGE, true );
	}


	// PREPARE THE HISTORY...
	long lHistorySize;
	l_Data.lvoSize.GetValue( lHistorySize );
	l_Data.vecHistory.resize( lHistorySize );

	std::vector<SVConditionalHistoryEntry>::iterator iterHistory;
	for ( iterHistory = l_Data.vecHistory.begin(); iterHistory != l_Data.vecHistory.end(); ++iterHistory )
	{
		SVConditionalHistoryEntry& rEntry = *iterHistory;

		// SIZE THE VECTORS THAT WILL CONTAIN THE DATA
		rEntry.vecConditionals.resize( l_Data.vecConditionals.size() );
		rEntry.vecValues.resize( l_Data.vecValues.size() );
		rEntry.vecImages.resize( l_Data.vecImages.size() );

		// PREALLOCATE STRING BUFFERS
		size_t iIndex=0;
		for ( iIndex = 0; iIndex < rEntry.vecValues.size(); ++iIndex )
		{
			SVScalarValue& rValue = rEntry.vecValues.at(iIndex);
			rValue = l_Data.vecValues.at(iIndex);	// copy item to history entry
			// preallocate strings so no dynamic alloc during runtime
			if ( rValue.strValue.IsEmpty() )
			{
				rValue.strValue.GetBufferSetLength( 256 ); rValue.strValue.ReleaseBuffer();
			}
		}// end for ( iIndex = 0; iIndex < rEntry.vecValues.size(); ++iIndex )

		for ( iIndex = 0; iIndex < rEntry.vecConditionals.size(); ++iIndex )
		{
			SVScalarValue& rValue = rEntry.vecConditionals.at(iIndex);
			SVScalarValue& rInputValue = l_Data.vecConditionals.at(iIndex);
			rValue = rInputValue;	// copy item to history entry
			// preallocate strings so no dynamic alloc during runtime
			if ( rValue.strValue.IsEmpty() )
			{
				rValue.strValue.GetBufferSetLength( 256 ); rValue.strValue.ReleaseBuffer();
			}
			// this is used as temporary storage in CollectDataAndStore
			if ( rInputValue.strValue.IsEmpty() )
			{
				rInputValue.strValue.GetBufferSetLength( 256 ); rInputValue.strValue.ReleaseBuffer();
			}
		}// end for ( iIndex = 0; iIndex < rEntry.vecConditionals.size(); ++iIndex )

		// ALLOCATE IMAGE BUFFERS
		for ( iIndex = 0; iIndex < rEntry.vecImages.size(); ++iIndex )
		{
			SVRefCountImageBufferStruct& rEntryImage = rEntry.vecImages.at(iIndex);
			SVImageBufferStruct& rSpecifiedImageDef = l_Data.vecImages.at(iIndex);
			
			// compare currently specified buffer info to info that is already in the history entry
			if ( rEntryImage->info != rSpecifiedImageDef.info )	// only if info has changed do we need to recreate the buffers
			{
				// assign a raw SVImageBufferStruct to a ref counted struct;
				//  releases reference to (and destroys if necessary) old buffer;
				//  clears rImageDef.handle (which is NULL anyway)
				rEntryImage = rSpecifiedImageDef;

				// now allocate buffer
				hr = rEntryImage->CreateBuffer();
			}
			else	// no change in image info; just assign all non-buffer fields
			{
				rEntryImage->strName = rSpecifiedImageDef.strName;
				rEntryImage->object = rSpecifiedImageDef.object;
			}
		}// end for ( iIndex = 0; iIndex < rEntry.vecImages.size(); ++iIndex )
	}// end for ( iterHistory = l_vecHistory.begin(); iterHistory != l_vecHistory.end(); ++iterHistory )
	
	// EFFICIENTLY EXCHANGE CURRENT DATA WITH NEW DATA
	// **UNNECESSARY NOW BECAUSE CH IS MANAGED TOTALLY IN THE INSPECTION THREAD

	{// block scope for lock
		SVSingleLock lock( m_CritSec );
		m_Data.swap( l_Data );	// does nothing now because l_Data IS m_Data (see top of function for more info)
		if ( m_bClearQueue )	// signaled by a change in value or image inputs
		{
			m_Queue.reset( lHistorySize );
		}
	}// end block

	return hr;
}

HRESULT SVConditionalHistory::SetProperties( SVScalarValueVectorType& rvecProperties, bool p_bResetObject )
{
	HRESULT hr = S_OK;

	// CHECK FOR CHANGES

	bool bOverwriteChanged = false;
	bool bSizeChanged = false;

	SVScalarValueVectorType::iterator iter;

	for ( iter = rvecProperties.begin(); iter != rvecProperties.end(); ++iter )
	{
		SVScalarValue& rProperty = *iter;

		if ( rProperty.strName.CompareNoCase( PPQCH_Overwrite ) == 0 )
		{
			CString strNormalizedValue;
			HRESULT hrNormalize = m_Data.bvoOverwrite.GetNormalizedValue( rProperty.strValue, strNormalizedValue );
			if ( hrNormalize == S_OK )
			{
				bOverwriteChanged = (S_OK == m_Data.bvoOverwrite.CompareWithCurrentValue( strNormalizedValue ) );
				if ( bOverwriteChanged )
				{
					hr = m_bvoOverwrite.SetValue( 1, strNormalizedValue );	// set input to be read in ResetObject
					m_bClearQueue = true;
				}
			}
			else
			{
				hr = (unsigned long) -12391;	//!! SVO_ERR_BLAH_BLAH
			}
		}
		else if ( rProperty.strName.CompareNoCase( PPQCH_Size ) == 0 )
		{
			bSizeChanged = ( S_OK == m_Data.lvoSize.CompareWithCurrentValue( rProperty.strValue ) );

			// because we don't know what format the number is coming in as,
			// we need to remember the original value so we can set it back
			// if the value is out of range.
			CString strOriginalSize;
			m_lvoSize.GetValue( strOriginalSize );
			long lOriginalSize = 0;
			m_lvoSize.GetValue( lOriginalSize );

			KeepPrevError( hr, m_lvoSize.SetValue( 1, rProperty.strValue ) );

			CString strNew;
			m_lvoSize.GetValue( strNew );

			// do range checking
			long lSize = atol( strNew );
			if ( lSize < 0 || lSize > 10 )	// if outside valid range
			{
				m_lvoSize.SetValue( 1, strOriginalSize );	// set back to original value
				hr = (unsigned long) -12389;	//!! SVO_ERR_BLAH_BLAH
			}
			else	// valid size; flag a queue reset
			{
				m_bClearQueue = true;
			}
		}// end else if ( rProperty.strName.CompareNoCase( PPQCH_Size) )
		else
		{
			rProperty.status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
	}// end for ( iter = rvecProperties.begin(); iter != rvecProperties.end(); ++iter )

	// IF PROPERTIES HAVE CHANGED

	if ( p_bResetObject && ( bOverwriteChanged || bSizeChanged ) )
	{
		hr = ResetObject();
	}// end if ( bOverwriteChanged || bSizeChanged )

	return hr;
}

HRESULT SVConditionalHistory::GetProperties( SVScalarValueVectorType& rvecProperties )
{
	HRESULT hr = S_OK;
	rvecProperties.clear();
	CString strValue;
	
	m_Data.lvoSize.GetValue( strValue );
	rvecProperties.push_back( SVScalarValue(PPQCH_Size, strValue) );

	m_Data.bvoOverwrite.GetValue( strValue );
	rvecProperties.push_back( SVScalarValue(PPQCH_Overwrite, strValue) );
	
	return hr;
}

// if a pointer is NULL, there is no change for that list
HRESULT SVConditionalHistory::SetList( std::vector <SVScalarValue>* p_pvecValues, std::vector <SVScalarValue>* p_pvecImages, std::vector <SVScalarValue>* p_pvecConditionals, bool p_bResetObject )
{
	HRESULT hr = S_OK;

	std::vector<SVScalarValue> vecDummy;

	// yes, vecDummy may be passed multiple times.
	// It doesn't matter, it will be empty so therefore no validation will occur.
	hr = Validate( p_pvecValues ? *p_pvecValues : vecDummy,
	               p_pvecImages ? *p_pvecImages : vecDummy,
				   p_pvecConditionals ? *p_pvecConditionals : vecDummy );
	

	// For efficiency's sake, we will do this here instead of in ResetObject:

	// DETERMINE IF THERE IS A CHANGE IN THE IMAGE LIST OR THE VALUE LIST
	bool bValuesChanged = false;
	bool bImagesChanged = false;

	std::set<CString> setCurrentValues;
	std::set<CString> setNewValues;
	std::set<CString> setCurrentImages;
	std::set<CString> setNewImages;

	{// begin block: copy vectors to sets
		std::vector<SVScalarValue>::const_iterator iter;

		for ( iter = m_vecValues.begin(); iter != m_vecValues.end(); ++iter )
			setCurrentValues.insert( iter->strName );
		if ( p_pvecValues )
		{
			for ( iter = p_pvecValues->begin(); iter != p_pvecValues->end(); ++iter )
				setNewValues.insert( iter->strName );
			setNewValues.erase(CString());	// remove any empty string
		}

		std::vector<SVImageBufferStruct>::const_iterator iterImage;
		for ( iterImage = m_vecImages.begin(); iterImage != m_vecImages.end(); ++iterImage )
			setCurrentImages.insert( iterImage->strName );
		if ( p_pvecImages )
		{
			for ( iter = p_pvecImages->begin(); iter != p_pvecImages->end(); ++iter )
				setNewImages.insert( iter->strName );
			setNewImages.erase(CString());	// remove any empty string
		}
	}// end block

	// now compute differences
	std::set<CString> setValuesDifference;
	std::set<CString> setImagesDifference;

	if ( p_pvecValues )
	{
		std::set_symmetric_difference(setCurrentValues.begin(), setCurrentValues.end(),
									  setNewValues.begin(),     setNewValues.end(),
									  std::inserter(setValuesDifference, setValuesDifference.begin() ) );
		bValuesChanged = !setValuesDifference.empty();
	}
	if ( p_pvecImages )
	{
		std::set_symmetric_difference(setCurrentImages.begin(), setCurrentImages.end(),
									  setNewImages.begin(),     setNewImages.end(),
									  std::inserter(setImagesDifference, setImagesDifference.begin() ) );
		bImagesChanged = !setImagesDifference.empty();
	}

	// If list of values or images is different, clear current history (but not if just conditionals)
	if ( bValuesChanged || bImagesChanged )
	{
		m_bClearQueue = true;
	}

	// set inputs to be read in ResetObject
	if ( p_pvecValues )
	{
		m_vecValues = *p_pvecValues; 
	}
	if ( p_pvecImages )
	{
		m_vecImages.clear();
		m_vecImages.reserve( p_pvecImages->size() );
		for ( size_t i=0; i < p_pvecImages->size(); ++i )
		{
			SVScalarValue& rValue = p_pvecImages->at(i);
			SVImageBufferStruct buffer(rValue);
			m_vecImages.push_back( buffer );
		}
	}
	if ( p_pvecConditionals )
	{
		m_vecConditionals = *p_pvecConditionals;
	}

	if ( p_bResetObject )
		ResetObject();

	return hr;
}

HRESULT SVConditionalHistory::GetList( std::vector <SVScalarValue>& rvecData, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals )
{
	rvecData = m_Data.vecValues;
	rvecConditionals = m_Data.vecConditionals;
	rvecImages.clear();
	//rvecImages = m_vecImages;
	//*
	std::vector<SVImageBufferStruct>::iterator iter;
	for ( iter = m_Data.vecImages.begin(); iter != m_Data.vecImages.end(); ++iter )
	{
		rvecImages.push_back( SVScalarValue( iter->strName, _T("") ) );
	}
	//*/
	return S_OK;
}

HRESULT SVConditionalHistory::GetHistoryAndClear( std::vector < std::vector <SVScalarValue> >& rvecValues, std::vector < std::vector <SVImageBufferStruct> >& rvecImages, std::vector < std::vector <SVScalarValue> >& rvecConditionals, std::vector<long>& rvecProcessCount )
{
	HRESULT hr = S_OK;
	// do we want use the copy-and-swap here too?
	// this is different than ResetObject, because here we care about the actual data
	// that is being changed by another thread.
	// **UNNECESSARY for now, because CH is now controlled completely by the inspection thread
	SVSingleLock lock( m_CritSec );

	long lSize = m_Queue.get_used_count();	// can't use m_Data.vecHistory.size() because it is always equal to lvoSize
	rvecValues.resize( lSize );
	rvecImages.resize( lSize );
	rvecConditionals.resize( lSize );
	rvecProcessCount.resize( lSize );

	std::vector<long> vecHistoryIndexes;
	m_Queue.get_used(vecHistoryIndexes);

	ASSERT( vecHistoryIndexes.size() == lSize );
	ASSERT( rvecValues.size() == lSize );
	ASSERT( rvecImages.size() == lSize );
	ASSERT( rvecConditionals.size() == lSize );
	ASSERT( rvecProcessCount.size() == lSize );

	for ( long l = 0; l < lSize; ++l )
	{
		HRESULT l_hr = GetHistoryEntry( vecHistoryIndexes.at(l), rvecValues.at(l), rvecImages.at(l), rvecConditionals.at(l), rvecProcessCount.at(l) );
		KeepPrevError( hr, l_hr );
	}
	m_Queue.clear();
	return hr;
}

HRESULT SVConditionalHistory::GetMostRecentHistory( std::vector <SVScalarValue>& rvecData, std::vector <SVImageBufferStruct>& rvecImages, std::vector <SVScalarValue>& rvecConditionals, long& rlProcessCount )
{
	HRESULT hr = S_OK;

	SVSingleLock lock( m_CritSec );

	std::vector<long> vecHistoryIndexes;
	m_Queue.get_used(vecHistoryIndexes);

	if ( !vecHistoryIndexes.empty() )
	{
		hr = GetHistoryEntry( vecHistoryIndexes.back(), rvecData, rvecImages, rvecConditionals, rlProcessCount );
	}
	else	// no history available
	{
		rvecData.clear();
		rvecImages.clear();
		rvecConditionals.clear();
		hr = S_FALSE;	//!!!??? What do we want to do here?
	}

	return hr;
}

HRESULT SVConditionalHistory::GetHistoryEntry( long lIndex, std::vector <SVScalarValue>& p_rvecValues, std::vector <SVImageBufferStruct>& p_rvecImages, std::vector <SVScalarValue>& p_rvecConditionals, long& p_rlProcessCount )
{
	HRESULT hr = S_OK;

	SVConditionalHistoryEntry& rEntry = m_Data.vecHistory.at( lIndex );

	p_rlProcessCount = rEntry.lProcessCount;

	// COPY THE DATA AND CONDITIONALS
	p_rvecValues.resize( rEntry.vecValues.size() );
	std::copy( rEntry.vecValues.begin(), rEntry.vecValues.end(), p_rvecValues.begin() );

	p_rvecConditionals.resize( rEntry.vecConditionals.size() );
	std::copy( rEntry.vecConditionals.begin(), rEntry.vecConditionals.end(), p_rvecConditionals.begin() );

	// COPY THE IMAGE BUFFERS AND OVERLAYS
	p_rvecImages.resize( rEntry.vecImages.size() );

	for ( size_t i=0; i < p_rvecImages.size(); ++i )
	{
		SVImageBufferStruct& rImageCopy = p_rvecImages.at(i);
		SVImageBufferStruct& rImageSrc  = rEntry.vecImages.at(i).GetRef();

		rImageCopy = rImageSrc;	// make general assignment; copies info & overlay.
		                        // we need to manually handle the image buffer

		// create image buffers to copy into... or should caller be responsible???
		// CALLER CAN'T PRECREATE THE BUFFER because buffer can be different per entry  :(

		// create the new image buffer
		rImageCopy.handle.clear();	// assign null image handle
		rImageCopy.CreateBuffer();

		if( !( rImageCopy.handle.empty() ) && !( rImageSrc.handle.empty() ) )
		{
			SVImageBufferHandleImage l_MilHandle;
			rImageCopy.handle->GetData( l_MilHandle );

			SVImageBufferHandleImage l_SrcMilHandle;
			rImageSrc.handle->GetData( l_SrcMilHandle );

			// copy the image buffer
			SVMatroxBufferInterface::CopyBuffer( l_MilHandle.GetBuffer(), l_SrcMilHandle.GetBuffer() );
		}
	}

	return hr;
}

HRESULT SVConditionalHistory::CheckConditionals() const
{
	HRESULT l_Status = S_FALSE;

	for( size_t i = 0; l_Status != S_OK && i < m_Data.vecConditionals.size(); ++i )
	{
		const SVScalarValue& rValue = m_Data.vecConditionals.at(i);

		SVValueObjectReference refValueObject = rValue.object;

		if ( refValueObject.Object() )
		{
			double dValue = 0.0;
			
			refValueObject.GetValue( dValue );

			if( dValue != 0.0 )
			{
				l_Status = S_OK;
			}
		}
	}

	return l_Status;
}

HRESULT SVConditionalHistory::CollectDataAndStore()
{
	// collect the data here? or in InspectionProcess?
	bool bSlotAvailable = true;
	long lSize = 0;
	m_Data.lvoSize.GetValue( lSize );
	if ( lSize > 0 )
	{
		// check conditionals
		bool bConditionMet = false;
		size_t i=0;
		for ( i = 0; i < m_Data.vecConditionals.size(); ++i )
		{
			SVScalarValue& rValue = m_Data.vecConditionals.at(i);
			SVValueObjectReference refValueObject = rValue.object;
			if ( refValueObject.Object() )
			{
				double dValue = 0.0;
				refValueObject.GetValue( dValue );
				bConditionMet = bConditionMet || (dValue != 0.0);

				// store the current conditional value in a temporary spot (copy into history entry below)
				refValueObject.GetValue( rValue.strValue );	// stored in m_Data.vecConditionals
			}
		}// end for ( i = 0; i < m_Data.vecConditionals.size(); ++i )

		if ( bConditionMet )
		{
			bool bOverwrite = false;
			m_Data.bvoOverwrite.GetValue( bOverwrite );
			long lNextAvailable = -1;
			bSlotAvailable = m_Queue.get_next_available_index( lNextAvailable );

			if ( !bSlotAvailable && bOverwrite )
			{
				long lPoppedIndex;
				m_Queue.pop( lPoppedIndex );
				bSlotAvailable = m_Queue.get_next_available_index( lNextAvailable );
				ASSERT( bSlotAvailable );	// there should always be one available after a pop
			}

			if ( bSlotAvailable )
			{
				ASSERT( lNextAvailable != -1 );
				// get a reference to the next history entry.
				// it already has string/image buffers preallocated.
				// This will ensure that there is no dynamic memory allocation
				// being performed here.
				SVConditionalHistoryEntry& l_rNextEntry = m_Data.vecHistory.at( lNextAvailable );

				l_rNextEntry.lProcessCount = m_pInspection->LastProductGet( SV_OTHER ).ProcessCount();


				// store values
				for ( i = 0; i < m_Data.vecValues.size(); ++i )
				{
					SVScalarValue& rValue = m_Data.vecValues.at(i);
					SVValueObjectReference refValueObject = rValue.object;
					if ( refValueObject.Object() )
					{
						// store the current value in the history
						SVScalarValue& rValueEntry = l_rNextEntry.vecValues.at(i);
						refValueObject.GetValue( rValueEntry.strValue );
					}
				}

				// store conditional values
				for ( i = 0; i < m_Data.vecConditionals.size(); ++i )
				{
					SVScalarValue& rTempValue = m_Data.vecConditionals.at(i);
					SVScalarValue& rValueEntry = l_rNextEntry.vecConditionals.at(i);
					rValueEntry.strValue = rTempValue.strValue;
				}

				// store images
				for ( i = 0; i < m_Data.vecImages.size(); ++i )
				{
					SVImageBufferStruct& rImage = m_Data.vecImages.at(i);
					SVObjectReference refImage = rImage.object;
					SVImageClass* pImage = dynamic_cast <SVImageClass*> ( refImage.Object() );
					if ( pImage )
					{
						// get destination buffer struct
						SVRefCountImageBufferStruct& rImageBufferStruct = l_rNextEntry.vecImages.at(i);

						// get image's current info
						SVImageInfoClass l_SourceImageInfo = pImage->GetImageInfo();

						// if sizes are different...
						if ( rImageBufferStruct->info != l_SourceImageInfo )
						{
							// recreate the buffer
							rImageBufferStruct->info = l_SourceImageInfo;
							HRESULT hr = S_OK;
							hr = rImageBufferStruct->DestroyBuffer();
							hr = rImageBufferStruct->CreateBuffer();
						}

						// store the current image in the history
						SVSmartHandlePointer handle;
						pImage->GetImageHandle( handle );

						if( !( handle.empty() ) && !( rImageBufferStruct.Get()->handle.empty() ) )
						{
							SVImageBufferHandleImage l_MilHandle;
							handle->GetData( l_MilHandle );

							SVImageBufferHandleImage l_DestMilHandle;
							rImageBufferStruct.Get()->handle->GetData( l_DestMilHandle );

							SVMatroxBufferInterface::CopyBuffer( l_DestMilHandle.GetBuffer(), l_MilHandle.GetBuffer() );
						}

						// overlays
						// CollectOverlayData ignores the index (overlays aren't bucketized),
						// but it really doesn't matter because we are getting the data for the current inspection
						// THIS SHOULD BE A DATA INDEX NOT AN IMAGE INDEX
						SVImageOverlayClass& rOverlayClass = rImageBufferStruct->overlay;
						m_pInspection->CollectOverlayData( pImage, &rOverlayClass );
					}// end if ( pImage )
				}// end for ( i = 0; i < m_Data.vecImages.size(); ++i )

				// push queue index
				long lPushedIndex=-1;
				bool bPushed = m_Queue.push( lPushedIndex );
				ASSERT( bPushed );
				ASSERT( lPushedIndex == lNextAvailable );
			}// end if ( bSlotAvailable )
		}// end if ( bConditionMet )
	}// end if ( lSize > 0 )

	return bSlotAvailable ? S_OK : S_FALSE;
}

HRESULT SVConditionalHistory::DeleteTool( LPCTSTR p_strToolName )
{
	HRESULT l_hr = S_OK;
	bool l_bChanged = false;

	for( std::vector <SVScalarValue>::iterator l_it = m_vecValues.begin(); l_it != m_vecValues.end(); )
	{
		if( l_it->strName.Find( p_strToolName ) >= 0 )
		{
			// found tool name in value list.
			l_it = m_vecValues.erase( l_it );
			l_bChanged = true;
		}
		else
		{
			++l_it;
		}
	}

	for( std::vector <SVScalarValue>::iterator l_it = m_vecConditionals.begin() ; l_it != m_vecConditionals.end() ; )
	{
		if( l_it->strName.Find( p_strToolName ) >= 0 )
		{
			// found tool name in conditional list.
			l_it = m_vecConditionals.erase(l_it );
			l_bChanged = true;
		}
		else
		{
			++l_it;
		}
	}

	for( std::vector <SVImageBufferStruct>::iterator l_it = m_vecImages.begin() ; l_it != m_vecImages.end() ; )
	{
		if( l_it->strName.Find( p_strToolName ) >= 0)
		{
			// Found tool name in image list.
			l_it = m_vecImages.erase( l_it );
			l_bChanged = true;
		}
		else
		{
			++l_it;
		}
	}
 
	if( l_bChanged )
	{
		ResetObject();
	}
	return l_hr;
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistory.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 16:02:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   30 Apr 2013 09:29:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  831
 * SCR Title:  Fix Conditional History crash when tools are deleted
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DeleteTool function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:59:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   01 Aug 2012 12:49:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   02 Jul 2012 16:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Jun 2012 18:15:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   19 Dec 2011 15:40:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the evaluation process of conditional history.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   14 Jul 2011 08:42:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   25 Feb 2011 12:07:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Feb 2011 14:33:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   27 Jan 2011 10:58:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   28 Oct 2010 13:39:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Jul 2009 11:18:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   24 Jul 2007 11:47:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   21 Jun 2007 12:01:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9.1.0   14 Jul 2009 12:44:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated code to use the new data manager objects and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Mar 2006 09:23:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  557
 * SCR Title:  Fix Conditional History storage of images that change size in Run Mode
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for input image size change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   20 Mar 2006 13:01:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  548
 * SCR Title:  Fix Conditional History operation when History Size is 1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fix logic of checking conditional values so the rollover works with history size of 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   20 Mar 2006 12:48:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  546
 * SCR Title:  Fix bug in Conditional History transfer of overlays
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   call CollectOverlayData on the correct image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jan 2006 06:58:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed GetHistoryEntry to have the MbufCopy parameters in the right order
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   26 Jan 2006 14:10:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed GetHistoryAndClear to actually clear the queue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   26 Jan 2006 11:00:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed bug when CH size == 0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Jan 2006 14:17:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Conditional History bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jan 2006 08:54:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Process Count to Conditional History
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2006 14:55:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed property updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
