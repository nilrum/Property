//
// Created by user on 30.09.2019.
//
#include <gtest/gtest.h>
#include <PropertyEditor.h>
#include "TestCasePropertyClass.h"

TEST(TestPropertyEditor, TestInit)
{
    TPropertyEditor edit;
    EXPECT_EQ(edit.IsAll(), true);
    EXPECT_EQ(edit.IsAllType(), true);
    EXPECT_EQ(edit.IsAllProperty(), true);
}

TEST(TestPropertyEditor, TestSetObjectSimple)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher2>();
    TPropertyEditor edit;
    edit.SetObject(obj);
    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.IndProp(), -1);
    EXPECT_EQ(tree.IsLoaded(), false);//объекты не загружены
    EXPECT_EQ(tree.IsChilds(), false);//объектов свойтсв нету
    EXPECT_EQ(tree.IsProp(), false);

    EXPECT_EQ(tree.Name(), TString("Name"));
    EXPECT_EQ(tree.Value().ToString(), TString("TPropertyInher2::"));
    EXPECT_EQ(tree.Parent(), nullptr);
    EXPECT_EQ(tree.Num(), -1);

    EXPECT_EQ(tree.CountProps(), 2);

    EXPECT_EQ(tree.Prop(0).IsProp(), true);
    EXPECT_EQ(tree.Prop(0).IndProp(), 0);
    EXPECT_EQ(tree.Prop(0).Name(), TString("name"));
    EXPECT_EQ(tree.Prop(0).Value().ToString(), TString());
    EXPECT_EQ(tree.Prop(0).Parent(), &tree);
    EXPECT_EQ(tree.Prop(0).Num(), -1);

    EXPECT_EQ(tree.Prop(1).IsProp(), true);
    EXPECT_EQ(tree.Prop(1).IndProp(), 1);
    EXPECT_EQ(tree.Prop(1).Name(), TString("intVar2"));
    EXPECT_EQ(tree.Prop(1).Value().ToInt(), 0);
    EXPECT_EQ(tree.Prop(1).Parent(), &tree);
    EXPECT_EQ(tree.Prop(1).Num(), -1);

    edit.Tree().Load();
    EXPECT_EQ(tree.CountProps(), 2);
    EXPECT_TRUE(tree.IsLoaded());//объекты загружены
    EXPECT_FALSE(tree.IsChilds());//объектов свойтсв нету
    EXPECT_EQ(tree.CountChilds(), 0);
}

TEST(TestPropertyEditor, TestSetObjectNoObjects)
{
    TPtrPropertyInher obj3 = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetObject(obj3);
    TObjTree &tree = edit.Tree();

    obj3->SetClassVar(TPtrPropertyClass());
    obj3->SetClassVar2(TPtrPropertyInher2());

    EXPECT_EQ(tree.CountProps(), 6);
    EXPECT_EQ(tree.IsLoaded(), false);//объекты не загружены
    EXPECT_EQ(tree.IsChilds(), false);//объекты свойтсва есть но объектов в них нету
    EXPECT_EQ(tree.CountChilds(), 0);

    tree.Load();

    EXPECT_EQ(tree.CountProps(), 6);
    EXPECT_TRUE(tree.IsLoaded());//объекты загружены
    EXPECT_FALSE(tree.IsChilds());//объекты свойтсва есть но объектов в них нету
    EXPECT_EQ(tree.CountChilds(), 0);
}

TEST(TestPropertyEditor, TestSetObject)
{
    TPtrPropertyInher obj = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetObject(obj);
    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.CountProps(), 6);
    EXPECT_EQ(tree.IsLoaded(), false);//объекты не загружены
    EXPECT_EQ(tree.IsChilds(), true);//объекты свойтсва есть и объекты есть
    EXPECT_EQ(tree.CountChilds(), 0);//но они пока не загружены

    edit.Tree().Load();

    EXPECT_EQ(tree.CountProps(), 6);
    EXPECT_EQ(tree.IsLoaded(), true);//объекты загружены
    EXPECT_EQ(tree.IsChilds(), true);//объекты свойтсва есть
    EXPECT_EQ(tree.CountChilds(), 2);

    EXPECT_EQ(tree.Child(0).IsProp(), false);
    EXPECT_EQ(tree.Child(0).IndProp(), 5);
    EXPECT_EQ(tree.Child(0).Name(), TString("classVar"));
    EXPECT_EQ(tree.Child(0).Value().ToString(), TString("TPropertyClass::"));
    EXPECT_EQ(tree.Child(0).Parent(), &tree);
    EXPECT_EQ(tree.Child(0).Num(), 0);

    EXPECT_EQ(tree.Child(1).IsProp(), false);
    EXPECT_EQ(tree.Child(1).IndProp(), 6);
    EXPECT_EQ(tree.Child(1).Name(), TString("classVar2"));
    EXPECT_EQ(tree.Child(1).Value().ToString(), TString("TPropertyInher2::"));
    EXPECT_EQ(tree.Child(1).Parent(), &tree);
    EXPECT_EQ(tree.Child(1).Num(), 1);

    obj->AddChild(std::make_shared<TPropertyClass>());
    edit.Tree().Load(true);
    EXPECT_EQ(tree.CountChilds(), 3);

    EXPECT_EQ(tree.Child(2).IsProp(), false);
    EXPECT_EQ(tree.Child(2).IndProp(), 7);
    EXPECT_EQ(tree.Child(2).Name(), TString("childs"));
    EXPECT_EQ(tree.Child(2).Value().ToString(), TString("TPropertyClass::"));
    EXPECT_EQ(tree.Child(2).Parent(), &tree);
    EXPECT_EQ(tree.Child(2).Num(), 2);
//тест на отображаемое имя
    obj->ClassVar()->SetName("NewNameClassVar");
    obj->ClassVar2()->SetName("NewNameClassVar2");
    obj->Child(0)->SetName("ChildName");

    EXPECT_EQ(tree.Child(0).Name(), TString("classVar"));//у объектов классов просто свойств имя не меняется
    EXPECT_EQ(tree.Child(0).Value().ToString(), TString("TPropertyClass::NewNameClassVar"));

    EXPECT_EQ(tree.Child(1).Name(), TString("classVar2"));
    EXPECT_EQ(tree.Child(1).Value().ToString(), TString("TPropertyInher2::NewNameClassVar2"));

    EXPECT_EQ(tree.Child(2).Name(), TString("ChildName"));//у объектов из списка имя меняется при его наличии
    EXPECT_EQ(tree.Child(2).Value().ToString(), TString("TPropertyClass::ChildName"));
}

TEST(TestPropertyEditor, TestSetProp)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher2>();
    TPropertyEditor edit;
    edit.SetObject(obj);
    TObjTree & tree = edit.Tree();
    EXPECT_EQ(tree.CountProps(), 2);

    tree.Prop(0).SetValue("New name");
    tree.Prop(1).SetValue(20);

    EXPECT_EQ(tree.Prop(0).Value().ToString(), TString("New name"));
    EXPECT_EQ(tree.Prop(1).Value().ToInt(), 20);
}

TEST(TestPropertyEditor, TestListArrayProps)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetObject(obj);

    TObjTree & tree = edit.Tree();
    TObjTree::TVectArrayInfo info = tree.ArrayInfo();
    EXPECT_EQ(info.size(), 1);
    EXPECT_EQ(std::get<0>(info[0]),  TString("childs"));
    EXPECT_EQ(std::get<1>(info[0]),  7);
}

TEST(TestPropertyEditor, TestNotAll)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher2>();
    TPropertyEditor edit;
    edit.SetIsAll(false);
    edit.SetObject(obj);
    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.IndProp(), -1);
    EXPECT_EQ(tree.IsLoaded(), false);//объекты не загружены
    EXPECT_EQ(tree.IsChilds(), false);//объектов свойтсв нету
    EXPECT_EQ(tree.IsProp(), false);

    EXPECT_EQ(tree.Name(), TString("Name"));
    EXPECT_EQ(tree.Value().ToString(), TString("TPropertyInher2::"));
    EXPECT_EQ(tree.Parent(), nullptr);
    EXPECT_EQ(tree.Num(), -1);

    EXPECT_EQ(tree.CountProps(), 0);

    edit.Info().AddProp("name");
    edit.SetObject(obj);
    EXPECT_EQ(tree.CountProps(), 1);
    EXPECT_EQ(tree.Prop(0).Name(), "name");

    edit.Info().AddType("TPropertyInher2", TShowKind::Select).AddProp("intVar2");
    edit.SetObject(obj);
    EXPECT_EQ(tree.CountProps(), 1);
    EXPECT_EQ(tree.Prop(0).Name(), "intVar2");

    edit.Clear();
    edit.SetIsAllProperty(true);
    edit.SetObject(obj);
    EXPECT_EQ(tree.CountProps(), 2);
}

TEST(TestPropertyEditor, TestPropString)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetIsAll(false);
    edit.Info().AddProps("stringVar:true, intVar:true");
    edit.SetObject(obj);
    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.CountProps(), 2);
    EXPECT_EQ(tree.Prop(0).Name(), "intVar");
    EXPECT_EQ(tree.Prop(1).Name(), "stringVar");

    edit.Clear();
    edit.Info().AddProps("stringVar:false, intVar");
    edit.SetObject(obj);
    EXPECT_EQ(tree.CountProps(), 1);
    EXPECT_EQ(tree.Prop(0).Name(), "intVar");
}