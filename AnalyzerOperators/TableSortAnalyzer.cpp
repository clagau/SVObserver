//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file TableSortAnalyzer.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class coordinated the analyzer for sorting the table.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TableSortAnalyzer.h"
#include "SVObjectLibrary\SVClsids.h"
#include "Tools/SVTool.h"
#include "Tools/TableAnalyzerTool.h"
#include "Definitions/TextDefineSvDef.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(TableSortAnalyzer, TableAnaylzerSortGuid);

#pragma region Constructor
TableSortAnalyzer::TableSortAnalyzer( LPCTSTR ObjectName )
	: SVTaskObjectClass(ObjectName)
	, m_tmpArraySize(0)
{
	Initialize();
}

TableSortAnalyzer::TableSortAnalyzer( SVObjectClass* pOwner, int StringResourceID )
	: SVTaskObjectClass(pOwner, StringResourceID)
	, m_tmpArraySize(0)
{
	Initialize();
}

TableSortAnalyzer::~TableSortAnalyzer()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool TableSortAnalyzer::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = __super::CreateObject(rCreateStructure);

	m_isASC.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );

	return l_bOk;
}

bool TableSortAnalyzer::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_sortColumnObjectInfo);

	SVObjectClass* pObject = m_sortColumnObjectInfo.GetInputObjectInfo().getObject();
	if (!m_sortColumnObjectInfo.IsConnected() || nullptr == dynamic_cast<SvVol::DoubleSortValueObject*> (pObject)
		//check if column part of the right table object (The object must be from same tool as this analyzer.)
		|| nullptr == pObject->GetParent() || pObject->GetParent()->GetParent() != m_ownerObjectInfo.getObject())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer message;
			message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoValidColumnConnected, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back( message );
		}
	}

	if (Result)
	{
		//allocate m_tmpValues
		SvVol::DoubleSortValueObject* pColumnValues = dynamic_cast<SvVol::DoubleSortValueObject*> (m_sortColumnObjectInfo.GetInputObjectInfo().getObject());
		if (nullptr != pColumnValues)
		{
			SvVol::ValueObjectSortContainer sortContainer = pColumnValues->getSortContainer();
			size_t sizeTmp = sortContainer.capacity();
			CheckAndResizeTmpArray(sizeTmp);
		}
	}

	return Result;
}
#pragma endregion Public Methods

#pragma region Protected Methods
bool TableSortAnalyzer::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool returnValue = __super::onRun( rRunStatus, pErrorMessages );
	
	if (returnValue)
	{
		BOOL isASC( true );
		m_isASC.GetValue( isASC );
		SvTo::TableAnalyzerTool* pTool = dynamic_cast<SvTo::TableAnalyzerTool*> (m_ownerObjectInfo.getObject());
		SvVol::DoubleSortValueObject* pColumnValues = dynamic_cast<SvVol::DoubleSortValueObject*> (m_sortColumnObjectInfo.GetInputObjectInfo().getObject());
		if (nullptr != pTool && nullptr != pColumnValues)
		{
			SvVol::ValueObjectSortContainer sortContainer = pColumnValues->getSortContainer();
			size_t sizeValues = sortContainer.size();
			if (0 < sizeValues)
			{
				CheckAndResizeTmpArray(sizeValues);

				double* pValues = m_tmpValues.get();
				for (int i=0; i< sizeValues; ++i)
				{
					pColumnValues->GetValue(pValues[i], i );
				}

				bool isSwitched = false;
				do
				{ 
					isSwitched = false;
					for( int n=0; n<sizeValues-1; n++ )
					{
						if((pValues[n] > pValues[n+1] && isASC) || (pValues[n] < pValues[n+1] && !isASC))
						{
							std::swap( pValues[n], pValues[n+1] );
							std::swap( sortContainer[n], sortContainer[n+1] );
							isSwitched=true;
						}
					}
				}while(isSwitched);
				pTool->setSortContainer(sortContainer, rRunStatus);
			}
		}
		else
		{
			returnValue = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return returnValue;
}
#pragma endregion Protected Methods

#pragma region Private Methods
void TableSortAnalyzer::Initialize()
{
	// Set up your type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::TableAnalyzerType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::TableAnalyzerSortType;

	BuildInputObjectList();
	BuildEmbeddedObjectList();
}

void TableSortAnalyzer::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject( &m_isASC, TableAnaylzerSortIsASCGuid, IDS_OBJECTNAME_TABLEANALYZER_ISASC, true, SvOi::SVResetItemTool );
	m_isASC.SetDefaultValue( BOOL(true) );
}

void TableSortAnalyzer::BuildInputObjectList()
{
	// Source Table.
	m_sortColumnObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::DoubleSortValueObjectType );
	m_sortColumnObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_sortColumnObjectInfo, SvDef::cInputTag_SortColumn );

	addDefaultInputObjects();
}

void TableSortAnalyzer::CheckAndResizeTmpArray( size_t sizeTmp )
{
	if (sizeTmp > m_tmpArraySize || (0 < sizeTmp && nullptr == m_tmpValues))
	{
		m_tmpValues.reset();
		m_tmpValues = std::shared_ptr<double>(new double[sizeTmp], [](double* p){delete[] p;});
		m_tmpArraySize = sizeTmp;
	}
}

#pragma endregion Private Methods

} //namespace SvAo