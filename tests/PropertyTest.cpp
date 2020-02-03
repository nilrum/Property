//
// Created by user on 26.08.2019.
//
#include <gtest/gtest.h>
#include "TestCasePropertyClass.h"

TEST(PropertyTest, VariableAndTPropertyClass)
{
    TPtrPropertyClass prop = std::make_shared<TPropertyClass>();
    EXPECT_TRUE(prop);
    prop->SetName("propertyClass");
    EXPECT_EQ(prop->Name(), TString("propertyClass"));

    TVariable var = PropertyClassToVariable(prop);
    EXPECT_EQ(var.Type(), TVariableType::vtExt);

    TPtrPropertyClass prop2 = VariableToPropClass(var);
    EXPECT_TRUE(prop2);
    EXPECT_EQ(prop2->Name(), TString("propertyClass"));

    TPtrPropertyClass prop3 = var.ToType<TPtrPropertyClass>();
    EXPECT_TRUE(prop3);
    EXPECT_EQ(prop3->Name(), TString("propertyClass"));

    TVariable var2 = var;
    TPtrPropertyClass prop4 = VariableToPropClass(var2);
    EXPECT_TRUE(prop4);
    EXPECT_EQ(prop4->Name(), TString("propertyClass"));

}

TEST(PropertyTest, VariableAndTPropertyInher)
{
    TPtrPropertyInher inher = std::make_shared<TPropertyInher>();
    EXPECT_TRUE(inher);
    inher->SetName("inherClass");
    EXPECT_EQ(inher->Name(), TString("inherClass"));

    TVariable var = PropertyClassToVariable(inher);
    EXPECT_EQ(var.Type(), TVariableType::vtExt);

    TPtrPropertyClass prop = VariableToPropClass(var);
    EXPECT_TRUE(prop);
    EXPECT_EQ(prop->Name(), TString("inherClass"));

    TPtrPropertyClass prop2 = var.ToType<TPtrPropertyClass>();
    EXPECT_TRUE(prop2);
    EXPECT_EQ(prop2->Name(), TString("inherClass"));

    TPtrPropertyInher inher2 = VariableCastTo<TPtrPropertyInher>(var);
    EXPECT_TRUE(inher2);
    EXPECT_EQ(inher2->Name(), TString("inherClass"));

    TPtrPropertyInher inher3 = var.ToType<TPtrPropertyInher>();
    EXPECT_FALSE(inher3);
}

TEST(PropertyTest, TPropertyClass)
{
    TPropertyClass a;

    EXPECT_EQ(a.Name(), TString());

    a.SetName("hello");
    EXPECT_EQ(a.Name(), TString("hello"));

    EXPECT_EQ(a.TypeClass(), TString("TPropertyClass"));

    EXPECT_EQ(a.CountProperty(), 1);

    EXPECT_EQ(a.IndexProperty("name"), 0);
    EXPECT_EQ(a.IndexProperty("not find"), -1);

    EXPECT_EQ(a.ReadProperty(-1).ToString(), TString());
    EXPECT_EQ(a.ReadProperty(2).ToString(), TString());
    EXPECT_EQ(a.ReadProperty(0).ToString(), TString("hello"));

    a.WriteProperty(0, TVariable("newName"));
    EXPECT_EQ(a.ReadProperty(0).ToString(), TString("newName"));

    a.WriteProperty(-1, TVariable("newName"));//проверка нет исключения
    a.WriteProperty(2, TVariable("newName"));

    const TPropertyManager& man = a.Manager();
    EXPECT_EQ(man.CountProperty(), 1);
    EXPECT_EQ(man.Property(0).IsValid(), true);
    EXPECT_EQ(man.Property(0).IsStorable(), true);
    EXPECT_EQ(man.Property(0).IsLoadable(), true);
    EXPECT_EQ(man.Property(0).IsReadOnly(), false);
    EXPECT_EQ(man.Property(0).IsClass(), false);
    EXPECT_EQ(man.Property(0).IsArray(), false);

}

TEST(PropertyTest, TPropertyInher)
{
    TPropertyInher a;

    EXPECT_EQ(a.Name(), TString());

    a.SetName("nameValue");
    a.ClassVar()->SetName("classVar");
    a.ClassVar2()->SetName("classVar2");
    a.ClassVar2()->SetIntVar2(5);
    EXPECT_EQ(a.Name(), TString("nameValue"));
    EXPECT_EQ(a.ClassVar()->Name(), TString("classVar"));
    EXPECT_EQ(a.ClassVar2()->Name(), TString("classVar2"));

    EXPECT_EQ(a.TypeClass(), TString("TPropertyInher"));

    EXPECT_EQ(a.CountProperty(), 8);

    EXPECT_EQ(a.IndexProperty("name"), 0);
    EXPECT_EQ(a.IndexProperty("intVar"), 1);
    EXPECT_EQ(a.IndexProperty("stringVar"), 2);
    EXPECT_EQ(a.IndexProperty("boolVar"), 3);
    EXPECT_EQ(a.IndexProperty("doubleVar"), 4);
    EXPECT_EQ(a.IndexProperty("classVar"), 5);
    EXPECT_EQ(a.IndexProperty("classVar2"), 6);
    EXPECT_EQ(a.IndexProperty("childs"), 7);
    EXPECT_EQ(a.IndexProperty("notFind"), -1);

    EXPECT_EQ(a.ReadProperty(0).ToString(), TString("nameValue"));

    a.WriteProperty(0, TVariable("newNameValue"));
    EXPECT_EQ(a.ReadProperty(0).ToString(), TString("newNameValue"));

    //int
    a.WriteProperty("intVar", TVariable(6));
    EXPECT_EQ(a.ReadProperty("intVar").ToInt(), 6);

    a.WriteProperty(1, TVariable(5));
    EXPECT_EQ(a.ReadProperty(1).ToInt(), 5);
    int intVar = a.ReadProperty(1);
    EXPECT_EQ(intVar, 5);

    a.WriteProperty(1, TVariable("7"));
    EXPECT_EQ(a.ReadProperty(1).ToInt(), 7);

    a.WriteProperty(1, TVariable(7.5));
    EXPECT_EQ(a.ReadProperty(1).ToDouble(), 7.);

    a.WriteProperty(1, TVariable("notInt"));//неверный аргумент обнуляет значение
    EXPECT_EQ(a.ReadProperty(1).ToInt(), 0);

    //string
    a.WriteProperty(2, TVariable("newString"));
    EXPECT_EQ(a.ReadProperty(2).ToString(), TString("newString"));
    TString strVar = a.ReadProperty(2);
    EXPECT_EQ(strVar, TString("newString"));
    a.WriteProperty("stringVar", TVariable("newStringName"));
    EXPECT_EQ(a.ReadProperty("stringVar").ToString(), TString("newStringName"));

    //bool
    a.WriteProperty(3, TVariable(true));
    EXPECT_EQ(a.ReadProperty(3).ToBool(), true);
    bool boolVar = a.ReadProperty(3);
    EXPECT_EQ(boolVar, true);
    a.WriteProperty("boolVar", TVariable(false));
    EXPECT_EQ(a.ReadProperty("boolVar").ToBool(), false);

    //double
    a.WriteProperty(4, TVariable(5.6789));
    EXPECT_EQ(a.ReadProperty(4).ToDouble(), 5.6789);
    double doubleVar = a.ReadProperty(4);
    EXPECT_EQ(doubleVar, 5.6789);
    a.WriteProperty("doubleVar", TVariable(7.8901));
    EXPECT_EQ(a.ReadProperty("doubleVar").ToDouble(), 7.8901);

    //class TPropertyClass
    TPtrPropertyClass var = a.ReadProperty(5).ToType<TPtrPropertyClass>();
    EXPECT_TRUE(var);
    EXPECT_EQ(var->Name(), TString("classVar"));

    TPtrPropertyClass var2 = VariableToPropClass(a.ReadProperty(5));
    EXPECT_TRUE(var2);
    EXPECT_EQ(var2->Name(), TString("classVar"));

    a.WriteProperty(5, PropertyClassToVariable(std::make_shared<TPropertyClass>()));

    TPtrPropertyClass var3 = VariableToPropClass(a.ReadProperty(5));
    EXPECT_TRUE(var3);
    EXPECT_EQ(var3->Name(), TString(""));

    //class Inherited TPropertyClass
    TPtrPropertyClass inhr = VariableToPropClass(a.ReadProperty(6));
    EXPECT_TRUE(inhr);
    EXPECT_EQ(inhr->Name(), TString("classVar2"));
    EXPECT_EQ(inhr->ReadProperty("intVar2").ToInt(), 5);

    TPtrPropertyClass inhr2 = a.ReadProperty(6).ToType<TPtrPropertyClass>();
    EXPECT_TRUE(inhr2);

    TPtrPropertyInher2 inhrCast = a.ReadProperty(6).ToType<TPtrPropertyInher2>();
    EXPECT_FALSE(inhrCast);//Need use VariableCastTo<TPtrPropertyInher2>

    TPtrPropertyInher2 inhrCast2 = VariableCastTo<TPtrPropertyInher2>(a.ReadProperty(6));
    EXPECT_TRUE(inhrCast2);
    EXPECT_EQ(inhrCast2->Name(), TString("classVar2"));
    EXPECT_EQ(inhrCast2->IntVar2(), 5);

    a.WriteProperty(6, PropertyClassToVariable(std::make_shared<TPropertyInher2>()));
    EXPECT_TRUE(a.ClassVar2());

    TPtrPropertyInher2 inhrCast3 = VariableCastTo<TPtrPropertyInher2>(a.ReadProperty(6));
    EXPECT_TRUE(inhrCast3);
    EXPECT_EQ(inhrCast3->Name(), TString(""));
    EXPECT_EQ(inhrCast3->IntVar2(), 0);

    //index
    EXPECT_EQ(a.CountChilds(), 0);
    EXPECT_EQ(a.CountInArray("childs"), 0);

    a.AddToArray("childs", PropertyClassToVariable(std::make_shared<TPropertyClass>()));

    EXPECT_EQ(a.CountChilds(), 1);
    EXPECT_EQ(a.CountInArray("childs"), 1);

    a.Child(0)->SetName("childName");
    TPtrPropertyClass child = VariableToPropClass(a.ReadFromArray("childs", 0));
    ASSERT_TRUE(child);
    EXPECT_EQ(child->Name(), TString("childName"));

    a.AddToArray(7, PropertyClassToVariable(std::make_shared<TPropertyInher2>()));
    EXPECT_EQ(a.CountInArray("childs"), 2);

    TPtrPropertyClass childInher = VariableToPropClass(a.ReadFromArray("childs", 1));
    EXPECT_TRUE(childInher);
    EXPECT_EQ(childInher->Name(), TString(""));
    EXPECT_EQ(childInher->CountProperty(), 2);

    childInher->WriteProperty("intVar2", TVariable(70));

    TPtrPropertyInher2 childInher2 = VariableCastTo<TPtrPropertyInher2>(a.ReadFromArray("childs", 1));
    EXPECT_TRUE(childInher2);
    EXPECT_EQ(childInher2->Name(), TString(""));
    EXPECT_EQ(childInher2->ReadProperty("intVar2").ToInt(), 70);

    a.DelFromArray("childs", PropertyClassToVariable(child));
    EXPECT_EQ(a.CountInArray("childs"), 1);

    a.DelFromArray("childs", PropertyClassToVariable(childInher));
    EXPECT_EQ(a.CountInArray("childs"), 0);
}

TEST(PropertyTest, Serialization)
{
    TPropertyInher a;
    a.SetName("Var a");
    a.SetIntVar(10);
    a.SetStringVar("hello");
    a.SetBoolVar(true);
    a.SetDoubleVar(4.5678);
    a.ClassVar()->SetName("classVarName");
    a.ClassVar2()->SetName("classVar2Name");
    a.ClassVar2()->SetIntVar2(30);
    auto childInher = std::make_shared<TPropertyInher2>();
    childInher->SetName("childInher");
    childInher->SetIntVar2(80);
    a.AddChild(childInher);
    a.AddChild(std::make_shared<TPropertyClass>())->SetName("childProp");


    TString xml = TSerialization().SaveTo(a);
    EXPECT_FALSE(xml.empty());
    EXPECT_TRUE(TSerialization().SaveToFile("a.xml", a));

    TPropertyInher b;
    TPropertyInher c;
    EXPECT_TRUE(TSerialization().LoadFrom(b, xml));
    EXPECT_TRUE(TSerialization().LoadFromFile("a.xml", c));

    EXPECT_EQ(b.Name(), TString("Var a"));
    EXPECT_EQ(b.IntVar(), 10);
    EXPECT_EQ(b.StringVar(), TString("hello"));
    EXPECT_EQ(b.BoolVar(), true);
    EXPECT_EQ(b.DoubleVar(), 4.5678);
    EXPECT_EQ(b.ClassVar()->Name(), TString("classVarName"));
    EXPECT_EQ(b.ClassVar2()->Name(), TString("classVar2Name"));
    EXPECT_EQ(b.ClassVar2()->IntVar2(), 30);
    ASSERT_EQ(b.CountChilds(), 2);
    EXPECT_EQ(b.Child(0)->Name(), TString("childInher"));
    EXPECT_EQ(b.Child(0)->ReadProperty("intVar2").ToInt(), 80);
    EXPECT_EQ(b.Child(1)->Name(), TString("childProp"));

    EXPECT_EQ(c.Name(), TString("Var a"));
    EXPECT_EQ(c.IntVar(), 10);
    EXPECT_EQ(c.StringVar(), TString("hello"));
    EXPECT_EQ(c.BoolVar(), true);
    EXPECT_EQ(c.DoubleVar(), 4.5678);
    EXPECT_EQ(c.ClassVar()->Name(), TString("classVarName"));
    EXPECT_EQ(c.ClassVar2()->Name(), TString("classVar2Name"));
    EXPECT_EQ(c.ClassVar2()->IntVar2(), 30);
    ASSERT_EQ(c.CountChilds(), 2);
    EXPECT_EQ(c.Child(0)->Name(), TString("childInher"));
    EXPECT_EQ(c.Child(0)->ReadProperty("intVar2").ToInt(), 80);
    EXPECT_EQ(c.Child(1)->Name(), TString("childProp"));
}

TEST(PropertyTest, Manager)
{
    TPropertyManager v;
    EXPECT_EQ(v.Type(), TString(""));
    EXPECT_FALSE(v.CreateTypeFun());
    EXPECT_EQ(v.CountProperty(), 0);
    EXPECT_EQ(v.IsInit(), false);
    EXPECT_EQ(v.IsValid(), false);

    TPropertyManager m("type", [](){ return std::make_shared<TPropertyClass>(); });

    EXPECT_EQ(m.Type(), TString("type"));
    EXPECT_TRUE(m.CreateTypeFun());
    EXPECT_EQ(m.CountProperty(), 0);
    EXPECT_EQ(m.IsInit(), false);

    EXPECT_EQ(TPropertyManager::Manager("type").Type(), TString("type"));
    EXPECT_FALSE(TPropertyManager::Manager("typeNot").IsValid());

    TPropInfo prop;
    EXPECT_EQ(prop.Name(), TString());
    EXPECT_EQ(prop.Type(), TString());
    EXPECT_EQ(prop.IsValid(), false);
    EXPECT_EQ(prop.IsReadOnly(), true);
    EXPECT_EQ(prop.IsStorable(), false);
    EXPECT_EQ(prop.IsLoadable(), false);
    EXPECT_EQ(prop.IsClass(), false);
    EXPECT_EQ(prop.IsArray(), false);

    TPropInfo& info = m.AddProperty("type", "prop");
    EXPECT_EQ(m.CountProperty(), 1);
    EXPECT_EQ(info.Name(), TString("prop"));
    EXPECT_EQ(info.Type(), TString("type"));
    EXPECT_EQ(info.IsValid(), false);
    EXPECT_EQ(info.IsReadOnly(), true);
    EXPECT_EQ(info.IsStorable(), false);
    EXPECT_EQ(info.IsLoadable(), false);
    EXPECT_EQ(info.IsClass(), false);
    EXPECT_EQ(info.IsArray(), false);

    info.Get(GetFun(&TPropertyInher::IntVar));
    EXPECT_EQ(info.IsValid(), true);
    EXPECT_EQ(info.IsReadOnly(), true);
    EXPECT_EQ(info.IsStorable(), true);
    EXPECT_EQ(info.IsLoadable(), false);
    EXPECT_EQ(info.IsClass(), false);
    EXPECT_EQ(info.IsArray(), false);

    info.Set(SetFun(&TPropertyInher::SetIntVar));
    EXPECT_EQ(info.IsValid(), true);
    EXPECT_EQ(info.IsReadOnly(), false);
    EXPECT_EQ(info.IsStorable(), true);
    EXPECT_EQ(info.IsLoadable(), true);
    EXPECT_EQ(info.IsClass(), false);
    EXPECT_EQ(info.IsArray(), false);

    TPropInfo& infoArray = m.AddProperty("TPropertyClass", "propArray", true);
    EXPECT_EQ(m.CountProperty(), 2);
    EXPECT_EQ(infoArray.Name(), TString("propArray"));
    EXPECT_EQ(infoArray.Type(), TString("TPropertyClass"));

    infoArray.GetArray(GetFun(&TPropertyInher::CountChilds), GetIndFun(&TPropertyInher::Child));
    EXPECT_EQ(infoArray.IsValid(), true);
    EXPECT_EQ(infoArray.IsReadOnly(), true);
    EXPECT_EQ(infoArray.IsStorable(), true);
    EXPECT_EQ(infoArray.IsLoadable(), false);
    EXPECT_EQ(infoArray.IsClass(), true);
    EXPECT_EQ(infoArray.IsArray(), true);

    infoArray.AddArray(SetFun(&TPropertyInher::AddChild));
    infoArray.DelArray(SetFun(&TPropertyInher::DelChild));
    EXPECT_EQ(infoArray.IsValid(), true);
    EXPECT_EQ(infoArray.IsReadOnly(), false);
    EXPECT_EQ(infoArray.IsStorable(), true);
    EXPECT_EQ(infoArray.IsLoadable(), true);
    EXPECT_EQ(infoArray.IsClass(), true);
    EXPECT_EQ(infoArray.IsArray(), true);
}

