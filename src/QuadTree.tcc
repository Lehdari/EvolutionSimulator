template<template <typename... Args> class T_Container, typename T_First, typename... T_Rest>
template <typename... ContainerArgs>
QuadTree<T_Container, T_First, T_Rest...>::QuadTree(
    uint64_t depth, uint64_t width, uint64_t height, ContainerArgs... cArgs)
:   _objects(std::forward<ContainerArgs>(cArgs)...),
    _depth  (depth),
    _width  (width),
    _height (height),
    _invW   (1.0/_width),
    _invH   (1.0/_height)
{
    uint64_t nBranches = 0;
    for (auto i=0u; i<_depth; ++i) {
        nBranches += 1 << (i*2);
    }
    _branches.reserve(nBranches);
    _leaves.reserve(1 << (depth*2));

    addBranches(_depth, 0.0, (double)width, 0.0, (double)height);

    recalculate();
}

template<template <typename... Args> class T_Container, typename T_First, typename... T_Rest>
void QuadTree<T_Container, T_First, T_Rest...>::recalculate(void)
{
    for (auto& l : _leaves) {
        l.objects.clear();
    }

    for (auto it = _objects.begin(); it != _objects.end(); ++it) {
        auto* oo = static_cast<QuadTreeObject*>(&*it);
        auto& l = _leaves.at(posToMorton(oo->_x, oo->_y));
        l.objects.push_back(it);
    }
}

template<template <typename... Args> class T_Container, typename T_First, typename... T_Rest>
void QuadTree<T_Container, T_First, T_Rest...>::getObjects(
    std::vector<QuadTree<T_Container, T_First, T_Rest...>::Iterator>& objects,
    double xMin, double xMax, double yMin, double yMax, Branch* node)
{
    if (node == nullptr)
        node = &_branches[0];

    if (node->xMin > xMax || node->xMax < xMin ||
        node->yMin > yMax || node->yMax < yMin)
        return;

    switch (node->childType) {
    case Branch::BRANCH:
        getObjects(objects, xMin, xMax, yMin, yMax, (Branch*)node->children[0]);
        getObjects(objects, xMin, xMax, yMin, yMax, (Branch*)node->children[1]);
        getObjects(objects, xMin, xMax, yMin, yMax, (Branch*)node->children[2]);
        getObjects(objects, xMin, xMax, yMin, yMax, (Branch*)node->children[3]);
        break;
    case Branch::LEAF:
        getObjects(objects, xMin, xMax, yMin, yMax, (Leaf*)node->children[0]);
        getObjects(objects, xMin, xMax, yMin, yMax, (Leaf*)node->children[1]);
        getObjects(objects, xMin, xMax, yMin, yMax, (Leaf*)node->children[2]);
        getObjects(objects, xMin, xMax, yMin, yMax, (Leaf*)node->children[3]);
        break;
    default:
        break;
    }
}

template<template <typename... Args> class T_Container, typename T_First, typename... T_Rest>
uint64_t QuadTree<T_Container, T_First, T_Rest...>::posToMorton(double x, double y)
{
    x = 0.5+x*_invW*0.5;
    y = 0.5+y*_invH*0.5;

    uint64_t xx = *((uint64_t*)&x);
    uint64_t yy = *((uint64_t*)&y);

    xx = (xx & 0x000fffffffffffff) >> (52-_depth);
    yy = (yy & 0x000fffffffffffff) >> (52-_depth);

    xx = (xx | xx << 16) & 0x0000ffff0000ffff;
    xx = (xx | xx << 8) & 0x00ff00ff00ff00ff;
    xx = (xx | xx << 4) & 0x0f0f0f0f0f0f0f0f;
    xx = (xx | xx << 2) & 0x3333333333333333;
    xx = (xx | xx << 1) & 0x5555555555555555;

    yy = (yy | yy << 16) & 0x0000ffff0000ffff;
    yy = (yy | yy << 8) & 0x00ff00ff00ff00ff;
    yy = (yy | yy << 4) & 0x0f0f0f0f0f0f0f0f;
    yy = (yy | yy << 2) & 0x3333333333333333;
    yy = (yy | yy << 1) & 0x5555555555555555;

    return xx | (yy << 1);
}

template<template <typename... Args> class T_Container, typename T_First, typename... T_Rest>
typename QuadTree<T_Container, T_First, T_Rest...>::Branch*
    QuadTree<T_Container, T_First, T_Rest...>::addBranches(uint64_t level,
    double xMin, double xMax, double yMin, double yMax)
{
    _branches.emplace_back(xMin, xMax, yMin, yMax);
    Branch* newBranch = &_branches.back();
    if (--level) {
        newBranch->children[0] = addBranches(level, xMin, (xMin+xMax)*0.5, yMin, (yMin+yMax)*0.5);
        newBranch->children[1] = addBranches(level, (xMin+xMax)*0.5, xMax, yMin, (yMin+yMax)*0.5);
        newBranch->children[2] = addBranches(level, xMin, (xMin+xMax)*0.5, (yMin+yMax)*0.5, yMax);
        newBranch->children[3] = addBranches(level, (xMin+xMax)*0.5, xMax, (yMin+yMax)*0.5, yMax);
        newBranch->childType = Branch::BRANCH;
    } else {
        _leaves.emplace_back(xMin, (xMin+xMax)*0.5, yMin, (yMin+yMax)*0.5);
        newBranch->children[0] = &_leaves.back();
        _leaves.emplace_back((xMin+xMax)*0.5, xMax, yMin, (yMin+yMax)*0.5);
        newBranch->children[1] = &_leaves.back();
        _leaves.emplace_back(xMin, (xMin+xMax)*0.5, (yMin+yMax)*0.5, yMax);
        newBranch->children[2] = &_leaves.back();
        _leaves.emplace_back((xMin+xMax)*0.5, xMax, (yMin+yMax)*0.5, yMax);
        newBranch->children[3] = &_leaves.back();
        newBranch->childType = Branch::LEAF;
    }

    return newBranch;
}

template<template <typename... Args> class T_Container, typename T_First, typename... T_Rest>
void QuadTree<T_Container, T_First, T_Rest...>::getObjects(
    std::vector<QuadTree<T_Container, T_First, T_Rest...>::Iterator>& objects,
    double xMin, double xMax, double yMin, double yMax, Leaf* node)
{
    if (node->xMin > xMax || node->xMax < xMin ||
        node->yMin > yMax || node->yMax < yMin)
        return;

    for (auto& it : node->objects)
        objects.push_back(it);
}
