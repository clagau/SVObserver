#pragma region Includes
#include "StdAfx.h"
#include "MonitorEntry.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVMatroxLibrary\MatroxImageProps.h"
#include "SVProtoBuf\ConverterHelper.h"
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

	MonitorEntry::MonitorEntry() :data(), m_Guid(GUID_NULL)
	{
	};

	MonitorEntry::MonitorEntry(const std::string& na) :data(), name(na), m_Guid(GUID_NULL)
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
		SvPb::SetGuidInProtoBytes(rMesMonitorEntry.mutable_guid(), m_Guid);
		rMesMonitorEntry.set_name(SvUl::to_utf8(name));
		auto pEntryDataMessage = rMesMonitorEntry.mutable_entrydata();
		data.BuildProtoMessage(*pEntryDataMessage);
	}

	
	void MonitorEntry::BuildFromProtoMessage(const SvPml::MesMonitorEntry& rMesMonitorEntry)
	{
		SvPb::GetGuidFromProtoBytes(rMesMonitorEntry.guid(), m_Guid);
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

	std::string MonitorEntry::convertValue(variant_t value, int arrayIndex)
	{
		std::string retValue;

		bool isArray = (VT_ARRAY == (value.vt & VT_ARRAY));

		if (!isArray && arrayIndex != -1)
		{
			return "???";
		}

		if (isArray && 0 > arrayIndex)
		{
			//by now only one value per array can be converted. 
			return "???";
		}

		switch (value.vt & (~VT_ARRAY))
		{
			case VT_I1: //CHAR
			{
				retValue = convertValue<CHAR>(isArray, arrayIndex, value, _T("%c"));
				break;
			}
			case VT_UI1: //BYTE
			{
				retValue = convertValue<BYTE>(isArray, arrayIndex, value, _T("0x%2.2x"));
				break;
			}
			case VT_I2:
			{
				retValue = convertValue<short>(isArray, arrayIndex, value);
				break;
			}
			case VT_UI2:
			{
				retValue = SvUl::Format(_T("0x%4.4x"), static_cast<WORD>(value));
				break;
			}
			case VT_I4:
			{
				retValue = convertValue<int>(isArray, arrayIndex, value);
				break;
			}
			case VT_UI4:
			{
				retValue = convertValue<DWORD>(isArray, arrayIndex, value, _T("0x%8.8x"));
				break;
			}
			case VT_R4:
			{
				retValue = convertValue<float>(isArray, arrayIndex, value);
				break;
			}

			case VT_R8:
			{
				retValue = convertValue<double>(isArray, arrayIndex, value);
				break;
			}
			case VT_BSTR:
			{
				assert(!isArray);
				retValue = SvUl::createStdString(value.bstrVal);
				break;
			}

			case VT_I8:
			{
				retValue = convertValue<INT64>(isArray, arrayIndex, value, _T("%I64d"));
				break;

			}
			case VT_UI8:
			{
				retValue = convertValue<UINT64>(isArray, arrayIndex, value, _T("%I64d"));
				break;
			}

			case VT_INT:
			{
				assert(!isArray);
				// BasicvalueObject converts VT_INT to VT_BOOL
				int tmpValue = static_cast<int>(value);
				if (tmpValue)
					retValue = "TRUE";
				else
					retValue = "FALSE";
				break;
			}
			case VT_UINT:
			{
				assert(!isArray);
				retValue = SvUl::Format(_T("0x%8.8x"), static_cast<DWORD>(value));
				break;
			}

			case VT_BOOL:
			{
				assert(!isArray);
				if (value)
				{
					retValue = "TRUE";
				}
				else
				{
					retValue = "FALSE";
				}
				break;
			}

			default:
				retValue = "???";
				break;
		}
		return retValue;
	}
}
