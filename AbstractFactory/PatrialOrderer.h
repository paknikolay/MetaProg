#pragma once

#include "TypeList.h"

using Small = char;
using Big = long;

//U is base of T
template <class T, class U>
struct Conversion{
    static Small Test(const U&);

    static Big Test(...);

    static T makeT();

    constexpr static bool convExist = sizeof(Test(makeT())) == sizeof(Small);
};

//////////////////////////
template <bool Q, class A, class B>
struct Select{
    using res = EmptyType;
};


template <class A, class B>
struct Select <true, A, B> {
    using res = A;
};

template <class A, class B>
struct Select <false, A, B>{
    using res = B;
};
template <typename ...AParam, typename ...BParam,template <typename ...T> class A, template <typename ...T> class B>
struct Select <false, A<AParam...>, B<BParam...>>{
    using res = B<BParam...>;
};
template <typename ...AParam, typename ...BParam,template <typename ...T> class A, template <typename ...T> class B>
struct Select <true, A<AParam...>, B<BParam...>>{
    using res = A<AParam...>;
};

template <typename ...BParam, class A, template <typename ...T> class B>
struct Select <false, A, B<BParam...>>{
    using res = B<BParam...>;
};
template <typename ...BParam, class A, template <typename ...T> class B>
struct Select <true, A, B<BParam...>>{
    using res = A;
};

template <typename ...AParam,template <typename ...T> class A, class B>
struct Select <false, A<AParam...>, B>{
    using res = B;
};
template <typename ...AParam,template <typename ...T> class A, class B>
struct Select <true, A<AParam...>, B>{
    using res = A<AParam...>;
};
////////////////////////////
//Есть ли в листе база переданного типа, совпадающие игнорируются
template <typename ...T>
struct HasBaseInList{};


template <typename H, typename ...T, typename Desired>
struct HasBaseInList<TypeList<H, T...>, Desired>{
private:
    constexpr static bool IsHBase = Conversion<Desired, H>::convExist;
public:
    constexpr static bool hasBase = IsHBase || HasBaseInList<typename TypeList<H, T...>::Tail, Desired>::hasBase;
    using Base = typename Select<IsHBase, H, typename HasBaseInList<typename  TypeList<H, T...>::Tail, Desired>::Base>::res;
};

template <typename ...T, typename Desired>
struct HasBaseInList<TypeList<Desired, T...>, Desired>{
private:
public:
    constexpr static bool hasBase = HasBaseInList<typename TypeList<Desired, T...>::Tail, Desired>::hasBase;
    using Base = typename HasBaseInList<typename  TypeList<Desired, T...>::Tail, Desired>::Base;
};

template <typename H, typename Desired>
struct HasBaseInList<TypeList<H>, Desired>{
    constexpr static bool hasBase = Conversion<Desired, H>::convExist;
    using Base = typename Select<hasBase, H, EmptyType>::res;
};

template <typename Desired>
struct HasBaseInList<TypeList<Desired>, Desired>{
    constexpr static bool hasBase = false;
    using Base = EmptyType;
};
/////////////////////////////////
template <typename ...T>
struct HasDerivedInList{};


template <typename H, typename ...T, typename Desired>
struct HasDerivedInList<TypeList<H, T...>, Desired>{
private:
    constexpr static bool IsHDerived = Conversion<H, Desired>::convExist;
public:
    constexpr static bool hasDerived = IsHDerived || HasDerivedInList<typename TypeList<H, T...>::Tail, Desired>::hasDerived;
    using Derived = typename Select<IsHDerived, H, typename HasDerivedInList<typename  TypeList<H, T...>::Tail, Desired>::Derived>::res;
};

template <typename ...T, typename Desired>
struct HasDerivedInList<TypeList<Desired, T...>, Desired>{
private:
public:
    constexpr static bool hasDerived = HasDerivedInList<typename TypeList<Desired, T...>::Tail, Desired>::hasDerived;
    using Derived = typename HasDerivedInList<typename  TypeList<Desired, T...>::Tail, Desired>::Derived;
};

template <typename H, typename Desired>
struct HasDerivedInList<TypeList<H>, Desired>{
    constexpr static bool hasDerived = Conversion<H,Desired>::convExist;
    using Derived = typename Select<hasDerived, H, EmptyType>::res;
};

template <typename Desired>
struct HasDerivedInList<TypeList<Desired>, Desired>{
    constexpr static bool hasDerived = false;
    using Derived = EmptyType;
};
//////////////////////////
//база, прям самая база
template <typename T, typename ...Tail>
struct LeastDerived{
   // using Derived = EmptyType;
};


template <typename Desired, typename ...DesiredList>
struct LeastDerived<Desired, TypeList<DesiredList...>, EmptyList>{
public:
    using Base = Desired;
    using BaseList = TypeList<DesiredList...>;
};

template <typename Desired, typename ...DesiredList>
struct LeastDerived<Desired, TypeList<DesiredList...>, TypeListHolder<EmptyList>>{
public:
    using Base = Desired;
    using BaseList = TypeList<DesiredList...>;
};


template <typename Desired, typename ...DesiredList, typename ...T, typename ...OtherLists>
struct LeastDerived<Desired, TypeList<DesiredList...>, TypeListHolder<TypeList<T...>, OtherLists...>>{
private:
    using NextLeastDerived = LeastDerived<Desired, TypeList<DesiredList...>,typename TypeListHolder<TypeList<T...>, OtherLists...>::NextListHolder>;
    using Candidate = typename NextLeastDerived ::Base;
    using CandidateList = typename NextLeastDerived ::BaseList;
    constexpr static bool hasCandidateDerived = HasBaseInList<TypeList<T...>, Candidate>::hasBase;
public:
    using Base = typename Select<hasCandidateDerived,
                            typename HasBaseInList<TypeList<T...>, Candidate>::Base,
                            Candidate
                            >::res;
    using BaseList = typename Select<hasCandidateDerived,
                                    TypeList<T...>,
                                     CandidateList>::res;
};//0 res = head
//1 res = Candidate


//////////////////
//переставяет базу в начало списка
template <class ...T>
struct  BaseToFront
{

};

template <>
struct  BaseToFront<EmptyList>{
    using res = EmptyList;
};

template <class ...T, class ...OtherLists>
struct  BaseToFront<TypeListHolder<TypeList<T...>, OtherLists...>>{
private:
    using NextListHolder = typename TypeListHolder<TypeList<T...>, OtherLists...>::NextListHolder;
    using List = typename TypeListHolder<TypeList<T...>, OtherLists...>::List;
    using Head = typename List::Head;
    using TLD = typename LeastDerived<Head, List, NextListHolder>::BaseList;
    using L = typename Replace<TLD, List, NextListHolder>::res;//меняет в тайле все вхождение TLD на TypeList<T...>
public:
    using res = TypeListHolder<TLD, L>;

};

template <class ...T>
struct  BaseToFront<TypeListHolder<TypeList<T...>>>{
private:
public:
    using res = TypeList<T...>;
};

//////////////////////////////////////
//Создает частичный порядок: базы в начале
template <class ...T>
struct  MakePartialOrder
{

};

template <>
struct  MakePartialOrder<EmptyList>{
    using res = EmptyList;
};

template <class ...T, class ...OtherLists>
struct  MakePartialOrder<TypeListHolder<TypeList<T...>, OtherLists...>>{
private:
    using BaseToFrontRes = typename BaseToFront<TypeListHolder<TypeList<T...>, OtherLists...>>::res;
public:
    using res = typename Append<typename BaseToFrontRes::List,
                         typename MakePartialOrder<typename BaseToFrontRes::NextListHolder>::res>::res;
};

template <class ...H>
struct  MakePartialOrder<TypeListHolder<TypeList<H...>>>{
private:
public:
    using res = TypeListHolder<TypeList<H...>>;
};


//////////////////////////

template <typename ...T>
struct HasDerived{};

template <typename Desired, typename ...H, typename ...T>
struct HasDerived<Desired, TypeListHolder<TypeList<H...>, TypeListHolder<T...>>>{
private:
    using CurrentListHolder = TypeListHolder<TypeList<H...>, TypeListHolder<T...>>;
    using NextListHolder = typename CurrentListHolder::NextListHolder;
    using CurrentList = typename CurrentListHolder::List;
    using NextHasDerived = HasDerived<Desired, NextListHolder>;
    using CurrentHasDerivedInList = HasDerivedInList<CurrentList, Desired>;
public:
    constexpr static bool res = CurrentHasDerivedInList::hasDerived || NextHasDerived::res;
};

template <typename Desired, typename ...H>
struct HasDerived<Desired, TypeListHolder<TypeList<H...>>>{
    constexpr static bool res = HasDerivedInList<typename TypeListHolder<TypeList<H...>>::List, Desired>::hasDerived;
};
template <typename Desired>
struct HasDerived<Desired, TypeListHolder<EmptyList>>{
    constexpr static bool res = false;
};
///////////////////////////////////////
//возвращает следующую базу из списка, совпадающе типы игнорируются
template <typename ...T>
struct NextBase{};

template <typename Desired, typename ...H, typename ...T>
struct NextBase<Desired, TypeListHolder<TypeList<H...>, T...>>{
private:
    constexpr static bool hasBase = HasBaseInList<TypeList<H...>, Desired>::hasBase;
    using nextBase = NextBase<Desired, typename TypeListHolder<TypeList<H...>, T...>::NextListHolder>;
public:
    using res = typename Select<hasBase, TypeList<H...>, typename nextBase::res >::res ;
};

template <typename Desired, typename ...T>
struct NextBase<Desired, TypeListHolder<TypeList<T...>>>{
    using res = typename Select<HasBaseInList<TypeList<T...>, Desired>::hasBase, TypeList<T...>, EmptyList>::res;
};
