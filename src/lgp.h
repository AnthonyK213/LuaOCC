#ifndef _LuaOCC_lgp_Header
#define _LuaOCC_lgp_Header

#include <gp_Dir.hxx>
#include <gp_EulerSequence.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Quaternion.hxx>
#include <gp_QuaternionNLerp.hxx>
#include <gp_QuaternionSLerp.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>

#include "lbind.h"

int32_t luaocc_init_gp(lua_State *L);

#endif
