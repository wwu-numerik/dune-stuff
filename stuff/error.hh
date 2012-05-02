#ifndef DUNE_STUFF_ERROR_HH
#define DUNE_STUFF_ERROR_HH

#include <dune/fem/misc/l2norm.hh>
#include <dune/fem/misc/h1norm.hh>
#include <boost/format.hpp>
#include <utility>

namespace Stuff {
	/** \brief Class that facilitates getting absolute and relative error of a pair of DisceteFunctions
		\tparam NormType
	  **/
	template <class GridPartType, class NormType = Dune::L2Norm<GridPartType> >
	class L2Error {
			typedef NormType
				L2NormType;
			L2NormType l2norm_;

		public:
			L2Error( const GridPartType& gridPart )
				: l2norm_( gridPart )
			{}

			struct Errors : public std::pair<double,double> {
				typedef std::pair<double,double>
					BaseType;
				std::string name_;
				Errors( double abs, double rel, std::string name )
					: BaseType( abs, rel ),
					name_(name)
				{}

				//!make friend op <<
				std::string str() const
				{
					return ( boost::format( "%s L2 error: %e (abs) | %e (rel)\n") % name_ % absolute() % relative() ).str();
				}

				double absolute() const {
					return BaseType::first;
				}
				double relative() const {
					return BaseType::second;
				}
			};

			template < class DiscreteFunctionType >
			Errors get( const DiscreteFunctionType& function_A, const DiscreteFunctionType& function_B  ) const
			{
				DiscreteFunctionType tmp("L2Error::tmp", function_A.space() );
				return get( function_A, function_B, tmp );
			}

			template < class DiscreteFunctionType >
			Errors get( const DiscreteFunctionType& function_A, const DiscreteFunctionType& function_B, DiscreteFunctionType& diff ) const
			{
				diff.assign( function_A );
				diff -= function_B;
				const double abs = l2norm_.norm( diff );
				return Errors ( abs,
								abs / l2norm_.norm( function_B ),
								function_A.name()
								);
			}


	};

}//end namespace Stuff
#endif // DUNE_STUFF_ERROR_HH
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

