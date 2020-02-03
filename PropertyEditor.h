//
// Created by user on 30.09.2019.
//

#ifndef TESTAPP_PROPERTYEDITOR_H
#define TESTAPP_PROPERTYEDITOR_H

#include <deque>
#include "PropertyClass.h"

template<class T>
class TPtrVector{
public:
    using value_type = T;
    using iterator = T*;
    ~TPtrVector()
    {
        clear();
    }

    T& operator[] (int index){ *data[index]; }
    const T& operator[] (int index) const { *data[index]; }
    size_t size() const { return data.size(); }

    void push_back(T* value)
    {
        data.push_back(value);
    }
    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        data.push_back(new T(args...));
    }
    void clear()
    {
        for(int i = 0; i < data.size(); i++)
            delete data[i];
        data.clear();
    }

    auto begin()
    {
        return &data[0];
    }
    auto end()
    {
        return (&data.back()) + 1;
    }

    void erase(iterator value)
    {
        T* val = *value;
        data.erase(value);
        delete val;
    }
private:
    std::vector<T*> data;
};

class TObjTree{
public:
    TObjTree(const TPtrPropertyClass& value = TPtrPropertyClass(), TObjTree* parent = nullptr, int indProp = -1);
    void Clear();//очищаем полностью объект
    void ClearChilds();//очищаем объекты владения

    void SetObj(const TPtrPropertyClass& value);
    const TPtrPropertyClass& Obj() const;
    int IndProp() const;

    void Load();

    bool IsLoaded() const;
    bool IsChilds() const;//отображает может ли быть объекты в childs
    bool IsProp() const;    //отображает это свойство или класс

    size_t CountProps() const;
    TObjTree& Prop(int index);

    size_t CountChilds() const;
    TObjTree& Child(int index);
    const TObjTree& Child(int index) const;
    void AddChild(TPtrPropertyClass value, int indProp);
    void DelChild(int index);
    void DelChild(TPtrPropertyClass value, int indProp);

    int LoadedCount();

    TString Name() const;
    TVariable Value() const;
    void SetValue(const TVariable& value);

    TObjTree* Parent();
    int Num() const;

    using TArrayInfo = std::tuple<TString, int>;
    using TVectArrayInfo = std::vector<TArrayInfo>;
    TVectArrayInfo ArrayInfo() const;

    using TVecObjTree = std::deque<TObjTree>;
private:
    TObjTree* parent = nullptr;
    TPtrPropertyClass obj;
    int indProp = -1;

    TVecObjTree props;
    TVecObjTree childs;
    bool isLoaded = false;

    bool HasChild(const TVariable& value) const;
    bool HasChild(const TPtrPropertyClass& value) const;

    bool CheckType(const TVariable& value) const;
    bool CheckType(const TPtrPropertyClass& value) const;

    bool CheckProp(const TPropInfo& vale) const;
};

class TPropertyEditor{
public:
    TPropertyEditor() = default;

    virtual void SetObject(TPtrPropertyClass value);

    TPropertyEditor& SetIsAll(bool value = true);
    bool IsAll() const;

    TPropertyEditor& SetIsAllType(bool value = true);
    bool IsAllType() const;

    TPropertyEditor& SetIsAllProperty(bool value = true);
    bool IsAllProperty() const;

    TPropertyEditor& SetIsEdit(bool value = true);
    bool IsEdit() const;

    TObjTree& Tree();

protected:
    TPtrPropertyClass obj;
    bool isAllType = true;
    bool isAllProperty = true;
    bool isEdit = true;
    TObjTree tree;
};

/*class TEditorView : public TViewBase{
public:
    virtual TRezult ShowFor(TPtrPropertyClass value){ return TRezult(); };
    PROPERTIES(TEditorView, TViewBase,)
};*/


#endif //TESTAPP_PROPERTYEDITOR_H
