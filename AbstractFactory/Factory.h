#pragma once

#include "PatrialOrderer.h"
#include "TypeList.h"
//////////////////////////
template <typename ...T>
struct CanCreate{

};

template <typename T>
struct CanCreate<T>{
private:

    //используем SFINAE, чтобы определить, имеет ли тип консьруктор по умолчанию

    template <class X, class = decltype(X())>
    static constexpr bool test(X*){
        return true;
    };

    template <class X>
    static constexpr bool test(...){
        return false;
    };

public:
    static constexpr bool res = test<T>(0);
};

/////////////////////////
//Возврашает указатель на созданный через new объект, если тип EmptyList, EmptyType возвразает nullptr
template <typename Base, typename T>
struct CreateObject{
    static Base* create(){
        return new T();
    }
};

template <typename Base>
struct CreateObject<Base, EmptyType>{
    static Base* create(){
        return nullptr;
    }
};
template <typename Base>
struct CreateObject<Base, EmptyList>{
    static Base* create(){
        return nullptr;
    }
};
////////////

//Структура, которая содержит в себе функции по созданию наследников или же самой базы
//N отвечает за номер элемента в списке баз, который создает текущий класс
//Используется наследование от класса создающий, предыдущую базу из списка баз, чтобы покрыть все типы из списка баз
//При N = 0 наследуется от предыдущего списка баз, таким образом получается цепочка наследований, которая помогает
// правильно определить виртуальные функции
//Если среди наследников нет наследника текущей базы или же у него нет конструктора, то создается функция от EmptyType*,
// таким образом не создается перегрузака для текущей базы, то есть при вызове будет использоватся реализация базы
template <int N, typename ...T>
struct ObjectCreator{};

template <int N, typename AbstractBase, typename ...AbstractProductsList, typename ... BaseList, typename ...DerivedList, typename ... Bases>
struct ObjectCreator<N, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>>
        :  virtual public ObjectCreator<N-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>>{

private:
    using BaseObjectCreator = ObjectCreator<N-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>>;

    using ProductList = TypeList<AbstractProductsList...>;
    using CreatingProductList = TypeList<DerivedList...>;
    using CurBaseCreatingTypeCandidate = typename Get<N, ProductList>::res;
    using CurHasDerivedInList = HasDerivedInList<CreatingProductList, CurBaseCreatingTypeCandidate>;
    using CurCreatingTypeCandidate = typename CurHasDerivedInList::Derived;
    constexpr static bool canCreate = CanCreate<CurCreatingTypeCandidate>::res;
    using CurCreatingType = typename Select<canCreate, CurCreatingTypeCandidate, EmptyType>::res;
    using CurBaseCreatingType = typename Select<canCreate && CurHasDerivedInList::hasDerived, CurBaseCreatingTypeCandidate, EmptyType>::res;
public:

    using BaseObjectCreator::createObj;
    virtual CurBaseCreatingType* createObj(CurBaseCreatingType* p) {
        return CreateObject<CurBaseCreatingType, CurCreatingType>::create();
    }
};

template <typename AbstractBase, typename ...AbstractProductsList, typename ... BaseList, typename ...DerivedList, typename ... Bases>
struct ObjectCreator<0, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>>
: virtual public ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, typename TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>::NextListHolder>{

private:
    using BaseObjectCreator = ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, typename TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>::NextListHolder>;

    using ProductList = TypeList<AbstractProductsList...>;
    using CreatingProductList = TypeList<DerivedList...>;
    using CurBaseCreatingTypeCandidate = typename Get<0, ProductList>::res;
    using CurHasDerivedInList = HasDerivedInList<CreatingProductList, CurBaseCreatingTypeCandidate>;

    using CurCreatingTypeCandidate = typename CurHasDerivedInList::Derived;
    constexpr static bool canCreate = CanCreate<CurCreatingTypeCandidate>::res;
    using CurCreatingType = typename Select<canCreate, CurCreatingTypeCandidate, EmptyType>::res;

    using CurBaseCreatingType = typename Select<canCreate && CurHasDerivedInList::hasDerived, CurBaseCreatingTypeCandidate, EmptyType>::res;

public:
    using BaseObjectCreator::createObj;

    virtual CurBaseCreatingType* createObj(CurBaseCreatingType* p) {
        return CreateObject<CurBaseCreatingType, CurCreatingType>::create();
    }
};


//используется для списка баз, поэтому отдельно <table, sofa...>
template <int N, typename AbstractBase,typename ...AbstractProductsList, typename ... BaseList, typename ...DerivedList>
struct ObjectCreator<N, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>>>
        : virtual public ObjectCreator<N-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, TypeListHolder<TypeList<BaseList...>>>{

private:
    using BaseObjectCreator = ObjectCreator<N-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, TypeListHolder<TypeList<BaseList...>>>;

    using ProductList = TypeList<AbstractProductsList...>;
    using CreatingProductList = TypeList<DerivedList...>;
    using CurBaseCreatingType = typename Get<N, ProductList>::res;
    using CurCreatingTypeCandidate = CurBaseCreatingType;
    constexpr static bool canCreate = CanCreate<CurCreatingTypeCandidate>::res;
    using CurCreatingType = typename Select<canCreate, CurCreatingTypeCandidate, EmptyType>::res;

public:
    using BaseObjectCreator::createObj;

    virtual CurBaseCreatingType* createObj(CurBaseCreatingType* p) {
        return CreateObject<CurBaseCreatingType, CurCreatingType>::create();
    }
};

template <typename AbstractBase,typename ...AbstractProductsList, typename ... BaseList, typename ...DerivedList>
struct ObjectCreator<0, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>>>
        : virtual public ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, typename TypeListHolder<TypeList<BaseList...>>::NextListHolder>{

private:
    using BaseObjectCreator = ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, typename TypeListHolder<TypeList<BaseList...>>::NextListHolder>;

    using ProductList = TypeList<AbstractProductsList...>;
    using CreatingProductList = TypeList<DerivedList...>;
    using CurBaseCreatingType = typename Get<0, ProductList>::res;
    using CurCreatingTypeCandidate = CurBaseCreatingType;
    constexpr static bool canCreate = CanCreate<CurCreatingTypeCandidate>::res;
    using CurCreatingType = typename Select<canCreate, CurCreatingTypeCandidate, EmptyType>::res;

public:
    using BaseObjectCreator::createObj;

    virtual CurBaseCreatingType* createObj(CurBaseCreatingType* p) {
        return CreateObject<CurBaseCreatingType, CurCreatingType>::create();
    }
};


//от Product, тоже отдельно
template <int N, typename AbstractBase, typename ...AbstractProductsList, class Derived>
struct ObjectCreator<N, AbstractBase, TypeList<AbstractProductsList...>, TypeList<Derived>, EmptyList>
        : virtual public ObjectCreator<N-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<Derived>, EmptyList>{

private:
    using BaseObjectCreator = ObjectCreator<N-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<Derived>, EmptyList>;

    using ProductList = TypeList<AbstractProductsList...>;
    using CurBaseCreatingType = typename Get<N, ProductList>::res;

public:
    using BaseObjectCreator::createObj;
//странно, если вызовется этот метод
    virtual CurBaseCreatingType* createObj(CurBaseCreatingType* p) = 0;
};

template <typename AbstractBase, typename ...AbstractProductsList, class Derived>
struct ObjectCreator<0, AbstractBase, TypeList<AbstractProductsList...>, TypeList<Derived>, EmptyList> :
        virtual public ObjectCreator<0, AbstractBase>{

private:

    using ProductList = TypeList<AbstractProductsList...>;
    using CurBaseCreatingType = typename Get<0, ProductList>::res;

public:
    using ObjectCreator<0, AbstractBase>::createObj;
//чисто вирт. метод
    virtual CurBaseCreatingType* createObj(CurBaseCreatingType* p) = 0;

};
//чтобы была общая база
template <typename AbstractBase>
struct ObjectCreator<0, AbstractBase>{

public:
    virtual AbstractBase* createObj(){
        return nullptr;
    }
};


//////////////////////////////////////
//сама фабрика
//передается текущие изделия(русские деревянные), и все базы(деревянные изделия, просто изделия, и абстрактоное изделие)
//Благодаря этому можно отнаследоватся от базы, тем самым испанские деревянные изделия отнаследуются от такой же базы,
// и получистя древовидная структура
//Так же наследуемся от ObjectCreator,чтобы были соответсвующие вирт. функции по созданию объектов
template <typename ...T11>
struct Factory{
    using T = EmptyType;
};


template <typename AbstractBase, typename ...AbstractProductsList, typename ... BaseList, typename ...DerivedList, typename ... Bases>
struct Factory<AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>>
        : virtual public Factory<AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, typename TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>::NextListHolder>,
          virtual public ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>> {

public:
    using T = TypeList<DerivedList...>;
    using B = TypeList<BaseList...>;

private:
    using CurObjectCreator = ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>>;
    using CurObjectCreator::createObj;


    using ProductList = TypeList<AbstractProductsList...>;

    using BaseListHolder = TypeListHolder<TypeList<BaseList...>, TypeListHolder<Bases...>>;
    using BaseFactory = Factory<AbstractBase, ProductList, B, typename BaseListHolder::NextListHolder>;

    template <typename Product>
    using CurHasDerived = HasDerivedInList<T, Product>;

    template <typename Product>
    constexpr static bool canCreate = HasDerivedInList<T, Product>::hasDerived && CanCreate<typename HasDerivedInList<T, Product>::Derived>::res;

    template <typename Product>
    using GetProductType = typename Select<canCreate<Product>, typename CurHasDerived<Product>::Derived, typename BaseFactory::template GetProductType<Product>>::res;

public:
    template <typename Product>
    Product* Get(){
        Product* p = 0;
        return createObj(p);
    }

};

//от <sofa, table, chair>
template <typename AbstractBase,typename ...AbstractProductsList, typename ... BaseList, typename ...DerivedList>
struct Factory<AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>>>
        : virtual public Factory<AbstractBase, TypeList<AbstractProductsList...>, TypeList<BaseList...>, typename TypeListHolder<TypeList<BaseList...>>::NextListHolder>,
          virtual public ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>>> {

public:
    using T = TypeList<DerivedList...>;
    using B = TypeList<BaseList...>;

private:
    using CurObjectCreator = ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<DerivedList...>, TypeListHolder<TypeList<BaseList...>>>;

    using BaseListHolder = TypeListHolder<TypeList<BaseList...>>;
    using ProductList  = TypeList<AbstractProductsList...>;
    using BaseFactory = Factory<AbstractBase, ProductList, B, typename BaseListHolder::NextListHolder>;

    template <typename Product>
    using CurHasDerived = HasDerivedInList<T, Product>;

    template <typename Product>
    constexpr static bool canCreate = HasDerivedInList<T, Product>::hasDerived && CanCreate<typename HasDerivedInList<T, Product>::Derived>::res;

    template <typename Product>
    using GetProductType = typename Select<canCreate<Product>, typename CurHasDerived<Product>::Derived, typename BaseFactory::template GetProductType<Product>>::res;

private:
    using CurObjectCreator::createObj;

    template <typename Product>
    Product* Get(){
        Product* p = 0;
        return createObj(p);
        //return new GetProductType<Product>();
    }


};

//от <Product>
template <typename AbstractBase, typename ...AbstractProductsList, class Derived>
struct Factory<AbstractBase, TypeList<AbstractProductsList...>, TypeList<Derived>, EmptyList>
        : virtual public ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<Derived>, EmptyList> {

public:
    using T = Derived;
    using B = EmptyType;

private:
    using CurObjectCreator = ObjectCreator<Length<TypeList<AbstractProductsList...>>::length-1, AbstractBase, TypeList<AbstractProductsList...>, TypeList<Derived>, EmptyList>;

    using ProductList = TypeList<AbstractProductsList...>;
    using TYF = int;

    template <typename Product>
    using CurHasDerived = HasDerivedInList<ProductList, Product>;

    template <typename Product>
    constexpr static bool canCreate = HasDerivedInList<ProductList , Product>::hasDerived && CanCreate<typename HasDerivedInList<ProductList, Product>::Derived>::res;

    template <typename Product>
    using GetProductType = typename Select<canCreate<Product>, typename CurHasDerived<Product>::Derived, EmptyType>::res;

public:
    using CurObjectCreator::createObj;

    template <typename Product>
    Product* Get(){
        Product* p = 0;
        return createObj(p);
        //return new GetProductType<Product>();
    }

};
