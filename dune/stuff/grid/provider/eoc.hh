// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_GRID_PROVIDER_EOC_HH
#define DUNE_STUFF_GRID_PROVIDER_EOC_HH

#if HAVE_DUNE_GRID
#include <dune/grid/io/file/dgfparser.hh>
#endif

#include "default.hh"

namespace Dune {
namespace Stuff {
namespace Grid {
namespace Providers {

#if HAVE_DUNE_GRID


/**
 *  The purpose of this class is to behave like a Stuff::Grid::ProviderInterface and at the same time to provide a
 *  means to obtain the real grid level corresponding to a refinement level.
 */
template <class GridImp>
class EOC : public Default<GridImp>
{
  typedef Default<GridImp> BaseType;

public:
  using typename BaseType::GridType;
  using BaseType::Level;

  explicit EOC(GridType& grd, const size_t num_refs)
    : BaseType(grd)
  {
    setup(num_refs);
  }

  explicit EOC(GridType* grid_ptr, const size_t num_refs)
    : BaseType(grid_ptr)
  {
    setup(num_refs);
  }

  explicit EOC(std::shared_ptr<GridType> grid_ptr, const size_t num_refs)
    : BaseType(grid_ptr)
  {
    setup(num_refs);
  }

  explicit EOC(std::unique_ptr<GridType>&& grid_ptr, const size_t num_refs)
    : BaseType(grid_ptr)
  {
    setup(num_refs);
  }

  size_t num_refinements() const
  {
    assert(levels_.size() > 0);
    return levels_.size() - 1;
  }

  int level_of(const size_t refinement) const
  {
    assert(refinement <= num_refinements());
    return levels_[refinement];
  }

  int reference_level() const
  {
    return reference_level_;
  }

  typename BaseType::LevelGridViewType reference_grid_view() const
  {
    return this->level_view(reference_level_);
  }

private:
  void setup(const size_t num_refinements)
  {
    levels_.push_back(this->grid().maxLevel());
    static const int refine_steps_for_half = DGFGridInfo<GridType>::refineStepsForHalf();
    for (size_t rr = 0; rr < num_refinements; ++rr) {
      this->grid().globalRefine(refine_steps_for_half);
      levels_.push_back(this->grid().maxLevel());
    }
    this->grid().globalRefine(refine_steps_for_half);
    reference_level_ = this->grid().maxLevel();
  } // ... setup(...)

  std::vector<int> levels_;
  int reference_level_;
}; // class EOC


#else // HAVE_DUNE_GRID


template <class GridImp>
class EOC
{
  static_assert(AlwaysFalse<GridImp>::value, "You are missing dune-grid!");
};


#endif // HAVE_DUNE_GRID

} // namespace Providers
} // namespace Grid
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_GRID_PROVIDER_EOC_HH
