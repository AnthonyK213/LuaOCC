#ifndef _LuaOCCT_lbind_Header
#define _LuaOCCT_lbind_Header

// clang-format off
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Array.h>
#include <LuaBridge/Vector.h>
// clang-format on

#include "mod_header/Adaptor2d.h"
#include "mod_header/Adaptor3d.h"
#include "mod_header/BOPAlgo.h"
#include "mod_header/BOPDS.h"
#include "mod_header/BRep.h"
#include "mod_header/BRepAlgoAPI.h"
#include "mod_header/BRepBuilderAPI.h"
#include "mod_header/BRepGProp.h"
#include "mod_header/BRepLib.h"
#include "mod_header/BRepPrim.h"
#include "mod_header/BRepPrimAPI.h"
#include "mod_header/Bnd.h"
#include "mod_header/CPnts.h"
#include "mod_header/Convert.h"
#include "mod_header/GC.h"
#include "mod_header/GProp.h"
#include "mod_header/Geom.h"
#include "mod_header/Geom2d.h"
#include "mod_header/Geom2dAdaptor.h"
#include "mod_header/Geom2dConvert.h"
#include "mod_header/GeomAbs.h"
#include "mod_header/GeomAdaptor.h"
#include "mod_header/GeomConvert.h"
#include "mod_header/GeomFill.h"
#include "mod_header/GeomLib.h"
#include "mod_header/GeomProjLib.h"
#include "mod_header/IMeshTools.h"
#include "mod_header/IntTools.h"
#include "mod_header/Message.h"
#include "mod_header/Poly.h"
#include "mod_header/Precision.h"
#include "mod_header/Standard.h"
#include "mod_header/TCollection.h"
#include "mod_header/TDF.h"
#include "mod_header/TDocStd.h"
#include "mod_header/TopAbs.h"
#include "mod_header/TopExp.h"
#include "mod_header/TopLoc.h"
#include "mod_header/TopoDS.h"
#include "mod_header/XCAFDoc.h"
#include "mod_header/XCAFPrs.h"
#include "mod_header/gp.h"

#include "mod_header/LOAbs.h"
#include "mod_header/LODoc.h"
#include "mod_header/LOUtil.h"

#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Sequence.hxx>
#include <OSD_FileSystem.hxx>

#include <tuple>
#include <vector>

namespace luabridge {

template <class T> struct ContainerTraits<opencascade::handle<T>> {
  using Type = T;

  static opencascade::handle<T> construct(T *c) { return c; }

  static T *get(const opencascade::handle<T> &c) { return c.get(); }
};

template <> struct Stack<TCollection_AsciiString> {
  static Result push(lua_State *L, const TCollection_AsciiString &tStr) {
    lua_pushstring(L, tStr.ToCString());
    return {};
  }

  static TypeResult<TCollection_AsciiString> get(lua_State *L, int index) {
    if (lua_type(L, index) != LUA_TSTRING)
      return makeErrorCode(ErrorCode::InvalidTypeCast);

    std::size_t length = 0;
    const char *str = lua_tolstring(L, index, &length);
    if (str == nullptr)
      return makeErrorCode(ErrorCode::InvalidTypeCast);

    return TCollection_AsciiString(str);
  }

  static bool isInstance(lua_State *L, int index) {
    return lua_type(L, index) == LUA_TSTRING;
  }
};

template <> struct Stack<TCollection_ExtendedString> {
  static Result push(lua_State *L, const TCollection_ExtendedString &tStr) {
    Standard_Integer length = tStr.LengthOfCString();
    std::string str;
    str.reserve(length);
    Standard_PCharacter p = str.data();
    tStr.ToUTF8CString(p);
    lua_pushstring(L, str.c_str());
    return {};
  }

  static TypeResult<TCollection_ExtendedString> get(lua_State *L, int index) {
    if (lua_type(L, index) != LUA_TSTRING)
      return makeErrorCode(ErrorCode::InvalidTypeCast);

    std::size_t length = 0;
    const char *str = lua_tolstring(L, index, &length);
    if (str == nullptr)
      return makeErrorCode(ErrorCode::InvalidTypeCast);

    return TCollection_ExtendedString(str);
  }

  static bool isInstance(lua_State *L, int index) {
    return lua_type(L, index) == LUA_TSTRING;
  }
};

template <class T> struct Stack<NCollection_Array1<T>> {
  static Result push(lua_State *L, const NCollection_Array1<T> &array1) {
    const int init_stack_size = lua_gettop(L);

    lua_createtable(L, array1.Size(), 0);

    for (Standard_Integer i = 0; i < array1.Size(); ++i) {
      lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

      auto result = Stack<T>::push(L, array1.Value(i + array1.Lower()));

      if (!result) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return result;
      }

      lua_settable(L, -3);
    }

    return {};
  }

  static TypeResult<NCollection_Array1<T>> get(lua_State *L, int index) {
    if (!lua_istable(L, index)) {
      return makeErrorCode(ErrorCode::InvalidTypeCast);
    }

    const int init_stack_size = lua_gettop(L);

    NCollection_Array1<T> array1{1, get_length(L, index)};

    const int abs_index = lua_absindex(L, index);

    lua_pushnil(L);
    Standard_Integer i = 1;

    while (lua_next(L, abs_index) != 0) {
      auto item = Stack<T>::get(L, -1);

      if (!item) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return item.error();
      }

      array1.SetValue(i, *item);

      lua_pop(L, 1);
      i++;
    }

    return array1;
  }
};

template <class T> struct Stack<NCollection_Array2<T>> {
  static Result push(lua_State *L, const NCollection_Array2<T> &array2) {
    const int init_stack_size = lua_gettop(L);

    lua_createtable(L, array2.NbRows(), 0);

    for (Standard_Integer i = 0; i < array2.NbRows(); ++i) {
      lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

      NCollection_Array1<T> row{
          array2(i + array2.LowerRow(), array2.LowerCol()), 1,
          array2.RowLength()};

      auto result = Stack<NCollection_Array1<T>>::push(L, row);

      if (!result) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return result;
      }

      lua_settable(L, -3);
    }

    return {};
  }

  static TypeResult<NCollection_Array2<T>> get(lua_State *L, int index) {
    if (!lua_istable(L, index)) {
      return makeErrorCode(ErrorCode::InvalidTypeCast);
    }

    const int init_stack_size = lua_gettop(L);

    const int nb_row = get_length(L, index);
    int row_length = 0;
    NCollection_Array2<T> array2{};

    const int abs_index = lua_absindex(L, index);

    lua_pushnil(L);
    Standard_Integer i = 1;
    Standard_Boolean is_first = Standard_True;

    while (lua_next(L, abs_index) != 0) {
      auto item = Stack<NCollection_Array1<T>>::get(L, -1);

      if (!item) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return item.error();
      }

      if (is_first) {
        row_length = (*item).Length();
        array2 = NCollection_Array2<T>(1, nb_row, 1, row_length);
        is_first = Standard_False;
      } else {
        if (row_length != (*item).Length()) {
          lua_pop(L, lua_gettop(L) - init_stack_size);
          return makeErrorCode(ErrorCode::InvalidTypeCast);
        }
      }

      for (Standard_Integer j = 1; j <= row_length; ++j) {
        array2.SetValue(i, j, (*item)(j));
      }

      lua_pop(L, 1);
      i++;
    }

    return array2;
  }
};

template <class T> struct Stack<NCollection_List<T>> {
  static Result push(lua_State *L, const NCollection_List<T> &list) {
    const int init_stack_size = lua_gettop(L);

    lua_createtable(L, list.Size(), 0);

    Standard_Integer i = 0;
    for (auto it = list.cbegin(); it != list.cend(); ++it, ++i) {
      lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

      auto result = Stack<T>::push(L, *it);

      if (!result) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return result;
      }

      lua_settable(L, -3);
    }

    return {};
  }

  static TypeResult<NCollection_List<T>> get(lua_State *L, int index) {
    if (!lua_istable(L, index)) {
      return makeErrorCode(ErrorCode::InvalidTypeCast);
    }

    const int init_stack_size = lua_gettop(L);
    NCollection_List<T> list{};
    const int abs_index = lua_absindex(L, index);
    lua_pushnil(L);

    while (lua_next(L, abs_index) != 0) {
      auto item = Stack<T>::get(L, -1);

      if (!item) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return item.error();
      }

      list.Append(*item);

      lua_pop(L, 1);
    }

    return list;
  }
};

template <class T> struct Stack<NCollection_Sequence<T>> {
  static Result push(lua_State *L, const NCollection_Sequence<T> &seq) {
    const int init_stack_size = lua_gettop(L);

    lua_createtable(L, seq.Size(), 0);

    for (Standard_Integer i = 0; i < seq.Size(); ++i) {
      lua_pushinteger(L, static_cast<lua_Integer>(i + 1));

      auto result = Stack<T>::push(L, seq.Value(i + seq.Lower()));

      if (!result) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return result;
      }

      lua_settable(L, -3);
    }

    return {};
  }

  static TypeResult<NCollection_Sequence<T>> get(lua_State *L, int index) {
    if (!lua_istable(L, index)) {
      return makeErrorCode(ErrorCode::InvalidTypeCast);
    }

    const int init_stack_size = lua_gettop(L);
    NCollection_Sequence<T> seq{};
    const int abs_index = lua_absindex(L, index);
    lua_pushnil(L);

    while (lua_next(L, abs_index) != 0) {
      auto item = Stack<T>::get(L, -1);

      if (!item) {
        lua_pop(L, lua_gettop(L) - init_stack_size);
        return item.error();
      }

      seq.Append(*item);
      lua_pop(L, 1);
    }

    return seq;
  }
};

} // namespace luabridge

#define LuaBridge__G(L) luabridge::getGlobalNamespace(L)

#define Begin_Namespace(N) beginNamespace(#N)
#define Begin_Namespace0() beginNamespace("LuaOCCT")
#define Begin_Namespace1(U) beginNamespace("LuaOCCT").beginNamespace(#U)
#define Begin_Namespace2(U, V)                                                 \
  beginNamespace("LuaOCCT").beginNamespace(#U).beginNamespace(#V)
#define End_Namespace() endNamespace()
#define End_Namespace0() endNamespace()
#define End_Namespace1() endNamespace().endNamespace()
#define End_Namespace2() endNamespace().endNamespace().endNamespace()

#define Begin_Class(T) beginClass<T>(#T)
#define End_Class() endClass()

#define Begin_Derive(D, B) deriveClass<D, B>(#D)
#define End_Derive() End_Class()

#define Bind_Enum(E, V)                                                        \
  addProperty(                                                                 \
      #V, +[]() { return E::V; })

#define Bind_Property(T, G, S)                                                 \
  addProperty(#G "_", &T::G, &T::S)                                            \
      .addFunction(#G, &T::G)                                                  \
      .addFunction(#S, &T::S)
#define Bind_Property_Readonly(T, G)                                           \
  addProperty(#G "_", &T::G).addFunction(#G, &T::G)

#define Bind_Method(T, M) addFunction(#M, &T::M)
#define Bind_Method_Static(T, M) addStaticFunction(#M, &T::M)

#define Bind_DownCast(D)                                                       \
  addStaticFunction(                                                           \
      "DownCast", +[](const Handle(Standard_Transient) & h) -> Handle(D) {     \
        return Handle(D)::DownCast(h);                                         \
      })
#define Bind_DownCast1(D, B)                                                   \
  addStaticFunction(                                                           \
      "DownCast", +[](const Handle(B) & h) -> Handle(D) {                      \
        return Handle(D)::DownCast(h);                                         \
      })

#endif
