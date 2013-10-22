// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "functions.hh"

namespace Dune {
namespace Stuff {

// ====> See the documentation in functions.hh! <====
//       Your work begings below this macro

#define DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE(ddim) \
  DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE_COLS(ddim, 1) \
  DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE_COLS(ddim, 2) \
  DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE_COLS(ddim, 3)

#define DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE_COLS(ddim, rdim) \
  DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION(ddim, rdim, 1) \
  DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION(ddim, rdim, 2) \
  DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION(ddim, rdim, 3)

#define DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION(ddim, rdim, rCdim) \
  template< class E, class D, class R > \
  std::vector< std::string > Functions< E, D, ddim, R, rdim, rCdim >::available() \
  { \
    return { \
      Function::Constant< E, D, d, R, r, rC >::static_id() \
      FUNCTION_Expression_ ## ddim ## _ ## rdim ## _ ## rCdim ## _STATIC_ID(E, D, ddim, R, rdim, rCdim) \
      FUNCTION_Checkerboard_ ## ddim ## _ ## rdim ## _ ## rCdim ## _STATIC_ID(E, D, ddim, R, rdim, rCdim) \
      FUNCTION_Spe10Model1_ ## ddim ## _ ## rdim ## _ ## rCdim ## _STATIC_ID(E, D, ddim, R, rdim, rCdim) \
    }; \
  } /* ... available(...) */ \
 \
  template< class E, class D, class R > \
  Dune::ParameterTree Functions< E, D, ddim, R, rdim, rCdim >::defaultSettings(const std::string type) \
  { \
    if (type == Function::Constant< E, D, d, R, r, rC >::static_id()) \
      return Function::Constant< E, D, d, R, r, rC >::defaultSettings(); \
    FUNCTION_Expression_ ## ddim ## _ ## rdim ## _ ## rCdim ## _SETTINGS(E, D, ddim, R, rdim, rCdim) \
    FUNCTION_Checkerboard_ ## ddim ## _ ## rdim ## _ ## rCdim ## _SETTINGS(E, D, ddim, R, rdim, rCdim) \
    FUNCTION_Spe10Model1_ ## ddim ## _ ## rdim ## _ ## rCdim ## _SETTINGS(E, D, ddim, R, rdim, rCdim) \
    else \
      DUNE_THROW(Dune::RangeError, \
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:") \
                 << " unknown function '" << type << "' requested!"); \
  } /* ... defaultSettings(...) */ \
 \
  template< class E, class D, class R > \
      LocalizableFunctionInterface< E, D, ddim, R, rdim, rCdim >* \
  Functions< E, D, ddim, R, rdim, rCdim >::create(const std::string type, const ParameterTree settings) \
  { \
    if (type == Function::Constant< E, D, d, R, r, rC >::static_id()) \
      return Function::Constant< E, D, d, R, r, rC >::create(settings); \
    FUNCTION_Expression_ ## ddim ## _ ## rdim ## _ ## rCdim ## _CREATE(E, D, ddim, R, rdim, rCdim, settings) \
    FUNCTION_Checkerboard_ ## ddim ## _ ## rdim ## _ ## rCdim ## _CREATE(E, D, ddim, R, rdim, rCdim, settings) \
    FUNCTION_Spe10Model1_ ## ddim ## _ ## rdim ## _ ## rCdim ## _CREATE(E, D, ddim, R, rdim, rCdim, settings) \
    else \
      DUNE_THROW(Dune::RangeError, \
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:") \
                 << " unknown function '" << type << "' requested!"); \
  } // ... create(...)
// DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION

// ====================
// ==== Expression ====
// ====================
#define FUNCTION_Expression_1_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_1_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_1_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_1_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_2_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_2_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_2_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_3_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_3_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Expression< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Expression_3_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/

#define FUNCTION_Expression_1_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_1_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_1_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_1_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_1_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_2_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_2_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_2_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_2_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_3_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_3_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Expression_3_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Expression_3_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/

#define FUNCTION_Expression_1_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_1_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_1_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_1_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_1_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_1_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_1_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_1_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_1_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_2_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_2_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_2_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_2_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_2_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_2_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_2_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_2_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_2_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_3_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_3_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_3_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_3_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_3_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_3_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_3_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Expression< E, D, d, R, r, rC >::static_id()) return Function::Expression< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Expression_3_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Expression_3_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/

// ======================
// ==== Checkerboard ====
// ======================
#define FUNCTION_Checkerboard_1_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_1_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_2_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Checkerboard_3_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Checkerboard< E, D, ddim, R, rdim, rCdim >::static_id()

#define FUNCTION_Checkerboard_1_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_1_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_2_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Checkerboard_3_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::defaultSettings();

#define FUNCTION_Checkerboard_1_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_1_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_2_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Checkerboard_3_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Checkerboard< E, D, d, R, r, rC >::static_id()) return Function::Checkerboard< E, D, d, R, r, rC >::create(settings);

// =====================
// ==== Spe10Model1 ====
// =====================
#define FUNCTION_Spe10Model1_1_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) , Function::Spe10Model1< E, D, ddim, R, rdim, rCdim >::static_id()
#define FUNCTION_Spe10Model1_2_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_1_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_1_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_1_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_2_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_2_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_2_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_3_1_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_3_2_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_3_3_STATIC_ID(E, D, ddim, R, rdim, rCdim) /**/

#define FUNCTION_Spe10Model1_1_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_1_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) else if (type == Function::Spe10Model1< E, D, d, R, r, rC >::static_id()) return Function::Spe10Model1< E, D, d, R, r, rC >::defaultSettings();
#define FUNCTION_Spe10Model1_2_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_2_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_1_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_1_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_1_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_2_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_2_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_2_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_3_1_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_3_2_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/
#define FUNCTION_Spe10Model1_3_3_3_SETTINGS(E, D, ddim, R, rdim, rCdim) /**/

#define FUNCTION_Spe10Model1_1_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_1_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) else if (type == Function::Spe10Model1< E, D, d, R, r, rC >::static_id()) return Function::Spe10Model1< E, D, d, R, r, rC >::create(settings);
#define FUNCTION_Spe10Model1_2_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_2_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_1_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_1_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_1_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_2_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_2_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_2_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_3_1_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_3_2_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/
#define FUNCTION_Spe10Model1_3_3_3_CREATE(E, D, ddim, R, rdim, rCdim, settings) /**/

DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE(1)
DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE(2)
DUNE_STUFF_FUNCTIONS_CC_GENERATE_SPECIALIZATION_DIM_RANGE(3)

} // namespace Stuff
} // namespace Dune

#define DSF_LIST_DIMRANGE(etype, ddim) \
  DSF_LIST_DIMRANGECOLS(etype, ddim, 1) \
  DSF_LIST_DIMRANGECOLS(etype, ddim, 2) \
  DSF_LIST_DIMRANGECOLS(etype, ddim, 3)

#define DSF_LIST_DIMRANGECOLS(etype, ddim, rdim) \
  DSF_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, 1) \
  DSF_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, 2) \
  DSF_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, 3)

#define DSF_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, rcdim) \
  DSF_LIST_RANGEFIELDTYPES(etype, double, ddim, rdim, rcdim)

#define DSF_LIST_RANGEFIELDTYPES(etype, dftype, ddim, rdim, rcdim) \
  DSF_LAST_EXPANSION(etype, dftype, ddim, double, rdim, rcdim) \
  DSF_LAST_EXPANSION(etype, dftype, ddim, long double, rdim, rcdim)

#define DSF_LAST_EXPANSION(etype, dftype, ddim, rftype, rdim, rcdim) \
  template class Dune::Stuff::Functions< etype, dftype, ddim, rftype, rdim, rcdim >;

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneSGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneSGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneSGrid3dEntityType, 3)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneYaspGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneYaspGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneYaspGrid3dEntityType, 3)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneAluSimplexGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneAluSimplexGrid3dEntityType, 3)
DSF_LIST_DIMRANGE(DuneAluCubeGrid3dEntityType, 3)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID
