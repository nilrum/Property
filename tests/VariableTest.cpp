//
// Created by user on 22.08.2019.
//

#include "gtest/gtest.h"
#include "Variable.h"

TEST(TestVariable, TestInitNone) {
    TVariable v;
    EXPECT_EQ(v.Type(), TVariableType::vtNone);
    EXPECT_EQ(v.ToInt(), 0);
    EXPECT_EQ(v.ToDouble(), 0);
    EXPECT_EQ(v.ToBool(), false);
    EXPECT_EQ(v.ToString(), std::string());
}

TEST(TestVariable, TestInitInt) {
    TVariable vi(1);
    EXPECT_EQ(vi.Type(), TVariableType::vtInt);

    TVariable v64(0xDDFFABFFAB);
    EXPECT_EQ(vi.Type(), TVariableType::vtInt);

    TVariable v{2};
    EXPECT_EQ(v.Type(), TVariableType::vtInt);
    EXPECT_EQ(v.ToInt(), 2);
    EXPECT_EQ(v.ToDouble(), 2.);
    EXPECT_EQ(v.ToBool(), true);
    EXPECT_EQ(v.ToString(), std::string("2"));
}

TEST(TestVariable, TestInitUInt) {
    unsigned short a = 1;
    TVariable vi(a);
    EXPECT_EQ(vi.Type(), TVariableType::vtUInt);

    size_t b = 0xFFABFFAB;
    TVariable v64(b);
    EXPECT_EQ(vi.Type(), TVariableType::vtUInt);

    unsigned int vuv = 300;
    TVariable vu(vuv);
    EXPECT_EQ(vi.Type(), TVariableType::vtUInt);

    TVariable v{static_cast<unsigned char>(2)};
    EXPECT_EQ(v.Type(), TVariableType::vtUInt);
    EXPECT_EQ(v.ToInt(), 2);
    EXPECT_EQ(v.ToDouble(), 2.);
    EXPECT_EQ(v.ToBool(), true);
    EXPECT_EQ(v.ToString(), std::string("2"));
}


TEST(TestVariable, TestInitDouble) {

    TVariable vd(1.1);
    EXPECT_EQ(vd.Type(), TVariableType::vtDouble);

    TVariable v{2.2};
    EXPECT_EQ(v.Type(), TVariableType::vtDouble);
    EXPECT_EQ(v.ToInt(), 2);
    EXPECT_EQ(v.ToDouble(), 2.2);
    EXPECT_EQ(v.ToBool(), true);
    EXPECT_EQ(v.ToString(), std::string("2.200000"));
}

TEST(TestVariable, TestInitBool){

    TVariable vf(false);
    EXPECT_EQ(vf.Type(), TVariableType::vtUInt);
    EXPECT_EQ(vf.ToInt(), 0);
    EXPECT_EQ(vf.ToDouble(), 0.);
    EXPECT_EQ(vf.ToBool(), false);
    EXPECT_EQ(vf.ToString(), std::string("0"));

    TVariable vfs("0");
    EXPECT_EQ(vfs.Type(), TVariableType::vtStr);
    EXPECT_EQ(vfs.ToInt(), 0);
    EXPECT_EQ(vfs.ToDouble(), 0.);
    EXPECT_EQ(vfs.ToBool(), false);
    EXPECT_EQ(vfs.ToString(), std::string("0"));

    TVariable vt {true};
    EXPECT_EQ(vt.Type(), TVariableType::vtUInt);
    EXPECT_EQ(vt.ToInt(), 1);
    EXPECT_EQ(vt.ToDouble(), 1.);
    EXPECT_EQ(vt.ToBool(), true);
    EXPECT_EQ(vt.ToString(), std::string("1"));

    TVariable vts("1");
    EXPECT_EQ(vts.Type(), TVariableType::vtStr);
    EXPECT_EQ(vts.ToInt(), 1);
    EXPECT_EQ(vts.ToDouble(), 1.);
    EXPECT_EQ(vts.ToBool(), true);
    EXPECT_EQ(vts.ToString(), std::string("1"));

}

TEST(TestVariable, TestInitStr){

    TVariable vs(std::string("one"));
    EXPECT_EQ(vs.Type(), TVariableType::vtStr);

    TVariable vss("one");
    EXPECT_EQ(vss.Type(), TVariableType::vtStr);
    EXPECT_EQ(vss.ToInt(), 0);
    EXPECT_EQ(vss.ToDouble(), 0.);
    EXPECT_EQ(vss.ToBool(), true);
    EXPECT_EQ(vss.ToString(), std::string("one"));

    TVariable v { std::string("2") };
    EXPECT_EQ(v.Type(), TVariableType::vtStr);
    EXPECT_EQ(v.ToInt(), 2);
    EXPECT_EQ(v.ToDouble(), 2.);
    EXPECT_EQ(v.ToBool(), true);
    EXPECT_EQ(v.ToString(), std::string("2"));
}

ENUM(TTestEnum, one, two, three);
ENUM_CLASS(TTestEnumClass, first, second);

TEST(TestVariable, TestInitEnumExt)
{
    TVariableExtEnum<TTestEnum> twoVal(two);

    EXPECT_EQ(twoVal.Get(), two);

    EXPECT_EQ(twoVal.ToString(), "two");
    EXPECT_EQ(twoVal.ToInt(), 1);

    ASSERT_EQ(twoVal.EnumNamesCount(), 3);
    auto names = twoVal.EnumNames();
    EXPECT_EQ(names[0], "one");
    EXPECT_EQ(names[1], "two");
    EXPECT_EQ(names[2], "three");

    TVariableExtEnum<TTestEnumClass> secondVal(TTestEnumClass::second);

    EXPECT_EQ(secondVal.Get(), TTestEnumClass::second);

    EXPECT_EQ(secondVal.ToString(), "second");
    EXPECT_EQ(secondVal.ToInt(), 1);

    ASSERT_EQ(secondVal.EnumNamesCount(), 2);
    auto namesClass = secondVal.EnumNames();
    EXPECT_EQ(namesClass[0], "first");
    EXPECT_EQ(namesClass[1], "second");
}

TEST(TestVariable, TestInitEnum)
{
    TVariable ve(three);
    EXPECT_EQ(ve.Type(), TVariableType::vtEnum);
    EXPECT_EQ(ve.TypeName(), "enum");
    EXPECT_EQ(ve.ToInt(), 2);
    EXPECT_EQ(ve.ToDouble(), 2.);
    EXPECT_EQ(ve.ToBool(), true);
    EXPECT_EQ(ve.ToString(), std::string("three"));

    TVariable vz(one);
    EXPECT_EQ(vz.Type(), TVariableType::vtEnum);
    EXPECT_EQ(vz.TypeName(), "enum");
    EXPECT_EQ(vz.ToInt(), 0);
    EXPECT_EQ(vz.ToDouble(), 0.);
    EXPECT_EQ(vz.ToBool(), false);
    EXPECT_EQ(vz.ToString(), std::string("one"));
    auto names = EnumNamesFromVariable(vz);
    ASSERT_EQ(names.size(), 3);
    EXPECT_EQ(names[0], "one");
    EXPECT_EQ(names[1], "two");
    EXPECT_EQ(names[2], "three");

    TVariable vc { TTestEnumClass::second };
    EXPECT_EQ(vc.Type(), TVariableType::vtEnum);
    EXPECT_EQ(vc.TypeName(), "enum");
    EXPECT_EQ(vc.ToInt(), 1);
    EXPECT_EQ(vc.ToDouble(), 1.);
    EXPECT_EQ(vc.ToBool(), true);
    EXPECT_EQ(vc.ToString(), std::string("second"));

    auto namesC = EnumNamesFromVariable(vc);
    ASSERT_EQ(namesC.size(), 2);
    EXPECT_EQ(namesC[0], "first");
    EXPECT_EQ(namesC[1], "second");
}

TEST(TestVariable, TestAssign){
    TVariable a(5);
    TVariable b("str");
    TVariable c;

    TVariable va(a);
    EXPECT_EQ(va.Type(), TVariableType::vtInt);
    EXPECT_EQ(va.ToInt(), 5);

    TVariable vb = b;
    EXPECT_EQ(vb.Type(), TVariableType::vtStr);
    EXPECT_EQ(vb.ToString(), std::string("str"));

    c = a;
    EXPECT_EQ(c.Type(), TVariableType::vtInt);
    EXPECT_EQ(c.ToInt(), 5);

    c = b;
    EXPECT_EQ(c.Type(), TVariableType::vtStr);
    EXPECT_EQ(c.ToString(), std::string("str"));

    c.Assign(TVariable(3.4));
    EXPECT_EQ(c.Type(), TVariableType::vtDouble);
    EXPECT_EQ(c.ToDouble(), 3.4);
}

TEST(TestVariable, TestRvalue){
    TVariable a(TVariable(5));
    EXPECT_EQ(a.Type(), TVariableType::vtInt);
    EXPECT_EQ(a.ToInt(), 5);

    TVariable b(TVariable("str"));
    EXPECT_EQ(b.Type(), TVariableType::vtStr);
    EXPECT_EQ(b.ToString(), std::string("str"));

    TVariable c(std::move(a));
    EXPECT_EQ(c.Type(), TVariableType::vtInt);
    EXPECT_EQ(c.ToInt(), 5);
    EXPECT_EQ(a.Type(), TVariableType::vtNone);


    TVariable d = std::move(b);
    EXPECT_EQ(d.Type(), TVariableType::vtStr);
    EXPECT_EQ(d.ToString(), std::string("str"));
    EXPECT_EQ(b.Type(), TVariableType::vtNone);

    b = TVariable("newStr");
    EXPECT_EQ(b.Type(), TVariableType::vtStr);
    EXPECT_EQ(b.ToString(), std::string("newStr"));

    b = std::move(c);
    EXPECT_EQ(b.Type(), TVariableType::vtInt);
    EXPECT_EQ(b.ToInt(), 5);
    EXPECT_EQ(c.Type(), TVariableType::vtNone);
}
