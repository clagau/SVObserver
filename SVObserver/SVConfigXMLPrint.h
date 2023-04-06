//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#include "SVIOLibrary/SVIOEntryHostStruct.h"
#pragma region Includes

class SVConfigurationObject;
class SVObjectClass;
class SVInspectionProcess;
class SVPPQObject;

class SVDeviceParam;
class SVLongValueDeviceParam;
class SVBoolValueDeviceParam;
class SVi64ValueDeviceParam;
class SVStringValueDeviceParam;
class SVCustomDeviceParam;
class SVCameraFormatsDeviceParam;
class SVLutDeviceParam;

namespace SvIe
{
class SVTaskObjectClass;
class SVTaskObjectListClass;
class SVVirtualCamera;
}
namespace SvTrig
{
class SVTriggerObject;
}
namespace SvTo
{
class SVArchiveTool;
class SVToolClass;
}

typedef CComPtr<IXmlWriter> Writer;

namespace sv_xml
{
std::wstring space2underscore(std::wstring text);

class WriteStartEndElement
{
public:
	WriteStartEndElement(Writer writer, LPCWSTR pwszPrefix, LPCWSTR pwszLocalName, LPCWSTR pwszNamespaceUri)
		:m_Writer(writer)
	{
		std::wstring temp = space2underscore(std::wstring(pwszLocalName));
		m_Writer->WriteStartElement(pwszPrefix, temp.c_str(), pwszNamespaceUri);
	}

	~WriteStartEndElement()
	{
		m_Writer->WriteEndElement();
	}

	Writer m_Writer;
};

class SVConfigXMLPrint
{
public:
	SVConfigXMLPrint() = default;
	const std::string Print() const;

private:
	typedef std::map<std::string, SVInspectionProcess*> InspectionMap;

	void PrintXMLDoc(Writer  writer) const;
	void WriteTriggers(Writer  writer) const;
	void WriteTrigger(Writer  writer, SvTrig::SVTriggerObject* pTrigger) const;
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
	void WriteArchiveTool(Writer writer, SvTo::SVArchiveTool* pArchiveTool) const;
	void WriteModuleIO(Writer writer) const;
	void WriteResultIO(Writer writer) const;
	void WriteIOSection(Writer writer) const;
	void WriteMonitorListSection(Writer writer) const;
	void WritePPQBar(Writer writer) const;
	void WriteObject(Writer writer, SVObjectClass* pObj) const;
	void WriteValueObject(Writer writer, SVObjectClass* pObj) const;
	void WriteAllChildren(Writer writer, SvIe::SVTaskObjectListClass* pObj) const;
	void WriteChildren(Writer writer, SVObjectClass* pObj) const;
	void WriteFriends(Writer writer, SvIe::SVTaskObjectClass* pObj) const;
	void WriteInputOutputList(Writer writer, SvIe::SVTaskObjectClass* pTaskObj) const;
	void WriteValueObject(Writer writer, const std::wstring& rTag, const std::wstring& rName, const std::wstring& rValue) const;
	void WriteIOEntryObject(Writer writer, SVIOEntryHostStructPtr IOEntry) const;
	void WriteGlobalConstants(Writer writer) const;

	void WriteExternalFiles(Writer writer) const;
	mutable SVConfigurationObject* m_cfo {nullptr};
	mutable int nToolNumber {0};
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

	const std::string operator()() { return reinterpret_cast<const char*>(::GlobalLock(hg)); }
};

class SVDeviceParamConfigXMLHelper
{
public:
	explicit SVDeviceParamConfigXMLHelper(Writer writer);
	HRESULT Visit(SVDeviceParam*);
	HRESULT Visit(SVLongValueDeviceParam*);
	HRESULT Visit(SVi64ValueDeviceParam*);
	HRESULT Visit(SVBoolValueDeviceParam*);
	HRESULT Visit(SVStringValueDeviceParam*);
	HRESULT Visit(SVCustomDeviceParam*);
	HRESULT Visit(SVCameraFormatsDeviceParam*);
	HRESULT Visit(SVLutDeviceParam*);

private:
	Writer m_writer;
};
}

