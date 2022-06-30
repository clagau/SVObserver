//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file DependencyType.h
//*****************************************************************************
/// Bitwise structure to set the dependency type
//******************************************************************************
#pragma once

namespace SvOl
{
	struct JoinType
	{
	#pragma region Declarations
		enum JoinTypeEnum
		{
			NotSet		= 0x0,
			Owner		= 0X1,
			Dependent	= 0X2,
			All			= 0X3,
		};

		unsigned int ownerJoin		: 1;
		unsigned int dependentJoin	: 1;

	#pragma endregion Declarations

	#pragma region Constructor
	public:
		JoinType() { Assign( NotSet ); }

		explicit JoinType(int flags) { Assign( flags );}

	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		const JoinType& operator += (const JoinType& rRhs)
		{
			ownerJoin |= rRhs.ownerJoin; 
			dependentJoin |= rRhs.dependentJoin;
			return *this;
		}

		const JoinType& operator -= (const JoinType& rRhs)
		{
			ownerJoin &= ~rRhs.ownerJoin; 
			dependentJoin &= ~rRhs.dependentJoin;
			return *this;
		}

		JoinType(const JoinType& rRhs) 
		{
			*this = rRhs;
		}

		const JoinType& operator = (const JoinType& rRhs)
		{
			if (&rRhs != this)
			{
				ownerJoin = rRhs.ownerJoin; 
				dependentJoin = rRhs.dependentJoin;
			}
			return *this;
		}

		void Assign( unsigned int flags )
		{
			ownerJoin = (flags & Owner) ? true : false;
			dependentJoin = (flags & Dependent) ? true : false;
		}

		unsigned int Flags() const
		{
			unsigned int Result( ownerJoin | dependentJoin << 1 );
			
			return Result;
		}

		bool HasMember( const JoinType& rType ) const
		{
			bool bRetVal = false;

			if ((ownerJoin && rType.ownerJoin) ||
				(dependentJoin &&	rType.dependentJoin))
			{
				bRetVal = true;
			}
			return bRetVal;
		}
	#pragma endregion Public Methods
	};

	inline bool operator==( const JoinType& rLhs, const JoinType& rRhs )
	{
		return rLhs.Flags() == rRhs.Flags();
	}
} //namespace SvOl
