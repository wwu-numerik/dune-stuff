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
	double run_time,cumulative_run_time;
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
				= solver_accuracy = run_time = cumulative_run_time
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
		static boost::format line("%e,%d,%e,%d,%d,%d,%d,%d,%s,%e,%e,%e,%s,%d,%d,%d,%d,%e,%s,%e,%e,%e,%e,%e,%s,%e");
		static boost::format single(",%e");
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
				  algo_id%
				  cumulative_run_time;
		BOOST_FOREACH( double err, L2Errors ) {
			stream << 	single % err ;
		}
		BOOST_FOREACH( double err, H1Errors ) {
			stream << 	single % err ;
		}
		stream << std::endl;
	}
	template < class Stream >
	void tableHeader( Stream& stream ) const
	{
		static boost::format line("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s");
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
				  "algo_id"%
				  "cumulative_run_time";
		static boost::format err(",%s_%d");
		for( size_t i = 0; i < L2Errors.size(); ++i ) {
			stream << 	err % "L2" % i;
		}
		for( size_t i = 0; i < H1Errors.size(); ++i ) {
			stream << 	err % "H1" % i;
		}
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
/** Copyright (c) 2012, Rene Milk 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies, 
 * either expressed or implied, of the FreeBSD Project.
**/

