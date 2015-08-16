#include <lua.h>
#include <lauxlib.h>
#include <math.h>
#include <stdio.h>
extern void hello()
{
    printf("hello so");
}

static int l_sin(lua_State *L)
{
    double d = luaL_checknumber(L,1);
    lua_pushnumber(L, sin(d));
    return 1;
}

static const luaL_Reg testMod[] =
{
    {"sin",l_sin},
    {NULL, NULL}
};

int luaopen_testMod(lua_State *L)
{
    luaL_openlib(L, "testMod",testMod,0);
    return 1;
}
