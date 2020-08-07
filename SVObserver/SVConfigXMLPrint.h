//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConfigXMLPrint
//* .File Name       : $Workfile:   SVConfigXMLPrint.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Jul 2014 07:54:54  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <sstream>
//Moved to precompiled header: #define BOOST_DATE_TIME_NO_LIB
//Moved to precompiled header: #include <boost/date_time.hpp>
//Moved to precompiled header: #include <xmllite.h>

#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVi64ValueDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVParamListDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "CameraLibrary/SVCustomDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma region Includes

namespace SvTo
{
class SVArchiveTool;
class SVToolClass;
}

class SVObjectClass;

typedef CComPtr<IXmlWriter> Writer;

namespace sv_xml
{
	class SVConfigXMLPrint
	{
	public:
		const std::string Print() const;

	private:
		typedef std::map<std::string, SVInspectionProcess*> InspectionMap;

		void PrintXMLDoc(Writer  writer) const;
		void WriteTriggers(Writer  writer) const;
		void WriteTrigger(Writer  writer, SvTi::SVTriggerObject* pTrigger) const;
		void WriteCameras(Writer  writer) const;
		void WriteCamera(Writer  writer, SvIe::SVVirtualCamera* pCamera) const;
		void WriteHardwareAcq(Writer  writer, SvIe::SVVirtualCamera* pCamera) const;
		void WriteFileAcq(Writer  writer, SvIe::SVVirtualCamera* pCamera) const;
		void WritePPQs(Writer  writer) const;
		void WritePPQCameras(Writer  writer, SVPPQObject* pPPQ) const;
		void WritePPQInspections(Writer  writer, SVPPQObject* pPPQ) const;
		void WriteInspections(Writer  writer) const;
		void WriteToolSets(Writer writer) const;
		void WriteToolSet(Writer writer, SVInspectionProcess* pInspection) const;
		void WriteTool(Writer writer, SvTo::SVToolClass* pTool) const;
		void WriteArchiveTool(Writer writer, SvTo::SVArchiveTool* pArchiveTool) const;
		void WriteModuleIO(Writer writer) const;
		void WriteResultIO(Writer writer) const;
		void WriteIOSection(Writer writer) const;
		void WriteMonitorListSection(Writer writer) const;
		void WritePPQBar(Writer writer) const;
		void WriteObject( Writer writer, SVObjectClass* pObj ) const;
		void WriteValueObject( Writer writer, SVObjectClass* pObj ) const;
		void WriteAllChildren( Writer writer, SvIe::SVTaskObjectListClass* pObj ) const;
		void WriteChildren( Writer writer, SVObjectClass* pObj ) const;
		void WriteFriends( Writer writer, SvIe::SVTaskObjectClass* pObj ) const;
		void WriteInputOutputList( Writer writer, SVObjectClass* pObj ) const;
		void WriteValueObject(Writer writer, const std::wstring&  rTag, const std::wstring&  rName, const std::wstring&  rValue) const;
		void WriteIOEntryObject(Writer writer, SVIOEntryHostStructPtr IOEntry) const;
		void WriteGlobalConstants(Writer writer) const;

		mutable SVConfigurationObject* m_cfo;
		mutable int nToolNumber;
	};

	class HG2String
	{
		HGLOBAL hg;
	public:
		explicit HG2String(HGLOBAL h) : hg(h)
		{
		}
		~HG2String()
		{
			::GlobalUnlock(hg);
		}

		const std::string operator()() { return reinterpret_cast<const char *>(::GlobalLock(hg)); }
	};

	const std::wstring now()
	{
		const std::wstring sep = L" ";
		typedef boost::posix_time::ptime ptime;
		typedef boost::posix_time::microsec_clock clock;
		ptime pt = clock::local_time();
		std::wstringstream ss;
		ss << pt.date() << sep << pt.time_of_day() << std::ends;
		return ss.str();
	}

	class SVDeviceParamConfigXMLHelper :
		public SvCam::BaseVisitor,
		public SvCam::Visitor<SVDeviceParam>,
		public SvCam::Visitor<SVLongValueDeviceParam>,
		public SvCam::Visitor<SVi64ValueDeviceParam>,
		public SvCam::Visitor<SVBoolValueDeviceParam>,
		public SvCam::Visitor<SVStringValueDeviceParam>,
		public SvCam::Visitor<SVParamListDeviceParam>,
		public SvCam::Visitor<SVLutDeviceParam>,
		public SvCam::Visitor<SVLightReferenceDeviceParam>,
		public SvCam::Visitor<SVCameraFormatsDeviceParam>,
		public SvCam::Visitor<SVCustomDeviceParam>
	{
	public:
		SVDeviceParamConfigXMLHelper(Writer writer, SVDeviceParamCollection& rCamFileParams);
		HRESULT Visit(SVDeviceParam&);
		HRESULT Visit(SVLongValueDeviceParam&);
		HRESULT Visit(SVi64ValueDeviceParam&);
		HRESULT Visit(SVBoolValueDeviceParam&);
		HRESULT Visit(SVStringValueDeviceParam&);
		HRESULT Visit(SVParamListDeviceParam&);
		HRESULT Visit(SVLutDeviceParam&);
		HRESULT Visit(SVLightReferenceDeviceParam&);
		HRESULT Visit(SVCameraFormatsDeviceParam&);
		HRESULT Visit(SVCustomDeviceParam&);

	private:
		Writer m_writer;
		SVDeviceParamCollection & m_rCamFileParams;
	};
}

#include "SVConfigXMLPrint.inl"

