#pragma once

#include "TypeList.h"
#include "PatrialOrderer.h"
#include "HierarchyListsGenerator.h"
#include "Factory.h"


////// Создает фабрики по переданному листу из баз(Product, (Chair, ...), (WoodenChair...), (RussianWoodenChair...))
template <typename ...T>
struct CreateFactories{};


template <typename CommonBase, typename ...AbstractProductsList, typename ...BaseList, typename ...LeftClasses, typename ...Bases, typename ...FactoryTypes>
struct CreateFactories<CommonBase, TypeList<AbstractProductsList...>, TypeListHolder<TypeList<BaseList...>, TypeListHolder<LeftClasses...>>, TypeListHolder<Bases...>, Factory<FactoryTypes...>>{
private:
    using CurrentListHolder = TypeListHolder<TypeList<BaseList...>, TypeListHolder<LeftClasses...>>;
    using CurrentBaseList = typename  CurrentListHolder::List;
    using CurrentDerivedList = typename CurrentListHolder::NextListHolder::List;
    using NextListHolder = typename CurrentListHolder::NextListHolder;
    using NextBases = typename AppendFront<CurrentBaseList, TypeListHolder<Bases...>>::res;
    using ProducList = TypeList<AbstractProductsList...>;
    using CreatedFactory = Factory<CommonBase,ProducList, CurrentDerivedList, NextBases>;
    using NextCreateFactory = CreateFactories<CommonBase, ProducList, NextListHolder, NextBases, CreatedFactory>;

public:
    using ListFactory = typename NextCreateFactory::ListFactory;
};

template <typename CommonBase, typename ...AbstractProductsList, typename ...BaseList, typename ...Bases, typename ...FactoryTypes>
struct CreateFactories<CommonBase,TypeList<AbstractProductsList...>,TypeListHolder<TypeList<BaseList...>>, TypeListHolder<Bases...>, Factory<FactoryTypes...>>{
private:
    using ProducList = TypeList<AbstractProductsList...>;
    using NextCreateFactory = EmptyType;
    using CreatedFactory = EmptyType;
public:
    using ListFactory = Factory<FactoryTypes...>;
};
///////////////////////////////////////////

//Получает частично упорядоченный набор типов, выделяет цепочки баз и задает на них фабрики
//Сохраняет листовую фабрику в список типов
template <typename ...T>
struct GetListFactory{};

template <typename CommonBase, typename ...Bases, typename ...CurrentTypes,typename ...RestLists, typename ...ManagedLists>
struct GetListFactory<CommonBase, TypeList<Bases...>, TypeListHolder<TypeList<CurrentTypes...>, TypeListHolder<RestLists...>>, TypeListHolder<ManagedLists...>>{
private:
    using BaseList = TypeList<Bases...>;
    using CurrentListHolder = TypeListHolder<TypeList<CurrentTypes...>, TypeListHolder<RestLists...>>;
    using RestListHolder = typename CurrentListHolder::NextListHolder;
    using ManagedListHolder = TypeListHolder<ManagedLists...>;

    using CurrentList = typename CurrentListHolder::List;
    using CurrentType = typename CurrentListHolder::List::Head;

    constexpr static bool isNotList = HasDerived<CurrentType, ManagedListHolder>::res;

    using HierarchyList = typename AppendFront<TypeList<CommonBase>, typename GenerateHierarchyList<BaseList, CurrentList, CurrentListHolder>::res>::res;

    using NextManagedListHolder = TypeListHolder<CurrentList, ManagedListHolder>;

    using NextListFactory = GetListFactory<CommonBase, BaseList, RestListHolder , NextManagedListHolder>;
    using FactoryCreator = CreateFactories<CommonBase, BaseList, HierarchyList, TypeListHolder<EmptyList>, Factory<EmptyType>>;
    using CreatedFactory = typename CreateFactories<CommonBase, BaseList, HierarchyList, TypeListHolder<EmptyList>, Factory<EmptyType>>::ListFactory;
    using NextPossibleFactoryList = typename AppendFront<CreatedFactory, typename NextListFactory::res>::res;

public:
    using res =  typename Select<isNotList, typename NextListFactory::res, NextPossibleFactoryList>::res;
};


template <typename CommonBase, typename ...Bases, typename ...CurrentTypes, typename ...ManagedLists>
struct GetListFactory<CommonBase, TypeList<Bases...>, TypeListHolder<TypeList<CurrentTypes...>>, TypeListHolder<ManagedLists...>>{
public:
// так как в конце точно базы
    using res = EmptyList;
};


///////////////////////////
//возвращает конкретную фабрику, производящую нужный продукт
template <typename ...T>
struct GetFactoryByConcreteProduct{};


template <typename ...CurFactoryTypes, typename ...OtherFactories, typename Desired>
struct GetFactoryByConcreteProduct<TypeList<Factory<CurFactoryTypes...>, OtherFactories...>, Desired>{
private:
    using FactoryList = TypeList<Factory<CurFactoryTypes...>, OtherFactories...>;
    using CurFactory = typename FactoryList::Head;
    constexpr static bool HasProductInCurFactory = Has<Desired, typename CurFactory::T>::res;
    using NextGetFactoryByConcreteProduct = GetFactoryByConcreteProduct<typename FactoryList::Tail, Desired>;
public:
    using res = typename Select<HasProductInCurFactory, CurFactory, typename NextGetFactoryByConcreteProduct::res>::res;
};

template <typename ...CurFactoryTypes, typename Desired>
struct GetFactoryByConcreteProduct<TypeList<Factory<CurFactoryTypes...>>, Desired>{
private:
    using FactoryList = TypeList<Factory<CurFactoryTypes...>>;
    using CurFactory = typename FactoryList::Head;
    constexpr static bool HasProductInCurFactory = Has<Desired, typename CurFactory::T>::res;
public:
    using res = typename Select<HasProductInCurFactory, CurFactory, EmptyList>::res;
};
////////////////
//Создает все необходимые фабрики, так же хранит абстрактную фабрику
template <typename ...T>
struct GetAbstractFactory{};

template <typename AbstractBase, typename ...T>
struct GetAbstractFactory<AbstractBase, TypeListHolder<T...>>{
private:
    using CurrentListHolder = TypeListHolder<T...>;
    using OrderedListHolder = typename MakePartialOrder<CurrentListHolder>::res;
    using AbstractProductBases = typename LeastDerived<typename OrderedListHolder::List::Head, typename OrderedListHolder::List, OrderedListHolder>::BaseList;
    using ListFactoryGen = GetListFactory<AbstractBase, AbstractProductBases, OrderedListHolder, TypeListHolder<EmptyList>>;
    using FactoriesList = typename ListFactoryGen::res;
public:
    template <typename ConcreteProduct>
    using GetConcreteFactory = GetFactoryByConcreteProduct<FactoriesList, ConcreteProduct>;
    using AbstactFactory = Factory<AbstractBase,AbstractProductBases, TypeList<AbstractBase>, EmptyList>;
};