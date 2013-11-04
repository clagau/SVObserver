//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLutTransform
//* .File Name       : $Workfile:   SVLutTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:51:24  $
//******************************************************************************

#ifndef SVLUTTRANSFORM_H
#define SVLUTTRANSFORM_H

#include "SVContainerLibrary/SVVector.h"

enum SVLutTransformOperationEnum	// for legacy conversion
{
	LutTransformTypeNormal = 0,
	LutTransformTypeInversion = 1,
	LutTransformTypeSign = 2,
	LutTransformTypeClip = 3,
	//LutTransformTypeFormula = 4,
	LutTransformTypeFreeform = 5,
	LutTransformTypeTwoKnee = 6,

	MAX_LUT_TRANSFORM_TYPE,
	LutTransformTypeUnknown
};

class SVLutTransformOperation;
class SVLutTransform;

class SVLutTransformOperationMap
{
public:
	struct SVLutTransformTypeInfo
	{
		SVLutTransformOperationEnum eType;
		SVLutTransformOperation* pType;
		CString sType;
		SVLutTransformTypeInfo() {eType = LutTransformTypeUnknown; pType = NULL;}
		SVLutTransformTypeInfo(SVLutTransformOperationEnum e_, SVLutTransformOperation* pType_, const CString& sType_) {eType = e_; pType = pType_; sType = sType_;}
	};
	typedef SVVector< SVLutTransformTypeInfo > SVLutTransformTypeInfoArray;
public:
	SVLutTransformOperationMap();
	~SVLutTransformOperationMap();
	SVLutTransformOperation* GetType(SVLutTransformOperationEnum e);
	SVLutTransformOperationEnum GetType(const SVLutTransformOperation* pType);
	const SVLutTransformTypeInfo* GetInfo(SVLutTransformOperationEnum e);
	const SVLutTransformTypeInfo* GetInfo(const SVLutTransformOperation* pType);
	const SVLutTransformTypeInfo* GetInfo(const CString& sType);
	const SVLutTransformTypeInfoArray& GetTypes() {return maTypeInfo;}
private:
	SVLutTransformTypeInfoArray maTypeInfo;
};

class SVLutTransformParameters : public CDWordArray
{
};

class SVLutBand;

class SVLutTransformOperation;
class SVLutTransformOperationNormal;
class SVLutTransformOperationInvert;
class SVLutTransformOperationSign;
class SVLutTransformOperationClip;
class SVLutTransformOperationFreeform;
class SVLutTransformOperationTwoKnee;

class SVLutTransform
{
public:
	SVLutTransform();
	SVLutTransform(const SVLutTransformOperation& rType);
	SVLutTransform(const SVLutTransform& rhs);
	virtual ~SVLutTransform();
	virtual SVLutTransform* Clone() const = 0;
	SVLutTransform& operator = (const SVLutTransform& rhs);

	bool operator == (const SVLutTransform& rhs) const;

	HRESULT Transform(SVLutBand& data, const SVLutTransformParameters& param);

	SVLutTransform& operator = (const SVLutTransformOperation& rhs);

	bool SetOperation(const SVLutTransformOperation& rType);
	const SVLutTransformOperation* GetOperation() const;
	
	bool SetOperation(SVLutTransformOperationEnum eType);
	void GetOperation(SVLutTransformOperationEnum& eType) const;

	bool GetParameters(SVLutTransformParameters& rParam) const;
	bool SetParameters(const SVLutTransformParameters& rParam);

	virtual HRESULT Transform(SVLutTransformOperationNormal& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationInvert& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationSign& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationClip& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationFreeform& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual HRESULT Transform(SVLutTransformOperationTwoKnee& t, SVLutBand& data, const SVLutTransformParameters& param) = 0;

	static const SVLutTransformOperation* GetEquivalentType(SVLutTransformOperationEnum e) {return mMapTypes.GetType(e);}
	static SVLutTransformOperationEnum GetEquivalentType(const SVLutTransformOperation* pType) {return mMapTypes.GetType(pType);}


protected:
	SVLutTransformOperation* mpType;
	bool mbOwnType;
	static SVLutTransformOperationMap mMapTypes;
	SVLutTransformParameters mParam;

	void Cleanup();

};

class SVLutTransformOperation
{
public:
	virtual ~SVLutTransformOperation();
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) = 0;
	virtual SVLutTransformOperation* Clone() const = 0;

	bool operator == ( const SVLutTransformOperation& rhs ) const;
};

class SVLutTransformOperationNormal : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationNormal;}
};
class SVLutTransformOperationInvert : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationInvert;}
};
class SVLutTransformOperationSign : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationSign;}
};
class SVLutTransformOperationClip : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationClip;}
};
class SVLutTransformOperationFreeform : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationFreeform;}
};
class SVLutTransformOperationTwoKnee : public SVLutTransformOperation
{
	virtual HRESULT Transform(SVLutTransform& transform, SVLutBand& data, const SVLutTransformParameters& param) {return transform.Transform(*this, data, param);}
	virtual SVLutTransformOperation* Clone() const {return new SVLutTransformOperationTwoKnee;}
};





class SVDefaultLutTransform : public SVLutTransform
{
public:
	SVDefaultLutTransform(){};
	SVDefaultLutTransform(const SVLutTransformOperation& rType) : SVLutTransform(rType) {}
	SVDefaultLutTransform(const SVDefaultLutTransform& rhs) : SVLutTransform(rhs) {}
	virtual SVLutTransform* Clone() const {return new SVDefaultLutTransform(*this);}
	virtual HRESULT Transform(SVLutTransformOperationNormal& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationInvert& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationSign& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationClip& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationFreeform& t, SVLutBand& data, const SVLutTransformParameters& param);
	virtual HRESULT Transform(SVLutTransformOperationTwoKnee& t, SVLutBand& data, const SVLutTransformParameters& param);
};




#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVLutTransform.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:51:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Mar 2010 11:13:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to consolidate array container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Feb 2010 11:26:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new constructors to allow for single line construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 11:33:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Feb 2004 15:57:26   ebeyeler
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
 *    Rev 1.2   08 Dec 2003 15:14:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed equality comparison
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2003 14:57:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added operator ==
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Jul 2003 08:02:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 10:19:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   transfered from SVObserver project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 11:06:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
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
