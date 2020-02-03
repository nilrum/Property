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

    TVariable vb(false);
    EXPECT_EQ(vb.Type(), TVariableType::vtUInt);

    TVariable v {true};
    EXPECT_EQ(v.Type(), TVariableType::vtUInt);
    EXPECT_EQ(v.ToInt(), 1);
    EXPECT_EQ(v.ToDouble(), 1.);
    EXPECT_EQ(v.ToBool(), true);
    EXPECT_EQ(v.ToString(), std::string("1"));
}
/*
TEST(TestVariable, TestInitPtr){

    int i = 0;
    TVariable vp(&i);
    EXPECT_EQ(vp.Type(), TVariableType::vtPtr);

    TVariable v {&i};
    EXPECT_EQ(v.ToInt(), (int64_t)&i);
    EXPECT_EQ(v.ToDouble(), (int64_t)&i);
    EXPECT_EQ(v.ToBool(), true);
    EXPECT_EQ(v.ToString(), std::string("ptr"));
}*/

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
