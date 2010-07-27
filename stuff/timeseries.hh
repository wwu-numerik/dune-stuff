#ifndef DUNE_STUFF_TIMESERIES_HH
#define DUNE_STUFF_TIMESERIES_HH

#include <dune/stuff/misc.hh>
#include <dune/stuff/logging.hh>

namespace Stuff {


class TimeSeriesOutput {

	public:
		TimeSeriesOutput( const RunInfoVectorMap& runInfoVectorMap )
			: runInfoVectorMap_( runInfoVectorMap ),
			vector_count_( runInfoVectorMap_.size() ),
			vector_size_ ( runInfoVectorMap_.begin()->second.size() ),
			prefix_l2_velocity_( "L2-Velo_" ),
			prefix_l2_pressure_( "L2-Pres_" ),
			prefix_runtime_( "runtime_" )
		{
			const RunInfoVector& first = runInfoVectorMap_.begin()->second;
			for ( RunInfoVector::const_iterator it = first.begin();
				  it != first.end();
				  ++it )
			{
				timesteps_.push_back( it->current_time );
			}

			for ( RunInfoVectorMap::const_iterator it = runInfoVectorMap_.begin();
				  it != runInfoVectorMap_.end();
				  ++it )
			{
				const RunInfoVector& vec = it->second;
				double runtime = 0;
				double total_error_pressure = 0;
				double total_error_velocity = 0;
				for ( RunInfoVector::const_iterator vit = vec.begin();
					  vit != vec.end();
					  ++vit )
				{
					const RunInfo& info = *vit;
					runtime += info.run_time;
					total_error_velocity += info.L2Errors[0];
					total_error_pressure += info.L2Errors[1];
				}
				cummulated_runtime_[it->first] = runtime;
				averaged_error_pressure_[it->first] = total_error_pressure / double(vec.size());
				averaged_error_velocity_[it->first] = total_error_velocity / double(vec.size());
			}
			for ( RunInfoVectorMap::const_iterator it = runInfoVectorMap_.begin();
				  it != runInfoVectorMap_.end();
				  ++it )
			{
				const unsigned int refine = it->second.at(0).refine_level;
				Logger().Info() << "Refine " << refine << ", Avg L2 Error Velocity|Pressure "
						<< averaged_error_velocity_[it->first] << "|"
						<< averaged_error_pressure_[it->first] << "; total runtime: "
						<< cummulated_runtime_ [it->first] << std::endl;
			}
			marks_.push_back( "|" );
			marks_.push_back( "x" );
			marks_.push_back( "o" );
			marks_.push_back( "*" );
			marks_.push_back( "-" );
			colors_.push_back( "red" );
			colors_.push_back( "blue" );
			colors_.push_back( "green" );
			colors_.push_back( "yellow" );
			colors_.push_back( "cyan" );
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

		void writeTex( std::string basename )
		{
			std::string filename_csv = writeCSV( basename );
			std::string filename  = basename + ".tex";
			std::ofstream out( filename.c_str() );
			//pressure
			out << "\\begin{tikzpicture}[scale=\\plotscale]\n"
				<< "\\begin{axis}[\n"
				<< "legend style={ at={(1.02,1)},anchor=north west},\n"
				<< "xlabel=Zeit,\n"
				<< "ylabel=$||p_{err}||$]\n";

			size_t i = 0;
			for ( RunInfoVectorMap::const_iterator it = runInfoVectorMap_.begin();
				  it != runInfoVectorMap_.end();
				  ++it, ++i )
			{
				size_t color_index = i % colors_.size();
				size_t mark_index = i % marks_.size();
				const int refine = it->second.at(0).refine_level;
				out << 	"\\addplot[color=" << colors_[color_index] << ",mark=" << marks_[mark_index] << "]\n"
					<< "table[x=timestep,y=" << prefix_l2_pressure_ << i << "] {" << filename_csv << "};"
					<< "\\addlegendentry{refine " << refine << "}\n";
			}
			out << "\\end{axis} \n\\end{tikzpicture}\\\\\n";

			//velocity
			out << "\\begin{tikzpicture}[scale=\\plotscale]\n"
				<< "\\begin{axis}[\n"
				<< "legend style={ at={(1.02,1)},anchor=north west},\n"
				<< "xlabel=Zeit,\n"
				<< "ylabel=$||u_{err}||$]\n";

			i = 0;
			for ( RunInfoVectorMap::const_iterator it = runInfoVectorMap_.begin();
				  it != runInfoVectorMap_.end();
				  ++it, ++i )
			{
				size_t color_index = i % colors_.size();
				size_t mark_index = i % marks_.size();
				const int refine = it->second.at(0).refine_level;
				out << 	"\\addplot[color=" << colors_[color_index] << ",mark=" << marks_[mark_index] << "]\n"
					<< "table[x=timestep,y=" << prefix_l2_velocity_ << i << "] {" << filename_csv << "};"
					<< "\\addlegendentry{refine " << refine << "}\n";
			}
			out << "\\end{axis} \n\\end{tikzpicture}\\\\\n";

			//runtime
			out << "\\begin{tikzpicture}[scale=\\plotscale]\n"
				<< "\\begin{axis}[\n"
				<< "legend style={ at={(1.02,1)},anchor=north west},\n"
				<< "xlabel=Zeit,\n"
				<< "ylabel=$t_{step}$]\n";

			i = 0;
			for ( RunInfoVectorMap::const_iterator it = runInfoVectorMap_.begin();
				  it != runInfoVectorMap_.end();
				  ++it, ++i )
			{
				size_t color_index = i % colors_.size();
				size_t mark_index = i % marks_.size();
				const int refine = it->second.at(0).refine_level;
				out << 	"\\addplot[color=" << colors_[color_index] << ",mark=" << marks_[mark_index] << "]\n"
					<< "table[x=timestep,y=" << prefix_runtime_<< i << "] {" << filename_csv << "};"
					<< "\\addlegendentry{refine " << refine << "}\n";
			}
			out << "\\end{axis} \n\\end{tikzpicture}\\\\\n";
		}

	private:
		const RunInfoVectorMap& runInfoVectorMap_;
		std::vector<double> timesteps_;
		const size_t vector_count_;
		const size_t vector_size_;
		const std::string prefix_l2_velocity_;
		const std::string prefix_l2_pressure_;
		const std::string prefix_runtime_;
		std::vector<std::string> marks_;
		std::vector<std::string> colors_;
		std::map<RunInfoVectorMapKeyType,double> cummulated_runtime_;
		std::map<RunInfoVectorMapKeyType,double> averaged_error_velocity_;
		std::map<RunInfoVectorMapKeyType,double> averaged_error_pressure_;

		std::string  writeCSV( std::string basename )
		{
			std::string filename  = basename + ".csv";
			testCreateDirectory( pathOnly( filename ) );
			std::ofstream out( filename.c_str() );
			out << "timestep\t" ;

			for ( size_t i = 0; i < vector_count_; ++i )
			{
				out <<  prefix_l2_velocity_ << i << "\t"
					<<  prefix_l2_pressure_ << i << "\t"
					<<  prefix_runtime_ << i << "\t";
			}
			out << "\n";

			for ( size_t i = 0; i < vector_size_; ++i )
			{
				out << timesteps_[i] << "\t";

				for ( RunInfoVectorMap::const_iterator it = runInfoVectorMap_.begin();
					  it != runInfoVectorMap_.end();
					  ++it )
				{
					out << it->second.at(i).L2Errors[0] << "\t"
						<< it->second.at(i).L2Errors[1] << "\t"
						<< it->second.at(i).run_time << "\t" ;

				}
				out << "\n";
			}
			out << std::endl;

			return filename;
		}

};

} //namespace Stuff

#endif // DUNE_STUFF_TIMESERIES_HH
