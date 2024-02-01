#include "Lua.h"

namespace Kinetica::Lua
{

const TValue *luaO_nilobject;

// ====================
// lapi
// ====================
LUA_API lua_State *lua_newstate(lua_Alloc f, void *ud)
{
    return Offsets::CallID<"lua_newstate", decltype(&lua_newstate)>(f, ud);
}

LUA_API void lua_close(lua_State *L)
{
    Offsets::CallID<"lua_close", decltype(&lua_close)>(L);
}

LUA_API int lua_load(lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode)
{
    return Offsets::CallID<"lua_load", decltype(&lua_load)>(L, reader, data, chunkname, mode);
}

TValue *index2addr(lua_State *L, int idx)
{
    return Offsets::CallID<"index2addr", decltype(&index2addr)>(L, idx);
}

LUA_API int lua_gettop(lua_State *L)
{
    return cast_int(L->top - (L->ci->func + 1));
}

LUA_API void lua_settop(lua_State *L, int idx)
{
    StkId func = L->ci->func;
    lua_lock(L);
    if (idx >= 0) {
        api_check(L, idx <= L->stack_last - (func + 1), "new top too large");
        while (L->top < (func + 1) + idx)
            setnilvalue(L->top++);
        L->top = (func + 1) + idx;
    }
    else {
        api_check(L, -(idx + 1) <= (L->top - (func + 1)), "invalid new top");
        L->top += idx + 1;  /* `subtract' index (index is negative) */
    }
    lua_unlock(L);
}

LUA_API int lua_type(lua_State *L, int idx)
{
    StkId o = index2addr(L, idx);
    return (isvalid(o) ? ttypenv(o) : LUA_TNONE);
}

LUA_API void lua_getglobal(lua_State *L, const char *var)
{
    // 1409D9150
}

LUA_API void lua_gettable(lua_State *L, int idx)
{
    // 1409D8810
}

LUA_API const char *lua_tolstring(lua_State *L, int idx, size_t *len)
{
    return Offsets::CallID<"lua_tolstring", decltype(&lua_tolstring)>(L, idx, len);
}

// ====================
// lauxlib
// ====================
LUALIB_API lua_State *luaL_newstate(void)
{
    return Offsets::CallID<"luaL_newstate", lua_State *(*)()>();
}

struct LoadS
{
    const char *s;
    size_t size;
};

static const char *getS(lua_State *L, void *ud, size_t *size)
{
    LoadS *ls = (LoadS *)ud;
    (void)L;  /* not used */
    if (ls->size == 0) return NULL;
    *size = ls->size;
    ls->size = 0;
    return ls->s;
}

LUALIB_API int luaL_loadbufferx(lua_State *L, const char *buff, size_t size, const char *name, const char *mode)
{
    LoadS ls;
    ls.s = buff;
    ls.size = size;
    return lua_load(L, getS, &ls, name, mode);
}

LUALIB_API int luaL_loadstring(lua_State *L, const char *s)
{
    return luaL_loadbuffer(L, s, strlen(s), s);
}

LUALIB_API const char *luaL_tolstring(lua_State *L, int idx, size_t *len)
{
    return Offsets::CallID<"luaL_tolstring", decltype(&luaL_tolstring)>(L, idx, len);
}

int str_dump(lua_State *L)
{
    return Offsets::CallID<"str_dump", decltype(&str_dump)>(L);
}

}