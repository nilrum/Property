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

TEST(TestPropertyEditor, TestSetObject)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher2>();
    TPropertyEditor edit;
    edit.SetObject(obj);
    TObjTree & tree = edit.Tree();
    EXPECT_EQ(tree.CountProps(), 2);
    EXPECT_EQ(tree.Prop(0).IsProp(), true);
    EXPECT_EQ(tree.Prop(0).Name(), TString("name"));
    EXPECT_EQ(tree.Prop(0).Value().ToString(), TString());
    EXPECT_EQ(tree.Prop(1).Name(), TString("intVar2"));
    EXPECT_EQ(tree.Prop(1).Value().ToInt(), 0);

    EXPECT_FALSE(tree.IsLoaded());//объекты не загружены
    EXPECT_FALSE(tree.IsChilds());//объектов свойтсв нету
    EXPECT_EQ(edit.Tree().CountChilds(), 0);

    edit.Tree().Load();
    EXPECT_EQ(tree.CountProps(), 2);
    EXPECT_TRUE(tree.IsLoaded());//объекты загружены
    EXPECT_FALSE(tree.IsChilds());//объектов свойтсв нету
    EXPECT_EQ(tree.CountChilds(), 0);

    TPtrPropertyInher obj2 = std::make_shared<TPropertyInher>();
    edit.SetObject(obj2);

    EXPECT_EQ(edit.Tree().CountProps(), 5);
    EXPECT_FALSE(edit.Tree().IsLoaded());//объекты не загружены
    EXPECT_TRUE(edit.Tree().IsChilds());//объекты свойтсва есть и объекты есть
    EXPECT_EQ(edit.Tree().CountChilds(), 0);

    edit.Tree().Load();

    EXPECT_EQ(edit.Tree().CountProps(), 5);
    EXPECT_TRUE(edit.Tree().IsLoaded());//объекты загружены
    EXPECT_TRUE(edit.Tree().IsChilds());//объекты свойтсва есть
    EXPECT_EQ(edit.Tree().CountChilds(), 2);
    EXPECT_EQ(edit.Tree().Child(0).IsLoaded(), false);
    EXPECT_EQ(edit.Tree().Child(0).CountProps(), 1);
    EXPECT_EQ(edit.Tree().Child(1).IsLoaded(), false);
    EXPECT_EQ(edit.Tree().Child(1).CountProps(), 2);


    TPtrPropertyInher obj3 = std::make_shared<TPropertyInher>();
    edit.SetObject(obj3);
    obj3->SetClassVar(TPtrPropertyClass());
    obj3->SetClassVar2(TPtrPropertyInher2());

    EXPECT_EQ(tree.CountProps(), 5);
    EXPECT_FALSE(tree.IsLoaded());//объекты не загружены
    EXPECT_FALSE(tree.IsChilds());//объекты свойтсва есть но объектов в них нету
    EXPECT_EQ(tree.CountChilds(), 0);

    tree.Load();

    EXPECT_EQ(tree.CountProps(), 5);
    EXPECT_TRUE(tree.IsLoaded());//объекты загружены
    EXPECT_FALSE(tree.IsChilds());//объекты свойтсва есть но объектов в них нету
    EXPECT_EQ(tree.CountChilds(), 0);

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