#ifndef DUNE_STUFF_MIXINS_HH
#define DUNE_STUFF_MIXINS_HH

namespace Stuff {

//! a simple class instance counter
template <class Derived>
class InstanceCounter
{
	static int instanceCounter_;
public:
	InstanceCounter() {instanceCounter_++;}
	~InstanceCounter() {instanceCounter_--;}
	const int instanceCount() const {return instanceCounter_;}
};

template <class Derived> int InstanceCounter<Derived>::instanceCounter_ = 0;

} //namespace Stuff
#endif // DUNE_STUFF_MIXINS_HH
