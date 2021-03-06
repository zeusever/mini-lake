/**
* 
* @file     lstate
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/07/20
*/
#ifndef LEMONXX_LUABIND_LSTATE_HPP
#define LEMONXX_LUABIND_LSTATE_HPP

#include <map>
#include <cassert>
#include <stdexcept>
#include <lua/lua.hpp>
#include <lemonxx/utility/utility.hpp>

extern const void * LEMON_LUABIND_REGISTRY_KEY_VALUE;

#define LEMON_LUABIND_REGISTRY_KEY &LEMON_LUABIND_REGISTRY_KEY_VALUE

namespace lemon{namespace luabind{

	class mem_function_table;

	class pop_stack : private lemon::nocopyable
	{
	public:
		pop_stack(lua_State * L):_L(L),_top(lua_gettop(L)) {}

		~pop_stack(){ lua_settop(_L,_top); }

	private:

		int			_top;

		lua_State	*_L;
	};

	class lua_state : private lemon::nocopyable
	{
	public:

		typedef std::map<const std::type_info*,mem_function_table*>	classes;

		lua_state();

		explicit lua_state(lua_State * L);

		~lua_state() { unbound(); }

		operator lua_State * () { return _L; }

		void register_class(const std::type_info& id,mem_function_table * table);

		const mem_function_table * register_class(const std::type_info& id) const
		{
			classes::const_iterator iter = _classes.find(&id);

			if(iter == _classes.end()) return NULL;

			return iter->second;
		}

	private:

		void bind();

		void unbound();

	private:

		lua_State							*_L;

		classes								_classes;
	};


	inline lua_state::lua_state()
		:_L(luaL_newstate())
	{
		if(!_L) throw std::runtime_error("create lua state error");

		bind();
	}

	inline lua_state::lua_state(lua_State * L)
		:_L(L)
	{
		assert(_L && "L must not null");

		bind();
	}

	inline void lua_state::bind()
	{
		//first open the standard libraries
		luaL_openlibs(_L);
		
		//second register the lua_state * 
		lua_pushlightuserdata(_L,LEMON_LUABIND_REGISTRY_KEY);

		lua_pushlightuserdata(_L,this);

		lua_settable(_L,LUA_REGISTRYINDEX);
	}

	inline void lua_state::unbound()
	{
		lua_pushlightuserdata(_L,LEMON_LUABIND_REGISTRY_KEY);

		lua_pushnil(_L);

		lua_settable(_L,LUA_REGISTRYINDEX);
	}

	inline void lua_state::register_class(const std::type_info& id,mem_function_table * table)
	{
		_classes[&id] = table;
	}

	//
	// get the register lua_state object.
	// 
	inline lua_state * context(lua_State * L)
	{
		pop_stack stack(L);

		lua_pushlightuserdata(L,LEMON_LUABIND_REGISTRY_KEY);

		lua_gettable(L,LUA_REGISTRYINDEX);

		if(!lua_islightuserdata(L,-1)) assert(false && "the L is not register for luabind");

		lua_state * s = (lua_state *)lua_touserdata(L,-1);

		return s;
	}
}}


#endif //LEMONXX_LUABIND_LSTATE_HPP