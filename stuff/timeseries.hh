#ifndef DUNE_STUFF_TIMESERIES_HH
#define DUNE_STUFF_TIMESERIES_HH

#include <cstdio>
#include <dune/stuff/misc.hh>
#include <dune/stuff/logging.hh>
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>

namespace Stuff {

/** \brief mnice tex graph output for a time dependent simulation
	\todo use more boost::format
  **/
class TimeSeriesOutput {

	public:
		TimeSeriesOutput( const RunInfoTimeMapMap& runInfoVectorMap )
			: runInfoMapMap_( runInfoVectorMap ),
			vector_count_( runInfoMapMap_.size() ),
			vector_size_ ( runInfoMapMap_.begin()->second.size() ),
			prefix_l2_velocity_( "L2-Velo_" ),
			prefix_l2_pressure_( "L2-Pres_" ),
			prefix_runtime_( "runtime_" ),
			prefix_eoc_velocity_( "EOC_velocity" ),
			prefix_eoc_pressure_( "EOC_pressure" )
		{
			//select 'largest' runinfo map
			RunInfoTimeMapMap::const_iterator mit = runInfoMapMap_.begin();
			const RunInfoTimeMap* largest = &(runInfoMapMap_.begin()->second);
			++mit;
			for( ; mit != runInfoMapMap_.end(); ++mit )
			{
				if ( mit->second.size() > largest->size() )
					largest = &(mit->second);
			}

			for ( RunInfoTimeMap::const_iterator it = largest->begin();
				  it != largest->end();
				  ++it )
			{
				timesteps_.push_back( it->first );
			}

			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it )
			{
				const RunInfoTimeMap& vec = it->second;
				double runtime = 0;
				double total_error_pressure = 0;
				double total_error_velocity = 0;
				for ( RunInfoTimeMap::const_iterator vit = vec.begin();
					  vit != vec.end();
					  ++vit )
				{
					const RunInfo& info = vit->second;
					runtime += info.run_time;
					total_error_velocity += info.L2Errors[0];
					total_error_pressure += info.L2Errors[1];
				}
				cummulated_runtime_[it->first] = runtime;
				averaged_error_pressure_[it->first] = total_error_pressure / double(vec.size());
				averaged_error_velocity_[it->first] = total_error_velocity / double(vec.size());
			}

			for ( RunInfoTimeMapMap::const_iterator mit = runInfoMapMap_.begin();
				  mit != runInfoMapMap_.end();
				  ++mit )
			{
				const RunInfoTimeMap& vec = mit->second;
				double max_velocity = std::numeric_limits<double>::min();
				double max_pressure = std::numeric_limits<double>::min();
				double max_dt = std::numeric_limits<double>::min();

				for ( RunInfoTimeMap::const_iterator it = vec.begin();
					  it != vec.end();
					  ++it )
				{
					max_velocity = std::max( it->second.L2Errors[0], max_velocity );
					max_pressure = std::max( it->second.L2Errors[1], max_pressure );
					max_dt = std::max( it->second.delta_t, max_dt );
				}
				max_errors_velocity.push_back( std::make_pair( max_velocity, vec.begin()->second.grid_width ) );
				max_errors_pressure.push_back( std::make_pair( max_pressure, vec.begin()->second.grid_width ) );
				max_error_velocity_map_[mit->first] = max_velocity;
				max_error_pressure_map_[mit->first] = max_pressure;
				max_dt_vec.push_back( max_dt );
			}

			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it )
			{
				const unsigned int refine = it->second.begin()->second.refine_level;
				Logger().Info()
							<< boost::format ("Refine %d\tMax (Avg) L2 Error Velocity|Pressure\t %e (%e) | %e (%e) \t total runtime: %d ") % refine
									% max_error_velocity_map_[it->first] % averaged_error_velocity_[it->first]
									% max_error_pressure_map_[it->first] % averaged_error_pressure_[it->first]
									% cummulated_runtime_ [it->first]
							<< std::endl;
			}
			marks_.push_back( "|" );
			marks_.push_back( "x" );
			marks_.push_back( "o" );
			marks_.push_back( "*" );
			marks_.push_back( "-" );
			marks_.push_back( "#" );
			marks_.push_back( "@" );
			colors_.push_back( "red" );
			colors_.push_back( "blue" );
			colors_.push_back( "green" );
			colors_.push_back( "yellow" );
			colors_.push_back( "cyan" );
			colors_.push_back( "magenta" );
			colors_.push_back( "black" );
		}

		bool sanityCheck()
		{
//			for ( RunInfoVectorMap::const_iterator it = runInfoVectorMap_.begin();
//				  it != runInfoVectorMap_.end()-- ;
//				  ++it )
//			{
//				assert( all vectors have same length );
//				assert( all vectors have same timesteps );
//			}
			return false;
		}

		void endSubfloat(std::ofstream& out, bool y_axis_logarithmic = true, bool breakline = true )
		{
			if ( y_axis_logarithmic )
				out << "\\end{semilogyaxis} \n\\end{tikzpicture}}";
			else
				out << "\\end{axis} \n\\end{tikzpicture}}";
			if ( breakline )
				out << "\\\\\n";
			else
				out << "\n";
		}

		void beginSubfloat(std::ofstream& out, bool y_axis_logarithmic = true )
		{
			out << "\\subfloat{\n\\begin{tikzpicture}[scale=\\plotscale]\n";
			if ( y_axis_logarithmic )
				out << "\\begin{semilogyaxis}[\n";
			else
				out << "\\begin{axis}[\n";
			out << "legend style={ at={(1.02,1)},anchor=north west},\n";
		}

		void writeTex( std::string basename )
		{
			std::string filename_csv = filenameOnly( writeCSV( basename ) );
			std::string filename  = basename + ".tex";
			std::ofstream out( filename.c_str() );

			double dt = -666;//nonsense init value to see parse fail (or my devil worship, take your pick)
			out << "\\begin{figure}\n";
			//pressure
			beginSubfloat( out );
			out << "xlabel=Zeit,\n"
				<< "ylabel=$||p_{err}||$]\n";

			size_t i = 0;
			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it, ++i )
			{

				size_t color_index = i % colors_.size();
				size_t mark_index = i % marks_.size();
				RunInfo info = it->second.begin()->second;
				const int refine = info.refine_level;
				const std::string id = info.algo_id;
				const double reynolds = info.reynolds;
				dt = info.delta_t;
				out << "\\addplot[color=" << colors_[color_index] << ",mark=" << marks_[mark_index] << "]\n"
					<< "table[x=timestep,y=" << prefix_l2_pressure_ << i << "] {" << filename_csv << "};"
					<< boost::format("\\addlegendentry{%s: L %d, Re %d}\n") % id % refine % reynolds;
			}
			endSubfloat( out, true, false );

			//velocity
			beginSubfloat( out );
			out << "xlabel=Zeit,\n"
				<< "ylabel=$||u_{err}||$]\n";

			i = 0;
			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it, ++i )
			{
				size_t color_index = i % colors_.size();
				size_t mark_index = i % marks_.size();
				RunInfo info = it->second.begin()->second;
				const int refine = info.refine_level;
				const std::string id = info.algo_id;
				const double reynolds = info.reynolds;
				out << 	"\\addplot[color=" << colors_[color_index] << ",mark=" << marks_[mark_index] << "]\n"
					<< "table[x=timestep,y=" << prefix_l2_velocity_ << i << "] {" << filename_csv << "};"
					<< boost::format("\\addlegendentry{%s: L %d, Re %d}\n") % id % refine % reynolds;
			}
			endSubfloat( out );

			//eoc
			const bool have_eoc = vector_count_ > 1;
			if ( have_eoc ) {
				beginSubfloat( out, false );
				out << "xlabel=refine level,\n"
					<< "ylabel=$eoc$]\n";
				std::string eoc_csv_filename = filenameOnly( writeEOCcsv( basename ) );
				out << 	"\\addplot[color=" << colors_[0] << ",mark=" << marks_[1] << "]\n"
					<< "table[x=refine,y=" << prefix_eoc_velocity_ << "] {" << eoc_csv_filename << "};"
					<< "\\addlegendentry{$eoc_u$}\n";
				out << 	"\\addplot[color=" << colors_[1] << ",mark=" << marks_[0] << "]\n"
					<< "table[x=refine,y=" << prefix_eoc_pressure_ << "] {" << eoc_csv_filename << "};"
					<< "\\addlegendentry{$eoc_p$}\n";
				endSubfloat( out, false, false );
			}

			//runtime
			beginSubfloat( out, false );
			out << "xlabel=Zeit,\n"
				<< "ylabel=$t_{step}$]\n";

			i = 0;
			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it, ++i )
			{
				size_t color_index = i % colors_.size();
				size_t mark_index = i % marks_.size();
				RunInfo info = it->second.begin()->second;
				const int refine = info.refine_level;
				const std::string id = info.algo_id;
				const double reynolds = info.reynolds;
				out << 	"\\addplot[color=" << colors_[color_index] << ",mark=" << marks_[mark_index] << "]\n"
					<< "table[x=timestep,y=" << prefix_runtime_<< i << "] {" << filename_csv << "};"
					<< boost::format("\\addlegendentry{%s: L %d, Re %d}\n") % id % refine % reynolds;
			}
			endSubfloat( out, false );

			BOOST_AUTO( common_info,runInfoMapMap_.begin()->second.begin()->second );
			boost::format meta_info("\\subfloat{"
			                        "\\begin{tabular}{ll}\n"
									" Grid & %s\\\\\n"
									" Polynomial orders & $P_\\sigma = %d$, $P_p = %d$,$P_u = %d$\\\\\n"
									" Problem id & %s \\\\\n"
									" Solver accuracy & \\begin{tabular}{ll}\n"
									"                    inner & %e \\\\\n"
									"                    outer & %e \\\\\n"
									"                   \\end{tabular}\\\\\n"
									"	Misc info & %s \\\\\n"
									"\\end{tabular}\n}" );
			meta_info	% common_info.gridname
						% common_info. polorder_sigma
						% common_info. polorder_pressure
						% common_info. polorder_velocity
						% common_info. problemIdentifier
						% common_info. inner_solver_accuracy
						% common_info. solver_accuracy
						% common_info. extra_info;
			out << meta_info;

			out << "\\caption{dt " << dt << "}"
				<< "\n\\end{figure}\n";
		}

	private:
		const RunInfoTimeMapMap& runInfoMapMap_;
		typedef std::vector<double>
			TimestepVector;
		TimestepVector timesteps_;
		const size_t vector_count_;
		const size_t vector_size_;
		const std::string prefix_l2_velocity_;
		const std::string prefix_l2_pressure_;
		const std::string prefix_runtime_;
		const std::string prefix_eoc_velocity_;
		const std::string prefix_eoc_pressure_;
		std::vector<std::string> marks_;
		std::vector<std::string> colors_;
		std::map<RunInfoVectorMapKeyType,double> cummulated_runtime_;
		std::map<RunInfoVectorMapKeyType,double> averaged_error_velocity_;
		std::map<RunInfoVectorMapKeyType,double> averaged_error_pressure_;
		std::map<RunInfoVectorMapKeyType,double> max_error_velocity_map_;
		std::map<RunInfoVectorMapKeyType,double> max_error_pressure_map_;
		std::vector< std::pair<double,double > > max_errors_velocity;
		std::vector< std::pair<double,double > > max_errors_pressure;
		std::vector< double > max_dt_vec;

		std::string  writeCSV( std::string basename )
		{
			std::string filename  = basename + ".csv";
			testCreateDirectory( pathOnly( filename ) );
			std::ofstream out( filename.c_str() );

			//header
			out << "timestep\t";
			for ( size_t i = 0; i < vector_count_; ++i )
			{
				out <<  prefix_l2_velocity_ << i << "\t"
					<<  prefix_l2_pressure_ << i << "\t"
					<<  prefix_runtime_ << i << "\t";
			}
			out << "\n";

			//data
			for ( size_t i = 0; i < vector_size_; ++i )
			{
				const double current_time = timesteps_[i];
				out << current_time << "\t";

				for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
					  it != runInfoMapMap_.end();
					  ++it )
				{
					RunInfoTimeMap::const_iterator c_it = it->second.find( current_time );
					if ( c_it != it->second.end() )
					{
						RunInfo d = c_it->second;
						out << d.L2Errors[0] << "\t"
							<< d.L2Errors[1] << "\t"
							<< d.run_time << "\t" ;
					}
					else
					{
						out << "NaN" << "\t"
							<< "NaN" << "\t"
							<< "NaN" << "\t" ;

					}

				}
				out << "\n";
			}
			out << std::endl;
			return filename;
		}

		std::string writeEOCcsv( std::string basename )
		{
			std::string filename = basename + ".eoc.csv";
			testCreateDirectory( pathOnly( filename ) );
			std::ofstream out( filename.c_str() );

			out << "refine\t" << prefix_eoc_velocity_ << "\t" << prefix_eoc_pressure_ << "\n";
			for ( size_t i = 0; i < max_errors_pressure.size()-1; ++i )
			{
				const double width_qout = max_errors_pressure[i].second / max_errors_pressure[i+1].second;
				const double dt_qout = max_dt_vec[i] / max_dt_vec[i+1];
				const double total_qout = width_qout * dt_qout;
				const double pressure_qout = max_errors_pressure[i].first/ max_errors_pressure[i+1].first;
				const double velocity_qout = max_errors_velocity[i].first/ max_errors_velocity[i+1].first;
				const double pressure_eoc = std::log( pressure_qout  ) / std::log( total_qout );
				const double velocity_eoc = std::log( velocity_qout ) / std::log( total_qout );
				out << i + 1 << "\t"
					<< velocity_eoc << "\t"
					<< pressure_eoc << "\n";
			}

			out << std::endl;
			return filename;
		}
};

} //namespace Stuff

#endif // DUNE_STUFF_TIMESERIES_HH
