#pragma once

struct EmptyList{
};

struct  EmptyType{};
////////////////////////////////////

template <typename ... T>
struct TypeList{
};

template <typename H, typename ... T>
struct TypeList<H, T...>{
    using Head = H;
    using Tail = TypeList<T...>;
};

template <typename H>
struct TypeList<H>{
    using Head = H;
    using Tail = EmptyList;
};

template <typename H, typename ... T>
struct TypeList<H, TypeList<T...>>{
    using Head = H;
    using Tail = TypeList<T...>;
};

template <typename H>
struct TypeList<TypeList<H>>{
    using Head = H;
    using Tail = EmptyList;
};

template <typename H>
struct TypeList<H, EmptyList>{
    using Head = H;
    using Tail = EmptyList;
};
template <typename H>
struct TypeList<H, TypeList<EmptyList>>{
    using Head = H;
    using Tail = EmptyList;
};

template <typename H>
struct TypeList<TypeList<EmptyList>, H>{
    using Head = H;
    using Tail = EmptyList;
};

////////////////

template <int N, typename ...T>
struct Get{};

template <int N, typename H, typename ...T>
struct Get<N, TypeList<H, T...>>{
    using CurList = TypeList<H, T...>;
    using NextList = typename CurList::Tail;
    using res = typename Get<N-1, NextList>::res;
};

template <typename H, typename ...T>
struct Get<0, TypeList<H, T...>>{
    using CurList = TypeList<H, T...>;
    using NextList = typename CurList::Tail;
    using res = H;
};

template <int N>
struct Get<N, EmptyList>{
    using res = EmptyType;
};


///////////////////////////////////
//Заменяет все вхождения TypeToBeReplaced на ReplacingType
template <typename TypeToBeReplaced, typename ReplacingType, typename ...Tail>
struct Replace{
    //using res = EmptyType;
};

template <typename TypeToBeReplaced, typename ReplacingType, typename H, typename ...T>
struct Replace<TypeToBeReplaced, ReplacingType, TypeList<H, T...>>{
    using res = TypeList<H, typename Replace<TypeToBeReplaced, ReplacingType, typename TypeList<H,T...>::Tail >::res>;
};

template <typename TypeToBeReplaced, typename ReplacingType, typename ...T>
struct Replace<TypeToBeReplaced, ReplacingType, TypeList<TypeToBeReplaced, T...>>{
    using res = TypeList<ReplacingType, typename Replace<TypeToBeReplaced, ReplacingType, typename TypeList<TypeToBeReplaced, T...>::Tail >::res>;
};

template <typename TypeToBeReplaced, typename ReplacingType, typename H>
struct Replace<TypeToBeReplaced, ReplacingType, TypeList<H>>{
    using res = TypeList<H>;
};

template <typename TypeToBeReplaced, typename ReplacingType>
struct Replace<TypeToBeReplaced, ReplacingType, TypeList<TypeToBeReplaced> >{
    using res = TypeList<ReplacingType>;
};


//////////////////////////////////////////////////////

template <size_t pos, typename Type, typename H,  typename ... T>
struct SetAt{

};

template <size_t pos, typename Type, typename H,  typename ... T>
struct SetAt<pos, Type, TypeList<H, T...>>{
    using res = TypeList<H, typename SetAt<pos - 1, Type, TypeList<T...> >::res>;
};

template <typename Type, typename H,  typename ... T>
struct SetAt<0, Type, TypeList<H, T...>>{
    using res = TypeList<Type, H, T...>;
};

template <size_t pos, typename Type>
struct SetAt<pos, Type, EmptyList>{
    using res = TypeList<Type>;
};
/////////////////////////////////////////////////////////////
template <typename ...T>
struct Has {
};

template <typename Desired, typename H, typename ...T>
struct Has<Desired, TypeList<H, T...>>{
    constexpr static bool res = Has<Desired, typename TypeList<H, T...>::Tail>::res;
};

template <typename Desired, typename ...T>
struct Has<Desired, TypeList<Desired, T...>>{
    constexpr static bool res = true;
};

template <typename Desired>
struct Has<Desired, EmptyList>{
    constexpr static bool res = false;
};

//////////////////////

template <typename ...T>
struct Length{};

template <typename H, typename ...T>
struct Length<TypeList<H, T...>>{
    using NextLength = Length<typename TypeList<H, T...>::Tail>;
    constexpr static int length = 1 + NextLength::length;
};

template <typename T>
struct Length<TypeList<T>>{
    constexpr static int length = 1;
};
template <>
struct Length<EmptyList>{
    constexpr static int length = 0;
};
////////////////////////////////////////

template <typename ...T>
struct Append {
};

template <typename Desired, typename H, typename ...T>
struct Append<Desired, TypeList<H, T...>>{
    using res = TypeList<H, typename Append<Desired, typename TypeList<H, T...>::Tail>::res>;
};


template <typename Desired>
struct Append<Desired, EmptyList>{
    using res = TypeList<Desired>;
};

////////////////////////////
template <typename ...T>
struct AppendFront {
};
template <typename Desired, typename ...T>
struct AppendFront<Desired, TypeList<T...>> {
    using res = TypeList<Desired, TypeList<T...>>;
};

template <typename Desired>
struct AppendFront<Desired, EmptyList>{
    using res = TypeList<Desired>;
};

template <typename Desired>
struct AppendFront<Desired, TypeList<EmptyList>>{
    using res = TypeList<Desired>;
};
//////////////////////////////


//вспомогательные структуры для работы с TypeListHolder
///////////////////
template <typename ...T>
struct TypeListHolder{};

template <typename  ...T, typename ...OtherLists>
struct  TypeListHolder<TypeList<T...>, OtherLists...>{
    using List = TypeList<T...>;
    using NextListHolder = TypeListHolder<OtherLists...>;
};

template <typename  ...T, typename ...T2>
struct  TypeListHolder<TypeList<T...>, TypeList<T2...>>{
    using List = TypeList<T...>;
    using NextListHolder = TypeListHolder<TypeList<T2...>>;
};

template <typename  ...T, typename ...OtherLists>
struct  TypeListHolder<TypeList<T...>, TypeListHolder<OtherLists...>>{
    using List = TypeList<T...>;
    using NextListHolder = TypeListHolder<OtherLists...>;
};
template <typename  ...T>
struct  TypeListHolder<TypeList<T...>>{
    using List = TypeList<T...>;
    using NextListHolder = EmptyList;
};

template <typename  ...T>
struct  TypeListHolder<TypeListHolder<TypeList<T...>>>{
    using List = typename TypeListHolder<TypeList<T...>>::List ;
    using NextListHolder = typename TypeListHolder<TypeList<T...>>::NextListHolder ;
    using V = int;
};

///////////////////////
template <typename ...TypeListToBeReplacedTypes, typename ...ReplacingTypeListTypes, typename ...T, typename ...OtherLists>
struct Replace<TypeList<TypeListToBeReplacedTypes...>, TypeList<ReplacingTypeListTypes...>, TypeListHolder<TypeList<T...>, OtherLists...>>{
    using res = TypeListHolder<TypeList<T...>, typename Replace<TypeList<TypeListToBeReplacedTypes...>, TypeList<ReplacingTypeListTypes...>, typename TypeListHolder<TypeList<T...>, OtherLists...>::NextListHolder>::res>;
};

template <typename ...TypeListToBeReplacedTypes, typename ...ReplacingTypeListTypes, typename ...OtherLists>
struct Replace<TypeList<TypeListToBeReplacedTypes...>, TypeList<ReplacingTypeListTypes...>, TypeListHolder<TypeList<TypeListToBeReplacedTypes...>, OtherLists...>>{
    using res = TypeListHolder<TypeList<ReplacingTypeListTypes...>, typename Replace<TypeList<TypeListToBeReplacedTypes...>, TypeList<ReplacingTypeListTypes...>, typename TypeListHolder<TypeList<ReplacingTypeListTypes...>, OtherLists...>::NextListHolder>::res>;
};

template <typename ...TypeListToBeReplacedTypes, typename ...ReplacingTypeListTypes, typename ...T>
struct Replace<TypeList<TypeListToBeReplacedTypes...>, TypeList<ReplacingTypeListTypes...>, TypeListHolder<TypeList<T...>>>{
    using res = TypeListHolder<TypeList<T...>>;
};

template <typename ...TypeListToBeReplacedTypes, typename ...ReplacingTypeListTypes>
struct Replace<TypeList<TypeListToBeReplacedTypes...>, TypeList<ReplacingTypeListTypes...>, TypeListHolder<TypeList<TypeListToBeReplacedTypes...>>>{
    using res = TypeListHolder<TypeList<ReplacingTypeListTypes...>>;
};

////


template <typename ...Desired, typename ...H, typename ...T>
struct Append<TypeList<Desired...>, TypeListHolder<TypeList<H...>, T...>>{
    using res = TypeListHolder<TypeList<H...>, typename Append<TypeList<Desired...>, typename TypeListHolder<TypeList<H...>, T...>::NextListHolder>::res>;
};


template <typename ...Desired>
struct Append<TypeList<Desired...>, EmptyList>{
    using res = TypeListHolder<TypeList<Desired...>>;
};

/////////////////////////
template <typename ...Desired, typename ...T>
struct AppendFront<TypeList<Desired...>, TypeListHolder<T...>> {
    using res = TypeListHolder<TypeList<Desired...>, TypeListHolder<T...>>;
};

template <typename ...Desired>
struct AppendFront<TypeList<Desired...>, EmptyList>{
    using res = TypeListHolder<TypeList<Desired...>>;
};
template <typename ...Desired>
struct AppendFront<TypeList<Desired...>, TypeListHolder<EmptyList>>{
    using res = TypeListHolder<TypeList<Desired...>>;
};

///////////////
//Показывает тип Т, используется вывод типов С++, падает и в ошибке показывает тип переменной t
template <typename T>
void showType(const T t){
    t = 4;
}
