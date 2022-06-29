//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomParameterBuilder
//* .File Name       : $Workfile:   SVCustomParameterBuilder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:41:48  $
//******************************************************************************
#pragma once

#include "CameraLibrary/SVDeviceParam.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"

class SVBoolValueDeviceParam;
class SVCustomDeviceParam;
class SVDeviceParam;
class SVi64ValueDeviceParam;
class SVLongValueDeviceParam;
class SVStringValueDeviceParam;

class SVCustomParameterBuilder
{
public:
	static void BuildCustomDeviceParams(const SVMaterialsTree::SVTreeContainer& rTree, std::insert_iterator<SVDeviceParamMap> insertor);

private:
	static SVCustomDeviceParam* BuildCustomDeviceParam(const SVMaterialsTree::SVTreeContainer& rOptions);
	static void BuildLongParam(SVLongValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions);
	static void BuildInt64Param(SVi64ValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions);
	static void BuildStringParam(SVStringValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions);
	static void BuildBoolParam(SVBoolValueDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions);
	static void BuildCommonAttributes(SVDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions);
	static void BuildOptions(SVDeviceParam* pParam, const SVMaterialsTree::SVTreeContainer& rOptions);
};

