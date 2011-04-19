#ifndef DUNE_STUFF_TIMESERIES_HH
#define DUNE_STUFF_TIMESERIES_HH

#include "cmake_config.h" //otherwise array has issues
#include <cstdio>
#include <algorithm>
#include <set>
#include <dune/common/fixedarray.hh>
#include <dune/stuff/misc.hh>
#include <dune/stuff/logging.hh>
#include <boost/format.hpp>
#include <boost/typeof/typeof.hpp>

namespace Stuff {

/** \brief nice tex graph output for a time dependent simulation
	\todo use more boost::format
  **/
class TimeSeriesOutput {

	static double integralPointValue( const double right, const double left, const double weight )
	{
		return (right*right+left*left)*weight*0.5;
	}

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
			typedef std::set<std::string>
				TmpSet;
			TmpSet tmp;
			for( RunInfoTimeMapMap::const_iterator mit = runInfoMapMap_.begin(); mit != runInfoMapMap_.end(); ++mit )
			{
				for ( RunInfoTimeMap::const_iterator vit = mit->second.begin();
					  vit != mit->second.end();
					  ++vit )
				{
					tmp.insert( (boost::format("%f") % vit->first).str() );
				}
			}
			vector_size_ = tmp.size();
			for ( TmpSet::const_iterator s_it = tmp.begin(); s_it != tmp.end(); ++s_it )
			{
				timesteps_.push_back( *s_it );
			}
			std::sort( timesteps_.begin(), timesteps_.end() );

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
					assert( info.L2Errors.size() > 1 );
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

			// L^2(t_0,T;L^2) errors (trapezregel)
			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it )
			{
				const RunInfoTimeMap& info_map = it->second;
				RunInfoVectorMapKeyType key = it->first;
				//integrate errors in [t_0;T]
				double velocity_sum = 0;
				double pressure_sum = 0;
				double h1_velocity_sum = 0;
				RunInfoTimeMap::const_iterator mit = info_map.begin();
				for ( size_t idx = 0;
					  idx < info_map.size()-1;
					  ++idx )
				{
					assert( mit->second.H1Errors.size() > 0 );
					const RunInfo& current	= mit->second;
					const RunInfo& next		= (++mit)->second;
					velocity_sum	+= integralPointValue( next.L2Errors[0], current.L2Errors[0], next.current_time - current.current_time );
					pressure_sum	+= integralPointValue( next.L2Errors[1], current.L2Errors[1], next.current_time - current.current_time );
					h1_velocity_sum += integralPointValue( next.H1Errors[0], current.H1Errors[0], next.current_time - current.current_time );
				}
				velocity_sum = std::sqrt( velocity_sum );
				h1_velocity_sum = std::sqrt( h1_velocity_sum );
				pressure_sum = std::sqrt( pressure_sum );
				avg_errors_velocity_map_[key] = velocity_sum;
				avg_h1_errors_velocity_map_[key] = h1_velocity_sum;
				avg_errors_pressure_map_[key] = pressure_sum;
				avg_errors_velocity_.push_back( std::make_pair( velocity_sum, info_map.begin()->second.grid_width )  );
				avg_errors_pressure_.push_back( std::make_pair( pressure_sum, info_map.begin()->second.grid_width )  );
				avg_h1_errors_velocity_.push_back( std::make_pair( h1_velocity_sum, info_map.begin()->second.grid_width )  );

			}

			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it )
			{
				const unsigned int refine = it->second.begin()->second.refine_level;
				Logger().Info()
							<< boost::format ("Refine %d\tMax (Avg) L2 Error Velocity|Pressure\t %e (%e) | %e (%e)\n\t\tH1 Velocity %e \t total runtime: %d ") % refine
									% max_error_velocity_map_[it->first] % avg_errors_velocity_map_[it->first]
									% max_error_pressure_map_[it->first] % avg_errors_pressure_map_[it->first]
									% avg_h1_errors_velocity_map_[it->first]
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

			//velocity L2
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
			endSubfloat( out, true, true );

			//velocity H1
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
					<< "table[x=timestep,y=" << prefix_l2_velocity_ << i << "_h1] {" << filename_csv << "};"
					<< boost::format("\\addlegendentry{%s H1: L %d, Re %d}\n") % id % refine % reynolds;
			}
			endSubfloat( out, true, false );

			//eoc
			const bool have_eoc = vector_count_ > 1;
			if ( have_eoc ) {
				beginSubfloat( out, false );
				out << "xlabel=refine level,\n"
					<< "ylabel=$eoc$]\n";
				std::string eoc_csv_filename = filenameOnly( writeEOCcsv( basename ) );
				out << 	"\\addplot[color=" << colors_[0] << ",mark=" << marks_[1] << "]\n"
					<< "table[x=refine,y=" << prefix_eoc_velocity_ << "_avg" << "] {" << eoc_csv_filename << "};"
					<< "\\addlegendentry{$eoc_u$}\n";
				out << 	"\\addplot[color=" << colors_[1] << ",mark=" << marks_[0] << "]\n"
					<< "table[x=refine,y=" << prefix_eoc_pressure_ << "_avg" << "] {" << eoc_csv_filename << "};"
					<< "\\addlegendentry{$eoc_p$}\n";
				out << 	"\\addplot[color=" << colors_[3] << ",mark=" << marks_[2] << "]\n"
					<< "table[x=refine,y=" << prefix_eoc_velocity_ << "_avg" << "] {" << eoc_csv_filename << "};"
					<< "\\addlegendentry{$eoc^a_u$}\n";
				out << 	"\\addplot[color=" << colors_[2] << ",mark=" << marks_[3] << "]\n"
					<< "table[x=refine,y=" << prefix_eoc_pressure_ << "_avg" << "] {" << eoc_csv_filename << "};"
					<< "\\addlegendentry{$eoc^a_p$}\n";
				out << 	"\\addplot[color=" << colors_[4] << ",mark=" << marks_[4] << "]\n"
					<< "table[x=refine,y=" << prefix_eoc_velocity_ << "_h1" << "] {" << eoc_csv_filename << "};"
					<< "\\addlegendentry{$eoc^H_u$}\n";
				endSubfloat( out, false, true );
			}

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

			std::ostringstream out_tmp;
			out_tmp << "\\begin{tabular}{cccc}\n"
						"Lvl & $ u_{err}$ & $p_{err}$ & runtime\\\\ ";
			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it )
			{
				const unsigned int refine = it->second.begin()->second.refine_level;

				out_tmp << boost::format ("%d & %e & %e & %d\\\\ ") % refine
									% avg_errors_velocity_map_[it->first]
									% avg_errors_pressure_map_[it->first]
									% cummulated_runtime_ [it->first];
			}
			out_tmp << "\\end{tabular}\n";
			out << boost::format ("\\subfloat{%s}") % out_tmp.str();
			out << "\\caption{dt " << dt << "}"
				<< "\n\\end{figure}\n";
			out << "\\pagebreak\\begin{figure}\n";
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
			out << "\n\\end{figure}\n";
		}

	private:
		const RunInfoTimeMapMap& runInfoMapMap_;
		typedef std::vector<std::string>
			TimestepVector;
		TimestepVector timesteps_;
		const size_t vector_count_;
		size_t vector_size_;
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
		//!error - gridwith/dt pair
		std::vector< std::pair<double,double > > max_errors_velocity;
		std::vector< std::pair<double,double > > max_errors_pressure;

		//! L^2(t_0,T;L^2) errors
		std::map<RunInfoVectorMapKeyType,double> avg_errors_velocity_map_;
		std::map<RunInfoVectorMapKeyType,double> avg_h1_errors_velocity_map_;
		std::map<RunInfoVectorMapKeyType,double> avg_errors_pressure_map_;
		std::vector< std::pair<double,double > > avg_errors_velocity_;
		std::vector< std::pair<double,double > > avg_h1_errors_velocity_;
		std::vector< std::pair<double,double > > avg_errors_pressure_;

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
					<<  prefix_l2_velocity_ << i << "_h1\t"
					<<  prefix_l2_pressure_ << i << "\t"
					<<  prefix_runtime_ << i << "\t";
			}
			out << "\n";

			typedef Dune::array<double,4>
				DataArrayType;
			DataArrayType nans;
			std::fill( nans.begin(), nans.end(), std::numeric_limits<double>::quiet_NaN() );
			std::vector< DataArrayType > datapoints ( runInfoMapMap_.size(), nans );
			typedef std::vector< std::vector< DataArrayType > >
				HelperVector;
			HelperVector remapped_data ( timesteps_.size(), datapoints );

			int el_pos = 0;
			for ( RunInfoTimeMapMap::const_iterator it = runInfoMapMap_.begin();
				  it != runInfoMapMap_.end();
				  ++it,++el_pos )
			{
				for ( RunInfoTimeMap::const_iterator vit = it->second.begin();
					  vit != it->second.end();
					  ++vit )
				{
					const std::string time = (boost::format("%f")%vit->first).str();
					const RunInfo& info = vit->second;
					int pos = std::find(timesteps_.begin(), timesteps_.end(), time) - timesteps_.begin();
					if ( pos >= int(timesteps_.size()) )
					{
						std::cerr << boost::format("map %d exceed limit %d -- %f \n") %el_pos	% pos % time;
						continue;
					}
					DataArrayType& b = remapped_data[pos][el_pos];
					b[0] = info.L2Errors[0];
					b[1] = info.H1Errors[0];
					b[2] = info.L2Errors[1];
					b[3] = info.run_time;
				}
			}
			//data
			for ( size_t i = 0; i < timesteps_.size(); ++i )
			{
				const std::string current_time = timesteps_[i];
				out << current_time << "\t";

				for( int j = 0; j < int(vector_count_); ++j)
				{
					out << boost::format("%e\t%e\t%e\t%e\t")
						   % remapped_data[i][j][0]
						   % remapped_data[i][j][1]
						   % remapped_data[i][j][2]
						   % remapped_data[i][j][3];
				}
				out << "\n";
			}
			out << std::endl;
			return filename;
		}

		std::string writeEOCcsv( std::string basename )
		{
			std::string filename = basename + ".eoc.csv";
			std::string integrated_filename = basename + ".int.csv";
			testCreateDirectory( pathOnly( filename ) );
			std::ofstream out( filename.c_str() );
			std::ofstream int_out( integrated_filename.c_str() );

			boost::format header("refine\t%s\t%s_avg\t%s\t%s_avg\t%s_h1\n");

			out << header
				   % prefix_eoc_velocity_ % prefix_eoc_velocity_
				   % prefix_eoc_pressure_ % prefix_eoc_pressure_
				   % prefix_eoc_velocity_;
			int_out << "refine\tL2t_velocity\tL2t_pressure\tL2t_h1_velocity\tMaxL2_velocity\tMaxL2_pressure\n";

			const size_t errordata_point_count = max_errors_pressure.size();
			for ( size_t i = 0; i < errordata_point_count-1; ++i )
			{
				const double width_qout = max_errors_pressure[i].second / max_errors_pressure[i+1].second;
				const double dt_qout = max_dt_vec[i] / max_dt_vec[i+1];
				const double total_qout = width_qout * dt_qout;
				const double pressure_qout = max_errors_pressure[i].first/ max_errors_pressure[i+1].first;
				const double velocity_qout = max_errors_velocity[i].first/ max_errors_velocity[i+1].first;
				const double avg_pressure_qout = avg_errors_pressure_[i].first/ avg_errors_pressure_[i+1].first;
				const double avg_velocity_qout = avg_errors_velocity_[i].first/ avg_errors_velocity_[i+1].first;
				const double h1_avg_velocity_qout = avg_h1_errors_velocity_[i].first/ avg_h1_errors_velocity_[i+1].first;
				const double pressure_eoc = std::log( pressure_qout  ) / std::log( total_qout );
				const double velocity_eoc = std::log( velocity_qout ) / std::log( total_qout );
				const double avg_pressure_eoc = std::log( avg_pressure_qout ) / std::log( total_qout );
				const double avg_velocity_eoc = std::log( avg_velocity_qout ) / std::log( total_qout );
				const double h1_avg_velocity_eoc = std::log( h1_avg_velocity_qout ) / std::log( total_qout );
				out << boost::format("%d\t%e\t%e\t%e\t%e\t%e\n")
						% (i + 1)
						% velocity_eoc % avg_velocity_eoc
						% pressure_eoc % avg_pressure_eoc
						% h1_avg_velocity_eoc;
				int_out << boost::format("%d\t%e\t%e\t%e\t%e\t%e\n")
							% i
							% avg_errors_velocity_[i].first
							% avg_errors_pressure_[i].first
							% avg_h1_errors_velocity_[i].first
							% max_errors_velocity[i].first
							% max_errors_pressure[i].first;

			}
			const size_t last_idx = errordata_point_count -1;
			int_out << boost::format("%d\t%e\t%e\t%e\t%e\t%e\n")
					   % last_idx
					   % avg_errors_velocity_[last_idx].first
					   % avg_errors_pressure_[last_idx].first
					   % avg_h1_errors_velocity_[last_idx].first
					   % max_errors_velocity[last_idx].first
					   % max_errors_pressure[last_idx].first;

			out << std::endl;
			int_out << std::endl;
			return filename;
		}
};

} //namespace Stuff

#endif // DUNE_STUFF_TIMESERIES_HH
