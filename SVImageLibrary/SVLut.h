//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLut
//* .File Name       : $Workfile:   SVLut.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:44  $
//******************************************************************************

#ifndef SVLUT_H
#define SVLUT_H

#include <vector>
#include "SVContainerLibrary/SVVector.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVLutTransform.h"

enum SVLutFormatEnum
{
	SVLUT_FORMAT_NEUTRAL = 987654321,
};

class SVLutInfo
{
public:
	SVLutInfo();
	SVLutInfo( UINT p_Bands, UINT p_BandSize, UINT p_BandMaxValue, const SVLutTransform& p_rTransform );
	SVLutInfo(const SVLutInfo& rhs);
	const SVLutInfo& operator = (const SVLutInfo& rhs);
	~SVLutInfo();
	void CopyNoTransform(const SVLutInfo& rhs);
	UINT Bands() const;
	UINT Format() const;
	UINT BandSize() const;
	UINT MaxValue() const;
	const SVLutTransform* GetTransform() const;
	SVLutTransform*& GetTransform();
	bool SetTransform(const SVLutTransform& pTransform);
	const SVLutTransformOperation* GetTransformOperation() const;
	bool SetTransformOperation(const SVLutTransformOperation& pType);
	bool GetTransformParameters(SVLutTransformParameters& rParam) const;
	bool GetTransformParameters(SAFEARRAY*& rpsaParam) const;
	void SetBands(UINT uiBands);
	void SetFormat(UINT uiFormat);
	void SetBandSize(UINT uiBandSize);
	void SetMaxValue(UINT uiMaxValue);
	bool IsNullHandle() const;
	SVHANDLE GetHandle() const;
	void Cleanup();
private:
	SVHANDLE        mHandle;        // handle to Acquisition Device

	UINT            muiBands;       // number of LUT bands to create
	UINT            muiBandSize;    // Size of one band in neutral format
	UINT            muiFormat;      // color format of the LUT
	UINT            muiMaxValue;    // max LUT value
	SVLutTransform* mpTransform;	// transform object;

friend class SVLut;	// so can set the handles to NULL on Create
};

typedef ULONG SVLutValueType;

template <typename T> class SVLutValue
{
public:
	SVLutValue();
	SVLutValue(const T& val);
	operator T() const;
	const T& operator = (const T& rhs);

	T value;
	typedef T ValueType;
};

//typedef SVLutValue<SVLutValueType> SVLutEntry;
typedef SVLutValue<UINT> SVLutEntry;   // this is needed so the specialization can be called.
                                       // there is still an automatic conversion to/from SVLutValueType. (!)

class SVLutBand
{
public:
	SVLutBand();
	SVLutBand( const SVLutBand& rhs );
	~SVLutBand();
	const SVLutBand& operator = ( const SVLutBand& rhs );
	void CopyNoTransform( const SVLutBand& rhs );

	bool operator == ( const SVLutBand& rhs ) const;

	SVLutEntry& operator () (UINT nIndex);
	const SVLutEntry& operator () (UINT nIndex) const;

	ULONG Band() const;
	bool Create(const SVLutInfo& info, UINT uiBand);
	bool Destroy();
	bool CopyBandData(const SVLutBand& lutband);
	bool SetBandData(SAFEARRAY* psaBandData);
	bool GetBandData(SAFEARRAY*& rpsaBandData) const;

	UINT Size() const;

	const SVLutTransformOperation* GetTransformOperation() const;
	bool SetTransformOperation(const SVLutTransformOperation& pType);
	bool Transform(const SVLutTransformParameters& param = SVLutTransformParameters());

	const SVLutInfo& Info() const;

private:
	std::vector < SVLutEntry > maTable;
	ULONG mulBand;
	SVLutInfo mInfo;

friend class SVLut;
};



class SVLut
{
public:
	SVLut();
	SVLut( const SVLut& rhs );
	~SVLut();
	const SVLut& operator = ( const SVLut& rhs );
	void CopyNoTransform( const SVLut& rhs );

	bool IsCreated();
	bool Create(const SVLutInfo& info);
	bool Destroy();
	bool CopyBandData(const SVLut& lut, int iBand=-1);
	bool CopyBandData(const SVLutBand& lutband);
	bool SetBandData(SAFEARRAY* psaBands);
	bool GetBandData(SAFEARRAY*& rpsaBands) const;

	const SVLutTransformOperation* GetTransformOperation() const;
	bool SetTransformOperation(const SVLutTransformOperation& pType);
	bool SetTransform(const SVLutTransform& pTransform);
	bool Transform(const SVLutTransformParameters& param = SVLutTransformParameters());

	SVLutBand& operator () (UINT nBand);
	const SVLutBand& operator () (UINT nBand) const;
	SVLutEntry& operator () (UINT nBand, UINT nIndex);
	const SVLutEntry& operator () (UINT nBand, UINT nIndex) const;

	ULONG NumBands() const;
	const SVLutInfo& Info() const;

private:
	std::vector< SVLutBand > maBands;
	SVLutInfo mInfo;
};


template <typename T>
inline SVLutValue<T>::SVLutValue()
{
	//TRACE("SVLutValue<T>\n");
}

// specialize for UINT so can init to 0
inline SVLutValue<UINT>::SVLutValue()
{
	//TRACE("SVLutValue<UINT>\n");
	value = 0;
}

template <typename T>
inline SVLutValue<T>::SVLutValue(const T& rVal)
{
	value = rVal;
}

template <typename T>
inline SVLutValue<T>::operator T() const
{
	return value;
}

template <typename T>
inline const T& SVLutValue<T>::operator = (const T& rhs)
{
	value = rhs;
	return value;
}




class SVLutTestCases
{
public:
	SVLutTestCases();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVLut.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Aug 2012 15:34:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed unused typdef for LUT array.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Sep 2011 14:14:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made clean-up method public to allow shutdown process to call method directly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Mar 2010 11:13:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to consolidate array container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Feb 2010 11:26:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new constructors to allow for single line construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 11:34:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
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
 *    Rev 1.1   24 Sep 2003 07:43:18   ebeyeler
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
 *    Rev 1.0   31 Jul 2003 08:02:00   ebeyeler
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
 *    Rev 1.4   22 Apr 2003 10:58:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Feb 2003 10:38:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed const-correctness
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2003 12:53:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SVLut::GetBandData and SVLut::SetBandData
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 09:51:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   General Lut changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jan 2003 12:37:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   First iteration of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
