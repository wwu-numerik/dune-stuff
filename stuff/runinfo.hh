#ifndef DUNE_STUFF_RUNINFO_HH
#define DUNE_STUFF_RUNINFO_HH

#define HAS_RUN_INFO

#include <map>
#include <vector>
#include <string>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

namespace Stuff {
/** \brief wrap any info that might be remotely interesting about a single run
	**/
struct RunInfo
{
	std::vector< double > L2Errors;
	std::vector< double > H1Errors;
	double grid_width;
	int refine_level;
	double run_time;
	long codim0;
	int polorder_velocity;
	int polorder_pressure;
	int polorder_sigma;
	std::pair<int,double> c11,d11,c12,d12;
	bool bfg;
	std::string gridname;
	double solver_accuracy;
	double inner_solver_accuracy;
	double bfg_tau;
	std::string extra_info;
	int iterations_inner_avg;
	int iterations_inner_min;
	int iterations_inner_max;
	int iterations_outer_total;
	double max_inner_accuracy;
	std::string problemIdentifier;
	double current_time, delta_t, viscosity, reynolds, alpha;
	std::string algo_id;

	RunInfo() {
		refine_level = codim0 = polorder_velocity
			= polorder_sigma = polorder_pressure
			= iterations_inner_avg = iterations_inner_min
			= iterations_inner_max = iterations_outer_total = -1;
		bfg = true;
		bfg_tau = max_inner_accuracy = grid_width
				= solver_accuracy = run_time
				= alpha = inner_solver_accuracy = -1.0;
		gridname = problemIdentifier = "UNSET";
		extra_info = "none";
		delta_t = 0.1;
		current_time = 0.0;
		viscosity = reynolds = 1.0;
		algo_id = "N.A.";
	}

	static RunInfo dummy( const size_t error_vector_size = 3 )
	{
		RunInfo info;
		info.L2Errors.resize( error_vector_size, std::numeric_limits<double>::quiet_NaN() );
		info.H1Errors.resize( error_vector_size, std::numeric_limits<double>::quiet_NaN() );
		return info;
	}

	template < class Stream >
	void tableLine( Stream& stream ) const
	{
		static boost::format line("%e\t%d\t%e\t%d\t%d\t%d\t%d\t%d\t%s\t%e\t%e\t%e\t%s\t%d\t%d\t%d\t%d\t%e\t%s\t%e\t%e\t%e\t%e\t%e\t%s");
		stream << line %
				grid_width%
				refine_level%
				run_time%
				codim0%
				polorder_velocity%
				polorder_pressure%
				polorder_sigma%
		//		std::pair<int% double> c11,d11,c12,d12%
				 bfg%
				gridname%
				solver_accuracy%
				inner_solver_accuracy%
				bfg_tau%
				extra_info%
				iterations_inner_avg%
				iterations_inner_min%
				iterations_inner_max%
				iterations_outer_total%
				max_inner_accuracy%
				problemIdentifier%
				current_time%  delta_t%  viscosity%  reynolds%  alpha%
				algo_id;
		stream << std::endl;
	}
	template < class Stream >
	void tableHeader( Stream& stream ) const
	{
		static boost::format line("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s");
		stream << line %
				"grid_width"%
				"refine_level"%
				"run_time"%
				"codim0"%
				"polorder_velocity"%
				"polorder_pressure"%
				"polorder_sigma"%
		//		std::pair<int"% double> c11,d11,c12,d12"%
				 "bfg"%
				"gridname"%
				"solver_accuracy"%
				"inner_solver_accuracy"%
				"bfg_tau"%
				"extra_info"%
				"iterations_inner_avg"%
				"iterations_inner_min"%
				"iterations_inner_max"%
				"iterations_outer_total"%
				"max_inner_accuracy"%
				"problemIdentifier"%
				"current_time"%  "delta_t"%  "viscosity"%  "reynolds"%  "alpha"%
				"algo_id";
		stream << std::endl;
	}
};

//! used in all runs to store L2 errors across runs
typedef std::vector< RunInfo >
	RunInfoVector;
typedef std::map< double, RunInfo >
	RunInfoTimeMap;

typedef int
	RunInfoVectorMapKeyType;

typedef std::map< RunInfoVectorMapKeyType, RunInfoVector >
	RunInfoVectorMap;

typedef std::map< RunInfoVectorMapKeyType, RunInfoTimeMap >
	RunInfoTimeMapMap;

void dumpRunInfoVectorToFile( const RunInfoVector& vec, const std::string fn = "runinfos.csv" )
{
	std::ofstream file( getFileinDatadir( fn ).c_str() );
	assert( vec.size() > 0 );
	vec.front().tableHeader( file );
	BOOST_FOREACH( RunInfo info, vec ) {
		info.tableLine( file );
	}
	file.flush();
	file.close();
}

} //namespace Stuff {

#endif // DUNE_STUFF_RUNINFO_HH
