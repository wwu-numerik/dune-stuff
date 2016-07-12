// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014 - 2015)
//   Rene Milk       (2014 - 2015)
//   Tobias Leibner  (2014)

#ifndef DUNE_STUFF_GRID_WALKER_WRAPPER_HH
#define DUNE_STUFF_GRID_WALKER_WRAPPER_HH

// nothing here will compile w/o grid present
#if HAVE_DUNE_GRID

#include "functors.hh"
#include "apply-on.hh"

namespace Dune {
namespace Stuff {
namespace Grid {
namespace internal {

template <class GridViewType>
class Codim0Object : public Functor::Codim0<GridViewType>
{
  typedef Functor::Codim0<GridViewType> BaseType;

public:
  typedef typename BaseType::EntityType EntityType;

  virtual ~Codim0Object() = default;

  virtual bool apply_on(const GridViewType& grid_view, const EntityType& entity) const = 0;
};


template< class GridViewImp, class ReturnType >
class Codim0ReturnObject
  : public Codim0ReturnFunctor< GridViewImp, ReturnType >
{
  typedef Codim0ReturnFunctor< GridViewImp, ReturnType > BaseType;
public:
  using typename BaseType::GridViewType;
  using typename BaseType::EntityType;

  virtual ~Codim0ReturnObject() = default;

  virtual bool apply_on(const GridViewType& grid_view, const EntityType& entity) const = 0;
};

template <class GridViewType, class Codim0FunctorType>
class Codim0FunctorWrapper : public Codim0Object<GridViewType>
{
  typedef Codim0Object<GridViewType> BaseType;

public:
  typedef typename BaseType::EntityType EntityType;

  Codim0FunctorWrapper(Codim0FunctorType& wrapped_functor, const ApplyOn::WhichEntity<GridViewType>* where)
    : wrapped_functor_(wrapped_functor), where_(where)
  {
  }

  virtual ~Codim0FunctorWrapper() = default;

  virtual void prepare() override final { wrapped_functor_.prepare(); }

  virtual bool apply_on(const GridViewType& grid_view, const EntityType& entity) const override final
  {
    return where_->apply_on(grid_view, entity);
  }

  virtual void apply_local(const EntityType& entity) override final { wrapped_functor_.apply_local(entity); }

  virtual void finalize() override final { wrapped_functor_.finalize(); }

private:
  Codim0FunctorType& wrapped_functor_;
  std::unique_ptr<const ApplyOn::WhichEntity<GridViewType>> where_;
}; // class Codim0FunctorWrapper

template <class GridViewType>
class Codim1Object : public Functor::Codim1<GridViewType>
{
  typedef Functor::Codim1<GridViewType> BaseType;

public:
  typedef typename BaseType::IntersectionType IntersectionType;

  virtual ~Codim1Object() = default;

  virtual bool apply_on(const GridViewType& grid_view, const IntersectionType& intersection) const = 0;
};

template <class GridViewType, class Codim1FunctorType>
class Codim1FunctorWrapper : public Codim1Object<GridViewType>
{
  typedef Codim1Object<GridViewType> BaseType;

public:
  typedef typename BaseType::EntityType EntityType;
  typedef typename BaseType::IntersectionType IntersectionType;

  Codim1FunctorWrapper(Codim1FunctorType& wrapped_functor, const ApplyOn::WhichIntersection<GridViewType>* where)
    : wrapped_functor_(wrapped_functor), where_(where)
  {}

  virtual ~Codim1FunctorWrapper() = default;

  virtual void prepare() override final { wrapped_functor_.prepare(); }

  virtual bool apply_on(const GridViewType& grid_view, const IntersectionType& intersection) const override final
  {
    return where_->apply_on(grid_view, intersection);
  }

  virtual void apply_local(const IntersectionType& intersection, const EntityType& inside_entity,
                           const EntityType& outside_entity) override final
  {
    wrapped_functor_.apply_local(intersection, inside_entity, outside_entity);
  }

  virtual void finalize() override final { wrapped_functor_.finalize(); }

private:
  Codim1FunctorType& wrapped_functor_;
  std::unique_ptr<const ApplyOn::WhichIntersection<GridViewType>> where_;
}; // class Codim1FunctorWrapper

template <class GridViewType, class WalkerType>
class WalkerWrapper : public Codim0Object<GridViewType>, public Codim1Object<GridViewType>
{
public:
  typedef typename Codim1Object<GridViewType>::EntityType EntityType;
  typedef typename Codim1Object<GridViewType>::IntersectionType IntersectionType;

  WalkerWrapper(WalkerType& grid_walker, const ApplyOn::WhichEntity<GridViewType>* which_entities)
    : grid_walker_(grid_walker)
    , which_entities_(which_entities)
    , which_intersections_(new ApplyOn::AllIntersections<GridViewType>())
  {
  }

  WalkerWrapper(WalkerType& grid_walker, const ApplyOn::WhichIntersection<GridViewType>* which_intersections)
    : grid_walker_(grid_walker)
    , which_entities_(new ApplyOn::AllEntities<GridViewType>())
    , which_intersections_(which_intersections)
  {
  }

  virtual ~WalkerWrapper() = default;

  virtual void prepare() override final { grid_walker_.prepare(); }

  virtual bool apply_on(const GridViewType& grid_view, const EntityType& entity) const override final
  {
    return which_entities_->apply_on(grid_view, entity) && grid_walker_.apply_on(entity);
  }

  virtual bool apply_on(const GridViewType& grid_view, const IntersectionType& intersection) const override final
  {
    return which_intersections_->apply_on(grid_view, intersection) && grid_walker_.apply_on(intersection);
  }

  virtual void apply_local(const EntityType& entity) override final { grid_walker_.apply_local(entity); }

  virtual void apply_local(const IntersectionType& intersection, const EntityType& inside_entity,
                           const EntityType& outside_entity) override final
  {
    grid_walker_.apply_local(intersection, inside_entity, outside_entity);
  }

  virtual void finalize() override final { grid_walker_.finalize(); }

private:
  WalkerType& grid_walker_;
  std::unique_ptr<const ApplyOn::WhichEntity<GridViewType>> which_entities_;
  std::unique_ptr<const ApplyOn::WhichIntersection<GridViewType>> which_intersections_;
}; // class WalkerWrapper

template <class GridViewType>
class Codim0LambdaWrapper : public Codim0Object<GridViewType>
{
  typedef Codim0Object<GridViewType> BaseType;

public:
  typedef typename BaseType::EntityType EntityType;
  typedef std::function<void(const EntityType&)> LambdaType;

  Codim0LambdaWrapper(LambdaType lambda, const ApplyOn::WhichEntity<GridViewType>* where)
    : lambda_(lambda), where_(where)
  {
  }

  virtual ~Codim0LambdaWrapper() = default;

  virtual bool apply_on(const GridViewType& grid_view, const EntityType& entity) const override final
  {
    return where_->apply_on(grid_view, entity);
  }

  virtual void apply_local(const EntityType& entity) override final { lambda_(entity); }

private:
  LambdaType lambda_;
  std::unique_ptr<const ApplyOn::WhichEntity<GridViewType>> where_;
}; // class Codim0LambdaWrapper


template <class GridViewType>
class Codim1LambdaWrapper : public Codim1Object<GridViewType>
{
  typedef Codim1Object<GridViewType> BaseType;

public:
  typedef typename BaseType::EntityType EntityType;
  using typename BaseType::IntersectionType;
  typedef std::function<void(const IntersectionType&, const EntityType&, const EntityType&)> LambdaType;

  Codim1LambdaWrapper(LambdaType lambda, const ApplyOn::WhichIntersection<GridViewType>* where)
    : lambda_(lambda), where_(where)
  {
  }

  virtual ~Codim1LambdaWrapper() = default;

  virtual bool apply_on(const GridViewType& grid_view, const IntersectionType& intersection) const override final
  {
    return where_->apply_on(grid_view, intersection);
  }

  virtual void apply_local(const IntersectionType& intersection, const EntityType& inside_entity,
                           const EntityType& outside_entity) override final
  {
    lambda_(intersection, inside_entity, outside_entity);
  }

private:
  LambdaType lambda_;
  std::unique_ptr<const ApplyOn::WhichIntersection<GridViewType>> where_;
}; // class Codim1FunctorWrapper

} // namespace internal
} // namespace Grid
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_GRID

#endif // DUNE_STUFF_GRID_WALKER_WRAPPER_HH
