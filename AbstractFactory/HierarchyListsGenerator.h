#pragma once

#include "TypeList.h"
#include "PatrialOrderer.h"

//создает цепочку из баз Product, (table, chair), (wooden...) (russian wooden..)
template <typename ...T>
struct GenerateHierarchyList{

};


template <typename ...BaseList, typename ...LastDerivedList, typename ...T>
struct GenerateHierarchyList<TypeList<BaseList...>, TypeList<LastDerivedList...>, TypeListHolder<T...>>{
private:
    using LastDerived = typename TypeList<LastDerivedList...>::Head;
    using NextBaseList = typename NextBase<LastDerived, TypeListHolder<T...>>::res;
    using NextHierarchyList = typename GenerateHierarchyList<TypeList<BaseList...>, NextBaseList, TypeListHolder<T...>>::res;
public:
    using res = typename Append<TypeList<LastDerivedList...>, NextHierarchyList>::res;
};

template <typename ...BaseList, typename ...T>
struct GenerateHierarchyList<TypeList<BaseList...>, TypeList<BaseList...>, TypeListHolder<T...>>{
    using res = TypeListHolder<TypeList<BaseList...>>;
};
