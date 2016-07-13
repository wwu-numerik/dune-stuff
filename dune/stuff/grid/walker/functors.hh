//   https://github.com/wwu-numerik/dune-stuff
// This file is part of the dune-stuff project:
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014)
//   Rene Milk       (2014 - 2015)

#ifndef DUNE_STUFF_GRID_WALKER_FUNCTORS_HH
#define DUNE_STUFF_GRID_WALKER_FUNCTORS_HH

// nothing here will compile w/o grid present
#if HAVE_DUNE_GRID

#include <dune/common/deprecated.hh>

#include <dune/stuff/grid/entity.hh>
#include <dune/stuff/grid/intersection.hh>
#include <dune/stuff/grid/boundaryinfo.hh>

namespace Dune {
namespace Stuff {
namespace Grid {

template <class GridViewImp>
class Codim0Functor
{
public:
  typedef GridViewImp GridViewType;
  typedef typename Stuff::Grid::Entity<GridViewType>::Type EntityType;

  virtual ~Codim0Functor() = default;

  virtual void prepare() {}

  virtual void apply_local(const EntityType& entity) = 0;

  virtual void finalize() {}
}; // class Codim0Functor


template< class GridViewImp, class ReturnImp >
class Codim0ReturnFunctor
  : public Codim0Functor< GridViewImp >
{
  typedef Codim0Functor< GridViewImp > BaseType;
public:
  typedef ReturnImp        ReturnType;
  using typename BaseType::EntityType;

  virtual ~Codim0ReturnFunctor() {}

  virtual ReturnType compute_locally(const EntityType& entity) = 0;

  virtual ReturnType result() const = 0;
}; // class Codim0ReturnFunctor

template <class GridViewImp>
class Codim1Functor
{
public:
  typedef GridViewImp GridViewType;
  typedef typename Stuff::Grid::Entity<GridViewType>::Type EntityType;
  typedef typename Stuff::Grid::Intersection<GridViewType>::Type IntersectionType;

  virtual ~Codim1Functor() = default;

  virtual void prepare() {}

  virtual void apply_local(const IntersectionType& /*intersection*/, const EntityType& /*inside_entity*/,
                           const EntityType& outside_entity) = 0;

  virtual void finalize() {}
}; // class Codim1Functor

template <class GridViewImp>
class Codim0And1
{
public:
  typedef GridViewImp GridViewType;
  typedef typename Stuff::Grid::Entity<GridViewType>::Type EntityType;
  typedef typename Stuff::Grid::Intersection<GridViewType>::Type IntersectionType;

  virtual ~Codim0And1() {}

  virtual void prepare() {}

  virtual void apply_local(const EntityType& entity) = 0;

  virtual void apply_local(const IntersectionType& /*intersection*/, const EntityType& /*inside_entity*/,
                           const EntityType& /*outside_entity*/) = 0;

  virtual void finalize() {}
}; // class Codim0And1

template <class T>
using Codim0And1Functor = Codim0And1<T>;

template <class GridViewImp>
class DirichletDetector : public Codim1Functor<GridViewImp>
{
  typedef Codim1Functor<GridViewImp> BaseType;

public:
  typedef typename BaseType::GridViewType GridViewType;
  typedef typename BaseType::EntityType EntityType;
  typedef typename BaseType::IntersectionType IntersectionType;

  explicit DirichletDetector(const BoundaryInfoInterface<IntersectionType>& boundary_info)
    : boundary_info_(boundary_info), found_(0)
  {
  }

  virtual ~DirichletDetector() {}

  virtual void apply_local(const IntersectionType& intersection, const EntityType& /*inside_entity*/,
                           const EntityType& /*outside_entity*/) override
  {
    if (boundary_info_.dirichlet(intersection))
      ++found_;
  }

  bool found() const { return found_ > 0; }

private:
  const BoundaryInfoInterface<IntersectionType>& boundary_info_;
  size_t found_;
}; // class DirichletDetector

} // namespace Grid
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_GRID

#endif // DUNE_STUFF_GRID_WALKER_FUNCTORS_HH
