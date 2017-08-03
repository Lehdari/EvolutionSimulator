#ifndef QUAD_TREE_HPP
#define QUAD_TREE_HPP


#include <cstdint>
#include <vector>

#include <cstdio>   //  TEMP


class QuadTreeObject;


template<template <typename... Args> class T_Container, typename T_First, typename... T_Rest>
class QuadTree {
private:
    struct Branch;
    struct Node;

public:
    using Iterator = decltype(std::declval<T_Container<T_First, T_Rest...>>().begin());

    template <typename... ContainerArgs>
    QuadTree(uint64_t depth, uint64_t width, uint64_t height, ContainerArgs... cArgs);

    void recalculate(void);

    void getObjects(std::vector<Iterator>& objects,
        double xMin, double xMax, double yMin, double yMax,
        Branch* node = nullptr);

    T_Container<T_First, T_Rest...>   _objects;
private:
    struct Branch {
        double  xMin;
        double  xMax;
        double  yMin;
        double  yMax;
        uint64_t    nObjects;

        enum ChildType {
            NONE,
            BRANCH,
            LEAF
        } childType;

        void*   children[4];

        Branch(double xMin, double xMax, double yMin, double yMax) :
            xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax),
            nObjects(0), childType(NONE), children{nullptr}
        {}
    };

    struct Leaf {
        double  xMin;
        double  xMax;
        double  yMin;
        double  yMax;
        std::vector<Iterator>  objects;

        Leaf(double xMin, double xMax, double yMin, double yMax) :
            xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax)
        {}
    };

    uint64_t            _depth;
    uint64_t            _width;
    uint64_t            _height;
    double              _invW;
    double              _invH;

    std::vector<Branch>     _branches;
    std::vector<uint64_t>   _layerStarts;
    std::vector<Leaf>       _leaves;

    uint64_t posToMorton(double x, double y);

    Branch* addBranches(uint64_t level, double xMin, double xMax, double yMin, double yMax);

    void getObjects(std::vector<Iterator>& objects,
        double xMin, double xMax, double yMin, double yMax,
        Leaf* node);
};


class QuadTreeObject {
public:
    template<template <typename... Args> class Container, typename T_First, typename... T_Rest>
    friend class QuadTree;

    QuadTreeObject(double x = 0.0, double y = 0.0) :
        _x(x), _y(y)
    {}

protected:
    double  _x;
    double  _y;
};


#include "QuadTree.tcc"


#endif // QUAD_TREE_HPP
