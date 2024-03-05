#include <luaocct/lutil.h>

#include <luaocct/util_curve.h>
#include <luaocct/util_math.h>
#include <luaocct/util_shape.h>

template <>
struct luabridge::Stack<luaocct::util::Curve::CurveEnd>
    : luabridge::Enum<luaocct::util::Curve::CurveEnd,
                      luaocct::util::Curve::CurveEnd::None,
                      luaocct::util::Curve::CurveEnd::Start,
                      luaocct::util::Curve::CurveEnd::End,
                      luaocct::util::Curve::CurveEnd::Both> {};

void luaocct_init_util(lua_State *L) {
  LuaBridge__G(L)
      .Begin_Namespace2(util, Math)

      .Bind_Method(luaocct::util::Math, EpsilonEquals)

      .End_Namespace2();

  LuaBridge__G(L)
      .Begin_Namespace2(util, Curve)

      .Begin_Namespace(CurveEnd)
      .Bind_Enum(luaocct::util::Curve::CurveEnd, None)
      .Bind_Enum(luaocct::util::Curve::CurveEnd, Start)
      .Bind_Enum(luaocct::util::Curve::CurveEnd, End)
      .Bind_Enum(luaocct::util::Curve::CurveEnd, Both)
      .End_Namespace()

      .Bind_Method(luaocct::util::Curve, Duplicate)
      .Bind_Method(luaocct::util::Curve, IsClosed)
      .addFunction(
          "GetLength",
          luabridge::overload<const Handle(Geom_Curve) &>(
              &luaocct::util::Curve::GetLength),
          luabridge::overload<const Handle(Geom_Curve) &, const Standard_Real,
                              const Standard_Real>(
              &luaocct::util::Curve::GetLength))
      .addFunction(
          "LengthParameter",
          +[](const Handle(Geom_Curve) & theCurve,
              const Standard_Real theSegmentLength,
              const Standard_Real theTolerance)
              -> std::tuple<Standard_Boolean, Standard_Real> {
            Standard_Real t = 0.0;
            Standard_Boolean ok = luaocct::util::Curve::LengthParameter(
                theCurve, theSegmentLength, t, theTolerance);
            return {ok, t};
          })
      .addFunction(
          "NormalizedLengthParameter",
          +[](const Handle(Geom_Curve) & theCurve, const Standard_Real theS,
              const Standard_Real theTolerance)
              -> std::tuple<Standard_Boolean, Standard_Real> {
            Standard_Real t = 0.0;
            Standard_Boolean ok =
                luaocct::util::Curve::NormalizedLengthParameter(
                    theCurve, theS, t, theTolerance);
            return {ok, t};
          })
      .addFunction(
          "PointAtNormalizedLength",
          +[](const Handle(Geom_Curve) & theCurve, const Standard_Real theS,
              const Standard_Real theTolerance)
              -> std::tuple<Standard_Boolean, gp_Pnt> {
            gp_Pnt aPnt{};
            Standard_Boolean ok = luaocct::util::Curve::PointAtNormalizedLength(
                theCurve, theS, aPnt, theTolerance);
            return {ok, aPnt};
          })
      .Bind_Method(luaocct::util::Curve, Extend)
      .Bind_Method(luaocct::util::Curve, Trim)
      .Bind_Method(luaocct::util::Curve, TrimByLength)
      .Bind_Method(luaocct::util::Curve, IsLinear)
      .Bind_Method(luaocct::util::Curve, ClosestParameters)
      .Bind_Method(luaocct::util::Curve, DivideByCount)
      .Bind_Method(luaocct::util::Curve, Explode)
      .End_Namespace2();

  LuaBridge__G(L)
      .Begin_Namespace2(util, Shape)

      .Bind_Method(luaocct::util::Shape, ToMesh)

      .End_Namespace2();
}
