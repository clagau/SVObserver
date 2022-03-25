//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassRegisterListClass
//* .File Name       : $Workfile:   SVClassRegisterListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:36  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#pragma endregion Includes

class SVObjectClass;
class SVClassRegisterClass;
#pragma warning (push)
#pragma warning (disable : 4471)
namespace SvPb
{
	enum ClassIdEnum;
	enum EmbeddedIdEnum;
}
#pragma warning (pop)

SvPb::ClassIdEnum calcClassId(const _variant_t& rClassIdVariant);
SvPb::EmbeddedIdEnum calcEmbeddedId(const _variant_t& rEmbeddedIdVariant, const _variant_t& rObjectName);
SvPb::EmbeddedIdEnum calcInputEmbeddedId(const _variant_t& rObjectName);
SvPb::EmbeddedIdEnum getEmbeddedIdFromIndirectLinked(SvPb::EmbeddedIdEnum indirectLinkedEmbeddedId);
void fillExchangeObjectId();
void fillExchangeEmbedded();
void fillEmbeddedIdFromIndirectLinked();
uint32_t calcObjectId(const std::string& objectIdString);
uint32_t calcObjectId(const _variant_t& rObjectIdVariant);
std::string convertObjectIdToString(uint32_t objectId);
_variant_t convertObjectIdToVariant(uint32_t objectId);
uint32_t getNextAcquisitionId();
bool isSafeToDeleteWebAppIdsJson();
std::string saveObjectIdMapping();
void resetExchangeObjectIdMap();

class SVClassRegisterListClass
{
public:
	static SVClassRegisterListClass& Instance();

	virtual ~SVClassRegisterListClass();

	SVObjectClass* ConstructNewObject(SvPb::ClassIdEnum classId);

	void Add( SVClassRegisterClass* pClass);

	// This method, Shutdown, is only meant to be called by the main application class and no other
	// It used to be protected and a friend class declaration was used, but that was a bad design as the friend was declared in another project
	// So for now the restriction is made manually, just don't call this method anywhere else, as described via this comment
	void Shutdown();	// This method is only meant to be called by the main application class

protected:
	typedef std::unordered_map< SvPb::ClassIdEnum, SVClassRegisterClass* > SVClassRegisterMap;
	
	SVClassRegisterListClass();

	SVClassRegisterMap m_Registers;

private:
	// Do not Implement
	SVClassRegisterListClass( const SVClassRegisterListClass& rObject );

	// Do not Implement
	const SVClassRegisterListClass& operator=( const SVClassRegisterListClass& rObject );
};

constexpr int c_NumberOfFilterCells = 49;
constexpr int COUNT_OF_INPUT_OUTPUT_IDs = 50;
