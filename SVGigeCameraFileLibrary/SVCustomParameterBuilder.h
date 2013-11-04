//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomParameterBuilder
//* .File Name       : $Workfile:   SVCustomParameterBuilder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:41:48  $
//******************************************************************************
#ifndef INCL_SVCUSTOMPARAMETERBUILDER_H
#define INCL_SVCUSTOMPARAMETERBUILDER_H

#include "SVOMFCLibrary/SVCustomDeviceParam.h"
#include "SVMaterialsLibrary/SVMaterialsTree.h"
#include "SVMaterialsLibrary/SVMaterials.h"

class SVCustomParameterBuilder
{
public:
	template<typename Insertor>
	static void BuildCustomDeviceParams(const SVMaterialsTree& rTree, Insertor insertor);

private:
	static SVCustomDeviceParam* BuildCustomDeviceParam(const SVMaterials& rMaterials, const SVMaterialsTreeAdapter::SVTreeContainer* pOptions);
	static void BuildLongParam(SVLongValueDeviceParam* pParam, const SVMaterials& rMaterials, const SVMaterialsTreeAdapter::SVTreeContainer* pOptions);
	static void BuildInt64Param(SVi64ValueDeviceParam* pParam, const SVMaterials& rMaterials);
	static void BuildStringParam(SVStringValueDeviceParam* pParam, const SVMaterials& rMaterials, const SVMaterialsTreeAdapter::SVTreeContainer* pOptions);
	static void BuildBoolParam(SVBoolValueDeviceParam* pParam, const SVMaterials& rMaterials, const SVMaterialsTreeAdapter::SVTreeContainer* pOptions);
	static void BuildCommonAttributes(SVDeviceParam* pParam, const SVMaterials& rMaterials);
	static void BuildOptions(SVDeviceParam* pParam, const SVMaterialsTreeAdapter::SVTreeContainer* pOptions);
};

#include "SVCustomParameterBuilder.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraFileLibrary\SVCustomParameterBuilder.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:41:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Apr 2013 13:50:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/