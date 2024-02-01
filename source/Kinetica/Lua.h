#pragma once

namespace Kinetica::Lua
{

#define LUA_API
#define LUALIB_API

// ====================
// luaconf
// ====================
#define LUA_INT32	int
#define LUAI_UMEM	size_t
#define LUAI_MEM	ptrdiff_t

// ====================
// lua
// ====================
typedef struct lua_State lua_State;
typedef struct lua_Debug lua_Debug;

typedef int (*lua_CFunction) (lua_State *L);
typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);
typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);

#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 4
#define LUA_TTABLE 5
#define LUA_TFUNCTION 6
#define LUA_TUSERDATA 7
#define LUA_TTHREAD 8
#define LUA_NUMTAGS 9

#define lua_tonumber(L,i)	lua_tonumberx(L,i,NULL)
#define lua_tointeger(L,i)	lua_tointegerx(L,i,NULL)
#define lua_tounsigned(L,i)	lua_tounsignedx(L,i,NULL)
#define lua_pop(L,n)		lua_settop(L, -(n)-1)
#define lua_newtable(L)		lua_createtable(L, 0, 0)
#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))
#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)
#define lua_isfunction(L,n) (lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n) (lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n) (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n) (lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n) (lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n) (lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n) (lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n) (lua_type(L, (n)) <= 0)
#define lua_pushliteral(L, s) lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#define lua_pushglobaltable(L) lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS)
#define lua_tostring(L,i) lua_tolstring(L, (i), NULL)

// ====================
// limits
// ====================
typedef unsigned LUA_INT32 lu_int32;
typedef LUAI_UMEM lu_mem;
typedef LUAI_MEM l_mem;
typedef unsigned char lu_byte;
typedef lu_int32 Instruction;

#define api_check(l, e, msg) ((void) 0)

#define cast(t, exp) ((t)(exp))
#define cast_byte(i) cast(lu_byte, (i))
#define cast_num(i) cast(lua_Number, (i))
#define cast_int(i) cast(int, (i))
#define cast_uchar(i) cast(unsigned char, (i))

#define lua_lock(L) ((void) 0)
#define lua_unlock(L) ((void) 0)

// ====================
// lobject
// ====================
#define TValuefields Value value_; int tt_

#define numfield

typedef union GCObject GCObject;
typedef struct lua_TValue TValue;
typedef TValue *StkId;

#define CommonHeader GCObject *next; lu_byte tt; lu_byte marked

#define settt_(o,t)	((o)->tt_=(t))

#define setnvalue(obj,x) \
  { TValue *io=(obj); num_(io)=(x); settt_(io, LUA_TNUMBER); }

#define setnilvalue(obj) settt_(obj, LUA_TNIL)

#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_TLCF); }

#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_TLIGHTUSERDATA); }

#define setbvalue(obj,x) \
  { TValue *io=(obj); val_(io).b=(x); settt_(io, LUA_TBOOLEAN); }

#define rttype(o) ((o)->tt_)
#define novariant(x) ((x) & 0x0F)
#define ttype(o) (rttype(o) & 0x3F)
#define ttypenv(o) (novariant(rttype(o)))
#define checktag(o,t) (rttype(o) == (t))
#define checktype(o,t) (ttypenv(o) == (t))
#define ttisnumber(o) checktag((o), LUA_TNUMBER)
#define ttisnil(o) checktag((o), LUA_TNIL)
#define ttisboolean(o) checktag((o), LUA_TBOOLEAN)
#define ttislightuserdata(o) checktag((o), LUA_TLIGHTUSERDATA)
#define ttisstring(o) checktype((o), LUA_TSTRING)
#define ttisshrstring(o) checktag((o), ctb(LUA_TSHRSTR))
#define ttislngstring(o) checktag((o), ctb(LUA_TLNGSTR))
#define ttistable(o) checktag((o), ctb(LUA_TTABLE))
#define ttisfunction(o) checktype(o, LUA_TFUNCTION)
#define ttisclosure(o) ((rttype(o) & 0x1F) == LUA_TFUNCTION)
#define ttisCclosure(o) checktag((o), ctb(LUA_TCCL))
#define ttisLclosure(o) checktag((o), ctb(LUA_TLCL))
#define ttislcf(o) checktag((o), LUA_TLCF)
#define ttisuserdata(o) checktag((o), ctb(LUA_TUSERDATA))
#define ttisthread(o) checktag((o), ctb(LUA_TTHREAD))
#define ttisdeadkey(o) checktag((o), LUA_TDEADKEY)
#define ttisequal(o1,o2) (rttype(o1) == rttype(o2))

union Value
{
	GCObject *gc;    /* collectable objects */
	void *p;         /* light userdata */
	int b;           /* booleans */
	lua_CFunction f; /* light C functions */
	numfield         /* numbers */
};

struct lua_TValue
{
	TValuefields;
};

// ====================
// lstate
// ====================
typedef struct CallInfo {
	StkId func;  /* function index in the stack */
	StkId	top;  /* top for this function */
	struct CallInfo *previous, *next;  /* dynamic call link */
	short nresults;  /* expected number of results from this function */
	lu_byte callstatus;
	ptrdiff_t extra;
	union {
		struct {  /* only for Lua functions */
			StkId base;  /* base for this function */
			const Instruction *savedpc;
		} l;
		struct {  /* only for C functions */
			int ctx;  /* context info. in case of yields */
			lua_CFunction k;  /* continuation in case of yields */
			ptrdiff_t old_errfunc;
			lu_byte old_allowhook;
			lu_byte status;
		} c;
	} u;
} CallInfo;

struct global_State;

struct lua_State
{
	CommonHeader;
	lu_byte status;
	StkId top;  /* first free slot in the stack */
	global_State *l_G;
	CallInfo *ci;  /* call info for current function */
	const Instruction *oldpc;  /* last pc traced */
	StkId stack_last;  /* last free slot in the stack */
	StkId stack;  /* stack base */
	int stacksize;
	unsigned short nny;  /* number of non-yieldable calls in stack */
	unsigned short nCcalls;  /* number of nested C calls */
	lu_byte hookmask;
	lu_byte allowhook;
	int basehookcount;
	int hookcount;
	lua_Hook hook;
	GCObject *openupval;  /* list of open upvalues in this stack */
	GCObject *gclist;
	struct lua_longjmp *errorJmp;  /* current error recover point */
	ptrdiff_t errfunc;  /* current error handling function (stack index) */
	CallInfo base_ci;  /* CallInfo for first level (C calling Lua) */
};

// ====================
// lapi
// ====================
#define NONVALIDVALUE cast(TValue *, luaO_nilobject)
#define isvalid(o) ((o) != luaO_nilobject)

LUA_API lua_State *lua_newstate(lua_Alloc f, void *ud);
LUA_API void lua_close(lua_State *L);
LUA_API int lua_load(lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode);

TValue *index2addr(lua_State *L, int idx);

LUA_API int lua_gettop(lua_State *L);
LUA_API void lua_settop(lua_State *L, int idx);
LUA_API int lua_type(lua_State *L, int idx);
LUA_API const char *lua_tolstring(lua_State *L, int idx, size_t *len);

// ====================
// lauxlib
// ====================
#define luaL_loadbuffer(L,s,sz,n) luaL_loadbufferx(L,s,sz,n,NULL)

LUALIB_API lua_State *luaL_newstate(void);
LUALIB_API int luaL_loadbufferx(lua_State *L, const char *buff, size_t size, const char *name, const char *mode);
LUALIB_API int luaL_loadstring(lua_State *L, const char *s);
LUALIB_API const char *luaL_tolstring(lua_State *L, int idx, size_t *len);

int luaL_loadstring(lua_State *L, const char *s);
int str_dump(lua_State *L);

}