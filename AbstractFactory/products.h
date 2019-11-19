#pragma once

class Product{
public:
    virtual ~Product(){};
};

class Chair : virtual public Product{};
class Table : virtual public Product{};
class Sofa : virtual public Product{};

class WoodenChair : virtual public Chair{};
class WoodenTable : virtual public Table{};
class WoodenSofa : virtual public Sofa{};

class PlasticChair : virtual public Chair{};
class PlasticTable : virtual public Table{};
class PlasticSofa : virtual public Sofa{
    PlasticSofa() = delete;
};

class IronChair : virtual public Chair{};
class IronTable : virtual public Table{};
class IronSofa : virtual public Sofa{};

class RussianWoodenChair : virtual public WoodenChair{};
class ChineseWoodenChair : virtual public WoodenChair{};
class SpanishWoodenChair : virtual public WoodenChair{};

class RussianWoodenTable : virtual public WoodenTable{};
class ChineseWoodenTable : virtual public WoodenTable{};
class SpanishWoodenTable : virtual public WoodenTable{};

class RussianWoodenSofa : virtual public WoodenSofa{};
class ChineseWoodenSofa : virtual public WoodenSofa{};
class SpanishWoodenSofa : virtual public WoodenSofa{};


class RussianPlasticChair : virtual public PlasticChair{};
class ChinesePlasticChair : virtual public PlasticChair{};
class SpanishPlasticChair : virtual public PlasticChair{};

class RussianPlasticTable : virtual public PlasticTable{};
class ChinesePlasticTable : virtual public PlasticTable{};
class SpanishPlasticTable : virtual public PlasticTable{};

class RussianPlasticSofa : virtual public PlasticSofa{
    RussianPlasticSofa() = delete;
};
class ChinesePlasticSofa : virtual public PlasticSofa{};
class SpanishPlasticSofa : virtual public PlasticSofa{};


class RussianIronChair : virtual public IronChair{};
class ChineseIronChair : virtual public IronChair{};
class SpanishIronChair : virtual public IronChair{};

class RussianIronTable : virtual public IronTable{};
class ChineseIronTable : virtual public IronTable{};
class SpanishIronTable : virtual public IronTable{};

class RussianIronSofa : virtual public IronSofa{};
class ChineseIronSofa : virtual public IronSofa{};
class SpanishIronSofa : virtual public IronSofa{};
