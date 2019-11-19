#include <iostream>
#include <assert.h>
#include "products.h"

#include "FactoryGenerator.h"
#include "TypeList.h"

//фабрика принимает TypeListHolder от TypeListов с типами
using ProductTypeListHolder = TypeListHolder<
        TypeList<SpanishIronChair, SpanishIronSofa, SpanishIronTable>,
        TypeList<Table, Chair, Sofa>,
        TypeList<SpanishWoodenChair, SpanishWoodenSofa, SpanishWoodenTable>,
        TypeList<IronTable, IronSofa, IronChair>,

        TypeList</*WoodenChair,*/ WoodenSofa, WoodenTable>,
        TypeList<PlasticChair, PlasticSofa, PlasticTable>,
        TypeList<ChineseIronChair, ChineseIronSofa, ChineseIronTable>,

        TypeList<ChinesePlasticChair, ChinesePlasticSofa, ChinesePlasticTable>,
        TypeList<ChineseWoodenChair, ChineseWoodenSofa, ChineseWoodenTable>,
        TypeList<RussianIronChair, RussianIronSofa, RussianIronTable>,
        TypeList<RussianPlasticChair, RussianPlasticSofa, RussianPlasticTable>,
        TypeList</*RussianWoodenChair, RussianWoodenSofa,*/ RussianWoodenTable>,
        TypeList<SpanishPlasticChair, SpanishPlasticSofa,SpanishPlasticTable>
        >;
//Класс для получения конкретных фабрик
using AbstractFactoryCreator = GetAbstractFactory<Product, ProductTypeListHolder>;
//абстрактная фабрика - база всех фабрик
using AbstractFactory = AbstractFactoryCreator::AbstactFactory;


//все конструкторы доступны, все листы указаны
void test1(){
    AbstractFactory* spanishIronFactory = new AbstractFactoryCreator::GetConcreteFactory<SpanishIronTable>::res();

    auto table = spanishIronFactory->Get<Table>();
    assert(table != 0);
    assert(dynamic_cast<SpanishIronTable*>(table) != 0);
    assert(dynamic_cast<IronTable*>(table) != 0);
    assert(dynamic_cast<Table*>(table) != 0);
    assert(dynamic_cast<Product*>(table) != 0);

    auto sofa = spanishIronFactory->Get<Sofa>();
    assert(sofa != 0);
    assert(dynamic_cast<SpanishIronSofa*>(sofa) != 0);
    assert(dynamic_cast<IronSofa*>(sofa) != 0);
    assert(dynamic_cast<Sofa*>(sofa) != 0);
    assert(dynamic_cast<Product*>(sofa) != 0);

    auto chair = spanishIronFactory->Get<Chair>();
    assert(chair != 0);
    assert(dynamic_cast<SpanishIronChair*>(chair) != 0);
    assert(dynamic_cast<IronChair*>(chair) != 0);
    assert(dynamic_cast<Chair*>(chair) != 0);
    assert(dynamic_cast<Product*>(chair) != 0);
}

//все конструкторы доступны, не все листы указаны, нет RussianWoodenSofa, RussianWoodenChair, WoodenChair
void test2(){
    AbstractFactory* factory = new AbstractFactoryCreator::GetConcreteFactory<RussianWoodenTable>::res();

    auto table = factory->Get<Table>();
    assert(table != 0);
    assert(dynamic_cast<RussianWoodenTable*>(table) != 0);
    assert(dynamic_cast<WoodenTable*>(table) != 0);
    assert(dynamic_cast<Table*>(table) != 0);
    assert(dynamic_cast<Product*>(table) != 0);

    auto sofa = factory->Get<Sofa>();
    assert(sofa != 0);
    //нет в списке
    assert(dynamic_cast<RussianWoodenSofa*>(sofa) == 0);
    assert(dynamic_cast<WoodenSofa*>(sofa) != 0);
    assert(dynamic_cast<Sofa*>(sofa) != 0);
    assert(dynamic_cast<Product*>(sofa) != 0);

    auto chair = factory->Get<Chair>();
    assert(chair != 0);
    //нет в списке
    assert(dynamic_cast<RussianWoodenChair*>(chair) == 0);
    //нет в списке
    assert(dynamic_cast<WoodenChair*>(chair) == 0);
    assert(dynamic_cast<Chair*>(chair) != 0);
    assert(dynamic_cast<Product*>(chair) != 0);
}


//не все конструкторы доступны, все листы указаны
void test3(){
    AbstractFactory* factory = new AbstractFactoryCreator::GetConcreteFactory<RussianPlasticSofa>::res();

    auto table = factory->Get<Table>();
    assert(table != 0);
    assert(dynamic_cast<RussianPlasticTable*>(table) != 0);
    assert(dynamic_cast<PlasticTable*>(table) != 0);
    assert(dynamic_cast<Table*>(table) != 0);
    assert(dynamic_cast<Product*>(table) != 0);

    auto sofa = factory->Get<Sofa>();
    assert(sofa != 0);
    //нет конструктора
    assert(dynamic_cast<RussianPlasticSofa*>(sofa) == 0);
    //нет конструктора
    assert(dynamic_cast<PlasticSofa*>(sofa) == 0);
    assert(dynamic_cast<Sofa*>(sofa) != 0);
    assert(dynamic_cast<Product*>(sofa) != 0);

    auto chair = factory->Get<Chair>();
    assert(chair != 0);
    assert(dynamic_cast<RussianPlasticChair*>(chair) != 0);
    assert(dynamic_cast<PlasticChair*>(chair) != 0);
    assert(dynamic_cast<Chair*>(chair) != 0);
    assert(dynamic_cast<Product*>(chair) != 0);
}

int main() {
    test1();
    test2();
    test3();
    return 0;
}