//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLut
//* .File Name       : $Workfile:   SVLut.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVLut.h"

//#if defined(_DEBUG) || defined(_MINDEBUG)	// for SVLutTestCases
//#include "SVObserver.h"
//#include "SVAcquisitionClass.h"

//	#include "SVImageProcessingClass.h"
//#endif


SVLutInfo::SVLutInfo() : mHandle(NULL), muiFormat(SVLUT_FORMAT_NEUTRAL), muiBands(0), muiBandSize(0), muiMaxValue(0), mpTransform(NULL)
{
}

SVLutInfo::SVLutInfo( UINT p_Bands, UINT p_BandSize, UINT p_BandMaxValue, const SVLutTransform& p_rTransform )
: mHandle( NULL )
, muiFormat( SVLUT_FORMAT_NEUTRAL )
, muiBands( p_Bands )
, muiBandSize( p_BandSize )
, muiMaxValue( p_BandMaxValue )
, mpTransform( p_rTransform.Clone() )
{
}

SVLutInfo::SVLutInfo(const SVLutInfo& rhs) : mHandle(NULL), muiFormat(SVLUT_FORMAT_NEUTRAL), muiBands(0), muiBandSize(0), muiMaxValue(0), mpTransform(NULL)
{
	*this = rhs;
}

SVLutInfo::~SVLutInfo()
{
	Cleanup();
}

void SVLutInfo::Cleanup()
{
	if (mpTransform)
		delete mpTransform;
	mpTransform = NULL;
}

const SVLutInfo& SVLutInfo::operator = (const SVLutInfo& rhs)
{
//	ASSERT( this != &rhs );
	if (this != &rhs)
	{
		Cleanup();
		muiFormat = rhs.muiFormat;
		muiBands = rhs.muiBands;
		muiBandSize = rhs.muiBandSize;
		muiMaxValue = rhs.muiMaxValue;
		if ( rhs.mpTransform )
			mpTransform = rhs.mpTransform->Clone();
	}
	return *this;
}

void SVLutInfo::CopyNoTransform(const SVLutInfo& rhs)
{
	ASSERT( this != &rhs );
	if (this != &rhs)
	{
		muiFormat = rhs.muiFormat;
		muiBands = rhs.muiBands;
		muiBandSize = rhs.muiBandSize;
		muiMaxValue = rhs.muiMaxValue;
	}
}


bool SVLutInfo::SetTransform(const SVLutTransform& rTransform)
{
	if (mpTransform != &rTransform)
	{
		if (mpTransform)
			delete mpTransform;
		mpTransform = rTransform.Clone();
	}
	return mpTransform != NULL;
}

bool SVLutInfo::SetTransformOperation(const SVLutTransformOperation& rOperation)
{
	if (mpTransform)
	{
		mpTransform->SetOperation(rOperation);
	}
	return mpTransform != NULL;
}


SVHANDLE SVLutInfo::GetHandle() const
{
	return mHandle;
}


void SVLutInfo::SetBands(UINT nBands)
{
	muiBands = nBands;
}

UINT SVLutInfo::Bands() const
{
	return muiBands;
}

bool SVLutInfo::IsNullHandle() const
{
	return mHandle == NULL;
}

UINT SVLutInfo::Format() const
{
	return muiFormat;
}

void SVLutInfo::SetFormat(UINT nFormat)
{
	muiFormat = nFormat;
}

void SVLutInfo::SetBandSize(UINT nSize)
{
	muiBandSize = nSize;
}

UINT SVLutInfo::BandSize() const
{
	return muiBandSize;
}

void SVLutInfo::SetMaxValue(UINT uiMaxValue)
{
	muiMaxValue = uiMaxValue;
}

UINT SVLutInfo::MaxValue() const
{
	return muiMaxValue;
}

const SVLutTransform* SVLutInfo::GetTransform() const
{
	return mpTransform;
}

SVLutTransform*& SVLutInfo::GetTransform()
{
	return mpTransform;
}

const SVLutTransformOperation* SVLutInfo::GetTransformOperation() const
{
	return mpTransform ? mpTransform->GetOperation() : NULL;
}

bool SVLutInfo::GetTransformParameters(SVLutTransformParameters& rParam) const
{
	bool bRet=false;
	if ( mpTransform )
	{
		bRet = mpTransform->GetParameters(rParam);
	}
	return bRet;
}

bool SVLutInfo::GetTransformParameters(SAFEARRAY*& rpsaParam) const
{
	bool bRet=false;
	ASSERT( rpsaParam == NULL );	// must clean up and set to NULL before calling
	if ( mpTransform && rpsaParam == NULL )
	{
		SVLutTransformParameters param;
		bRet = mpTransform->GetParameters(param);
		SAFEARRAYBOUND saBounds[1];
		saBounds[0].lLbound=0;
		saBounds[0].cElements = param.GetSize();
		rpsaParam = ::SafeArrayCreate( VT_I4, 1, saBounds);
		if (rpsaParam != NULL)
		{
			for (long l=0; l < param.GetSize(); l++)
			{
				long lVal = param.GetAt(l);
				::SafeArrayPutElement(rpsaParam, &l, (void*) &lVal);
			}
			bRet = true;
		}
	}
	return bRet;
}



SVLutBand::SVLutBand()
{
}

SVLutBand::SVLutBand( const SVLutBand& rhs )
{
	*this = rhs;
}

SVLutBand::~SVLutBand()
{
	// nothing necessary because all members self-cleanup
}

const SVLutBand& SVLutBand::operator = ( const SVLutBand& rhs )
{
	ASSERT( this != &rhs );
	if ( this != &rhs )
	{
		mulBand = rhs.mulBand;
		maTable = rhs.maTable;
		mInfo = rhs.mInfo;
	}

	return *this;
}

void SVLutBand::CopyNoTransform( const SVLutBand& rhs )
{
	ASSERT( this != &rhs );
	if ( this != &rhs )
	{
		mulBand = rhs.mulBand;
		maTable = rhs.maTable;
		mInfo.CopyNoTransform( rhs.mInfo );
	}
}

bool SVLutBand::CopyBandData(const SVLutBand& lutband)
{
	bool bRet = false;
	ASSERT( mulBand == lutband.Band() );
	ASSERT( mInfo.BandSize() == lutband.Info().BandSize() );
	if ( mulBand == lutband.Band() && mInfo.BandSize() == lutband.Info().BandSize() )
	{
		maTable = lutband.maTable;
		// copy only data-related Info
		if (lutband.Info().GetTransformOperation())
		{
			if ( mInfo.GetTransform() == NULL && lutband.Info().GetTransform() != NULL )
			{
				mInfo.SetTransform( *lutband.Info().GetTransform() );
			}
			mInfo.SetTransformOperation(*lutband.Info().GetTransformOperation());
			SVLutTransformParameters param;
			lutband.Info().GetTransformParameters(param);
			mInfo.GetTransform()->SetParameters(param);
		}
		bRet = true;
	}
	return bRet;
}

bool SVLutBand::Create(const SVLutInfo& info, UINT uiBand)
{
	Destroy();

	maTable.clear();
	maTable.resize( info.BandSize() );

	mInfo = info;
	mInfo.SetBands( 1 );
	mulBand = uiBand;

	return true;
}

bool SVLutBand::Destroy()
{
	maTable.clear();
	return true;
}

bool SVLutBand::Transform(const SVLutTransformParameters& param)
{
	if (mInfo.GetTransform())
		return mInfo.GetTransform()->Transform(*this, param) == S_OK;
	else
		return false;
}

ULONG SVLutBand::Band() const
{
	return mulBand;
}

SVLutEntry& SVLutBand::operator () (UINT nIndex)
{
	return maTable[nIndex];
}

const SVLutEntry& SVLutBand::operator () (UINT nIndex) const
{
	return maTable[nIndex];
}

const SVLutInfo& SVLutBand::Info() const
{
	return mInfo;
}

UINT SVLutBand::Size() const
{
	return maTable.size();
}

const SVLutTransformOperation* SVLutBand::GetTransformOperation() const
{
	return mInfo.GetTransformOperation();
}

bool SVLutBand::SetTransformOperation(const SVLutTransformOperation& pType)
{
	return mInfo.SetTransformOperation(pType);
}

bool SVLutBand::SetBandData(SAFEARRAY* psaBandData)
{
	bool bRet=false;
	ASSERT( psaBandData != NULL );	// must be valid SAFEARRAY
	if (psaBandData != NULL)
	{
		VARTYPE vt;
		::SafeArrayGetVartype(psaBandData, &vt);
		ASSERT( vt == VT_I4 || vt == VT_UI4 );
		if ( vt == VT_I4 || vt == VT_UI4 )
		{
			long lSize;
			::SafeArrayGetUBound(psaBandData, 1, &lSize);
			lSize++;
			ASSERT( lSize == maTable.size() );	// size must match!
			if ( lSize == maTable.size() )
			{
				long lBegin;
				::SafeArrayGetLBound(psaBandData, 1, &lBegin);
				long lEnd = lBegin + lSize;
				for (long l = lBegin; l < lEnd; l++)
				{
					long lVal;
					HRESULT hr = ::SafeArrayGetElement(psaBandData, &l, (void*) &lVal);
					if (hr == S_OK )
					{
						maTable[l-lBegin] = lVal;
					}
				}
				bRet = true;
			}
		}
	}
	return bRet;
}

bool SVLutBand::GetBandData(SAFEARRAY*& rpsaBandData) const
{
	bool bRet=false;
	ASSERT( rpsaBandData == NULL );	// must clean up and set to NULL before calling
	if (rpsaBandData == NULL)
	{
		SAFEARRAYBOUND saBounds[1];
		saBounds[0].lLbound=0;
		saBounds[0].cElements = maTable.size();
		rpsaBandData = ::SafeArrayCreate( VT_I4, 1, saBounds);
		if (rpsaBandData != NULL)
		{
			for ( long l = 0; static_cast< unsigned long>( l ) < maTable.size(); l++)
			{
				long lVal = maTable[l];
				::SafeArrayPutElement(rpsaBandData, &l, (void*) &lVal);
			}
			bRet = true;
		}
	}
	return bRet;
}

bool SVLutBand::operator == ( const SVLutBand& rhs ) const
{
	// we will define equality as having the same transform type and same data
	bool bEqual = true;

	const SVLutTransform* pThisTransform = Info().GetTransform();
	const SVLutTransform* pRhsTransform = rhs.Info().GetTransform();
	if ( pThisTransform == NULL && pRhsTransform == NULL )
		{}
	else if ( pThisTransform == NULL || pRhsTransform == NULL )
		{bEqual = false;}
	else
		{bEqual = *pThisTransform == *pRhsTransform;}

	if ( bEqual )
	{
		int iSize = maTable.size();
		if ( iSize == rhs.maTable.size() )
		{
			for ( unsigned int i=0; (i < maTable.size()) && bEqual; i++)
			{
				bEqual = bEqual && (maTable[i] == rhs.maTable[i]);
			}
		}
		else
			bEqual = false;
	}

	return bEqual;
}



SVLut::SVLut()
{
}

SVLut::SVLut( const SVLut& rhs )
{
	*this = rhs;
}

SVLut::~SVLut()
{
	// nothing necessary because all members self-cleanup
}

const SVLut& SVLut::operator = ( const SVLut& rhs )
{
	//ASSERT( this != &rhs );
	if ( this != &rhs )
	{
		maBands = rhs.maBands;
		mInfo = rhs.mInfo;
	}

	return *this;
}

void SVLut::CopyNoTransform( const SVLut& rhs )
{
	if ( this != &rhs )
	{
		ASSERT( rhs.mInfo.Bands() == rhs.maBands.size() );
		mInfo.CopyNoTransform( rhs.mInfo );
		if ( maBands.size() != rhs.maBands.size() )
		{
			maBands.clear();
			maBands.resize(mInfo.Bands());	// this constructs the SVLutBand objects
		}
		for (UINT i = 0; i < rhs.mInfo.Bands(); i++)
		{
			maBands[i].CopyNoTransform( rhs(i) );
		}
	}
}


bool SVLut::CopyBandData(const SVLutBand& lutband)
{
	ASSERT( lutband.Band() >= 0 && lutband.Band() < mInfo.Bands() );
	if ( lutband.Band() >= 0 && lutband.Band() < mInfo.Bands() )
	{
		maBands[ lutband.Band() ].CopyBandData(lutband);
		return true;
	}
	else
		return false;
}

bool SVLut::CopyBandData(const SVLut& lut, int iBand)
{
	//ASSERT (   maBands.GetSize() == lut.maBands.GetSize() && mInfo.BandSize() == lut.Info().BandSize() );

	/*
	if ( NumBands() == 0 )
	{
		Create( lut.Info() );
	}
	*/


//	ASSERT (   maBands.size() == lut.maBands.size() && mInfo.BandSize() == lut.Info().BandSize() );

	bool bRet = false;

	if (   maBands.size()   == lut.maBands.size()
	    && mInfo.BandSize() == lut.Info().BandSize() )
	{
		if (iBand == -1)
		{
			// loop through all bands & copy data
			for (UINT i = 0; i < mInfo.Bands(); i++)
			{
				maBands[i].CopyBandData(lut(i));
			}
			bRet = true;
		}
		else
		{
			ASSERT( static_cast< unsigned long >( iBand ) < maBands.size() && iBand >= 0);
			if ( static_cast< unsigned long >( iBand ) < maBands.size() && iBand >= 0)
			{
				maBands[iBand].CopyBandData(lut(iBand));
				bRet = true;
			}
		}
	}

	return bRet;
}

bool SVLut::IsCreated()
{
	return maBands.size() > 0;
}


bool SVLut::Create(const SVLutInfo& info)
{
	bool bRet = true;

//	if ( maBands.size() > 0 || !mInfo.IsNullHandle() )
	if ( maBands.size() > 0 )
	{
		Destroy();
	}

	mInfo = info;
	maBands.clear();
	maBands.resize(mInfo.Bands());	// this constructs the SVLutBand objects
	for (UINT i=0; i < mInfo.Bands(); i++)
	{
		SVLutBand& rBand = maBands[i];
		rBand.Create(mInfo, i);
	}

	return bRet;
}

bool SVLut::Destroy()
{
	bool bRet = true;

	maBands.clear();

	return bRet;
}

bool SVLut::Transform(const SVLutTransformParameters& param)
{
	bool bRet = true;
	for (UINT i=0; i<mInfo.Bands(); i++)
	{
		bRet = bRet && maBands[i].Transform(param);
	}
	return bRet;
}

SVLutBand& SVLut::operator () (UINT nBand)
{
	return maBands[nBand];
}

const SVLutBand& SVLut::operator () (UINT nBand) const
{
	return maBands[nBand];
}

SVLutEntry& SVLut::operator () (UINT nBand, UINT nIndex)
{
	return maBands[nBand](nIndex);
}

const SVLutEntry& SVLut::operator () (UINT nBand, UINT nIndex) const
{
	return maBands[nBand](nIndex);
}

ULONG SVLut::NumBands() const
{
	return maBands.size();
}

const SVLutInfo& SVLut::Info() const
{
	return mInfo;
}

const SVLutTransformOperation* SVLut::GetTransformOperation() const
{
	return mInfo.GetTransformOperation();
}

bool SVLut::SetTransform(const SVLutTransform& rTransform)
{
	bool bRet = mInfo.GetTransform() == NULL;
	if ( bRet )
	{
		ASSERT( mInfo.Bands() == maBands.size() );
		for (UINT i=0; i<mInfo.Bands(); i++)
		{
			bRet = bRet && maBands[i].mInfo.SetTransform(rTransform);
		}
		bRet = mInfo.SetTransform(rTransform) && bRet;
	}
	return bRet;
}

bool SVLut::SetTransformOperation(const SVLutTransformOperation& pType)
{
	bool bRet = true;
	for (UINT i=0; i<mInfo.Bands(); i++)
	{
		bRet = bRet && maBands[i].mInfo.SetTransformOperation(pType);
	}
	return mInfo.SetTransformOperation(pType) && bRet;
}

bool SVLut::SetBandData(SAFEARRAY* psaBands)
{
	bool bRet=false;
	ASSERT( psaBands != NULL );	// must be valid SAFEARRAY
	if (psaBands != NULL)
	{
		UINT uiDims = ::SafeArrayGetDim(psaBands);
		ASSERT( uiDims <= 2 );
		if ( uiDims <= 2 )
		{
			ULONG lNumBands;
			long lBandLBound, lBandUBound;
			::SafeArrayGetLBound(psaBands, 2, &lBandLBound);
			::SafeArrayGetUBound(psaBands, 2, &lBandUBound);
			lNumBands = lBandUBound - lBandLBound + 1;

			ASSERT( lNumBands == mInfo.Bands() );
			if ( lNumBands == mInfo.Bands() )
			{
				VARTYPE vt;
				::SafeArrayGetVartype(psaBands, &vt);
				ASSERT( vt == VT_I4 || vt == VT_UI4 );
				if ( vt == VT_I4 || vt == VT_UI4 )
				{
					ULONG lBandSize;
					long lBandSizeLBound, lBandSizeUBound;
					::SafeArrayGetLBound(psaBands, 1, &lBandSizeLBound);
					::SafeArrayGetUBound(psaBands, 1, &lBandSizeUBound);
					lBandSize = lBandSizeUBound - lBandSizeLBound + 1;

					ASSERT( lBandSize == mInfo.BandSize() );	// size must match!
					if ( lBandSize == mInfo.BandSize() )
					{
						long alDimIndex[2];
						for (unsigned long lBand=0; lBand < lNumBands; lBand++)
						{
							alDimIndex[1] = lBand;
							long lBegin = lBandSizeLBound;
							long lEnd = lBegin + lBandSize;
							for (long l = lBegin; l < lEnd; l++)
							{
								long lVal;
								alDimIndex[0] = l;
								HRESULT hr = ::SafeArrayGetElement(psaBands, alDimIndex, (void*) &lVal);
								if (hr == S_OK )
								{
									maBands[lBand].maTable[l-lBegin] = lVal;
								}
							}// end for (long l = lBegin; l < lEnd; l++)
						}// end for (long lBand=0; lBand < lNumBands; lBand++)
						bRet = true;
					}// end if ( lBandSize == mInfo.BandSize() )
				}// end if ( vt == VT_I4 || vt == VT_UI4 )
			}// end if ( lNumBands == mInfo.Bands() )
		}// end if ( uiDims <= 2 )
	}
	return bRet;
}

bool SVLut::GetBandData(SAFEARRAY*& rpsaBands) const
{
	bool bRet = false;
	ASSERT( rpsaBands == NULL );	// must clean up and set to NULL before calling
	if( rpsaBands == NULL && 0 < mInfo.BandSize() && 0 < mInfo.Bands() )
	{
		SAFEARRAYBOUND saBounds[2];

		saBounds[0].lLbound=0;
		saBounds[0].cElements = mInfo.BandSize();
		saBounds[1].lLbound=0;
		saBounds[1].cElements = mInfo.Bands();

		rpsaBands = ::SafeArrayCreate( VT_I4, 2, saBounds);

		if (rpsaBands != NULL)
		{
			long alDimIndex[2];
			for (unsigned long lBand = 0; lBand < mInfo.Bands(); lBand++)
			{
				alDimIndex[1] = lBand;
				for (unsigned long l = 0; l < mInfo.BandSize(); l++)
				{
					alDimIndex[0] = l;
					long lVal = maBands[lBand](l);
					::SafeArrayPutElement(rpsaBands, alDimIndex, (void*) &lVal);
				}
			}
			bRet = true;
		}
	}
	return bRet;
}













SVLutTestCases::SVLutTestCases()
{
	TRACE("-----Begin LUT Test Cases-----\n");

	SVLutEntry entry;
	entry = 5;
	ULONG ul = entry;
	SVLutValueType val = 3;
	val = entry;
	entry = val;


	SVLut lut;
//	CORLUT cor = CORHANDLE_NULL;
//	MIL_ID mil = lut.Info();

//	cor = CORHANDLE_NULL;
//	mil = 123;

	SVLutInfo info;
//	info = cor;
//	info = mil;

	lut.Create(info);

/*	if ( CORHANDLE_IS_NULL(cor) )
	{
		TRACE("testing compiler\n");
	}
	*/

/*	if ( CORHANDLE_IS_NULL(lut.Info().COR()))
	{
		TRACE("Cor is NULL works\n");
	}
*/
/*	if (lut.Info().MIL() == M_NULL)
	{
		TRACE("uh-oh (mil)\n");
	}
*/

/*
	__CORHANDLE corh;
	cor = &corh;
	cor->location = 123;
	mil = M_NULL;
	
	info = cor;
	info = mil;

	info.SetCor(cor);
	info.SetMil(mil);
*/	
	//info.SetCOR(mil);	// shouldn't be able to compile
	//info.SetMIL(cor);	// shouldn't be able to compile

	lut.Destroy();
	lut.Create(info);
	lut.Create(info);	// check for auto cleanup

	/*
	if ( CORHANDLE_IS_NULL(lut.Info().COR()))
	{
		TRACE("uh-oh (cor)\n");
	}
	*/

	/*
	if (lut.Info().MIL() == M_NULL)
	{
		TRACE("MIL is NULL works\n");
	}
	*/

	if (lut.Info().IsNullHandle())
	{
		TRACE("Problem with IsNullHandle - false positive\n");
	}

	//info.SetCor(CORHANDLE_NULL);
	//info.SetMil(M_NULL);

	if (! info.IsNullHandle() )
	{
		TRACE("Problem with IsNullHandle - false negative\n");
	}

	//SVLutTransform transform_test;
	SVDefaultLutTransform transform;
	transform.SetOperation(SVLutTransformOperationClip());
	info.SetTransform(transform);
	transform.SetOperation(SVLutTransformOperationInvert());
	info.SetTransform(transform);

	const SVLutTransform* pTransform = info.GetTransform();
	const SVLutTransformOperation* pOperation = info.GetTransformOperation();

	info.SetTransform(*pTransform);

	info.SetTransformOperation(*pOperation);

	int a=0;
	a++;

	TRACE("-----Done LUT Test Cases-----\n");
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVLut.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Aug 2012 15:34:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed unused typdef for LUT array.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Dec 2010 15:41:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jul 2010 10:56:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Lut class to not produce an empty arrary for Device Parameters when there is not LUT information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Feb 2010 11:26:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new constructors to allow for single line construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   20 Jun 2007 11:09:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jan 2004 10:09:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed annoying ASSERT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Dec 2003 15:14:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed equality comparison
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Dec 2003 14:57:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added operator ==
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 Sep 2003 07:43:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleaned up code
 * added MaxValue property
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:01:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 10:57:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Mar 2003 16:14:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed the ASSERT in the assignment operator
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   06 Feb 2003 10:38:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed const-correctness
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Jan 2003 12:34:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified SetBandData and GetBandData
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2003 12:53:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SVLut::GetBandData and SVLut::SetBandData
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 09:51:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   General Lut changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jan 2003 12:37:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   First iteration of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
