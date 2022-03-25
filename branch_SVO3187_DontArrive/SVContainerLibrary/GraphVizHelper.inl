//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file GraphVizHelper.inl
/// All Rights Reserved 
//*****************************************************************************
/// The template methods required by boost graph library to output graphs in
/// GraphViz .dot format
//******************************************************************************

namespace SvCl
{
	//! Retrieve vertex name template class
	template< typename NameFunction, typename Graph, typename VertexName >
	class retrieve_name
	{
	public:
		//! Constructor method
		//! \param rNameFunction [in] function reference to lookup name
		//! \param rGraph [in] the graph
		retrieve_name( NameFunction& rNameFunction, const Graph& rGraph ) : m_rNameFunction( rNameFunction ), m_rGraph( rGraph ) {}

		//! Operator method
		//! \param rOutputStream [in] reference to the output stream
		//! \param rVertex [in] reference to the current vertex
		template< typename VertexOrEdge >
		void operator()( std::ostream& rOutputStream, const VertexOrEdge& rVertex ) const 
		{
			const VertexName& rVertexName = boost::get( boost::get(boost::vertex_name, m_rGraph ), rVertex );
			LPCTSTR pName = m_rNameFunction( rVertexName );
			if( nullptr == pName )
			{
				rOutputStream << _T("[label=\"\"]");
			}
			else
			{
				rOutputStream << _T("[label=\"") << pName << _T("\"]");
			}
		}

	private:
		NameFunction& m_rNameFunction;
		const Graph& m_rGraph;
	};

	//! Edge style template class
	template< typename EdgeType, typename Graph >
	class edge_style
	{
	public:
		//! Constructor method
		//! \param rGraph [in] reference to the graph
		edge_style( const Graph& rGraph ) : m_rGraph( rGraph ) {}

		//! Operator method
		//! \param rOutputStream [in] reference to the output stream
		//! \param rEdge [in] reference to the current edge
		template< typename VertexOrEdge >
		void operator()( std::ostream& rOutputStream, const VertexOrEdge& rEdge ) const
		{
			const EdgeType& rEdgeType = boost::get( edge_type_t(), m_rGraph)[rEdge];
			unsigned int Flags( rEdgeType.Flags() );
			int Index = -1;
			for( int i=0; i < sizeof(Flags) * 8 ; i++ )
			{
				if( (1 << i) & Flags )
				{
					Index = i;
					break;
				}
			}
			if( -1 != Index )
			{
				//@WARNING [gra][7.40][19.05.2015] This can later be changed to use different colors for different edge types
				rOutputStream << _T("[color=blue]");
			}
		}

	private:
		const Graph& m_rGraph;
	};

	//! OutputGraphviz template method to save a graph dot file
	//! \param Filename [in] the file name to save to
	//! \param rGraph [in] reference to the graph
	//! \param rName [in] the vertex lookup name method
	template< typename VertexName, typename EdgeType, typename Graph, typename NameFunction >
	std::string OutputGraphviz( LPCTSTR Filename, const Graph& rGraph, NameFunction& rName )
	{
		std::string Result;

		std::ostringstream OutputString;
		std::ofstream OutputFile;
		std::ostream* pOutput(nullptr);

		//If Filename is nullptr then we want to just place the output in a string to return
		if( nullptr == Filename )
		{
			pOutput = &OutputString;
		}
		else
		{
			OutputFile.open( Filename );

			if( !OutputFile.is_open() )
			{
				return Result;
			}
			pOutput = &OutputFile;
		}

		if( nullptr != pOutput )
		{
			boost::write_graphviz( *pOutput, rGraph, retrieve_name<NameFunction, Graph, VertexName>( rName, rGraph ), edge_style<EdgeType, Graph>(rGraph) );
		}
		if( OutputFile.is_open() )
		{
			OutputFile.close();
		}
		if( nullptr == Filename )
		{
			Result = OutputString.str();
		}
		return Result;
	}
} //namespace SvCl

