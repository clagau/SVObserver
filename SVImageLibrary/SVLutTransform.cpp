//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLutTransform
//* .File Name       : $Workfile:   SVLutTransform.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:48  $
//******************************************************************************


#include "stdafx.h"
#include "SVLutTransform.h"
#include "SVLut.h"
#include <typeinfo.h>

SVLutTransformOperationMap SVLutTransform::mMapTypes;

SVLutTransformOperationMap::SVLutTransformOperationMap()
{
	maTypeInfo.SetAtGrow(LutTransformTypeNormal, SVLutTransformTypeInfo(LutTransformTypeNormal, new SVLutTransformOperationNormal, _T("Normal")) );
	maTypeInfo.SetAtGrow(LutTransformTypeInversion, SVLutTransformTypeInfo(LutTransformTypeInversion, new SVLutTransformOperationInvert, _T("Invert")) );
	maTypeInfo.SetAtGrow(LutTransformTypeSign, SVLutTransformTypeInfo(LutTransformTypeSign, new SVLutTransformOperationSign(), _T("Sign")) );
	maTypeInfo.SetAtGrow(LutTransformTypeClip, SVLutTransformTypeInfo(LutTransformTypeClip, new SVLutTransformOperationClip(), _T("Clip")) );
	maTypeInfo.SetAtGrow(LutTransformTypeFreeform, SVLutTransformTypeInfo(LutTransformTypeFreeform, new SVLutTransformOperationFreeform(), _T("Freeform")) );
	maTypeInfo.SetAtGrow(LutTransformTypeTwoKnee, SVLutTransformTypeInfo(LutTransformTypeTwoKnee, new SVLutTransformOperationTwoKnee(), _T("Two Knee")) );
};

SVLutTransformOperationMap::~SVLutTransformOperationMap()
{
	for (int i=0; i<MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].pType)
			delete maTypeInfo[i].pType;
	}
}


SVLutTransformOperation* SVLutTransformOperationMap::GetType(SVLutTransformOperationEnum e)
{
	if (e < MAX_LUT_TRANSFORM_TYPE && (int)e >= 0)
		return maTypeInfo[e].pType;
	else
		return NULL;
}

SVLutTransformOperationEnum SVLutTransformOperationMap::GetType(const SVLutTransformOperation* pType)
{
	ASSERT( pType );
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (pType && maTypeInfo[i].pType && typeid(*maTypeInfo[i].pType) == typeid(*pType))
			return maTypeInfo[i].eType;
	}
	return LutTransformTypeUnknown;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(SVLutTransformOperationEnum e)
{
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].eType == e)
			return &maTypeInfo.ElementAt(i);
	}
	return NULL;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const SVLutTransformOperation* pType)
{
	ASSERT( pType );
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (pType && maTypeInfo[i].pType && typeid(*maTypeInfo[i].pType) == typeid(*pType))
			return &maTypeInfo.ElementAt(i);
	}
	return NULL;
}

const SVLutTransformOperationMap::SVLutTransformTypeInfo* SVLutTransformOperationMap::GetInfo(const CString& sType)
{
	for (int i=0; i < MAX_LUT_TRANSFORM_TYPE; i++)
	{
		if (maTypeInfo[i].sType == sType && !sType.IsEmpty())
			return &maTypeInfo.ElementAt(i);
	}
	return NULL;
}






SVLutTransformOperation::~SVLutTransformOperation()
{
}



SVLutTransform::SVLutTransform()
{
	mpType = NULL;
	mbOwnType = false;
}

SVLutTransform::SVLutTransform(const SVLutTransformOperation& rType)
{
	mpType = NULL;
	mbOwnType = false;
	SetOperation( rType );
}

SVLutTransform::SVLutTransform(const SVLutTransform& rhs)
{
	mpType = NULL;
	mbOwnType = false;
	*this = rhs;
}

SVLutTransform::~SVLutTransform()
{
	Cleanup();
}

SVLutTransform& SVLutTransform::operator = (const SVLutTransform& rhs)
{
	Cleanup();
	if (rhs.GetOperation())
		SetOperation(*(rhs.GetOperation()));
	mParam.Copy(rhs.mParam);
	return *this;
}

SVLutTransform& SVLutTransform::operator = (const SVLutTransformOperation& rhs)
{
	SetOperation(rhs);
	return *this;
}

HRESULT SVLutTransform::Transform(SVLutBand& data, const SVLutTransformParameters& param)
{
	if (mpType)
	{
		mParam.RemoveAll();
		mParam.Copy(param);
		return mpType->Transform(*this, data, param);
	}
	else
		return S_FALSE;
}

bool SVLutTransform::SetOperation(const SVLutTransformOperation& rType)
{
	mParam.RemoveAll();
	if (mpType != &rType)
	{
		Cleanup();
		mpType = rType.Clone();
		mbOwnType = true;
	}
	return true;
}

bool SVLutTransform::SetOperation(SVLutTransformOperationEnum eType)
{
	mParam.RemoveAll();
	mpType = mMapTypes.GetType(eType);
	mbOwnType = false;
	return mpType != NULL;
}

const SVLutTransformOperation* SVLutTransform::GetOperation() const
{
	return mpType;
}

void SVLutTransform::GetOperation(SVLutTransformOperationEnum& eType) const
{
	eType = mMapTypes.GetType(mpType);
}

void SVLutTransform::Cleanup()
{
	mParam.RemoveAll();
	if (mpType && mbOwnType)
	{
		delete mpType;
		mpType = NULL;
		mbOwnType = false;
	}
}

bool SVLutTransform::GetParameters(SVLutTransformParameters& rParam) const
{
	rParam.Copy(mParam);
	return true;
}

bool SVLutTransform::SetParameters(const SVLutTransformParameters& rParam)
{
	mParam.Copy(rParam);
	return true;
}

bool SVLutTransform::operator == (const SVLutTransform& rhs) const
{
	// equal if operaton and params are equal
	bool bEqual = true;
	if ( mpType == NULL && rhs.mpType == NULL )
		{}
	else if ( mpType == NULL || rhs.mpType == NULL )
		{ bEqual = false; }
	else
	{
		bEqual = (*mpType == *(rhs.mpType));
		if ( bEqual )
		{
			if ( mParam.GetSize() == rhs.mParam.GetSize() )
			{
				for ( int i=0; i < mParam.GetSize() && bEqual; i++ )
				{
					bEqual = bEqual && mParam[i] == rhs.mParam[i];
				}
			}
			else
				bEqual = false;
		}
	}
	return bEqual;
}

bool SVLutTransformOperation::operator == ( const SVLutTransformOperation& rhs ) const
{
	return (typeid(*this) == typeid(rhs)) != FALSE;
}




HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationNormal& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	for (unsigned long i=0; i < data.Size(); i++)
	{
		data(i) = i * ( data.Info().MaxValue() + 1 ) / data.Size();
	}
	return hr;
}
HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationInvert& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	for (unsigned long i=0; i < data.Size(); i++)
	{
		data(i) = (data.Size() - i - 1) * ( data.Info().MaxValue() + 1 ) / data.Size();
	}
	return hr;
}
HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationSign& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	unsigned long i;
	for (i=0; i < data.Size()/2; i++)
	{
		data(i) = i * ( data.Info().MaxValue() + 1 ) / data.Size() + data.Info().MaxValue()/2;
	}
	for (i=data.Size()/2; i < data.Size(); i++)
	{
		data(i) = i * ( data.Info().MaxValue() + 1 ) / data.Size() - data.Info().MaxValue()/2;
	}
	return hr;
}
HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationClip& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_FALSE;
	ASSERT ( param.GetSize() == 2 );
	if (param.GetSize() == 2)
	{
		long lClipMin = param[0];
		long lClipMax = param[1];
		ASSERT ( lClipMin <= lClipMax );
		for (unsigned long i=0; i < data.Size(); i++)
		{
			long lValue = i * ( data.Info().MaxValue() + 1 ) / data.Size();
			if (lValue <= lClipMin)
				data(i) = lClipMin;
			else if (lValue >= lClipMax)
				data(i) = lClipMax;
			else
				data(i) = lValue;
		}
		hr = S_OK;
	}
	return hr;
}
HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationFreeform& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT SVDefaultLutTransform::Transform(SVLutTransformOperationTwoKnee& t, SVLutBand& data, const SVLutTransformParameters& param)
{
	HRESULT hr = S_OK;
	ASSERT( param.GetSize() == 5 );
	if ( param.GetSize() == 5 )
	{
		unsigned long x1 = param[0];
		unsigned long y1 = param[1];
		unsigned long x2 = param[2];
		unsigned long y2 = param[3];
		unsigned long lMaxY = param[4];

		//ASSERT ( x1 >= 0 && x2 >= 0 && x1 < data.Size() && x2 < data.Size() && x2 >= x1 );
		if ( x1 >= 0 && x2 >= 0 && x1 < data.Size() && x2 < data.Size() && x2 >= x1 )
		{
			if (x1 > 0)
			{
				double dSlope = ((double)y1 / (double)x1);
				double dY = 0.0;
				for (unsigned long i=0; i <= x1; i++)
				{
					data(i) = (int) dY;
					dY += dSlope;
				}
			}
			if (x2 > 0 && x2 > x1)
			{
				double dSlope = ((double)(y2-y1) / (double)(x2-x1));
				double dY = y1;
				for (unsigned long i=x1+1; i <= x2; i++)
				{
					data(i) = (int) dY;
					dY += dSlope;
				}
			}
			if (x2 < data.Size() - 1)
			{
				double dSlope = ((double)(lMaxY-y2) / (double)(data.Size()-1-x2));
				double dY = y2;
				for (unsigned long i=x2+1; i < data.Size(); i++)
				{
					data(i) = (int) dY;
					dY += dSlope;
				}
			}
			hr = S_OK;
		}
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVLutTransform.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Feb 2010 11:26:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new constructors to allow for single line construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Jun 2007 11:08:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   11 Feb 2004 17:27:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed compiler warnings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Feb 2004 15:57:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed incorrect assumptions in computing LUTs
 * removed unused code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Dec 2003 15:14:18   ebeyeler
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
 *    Rev 1.1   17 Nov 2003 09:07:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed an annoying ASSERT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:02:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 11:06:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
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
