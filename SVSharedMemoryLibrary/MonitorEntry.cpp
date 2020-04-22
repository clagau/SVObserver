#pragma region Includes
#include "StdAfx.h"
#include "MonitorEntry.h"
#include "SVMatroxLibrary/MatroxImageProps.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvSml
{
	bool MonitorEntryData::GetMatroxImageProps(MatroxImageProps& rImageProps) const
	{
		if (ObjectType != SvPb::SVObjectTypeEnum::SVImageObjectType)
		{
			return false;
		}
		rImageProps.Pitch = Pitch;
		rImageProps.sizeX = sizeX;
		rImageProps.sizeY = sizeY;
		rImageProps.PitchByte = PitchByte;
		rImageProps.Matrox_type = Matrox_type;
		rImageProps.Attrib = Attrib;
		rImageProps.Bandsize = BandSize;
		rImageProps.Bytesize = ByteSize;
		return true;
	}
	void MonitorEntryData::SetMatroxImageProps(const MatroxImageProps& rImageProps)
	{
		ObjectType = SvPb::SVImageObjectType;
		Pitch = rImageProps.Pitch;
		sizeX = rImageProps.sizeX;
		sizeY = rImageProps.sizeY;
		PitchByte = rImageProps.PitchByte;
		Matrox_type = rImageProps.Matrox_type;
		Attrib = rImageProps.Attrib;
		ByteSize = rImageProps.Bytesize;
		BandSize = rImageProps.Bandsize;
	}
	void MonitorEntryData::BuildProtoMessage(SvPml::MesMonitorEntryData& rProtoMessage) const
	{
		rProtoMessage.set_inspectionstoreid(InspectionStoreId);
		rProtoMessage.set_objecttype(ObjectType);
		rProtoMessage.set_variant_type(variant_type);
		rProtoMessage.set_isarray(isArray);
		rProtoMessage.set_wholearray(wholeArray);
		rProtoMessage.set_arrayindex(arrayIndex);
		rProtoMessage.set_monitorlistflag(m_MonitorListFlag);
		rProtoMessage.set_sizey(sizeY);
		rProtoMessage.set_sizex(sizeX);
		rProtoMessage.set_pitchbyte(PitchByte);
		rProtoMessage.set_pitch(Pitch);
		rProtoMessage.set_matrox_type(Matrox_type);
		rProtoMessage.set_attrib(Attrib);
		rProtoMessage.set_bandsize(BandSize);
		rProtoMessage.set_bytesize(ByteSize);
		rProtoMessage.set_triggerrecordpos(m_triggerRecordPos);
		rProtoMessage.set_inspectiontrcpos(m_inspectionTRCPos);
	}

	void MonitorEntryData::BuildFromProtoMessage(const SvPml::MesMonitorEntryData& rProtoMessage)
	{

		InspectionStoreId = rProtoMessage.inspectionstoreid();
		ObjectType = rProtoMessage.objecttype();
		variant_type = rProtoMessage.variant_type();
		isArray = rProtoMessage.isarray();
		arrayIndex = rProtoMessage.arrayindex();
		wholeArray = rProtoMessage.wholearray();
		m_MonitorListFlag = rProtoMessage.monitorlistflag();
		sizeY = rProtoMessage.sizey();
		sizeX = rProtoMessage.sizex();
		PitchByte = rProtoMessage.pitchbyte();
		Pitch = rProtoMessage.pitch();
		Matrox_type = rProtoMessage.matrox_type();
		Attrib = rProtoMessage.attrib();
		BandSize = rProtoMessage.bandsize();
		ByteSize = rProtoMessage.bytesize();
		m_triggerRecordPos = rProtoMessage.triggerrecordpos();
		m_inspectionTRCPos = rProtoMessage.inspectiontrcpos();
	}

	MonitorEntry::MonitorEntry() :data()
	{
	};

	MonitorEntry::MonitorEntry(const std::string& na) :data(), name(na)
	{
	};

	bool MonitorEntry::GetMatroxImageProps(MatroxImageProps &ImageProps) const
	{
		return data.GetMatroxImageProps(ImageProps);
	}

	void MonitorEntry::SetMatroxImageProps(const MatroxImageProps  &rImageProps) 
	{
		 data.SetMatroxImageProps(rImageProps);
	}

	void  MonitorEntry::BuildProtoMessage(SvPml::MesMonitorEntry& rMesMonitorEntry) const
	{
		rMesMonitorEntry.set_objectid(m_objectId);
		rMesMonitorEntry.set_name(SvUl::to_utf8(name));
		auto pEntryDataMessage = rMesMonitorEntry.mutable_entrydata();
		data.BuildProtoMessage(*pEntryDataMessage);
	}

	
	void MonitorEntry::BuildFromProtoMessage(const SvPml::MesMonitorEntry& rMesMonitorEntry)
	{
		m_objectId = rMesMonitorEntry.objectid();
		name = SvUl::to_ansi(rMesMonitorEntry.name());
		data.BuildFromProtoMessage(rMesMonitorEntry.entrydata());
	}

	void MonitorEntry::AddListItem(SvPb::QueryListItemResponse& resp) const
	{
		if (IsImage())
		{
			auto pImageDef = resp.add_imagedeflist();
			pImageDef->set_name(SvUl::to_utf8(name));
			pImageDef->set_width(static_cast<INT32>(data.sizeX));
			pImageDef->set_height(static_cast<INT32>(data.sizeY));
			pImageDef->set_storeid(data.InspectionStoreId);
		}
		else
		{
			auto pValueDef = resp.add_valuedeflist();
			pValueDef->set_name(SvUl::to_utf8(name));
			pValueDef->set_type(data.variant_type);
			pValueDef->set_storeid (data.InspectionStoreId);
		}

	}

}
