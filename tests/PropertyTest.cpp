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
    EXPECT_EQ(var.Type(), TVariableType::Ext);

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
    EXPECT_EQ(var.Type(), TVariableType::Ext);

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
    EXPECT_EQ(man.Property(0).IsPod(), true);
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

    EXPECT_EQ(a.CountProperty(), 9);

    EXPECT_EQ(a.IndexProperty("name"), 0);
    EXPECT_EQ(a.IndexProperty("intVar"), 1);
    EXPECT_EQ(a.IndexProperty("stringVar"), 2);
    EXPECT_EQ(a.IndexProperty("boolVar"), 3);
    EXPECT_EQ(a.IndexProperty("doubleVar"), 4);
    EXPECT_EQ(a.IndexProperty("enumVar"), 5);
    EXPECT_EQ(a.IndexProperty("classVar"), 6);
    EXPECT_EQ(a.IndexProperty("classVar2"), 7);
    EXPECT_EQ(a.IndexProperty("children"), 8);

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

    //enum
    a.WriteProperty(5, TVariable(teTwo));
    EXPECT_EQ(a.ReadProperty(5).ToEnum<TTestEnumInher>(), teTwo);

    TTestEnumInher enumVar = a.ReadProperty(5).ToEnum<TTestEnumInher>();
    EXPECT_EQ(enumVar, teTwo);

    TString enumStr = a.ReadProperty(5);
    EXPECT_EQ(enumStr, "teTwo");

    a.WriteProperty("enumVar", TVariable(teThree));
    EXPECT_EQ(a.ReadProperty("enumVar").ToEnum<TTestEnumInher>(), teThree);

    a.WriteProperty("enumVar", TVariable(0));
    EXPECT_EQ(a.ReadProperty("enumVar").ToEnum<TTestEnumInher>(), teOne);

    a.WriteProperty("enumVar", TVariable("teTwo"));
    EXPECT_EQ(a.ReadProperty("enumVar").ToEnum<TTestEnumInher>(), teTwo);

    //class TPropertyClass
    TPtrPropertyClass var = a.ReadProperty(6).ToType<TPtrPropertyClass>();
    EXPECT_TRUE(var);
    EXPECT_EQ(var->Name(), TString("classVar"));

    TPtrPropertyClass var2 = VariableToPropClass(a.ReadProperty(6));
    EXPECT_TRUE(var2);
    EXPECT_EQ(var2->Name(), TString("classVar"));

    a.WriteProperty(6, PropertyClassToVariable(std::make_shared<TPropertyClass>()));

    TPtrPropertyClass var3 = VariableToPropClass(a.ReadProperty(6));
    EXPECT_TRUE(var3);
    EXPECT_EQ(var3->Name(), TString(""));

    //class Inherited TPropertyClass
    TPtrPropertyClass inhr = VariableToPropClass(a.ReadProperty(7));
    EXPECT_TRUE(inhr);
    EXPECT_EQ(inhr->Name(), TString("classVar2"));
    EXPECT_EQ(inhr->ReadProperty("intVar2").ToInt(), 5);

    TPtrPropertyClass inhr2 = a.ReadProperty(7).ToType<TPtrPropertyClass>();
    EXPECT_TRUE(inhr2);

    TPtrPropertyInher2 inhrCast = a.ReadProperty(7).ToType<TPtrPropertyInher2>();
    EXPECT_FALSE(inhrCast);//Need use VariableCastTo<TPtrPropertyInher2>

    TPtrPropertyInher2 inhrCast2 = VariableCastTo<TPtrPropertyInher2>(a.ReadProperty(7));
    EXPECT_TRUE(inhrCast2);
    EXPECT_EQ(inhrCast2->Name(), TString("classVar2"));
    EXPECT_EQ(inhrCast2->IntVar2(), 5);

    a.WriteProperty(7, PropertyClassToVariable(std::make_shared<TPropertyInher2>()));
    EXPECT_TRUE(a.ClassVar2());

    TPtrPropertyInher2 inhrCast3 = VariableCastTo<TPtrPropertyInher2>(a.ReadProperty(7));
    EXPECT_TRUE(inhrCast3);
    EXPECT_EQ(inhrCast3->Name(), TString(""));
    EXPECT_EQ(inhrCast3->IntVar2(), 0);

    //index
    EXPECT_EQ(a.CountChildren(), 0);
    EXPECT_EQ(a.CountInArray("children"), 0);

    a.AddToArray("children", PropertyClassToVariable(std::make_shared<TPropertyClass>()));

    EXPECT_EQ(a.CountChildren(), 1);
    EXPECT_EQ(a.CountInArray("children"), 1);

    a.Child(0)->SetName("childName");
    TPtrPropertyClass child = VariableToPropClass(a.ReadFromArray("children", 0));
    ASSERT_TRUE(child);
    EXPECT_EQ(child->Name(), TString("childName"));

    a.AddToArray(8, PropertyClassToVariable(std::make_shared<TPropertyInher2>()));
    EXPECT_EQ(a.CountInArray("children"), 2);

    TPtrPropertyClass childInher = VariableToPropClass(a.ReadFromArray("children", 1));
    EXPECT_TRUE(childInher);
    EXPECT_EQ(childInher->Name(), TString(""));
    EXPECT_EQ(childInher->CountProperty(), 2);

    childInher->WriteProperty("intVar2", TVariable(70));

    TPtrPropertyInher2 childInher2 = VariableCastTo<TPtrPropertyInher2>(a.ReadFromArray("children", 1));
    EXPECT_TRUE(childInher2);
    EXPECT_EQ(childInher2->Name(), TString(""));
    EXPECT_EQ(childInher2->ReadProperty("intVar2").ToInt(), 70);

    a.DelFromArray("children", PropertyClassToVariable(child));
    EXPECT_EQ(a.CountInArray("children"), 1);

    a.DelFromArray("children", PropertyClassToVariable(childInher));
    EXPECT_EQ(a.CountInArray("children"), 0);
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

    TString fileName = "a.xml";
    TSerialization ser(TSerializationKind::skXml);
    TString data = ser.SaveTo(a);
    EXPECT_FALSE(data.empty());

    TPropertyInher b;
    EXPECT_TRUE(ser.LoadFrom(data, b));

    EXPECT_EQ(b.Name(), TString("Var a"));
    EXPECT_EQ(b.IntVar(), 10);
    EXPECT_EQ(b.StringVar(), TString("hello"));
    EXPECT_EQ(b.BoolVar(), true);
    EXPECT_EQ(b.DoubleVar(), 4.5678);
    EXPECT_EQ(b.ClassVar()->Name(), TString("classVarName"));
    EXPECT_EQ(b.ClassVar2()->Name(), TString("classVar2Name"));
    EXPECT_EQ(b.ClassVar2()->IntVar2(), 30);
    ASSERT_EQ(b.CountChildren(), 2);
    EXPECT_EQ(b.Child(0)->Name(), TString("childInher"));
    EXPECT_EQ(b.Child(0)->ReadProperty("intVar2").ToInt(), 80);
    EXPECT_EQ(b.Child(1)->Name(), TString("childProp"));

    EXPECT_TRUE(ser.SaveToFile(fileName, a));

    TPropertyInher c;
    EXPECT_TRUE(ser.LoadFromFile(fileName, c));

    EXPECT_EQ(c.Name(), TString("Var a"));
    EXPECT_EQ(c.IntVar(), 10);
    EXPECT_EQ(c.StringVar(), TString("hello"));
    EXPECT_EQ(c.BoolVar(), true);
    EXPECT_EQ(c.DoubleVar(), 4.5678);
    EXPECT_EQ(c.ClassVar()->Name(), TString("classVarName"));
    EXPECT_EQ(c.ClassVar2()->Name(), TString("classVar2Name"));
    EXPECT_EQ(c.ClassVar2()->IntVar2(), 30);
    ASSERT_EQ(c.CountChildren(), 2);
    EXPECT_EQ(c.Child(0)->Name(), TString("childInher"));
    EXPECT_EQ(c.Child(0)->ReadProperty("intVar2").ToInt(), 80);
    EXPECT_EQ(c.Child(1)->Name(), TString("childProp"));

    EXPECT_TRUE(ser.SavePropToFileName("prop_" + fileName, a, "children"));

    TPropertyInher d;
    EXPECT_TRUE(ser.LoadPropFromFileName("prop_" + fileName, d, "children"));

    ASSERT_EQ(d.CountChildren(), 2);
    EXPECT_EQ(d.Child(0)->Name(), TString("childInher"));
    EXPECT_EQ(d.Child(0)->ReadProperty("intVar2").ToInt(), 80);
    EXPECT_EQ(d.Child(1)->Name(), TString("childProp"));
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
    EXPECT_EQ(prop.IsPod(), true);
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

    infoArray.GetArray(GetFun(&TPropertyInher::CountChildren), GetIndFun(&TPropertyInher::Child));
    EXPECT_EQ(infoArray.IsValid(), true);
    EXPECT_EQ(infoArray.IsReadOnly(), true);
    EXPECT_EQ(infoArray.IsStorable(), true);
    EXPECT_EQ(infoArray.IsLoadable(), false);
    EXPECT_EQ(infoArray.IsClass(), false);
    EXPECT_EQ(infoArray.IsArray(), true);

    infoArray.AddArray(SetFun(&TPropertyInher::AddChild));
    infoArray.DelArray(SetFun(&TPropertyInher::DelChild));
    EXPECT_EQ(infoArray.IsValid(), true);
    EXPECT_EQ(infoArray.IsReadOnly(), false);
    EXPECT_EQ(infoArray.IsStorable(), true);
    EXPECT_EQ(infoArray.IsLoadable(), true);
    EXPECT_EQ(infoArray.IsClass(), false);
    EXPECT_EQ(infoArray.IsArray(), true);
}

class TPropertyClass2;

class TPropertyManager2{
public:
    TPropertyManager2() = default;
    TPropertyManager2(const TPropertyManager2& oth) = default;
    TPropertyManager2(TPropertyManager2&& oth) = default;

    TString Type() const { return type; }

    virtual TVariable ReadProperty(int index, const TPropertyClass2* obj) const = 0;
    virtual void WriteProperty(int index, TPropertyClass *obj, const TVariable &value) const = 0;

protected:
    TString type;
    int countBase = 0;
    std::vector<TPropInfo> infos;
};

class TPropertyClass2 : public std::enable_shared_from_this<TPropertyClass2>{
public:
    class TPropertyManager_TPropertyClass : public TPropertyManager2{
    public:
        TPropertyManager_TPropertyClass()
        {
            type = "TPropertyClass";
            infos = { TPropInfo("name", "TString", false) };
        }
        virtual TVariable ReadProperty(int index, const TPropertyClass2* obj) const
        {
            if(index == 0) return obj->Name();
            return TVariable();
        }
        virtual void WriteProperty(int index, TPropertyClass *obj, const TVariable &value) const
        {
            if(index == 0) obj->SetName(value);
        }
    };
    static TPropertyManager2& ManagerStatic() noexcept
    {
        static TPropertyManager_TPropertyClass manager;
        return manager;
    }

    virtual const TPropertyManager2& Manager() const
    {
        return ManagerStatic();
    }

    TVariable ReadProperty(int index) const
    {
        return Manager().ReadProperty(index, this);
    }

    TString Name() const { return name; }
    void SetName(const TString& value) { name = value; }
private:
    TString name;
};

class TPropertyInherNew : public TPropertyClass2{
public:
    class TPropertyManager_TPropertyInherNew : public TPropertyClass2::TPropertyManager_TPropertyClass {
    public:
        using TBase = TPropertyClass2::TPropertyManager_TPropertyClass;
        TPropertyManager_TPropertyInherNew():TBase()
        {
            type = "TPropertyInherNew";
            countBase = infos.size();
            infos.push_back(TPropInfo("intVar2", "int", false));
        }

        virtual TVariable ReadProperty(int index, const TPropertyClass2* obj) const
        {
            if(index >= countBase)
            {
                const TPropertyInherNew *objType = dynamic_cast<const TPropertyInherNew *>(obj);
                return objType->IntVar2();
            }
            return TBase::ReadProperty(index, obj);
        }
        virtual void WriteProperty(int index, TPropertyClass *obj, const TVariable &value) const
        {
            if(index >= countBase)
            {
                TPropertyInherNew *objType = dynamic_cast<TPropertyInherNew *>(obj);
                objType->SetIntVar2(value);
            }
            else
                TBase::WriteProperty(index, obj, value);
        }
    };

    static TPropertyManager_TPropertyInherNew& ManagerStatic() noexcept
    {
        static TPropertyManager_TPropertyInherNew manager;
        return manager;
    }

    virtual const TPropertyManager2& Manager() const
    {
        return ManagerStatic();
    }

    int IntVar2() const { return intVar2; }
    void SetIntVar2(int value) { intVar2 = value; }
protected:
    int intVar2 = 0;
};


TEST(TestSpeed, ReadPropertyesRaw)
{
    TPropertyInher2 obj;
    obj.SetName("Hello world");
    obj.SetIntVar2(10);

    EXPECT_EQ(obj.ReadProperty(1).ToInt(), 10);
    EXPECT_EQ(obj.ReadProperty(0).ToString(), TString("Hello world"));

    for(int i = 0; i < 10000; i++)
    {
        int intValue = TVariable(obj.IntVar2());
        TString strValue = TVariable(obj.Name());
    }
}

TEST(TestSpeed, ReadPropertyesDef)
{
    TPropertyInher2 obj;
    obj.SetName("Hello world");
    obj.SetIntVar2(10);

    EXPECT_EQ(obj.ReadProperty(1).ToInt(), 10);
    EXPECT_EQ(obj.ReadProperty(0).ToString(), TString("Hello world"));

    for(int i = 0; i < 10000; i++)
    {
        int intValue = obj.ReadProperty(1);
        TString strValue = obj.ReadProperty(0);
    }
}

TEST(TestSpeed, ReadPropertyesNew)
{
    TPropertyInherNew obj;
    obj.SetName("Hello world");
    obj.SetIntVar2(10);

    EXPECT_EQ(obj.ReadProperty(1).ToInt(), 10);
    EXPECT_EQ(obj.ReadProperty(0).ToString(), TString("Hello world"));

    for(int i = 0; i < 10000; i++)
    {
        int intValue = obj.ReadProperty(1);
        TString strValue = obj.ReadProperty(0);
    }
}

TEST(TestFormatDouble, LowDouble)
{
    TFormatDouble f;
    EXPECT_EQ(f.Round(10), "10");

    EXPECT_EQ(f.Round(10.4), "10");
    EXPECT_EQ(f.Round(10.5), "10.5");
    EXPECT_EQ(f.Round(10.9), "10.9");

    EXPECT_EQ(f.Round(10.93), "10.9");
    EXPECT_EQ(f.Round(10.95), "10.95");
    EXPECT_EQ(f.Round(10.99), "10.99");

    EXPECT_EQ(f.Round(10.991), "10.99");
    EXPECT_EQ(f.Round(10.995), "10.995");
    EXPECT_EQ(f.Round(10.998), "10.998");

    EXPECT_EQ(f.Round(10.9982), "10.998");
    EXPECT_EQ(f.Round(10.9985), "10.998");
    EXPECT_EQ(f.Round(10.9989), "10.998");

    EXPECT_EQ(f.Round(10.99891), "10.998");
    EXPECT_EQ(f.Round(10.99895), "10.998");
    EXPECT_EQ(f.Round(10.99899), "10.998");
}

TEST(TestFormatDouble, BigDouble)
{
    TFormatDouble f;
    EXPECT_EQ(f.Round(999999.888), "999999.888");
    EXPECT_EQ(f.Round(1000000.), "1×10⁶");
    EXPECT_EQ(f.Round(1000000.999), "1×10⁶");

    EXPECT_EQ(f.Round(1200000.), "1×10⁶");
    EXPECT_EQ(f.Round(1500000.), "1.5×10⁶");
    EXPECT_EQ(f.Round(1900000.), "1.9×10⁶");

    EXPECT_EQ(f.Round(8930000.), "8.9×10⁶");
    EXPECT_EQ(f.Round(8950000.), "8.95×10⁶");
    EXPECT_EQ(f.Round(8980000.), "8.98×10⁶");

    EXPECT_EQ(f.Round(8981000.), "8.98×10⁶");
    EXPECT_EQ(f.Round(8985000.), "8.985×10⁶");
    EXPECT_EQ(f.Round(8989000.), "8.989×10⁶");

    EXPECT_EQ(f.Round(8989822.), "8.989×10⁶");
    EXPECT_EQ(f.Round(8989856.), "8.989×10⁶");
    EXPECT_EQ(f.Round(8989999.), "8.989×10⁶");

    EXPECT_EQ(f.Round(10000000.), "1×10⁷");
    EXPECT_EQ(f.Round(18989822.), "1.898×10⁷");
    EXPECT_EQ(f.Round(28989856.), "2.898×10⁷");
    EXPECT_EQ(f.Round(79899989.), "7.989×10⁷");

    EXPECT_EQ(f.Round(100000000.), "1×10⁸");
}


enum class TTypeErrors{Ok, Err1, Err2};
REGISTER_CODE(TTypeErrors, Ok, "No errors")
REGISTER_CODE(TTypeErrors, Err1, "Text for Err1")

TEST(Result, Init)
{
    TResult r;
    EXPECT_TRUE(r.IsNoError());
    EXPECT_TRUE(r.Is(0));

    TResult r2(TTypeErrors::Ok);
    EXPECT_TRUE(r2.IsNoError());
    EXPECT_FALSE(r2.Is(0));
    EXPECT_TRUE(r2.Is(TTypeErrors::Ok));

    TResult r3(TTypeErrors::Err1);
    EXPECT_FALSE(r3.IsNoError());
    EXPECT_FALSE(r3.Is(0));
    EXPECT_TRUE(r3.Is(TTypeErrors::Err1));

    EXPECT_EQ(TResult::TextError(r3), "Text for Err1");

    TResult i(3);
    EXPECT_EQ(i.Code(), 3);
    EXPECT_FALSE(i.IsNoError());
}
