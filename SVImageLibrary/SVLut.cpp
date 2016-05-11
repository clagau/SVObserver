//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLut
//* .File Name       : $Workfile:   SVLut.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 07:16:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLut.h"
#pragma endregion Includes

SVLutInfo::SVLutInfo() : mHandle(0), muiFormat(SVLUT_FORMAT_NEUTRAL), muiBands(0), muiBandSize(0), muiMaxValue(0), mpTransform(nullptr)
{
}

SVLutInfo::SVLutInfo( UINT p_Bands, UINT p_BandSize, UINT p_BandMaxValue, const SVLutTransform& p_rTransform )
: mHandle( 0 )
, muiFormat( SVLUT_FORMAT_NEUTRAL )
, muiBands( p_Bands )
, muiBandSize( p_BandSize )
, muiMaxValue( p_BandMaxValue )
, mpTransform( p_rTransform.Clone() )
{
}

SVLutInfo::SVLutInfo(const SVLutInfo& rhs) : mHandle(0), muiFormat(SVLUT_FORMAT_NEUTRAL), muiBands(0), muiBandSize(0), muiMaxValue(0), mpTransform(nullptr)
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
	{
		delete mpTransform;
	}
	mpTransform = nullptr;
}

const SVLutInfo& SVLutInfo::operator = (const SVLutInfo& rhs)
{
	if (this != &rhs)
	{
		Cleanup();
		muiFormat = rhs.muiFormat;
		muiBands = rhs.muiBands;
		muiBandSize = rhs.muiBandSize;
		muiMaxValue = rhs.muiMaxValue;
		if ( rhs.mpTransform )
		{
			mpTransform = rhs.mpTransform->Clone();
		}
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
		{
			delete mpTransform;
		}
		mpTransform = rTransform.Clone();
	}
	return nullptr != mpTransform;
}

bool SVLutInfo::SetTransformOperation(const SVLutTransformOperation& rOperation)
{
	if (mpTransform)
	{
		mpTransform->SetOperation(rOperation);
	}
	return nullptr != mpTransform;
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
	return 0 == mHandle;
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
	return mpTransform ? mpTransform->GetOperation() : nullptr;
}

bool SVLutInfo::GetTransformParameters(SVLutTransformParameters& rParam) const
{
	bool bRet = false;
	if ( mpTransform )
	{
		bRet = mpTransform->GetParameters(rParam);
	}
	return bRet;
}

bool SVLutInfo::GetTransformParameters(SAFEARRAY*& rpsaParam) const
{
	bool bRet = false;
	ASSERT( nullptr == rpsaParam );	// must clean up and set to nullptr before calling
	if ( mpTransform && nullptr == rpsaParam )
	{
		SVLutTransformParameters param;
		bRet = mpTransform->GetParameters(param);
		SAFEARRAYBOUND saBounds[1];
		saBounds[0].lLbound = 0;
		saBounds[0].cElements = static_cast< ULONG >( param.GetSize() );
		rpsaParam = ::SafeArrayCreate( VT_I4, 1, saBounds);
		if (nullptr != rpsaParam)
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
			if ( nullptr == mInfo.GetTransform() && nullptr != lutband.Info().GetTransform() )
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
	{
		return S_OK == mInfo.GetTransform()->Transform(*this, param);
	}
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
	return static_cast< UINT >( maTable.size() );
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
	ASSERT( nullptr != psaBandData );	// must be valid SAFEARRAY
	if (nullptr != psaBandData)
	{
		VARTYPE vt;
		::SafeArrayGetVartype(psaBandData, &vt);
		ASSERT( VT_I4 == vt || VT_UI4 == vt );
		if ( VT_I4 == vt || VT_UI4 == vt )
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
					if (S_OK == hr)
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
	bool bRet = false;
	ASSERT( nullptr == rpsaBandData );	// must clean up and set to nullptr before calling
	if (nullptr == rpsaBandData)
	{
		SAFEARRAYBOUND saBounds[1];
		saBounds[0].lLbound = 0;
		saBounds[0].cElements = static_cast< ULONG >( maTable.size() );
		rpsaBandData = ::SafeArrayCreate( VT_I4, 1, saBounds);
		if (nullptr != rpsaBandData)
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
	if ( nullptr == pThisTransform && nullptr == pRhsTransform)
	{}
	else if ( nullptr == pThisTransform || nullptr == pRhsTransform )
	{ bEqual = false; }
	else
	{ bEqual = *pThisTransform == *pRhsTransform; }

	if ( bEqual )
	{
		size_t iSize = maTable.size();
		if ( iSize == rhs.maTable.size() )
		{
			for ( unsigned int i=0; (i < maTable.size()) && bEqual; i++)
			{
				bEqual = bEqual && (maTable[i] == rhs.maTable[i]);
			}
		}
		else
		{
			bEqual = false;
		}
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
	return false;
}

bool SVLut::CopyBandData(const SVLut& lut, int iBand)
{
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

bool SVLut::IsCreated() const
{
	return maBands.size() > 0;
}

bool SVLut::Create(const SVLutInfo& info)
{
	bool bRet = true;

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
	return static_cast< ULONG >( maBands.size() );
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
	bool bRet = nullptr == mInfo.GetTransform();
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
	ASSERT( nullptr != psaBands );	// must be valid SAFEARRAY
	if (nullptr != psaBands)
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
				ASSERT( VT_I4 == vt || VT_UI4 == vt );
				if ( VT_I4 == vt || VT_UI4 == vt )
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
								if (S_OK == hr)
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
	ASSERT( nullptr == rpsaBands);	// must clean up and set to nullptr before calling
	if( nullptr == rpsaBands && 0 < mInfo.BandSize() && 0 < mInfo.Bands() )
	{
		SAFEARRAYBOUND saBounds[2];

		saBounds[0].lLbound = 0;
		saBounds[0].cElements = mInfo.BandSize();
		saBounds[1].lLbound=0;
		saBounds[1].cElements = mInfo.Bands();

		rpsaBands = ::SafeArrayCreate( VT_I4, 2, saBounds);

		if (nullptr != rpsaBands)
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
	SVLutInfo info;
	lut.Create(info);

	lut.Destroy();
	lut.Create(info);
	lut.Create(info);	// check for auto cleanup

	if (lut.Info().IsNullHandle())
	{
		TRACE("Problem with IsNullHandle - false positive\n");
	}

	if (! info.IsNullHandle() )
	{
		TRACE("Problem with IsNullHandle - false negative\n");
	}

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
