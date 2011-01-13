#ifndef TIMEAWAREDATAWRITER_HH
#define TIMEAWAREDATAWRITER_HH

#include <dune/fem/io/file/datawriter.hh>

namespace Dune {

	/** \brief a Dune::DataWriter derivative that handles current time internaly via passed Dune::Timeprovider
		\note TimeproviderType can only be FractionalTimeProvider atm
		**/
	template < class TimeproviderType, class GridType, class OutputTupleType >
	class TimeAwareDataWriter : public DataWriter< GridType, OutputTupleType >{
		protected:
			typedef DataWriter< GridType, OutputTupleType >
					BaseType;
			using BaseType::grape;
			using BaseType::grid_;
			using BaseType::path_;
			using BaseType::datapref_;
			using BaseType::writeStep_;
			using BaseType::outputFormat_;
			using BaseType::vtk;
			using BaseType::vtkvtx;
			using BaseType::gnuplot;
			using BaseType::data_;
			using BaseType::saveTime_;
			using BaseType::myRank_;
			using BaseType::saveStep_;

	   public:
			TimeAwareDataWriter( const TimeproviderType& timeprovider,
								 const GridType& grid,
								 OutputTupleType& tuple )
				: BaseType( grid, grid.name(), tuple, timeprovider.startTime(), timeprovider.endTime() ),
				timeprovider_( timeprovider )
			{

			}

			void write() const
			{
				write( timeprovider_.time() , timeprovider_.timeStep()  );
			}

			/** \brief write given data to disc
			   \param[in] time actual time of computation
			   \param[in] timestep current number of time step
			   \param[in] data data to write (template type, should be a tuple)
			*/
			void write(double time, int timestep ) const
			{
//				if( BaseType::willWrite( time, timestep ) )
				{
					{
						// check online display
						BaseType::display();
					}

					if( BaseType::outputFormat_ == grape )
					{
						// create new path for time step output
						std::string timeStepPath = IOInterface::createPath ( grid_.comm(),
																			 path_, datapref_ , writeStep_ );

						// for structured grids copy grid
						IOInterface::copyMacroGrid(grid_,path_,timeStepPath,datapref_);

						GrapeDataIO<GridType> dataio;
						// call output of IOTuple
						IOTuple<OutputTupleType>::output(dataio,
														 grid_ ,time, writeStep_, timeStepPath , datapref_, data_ );
					}
	#if USE_VTKWRITER
					else if ( outputFormat_ == vtk || outputFormat_ == vtkvtx )
					{
						// write data in vtk output format
						writeVTKOutput( Element<0>::get(data_) );
					}
	#endif
					else if ( outputFormat_ == gnuplot )
					{
						writeGnuPlotOutput( Element<0>::get(data_), time );
					}
					else
					{
						DUNE_THROW(NotImplemented,"DataWriter::write: wrong output format");
					}

					// only write info for proc 0, otherwise on large number of procs
					// this is to much output
//					if(myRank_ <= 0)
//					{
//						std::cout << "DataWriter["<<myRank_<<"]::write:  time = "<< time << "  writeData: step number " << writeStep_ << std::endl;
//					}
					saveTime_ += saveStep_;
					++writeStep_;
				}
				return;
			}

		protected:
			const TimeproviderType& timeprovider_;

			template <class VTKOut>
			class VTKOutputter {
				public:
				//! Constructor
					VTKOutputter(VTKOut& vtkOut,std::string path, bool parallel,int step)
					   : vtkOut_(vtkOut),
					   path_(path),
					   parallel_(parallel),
					   step_(step)
					{}

				//! Applies the setting on every DiscreteFunction/LocalFunction pair.
					template <class DFType>
					void visit(DFType* f)
					{
						if (!f)
							return;

						std::string name = genFilename( (parallel_) ? "" : path_, f->name(), step_ );
						if ( DFType::FunctionSpaceType::DimRange > 1 ) {
							vtkOut_.addVectorVertexData( *f );
							vtkOut_.addVectorCellData( *f );
						}
						else {
							vtkOut_.addVertexData( *f );
							vtkOut_.addCellData( *f );
						}

						if( parallel_ )
						{
							// write all data for parallel runs
							vtkOut_.pwrite( name.c_str(), path_.c_str(), "." , Dune::VTKOptions::binaryappended );
						}
						else
						{
							// write all data serial
							vtkOut_.write( name.c_str(), Dune::VTKOptions::binaryappended );
						}

						vtkOut_.clear();
					}
					static inline std::string genFilename(const std::string& path,
												   const std::string& fn,
												   int ntime,
												   int precision = 6)
					{
						std::ostringstream name;

						if(path.size() > 0)
						{
							name << path;
							name << "/";
						}
						name << fn;

						char cp[256];
						switch(precision)
						{
							case 2  : { sprintf(cp, "%02d", ntime); break; }
							case 3  : { sprintf(cp, "%03d", ntime); break; }
							case 4  : { sprintf(cp, "%04d", ntime); break; }
							case 5  : { sprintf(cp, "%05d", ntime); break; }
							case 6  : { sprintf(cp, "%06d", ntime); break; }
							case 7  : { sprintf(cp, "%07d", ntime); break; }
							case 8  : { sprintf(cp, "%08d", ntime); break; }
							case 9  : { sprintf(cp, "%09d", ntime); break; }
							case 10 : { sprintf(cp, "%010d", ntime); break; }
							default:
								{
									DUNE_THROW(Exception, "Couldn't gernerate filename with precision = "<<precision);
								}
						}
						name << cp;

						// here implicitly a string is generated
						return name.str();
					}

				private:
					VTKOut & vtkOut_;
					const std::string path_;
					const bool parallel_;
					const int step_;
			};

	#if USE_VTKWRITER
			template <class DFType>
			void writeVTKOutput(const DFType* func) const
			{

				if( !func )
					return;

				// check whether we have parallel run
				const bool parallel = (grid_.comm().size() > 1);

				// generate filename, with path only for serial run

				{
					// get grid part
					typedef typename DFType :: DiscreteFunctionSpaceType :: GridPartType GridPartType;
					const GridPartType& gridPart = func->space().gridPart();

					{
						// create vtk output handler
						typedef VTKIO < GridPartType > VTKIOType;
						VTKIOType vtkio ( gridPart, VTKOptions::nonconforming );

						// add all functions
						ForEachValue<OutputTupleType> forEach(data_);
						VTKOutputter< VTKIOType > io( vtkio, path_, parallel, timeprovider_.timeStep() );
						forEach.apply( io );

						// write all data

					}
				}
			}
	#endif

	};

} // end namespace Dune
#endif // DATAWRITER_HH
