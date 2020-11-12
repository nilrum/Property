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
    EXPECT_EQ(edit.IsEdit(), true);
    EXPECT_TRUE(edit.Obj().expired());
    EXPECT_EQ(edit.LockObj(), TPtrPropertyClass());

    TCustClass& cust = edit.ClassCustoms();
    EXPECT_EQ(cust.ShowClasses(), TShowClass::All);
    EXPECT_EQ(cust.ShowProperty(), TShowProp::All);
    EXPECT_EQ(cust.ValueClassProperty(), TString("name"));
    EXPECT_EQ(cust.CountTypes(), 0);
    EXPECT_EQ(cust.CountProps(), 0);
    EXPECT_FALSE(cust.CheckPropFun());

    TObjTree& tree = edit.Tree();

    EXPECT_EQ(tree.IndProp(), -1);
    EXPECT_TRUE(tree.Obj().expired());
    EXPECT_EQ(tree.CountItems(), 0);
    EXPECT_EQ(tree.Name(), TString());
    EXPECT_EQ(tree.Value(false).ToString(), TString());
    EXPECT_EQ(tree.IsLoaded(), false);
    EXPECT_EQ(tree.HasChildren(), false);
    EXPECT_EQ(tree.IsProp(), false);
    EXPECT_EQ(tree.IsCheckable(), false);
    EXPECT_EQ(tree.IsColor(), false);
    EXPECT_EQ(tree.IsBool(), false);
    EXPECT_EQ(tree.IsEditable(), false);
    EXPECT_EQ(tree.IsChecked(), false);
    EXPECT_TRUE(tree.Parent().expired());
    EXPECT_EQ(tree.Num(), -1);
}

TEST(TestPropertyEditor, TestSetObjectSimple)
{
    TPropertyEditor edit;
    TObjTree &tree = edit.Tree();
    {
        TPtrPropertyClass obj = std::make_shared<TPropertyInher2>();
        edit.SetObject(obj);

        //корневой объект
        EXPECT_EQ(tree.IndProp(), -1);
        EXPECT_EQ(tree.Obj().expired(), false);
        EXPECT_EQ(tree.LockObj(), obj);
        EXPECT_EQ(tree.CountItems(false), 0);
        EXPECT_EQ(tree.Parent().expired(), true);
        EXPECT_EQ(tree.Num(), -1);

        EXPECT_EQ(tree.Name(), TString("Name"));
        EXPECT_EQ(tree.Value().ToString(), TString("TPropertyInher2::"));

        EXPECT_EQ(tree.IsLoaded(), false);
        EXPECT_EQ(tree.HasChildren(), true);//возвращает есть вложенные элементы даже если они не загружены

        EXPECT_EQ(tree.IsProp(), false);
        EXPECT_EQ(tree.IsColor(), false);
        EXPECT_EQ(tree.IsBool(), false);
        EXPECT_EQ(tree.IsEditable(), false);
        EXPECT_EQ(tree.IsCheckable(), false);
        EXPECT_EQ(tree.IsChecked(), false);

        EXPECT_EQ(tree.CountItems(), 2);//если не указать автоматически загружает все элементы
        EXPECT_TRUE(tree.IsLoaded());//объекты загружены
        EXPECT_TRUE(tree.HasChildren());

        //первое свойство
        EXPECT_EQ(tree.Item(0)->IndProp(), 0);
        EXPECT_EQ(tree.Item(0)->Obj().expired(), false);
        EXPECT_EQ(tree.Item(0)->LockObj(), obj);
        EXPECT_EQ(tree.Item(0)->CountItems(false), 0);
        EXPECT_EQ(tree.Item(0)->Parent().expired(), false);
        EXPECT_EQ(tree.Item(0)->LockParent(), tree.shared_from_this());
        EXPECT_EQ(tree.Item(0)->Num(), 0);

        EXPECT_EQ(tree.Item(0)->Name(), TString("name"));
        EXPECT_EQ(tree.Item(0)->Value().ToString(), TString());

        EXPECT_EQ(tree.Item(0)->IsLoaded(), false);
        EXPECT_EQ(tree.Item(0)->HasChildren(), false);

        EXPECT_EQ(tree.Item(0)->IsProp(), true);
        EXPECT_EQ(tree.Item(0)->IsColor(), false);
        EXPECT_EQ(tree.Item(0)->IsBool(), false);
        EXPECT_EQ(tree.Item(0)->IsEditable(), true);
        EXPECT_EQ(tree.Item(0)->IsCheckable(), false);
        EXPECT_EQ(tree.Item(0)->IsChecked(), false);

        //второе свойство
        EXPECT_EQ(tree.Item(1)->IndProp(), 1);
        EXPECT_EQ(tree.Item(1)->Obj().expired(), false);
        EXPECT_EQ(tree.Item(1)->LockObj(), obj);
        EXPECT_EQ(tree.Item(1)->CountItems(false), 0);
        EXPECT_EQ(tree.Item(1)->Parent().expired(), false);
        EXPECT_EQ(tree.Item(1)->LockParent(), tree.shared_from_this());
        EXPECT_EQ(tree.Item(1)->Num(), 1);

        EXPECT_EQ(tree.Item(1)->Name(), TString("intVar2"));
        EXPECT_EQ(tree.Item(1)->Value().ToString(), TString("0"));

        EXPECT_EQ(tree.Item(1)->IsLoaded(), false);
        EXPECT_EQ(tree.Item(1)->HasChildren(), false);

        EXPECT_EQ(tree.Item(1)->IsProp(), true);
        EXPECT_EQ(tree.Item(1)->IsColor(), false);
        EXPECT_EQ(tree.Item(1)->IsBool(), false);
        EXPECT_EQ(tree.Item(1)->IsEditable(), true);
        EXPECT_EQ(tree.Item(1)->IsCheckable(), false);
        EXPECT_EQ(tree.Item(1)->IsChecked(), false);
    }

    //корневой объект
    EXPECT_EQ(tree.IndProp(), -1);
    EXPECT_EQ(tree.Obj().expired(), true);
    EXPECT_EQ(tree.LockObj(), TPtrPropertyClass());
    EXPECT_EQ(tree.CountItems(), 0);
    EXPECT_EQ(tree.Parent().expired(), true);
    EXPECT_EQ(tree.Num(), -1);

    EXPECT_EQ(tree.Name(), TString());
    EXPECT_EQ(tree.Value().ToString(), TString());

    EXPECT_EQ(tree.IsLoaded(), false);
    EXPECT_EQ(tree.HasChildren(), false);

    EXPECT_EQ(tree.IsProp(), false);
    EXPECT_EQ(tree.IsColor(), false);
    EXPECT_EQ(tree.IsBool(), false);
    EXPECT_EQ(tree.IsEditable(), false);
    EXPECT_EQ(tree.IsCheckable(), false);
    EXPECT_EQ(tree.IsChecked(), false);
}

TEST(TestPropertyEditor, TestSetObjectNoObjects)
{
    TPtrPropertyInher obj3 = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetObject(obj3);
    TObjTree &tree = edit.Tree();

    obj3->SetClassVar(TPtrPropertyClass());
    obj3->SetClassVar2(TPtrPropertyInher2());

    EXPECT_EQ(tree.CountItems(false), 0);
    EXPECT_EQ(tree.IsLoaded(), false);//объекты не загружены
    EXPECT_EQ(tree.HasChildren(), true);

    EXPECT_EQ(tree.CountItems(), 6);
    EXPECT_EQ(tree.IsLoaded(), true);//объекты загружены
    EXPECT_EQ(tree.HasChildren(), true);
}

TEST(TestPropertyEditor, TestSetObject)
{
    TPtrPropertyInher obj = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetObject(obj);
    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.CountItems(false), 0);
    EXPECT_EQ(tree.IsLoaded(), false);//объекты не загружены
    EXPECT_EQ(tree.HasChildren(), true);//объекты свойтсва есть и объекты есть

    EXPECT_EQ(tree.CountItems(), 8);// т.к. массив свойство children пустое
    EXPECT_EQ(tree.IsLoaded(), true);//объекты загружены
    EXPECT_EQ(tree.HasChildren(), true);//объекты свойтсва есть

    EXPECT_EQ(tree.Item(6)->IsProp(), false);
    EXPECT_EQ(tree.Item(6)->IndProp(), 6);
    EXPECT_EQ(tree.Item(6)->Name(), TString("classVar"));
    EXPECT_EQ(tree.Item(6)->Value().ToString(), TString("TPropertyClass::"));
    EXPECT_EQ(tree.Item(6)->LockParent(), tree.shared_from_this());
    EXPECT_EQ(tree.Item(6)->Num(), 6);

    EXPECT_EQ(tree.Item(7)->IsProp(), false);
    EXPECT_EQ(tree.Item(7)->IndProp(), 7);
    EXPECT_EQ(tree.Item(7)->Name(), TString("classVar2"));
    EXPECT_EQ(tree.Item(7)->Value().ToString(), TString("TPropertyInher2::"));
    EXPECT_EQ(tree.Item(7)->LockParent(), tree.shared_from_this());
    EXPECT_EQ(tree.Item(7)->Num(), 7);

    obj->AddChild(std::make_shared<TPropertyClass>());

    EXPECT_EQ(tree.CountItems(), 9);

    EXPECT_EQ(tree.Item(8)->IsProp(), false);
    EXPECT_EQ(tree.Item(8)->IndProp(), 8);
    EXPECT_EQ(tree.Item(8)->Name(), TString("children"));
    EXPECT_EQ(tree.Item(8)->Value().ToString(), TString("TPropertyClass::"));
    EXPECT_EQ(tree.Item(8)->LockParent(), tree.shared_from_this());
    EXPECT_EQ(tree.Item(8)->Num(), 8);

    obj->AddChild(std::make_shared<TPropertyClass>());

    EXPECT_EQ(tree.CountItems(), 10);

    EXPECT_EQ(tree.Item(9)->IsProp(), false);
    EXPECT_EQ(tree.Item(9)->IndProp(), 8);
    EXPECT_EQ(tree.Item(9)->Name(), TString("children"));
    EXPECT_EQ(tree.Item(9)->Value().ToString(), TString("TPropertyClass::"));
    EXPECT_EQ(tree.Item(9)->LockParent(), tree.shared_from_this());
    EXPECT_EQ(tree.Item(9)->Num(), 9);

//тест на отображаемое имя
    obj->ClassVar()->SetName("NewNameClassVar");
    obj->ClassVar2()->SetName("NewNameClassVar2");
    obj->Child(0)->SetName("ChildName");

    EXPECT_EQ(tree.Item(6)->Name(), TString("classVar"));//у объектов классов просто свойств имя не меняется
    EXPECT_EQ(tree.Item(6)->Value().ToString(), TString("TPropertyClass::NewNameClassVar"));

    EXPECT_EQ(tree.Item(7)->Name(), TString("classVar2"));
    EXPECT_EQ(tree.Item(7)->Value().ToString(), TString("TPropertyInher2::NewNameClassVar2"));

    EXPECT_EQ(tree.Item(8)->Name(), TString("ChildName"));//у объектов из списка имя меняется при его наличии
    EXPECT_EQ(tree.Item(8)->Value().ToString(), TString("TPropertyClass::ChildName"));
}

TEST(TestPropertyEditor, TestSetProp)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher2>();
    TPropertyEditor edit;
    edit.SetObject(obj);
    TObjTree & tree = edit.Tree();
    EXPECT_EQ(tree.CountItems(), 2);

    tree.Item(0)->SetValue("New name");
    tree.Item(1)->SetValue(20);

    EXPECT_EQ(tree.Item(0)->Value().ToString(), TString("New name"));
    EXPECT_EQ(tree.Item(1)->Value().ToInt(), 20);
}

TEST(TestPropertyEditor, TestListArrayProps)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetObject(obj);

    TObjTree & tree = edit.Tree();
    TObjTree::TVectArrayInfo info = tree.ArrayInfo();
    EXPECT_EQ(info.size(), 1);
    EXPECT_EQ(std::get<0>(info[0]),  TString("children"));
    EXPECT_EQ(std::get<1>(info[0]),  8);
}

TEST(TestPropertyEditor, TestNotAll)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher2>();
    TPropertyEditor edit;
    edit.SetIsAll(false);
    edit.SetObject(obj);
    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.CountItems(), 0);
    EXPECT_EQ(tree.IndProp(), -1);
    EXPECT_EQ(tree.IsLoaded(), true);//объекты не загружены
    EXPECT_EQ(tree.HasChildren(), false);
    EXPECT_EQ(tree.IsProp(), false);

    EXPECT_EQ(tree.Name(), TString("Name"));
    EXPECT_EQ(tree.Value().ToString(), TString("TPropertyInher2::"));
    EXPECT_EQ(tree.Parent().expired(), true);
    EXPECT_EQ(tree.Num(), -1);

    edit.ClassCustoms().AddProp("name");
    edit.Tree().RescanItems();//вариант с Rescan
    EXPECT_EQ(tree.CountItems(), 1);
    EXPECT_EQ(tree.Item(0)->Name(), "name");

    edit.ClassCustoms().AddType("TPropertyInher2", TShowProp::Select).AddProp("intVar2");
    edit.SetObject(obj);
    EXPECT_EQ(tree.CountItems(), 1);
    EXPECT_EQ(tree.Item(0)->Name(), "intVar2");

    edit.Clear();
    edit.SetIsAllProperty(true);
    edit.SetObject(obj);
    EXPECT_EQ(tree.CountItems(), 2);
}

TEST(TestPropertyEditor, TestPropString)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.SetIsAll(false);
    edit.ClassCustoms().AddProps("stringVar:true, intVar:true");
    edit.SetObject(obj);
    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.CountItems(), 2);
    EXPECT_EQ(tree.Item(0)->Name(), "intVar");
    EXPECT_EQ(tree.Item(1)->Name(), "stringVar");

    edit.Clear();
    edit.ClassCustoms().AddProps("stringVar:false, intVar");
    edit.SetObject(obj);
    EXPECT_EQ(tree.CountItems(), 1);
    EXPECT_EQ(tree.Item(0)->Name(), "intVar");
}

TEST(TestPropertyEditor, TestClassCustom)
{
    TPtrPropertyClass obj = std::make_shared<TPropertyInher>();
    TPropertyEditor edit;
    edit.ClassCustoms().SetShowClasses(TShowClass::All);
    auto& cust = edit.ClassCustoms().AddType(TPropertyInher::TypeStatic(), TShowProp::SelTrue);
    cust.AddProps("name:false,intVar:false");
    edit.ClassCustoms().AddType(TPropertyInher2::TypeStatic(), TShowProp::All);
    edit.SetObject(obj);

    TObjTree &tree = edit.Tree();

    EXPECT_EQ(tree.Value(false).ToString(), TString());
    EXPECT_EQ(tree.CountItems(), 6);
    EXPECT_EQ(tree.Item(0)->Name(), "stringVar");
    EXPECT_EQ(tree.Item(1)->Name(), "boolVar");
    EXPECT_EQ(tree.Item(2)->Name(), "doubleVar");
    EXPECT_EQ(tree.Item(3)->Name(), "enumVar");

    edit.ClassCustoms().SetValueClassProperty("intVar");
    edit.SetObject(obj);
    EXPECT_EQ(tree.Value(false).ToString(), TString("0"));


}