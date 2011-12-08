#ifndef DUNE_STUFF_RESTRICT_PROLONG_HH
#define DUNE_STUFF_RESTRICT_PROLONG_HH

#include <set>
#include <dune/fem/space/common/restrictprolonginterface.hh>

namespace Dune {
/** \brief an arbitrary size set of RestrictProlong operators of same type
  * This class is useful if you have more than a Pair of discrete functions that
	need to be restricted/prolonged via some AdaptionManger.\n
	All interface mandated functions are simply sequentially forwarded to each element in the set.
	\note This set does not take ownership of added operators.
  */
template < class RestrictProlongOperatorPointerType >
class RestrictProlongOperatorSet :public RestrictProlongInterface< RestrictProlongTraits<
                                                                    RestrictProlongOperatorSet<RestrictProlongOperatorPointerType>,double > >
{
public:
    typedef typename RestrictProlongOperatorPointerType::element_type::DomainFieldType
        DomainFieldType;
	RestrictProlongOperatorSet()
	    : pair_set_( SetType() )
	{}

    void setFatherChildWeight (const DomainFieldType& val) const
	{
		for (	SetConstIteratorType it = pair_set_.begin();
				it != pair_set_.end();
				++it )
		{
			(*it)->setFatherChildWeight(val);
		}
	}
	//! restrict data to father
	template <class EntityType>
	void restrictLocal ( EntityType &father,
						 EntityType &son,
						 bool initialize ) const
	{
		for (	SetConstIteratorType it = pair_set_.begin();
				it != pair_set_.end();
				++it )
		{
			(*it)->restrictLocal(father,son,initialize);
		}
	}
	//! prolong data to children
	template <class EntityType>
	void prolongLocal ( EntityType &father,
						EntityType &son,
						bool initialize ) const
	{
		for (	SetConstIteratorType it = pair_set_.begin();
				it != pair_set_.end();
				++it )
		{
			(*it)->prolongLocal(father,son,initialize);
		}
	}

	//! prolong data to children
	template <class CommunicatorImp>
	void addToList(CommunicatorImp& comm)
	{
		for (	typename SetType::iterator it = pair_set_.begin();
				it != pair_set_.end();
				++it )
		{
            RestrictProlongOperatorPointerType k = (*it);

            k->addToList(comm);
		}
	}

    void add( RestrictProlongOperatorPointerType rpair )
	{
		pair_set_.insert( rpair );
	}

    bool remove( RestrictProlongOperatorPointerType rpair )
	{
		// we use this erase signature so that only one of possibly multiple instances gets remvod
		SetIteratorType it = pair_set_.find( rpair );
		bool found = it != pair_set_.end();
		pair_set_.erase( it );
		return found;
	}

protected:
    typedef std::set<RestrictProlongOperatorPointerType>
		SetType;
	typedef typename SetType::iterator
		SetIteratorType;
	typedef typename SetType::const_iterator
		SetConstIteratorType;
	SetType pair_set_;
};

} //end namespace Dune
#endif // DUNE_STUFF_RESTRICT_PROLONG_HH

