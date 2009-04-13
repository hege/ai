
#pragma once



#include <stdexcept>
#include <map>


namespace AI {
	namespace Agents {
		class Enviroment;
		template < class _Elem, class _Key = *_Elem > class EnvWithElems;
	}
}


#include "world.h"


namespace AI {
namespace Agents {

class bad_env : public std::exception
{
public:
	bad_env(const char *const& msg) : std::exception(msg) { ; };
};



/*               ________________________
				/                        \
		* ---==<	  The Enviroment      >==--- *
				\________________________/
*/

class Enviroment
{
protected:
	const World	&	myWorld;
public:
	Enviroment( const World &world ) : myWorld( world ) { ; };

	virtual void		refresh( ) { ; };

	virtual	void		assign(const Enviroment &rhs) { ; };
	virtual	Enviroment*	duplicate() const { return new Enviroment(*this); };

	virtual	bool		operator==(const Enviroment &rhs) const { return false; };

						Enviroment( const Enviroment &env2 ) : myWorld(env2.myWorld) { assign(env2); };
	virtual	Enviroment&	operator=(const Enviroment &rhs) { assign(rhs); return *this; };
};



/*               ________________________
				/                        \
		* ---==< Enviroment with elements >==--- *
				\________________________/
*/

template < class _Elem, class _Key >
class EnvWithElems : public Enviroment
{
protected:
	typedef std::map<_Key, _Elem>	elemMap_t;

protected:
	elemMap_t		envElems;

public:
	EnvWithElems( const World &world ) : Enviroment(world) { ; };
	~EnvWithElems( ) { ; };

	void	addElement( const _Elem &elem, const _Key keyVal ) throw ( bad_env );
	_Elem	remElement( const _Key &keyVal ) throw ( bad_env );
	_Elem &	getElement( const _Key &keyVal ) throw ( bad_env );

	// Overrides
	void	refresh( ) { ; };

	bool	operator== ( const Enviroment *rhs ) const {
		EnvWithElems &rhsEnv = *((EnvWithElems*)rhs);
		if ( envElems.size() != rhsEnv.envElems.size() )
			return false;
		else {
			for ( elemMap_t::const_iterator it = envElems.begin(); it != envElems.end(); it++ ) {
				elemMap_t::const_iterator it2 = rhsEnv.envElems.find(it->first);
				if ( it2 == rhsEnv.envElems.end() || it->second != it2->second )
					return false;
			}
			return true;
		}
	};

	void	assign ( const Enviroment *rhs ) {
		EnvWithElems &rhsEnv = *((EnvWithElems*)rhs);
		envElems.clear();
		for ( elemMap_t::const_iterator it = rhsEnv.envElems.begin(); it != rhsEnv.envElems.end(); it++ )
			envElems[it->first] = it->second;
	};

	Enviroment*	duplicate() const { return new EnvWithElems(*this); };
};


template < class _Elem, class _Key >
void EnvWithElems<_Elem, _Key>::addElement( const _Elem &elem, const _Key keyVal ) throw ( bad_env )
{
	if ( envElems.find(keyVal) != envElems.end() )
		throw bad_env("Element key value already existing");
	else
		envElems[keyVal] = elem;
}

template < class _Elem, class _Key >
_Elem EnvWithElems<_Elem, _Key>::remElement( const _Key &keyVal ) throw ( bad_env )
{
	elemMap_t::iterator it(envElems.find(keyVal));
	if ( it == envElems.end() )
		throw bad_env("Element key value not existing");

	_Elem ret = it->second;
	envElems.erase(it);
	return ret;
}

template < class _Elem, class _Key >
_Elem& EnvWithElems<_Elem, _Key>::getElement( const _Key &keyVal ) throw ( bad_env )
{
	elemMap_t::iterator it(envElems.find(keyVal));
	if ( it == envElems.end() )
		throw bad_env("Element key value not existing");
	else
		return it->second;
}


/*
template < class _Elem, class _Key = *_Elem >
class EnvWithInfElems : public EnvWithElems
{
public:
	EnvWithInfElems( const World &world ) : EnvWithElems(world) { ; };
	~EnvWithInfElems( ) { ; };

protected:
	distance
};
*/


}; /* end of namespace Agents */

}; /* end of namespace AI */
