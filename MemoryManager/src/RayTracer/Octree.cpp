#include "Octree.h"


void* Octree::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{Octree} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Shape"));
}
void Octree::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{Octree} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* Octree::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{Octree} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Shape"));
}
void Octree::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{Octree} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

Octree* Octree::ConstructTree(Vector3f dimensions, Vector3f position, std::unordered_set<Sphere>& spheres)
{
    Octree* octree = new Octree();
    octree->rootNode = new OctreeNode();

    octree->rootNode->SetBounds(dimensions, position);
    for (auto &sphere : spheres)
    {
        bool isSet = octree->rootNode->SetChild(sphere);
        if (!isSet)
        {
            octree->externalSpheres.insert(sphere);
        }
    }
    octree->rootNode->Subdivide(0);

    return octree;
}

Octree::~Octree()
{
    delete rootNode;
}

std::unordered_set<Sphere> Octree::Intersect(Vector3f rayOrigin, Vector3f rayDirection)
{
    std::unordered_set<Sphere> possibleSpheres = std::unordered_set<Sphere>();

    Vector3f invDir = Vector3f(1.0f / rayDirection.x, 1.0f / rayDirection.y, 1.0f / rayDirection.z);

    rootNode->RayIntersect(possibleSpheres, rayOrigin, rayDirection, invDir);
    for (auto &sphere : externalSpheres)
    {
        possibleSpheres.insert(sphere);
    }
    return possibleSpheres;
}

void* OctreeNode::operator new(size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{OctreeNode} New Called" << std::endl;);
    return ManagedObject::operator new(size, HeapManager::Instance()->GetHeap("Shape"));
}
void OctreeNode::operator delete(void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{OctreeNode} Delete Called" << std::endl;);
    ::operator delete(pMem);
}
void* OctreeNode::operator new[](size_t size)
{
    VERBOSE_OUTPUT(std::cout << "{OctreeNode} New[] Called" << std::endl;);
    return ManagedObject::operator new[](size, HeapManager::Instance()->GetHeap("Shape"));
}
void OctreeNode::operator delete[](void* pMem)
{
    VERBOSE_OUTPUT(std::cout << "{OctreeNode} Delete[] Called" << std::endl;);
    ::operator delete[](pMem);
}

void OctreeNode::SetBounds(Vector3f dimensions, Vector3f position)
{
    this->dimensions = dimensions;
    this->position = position;
}

void OctreeNode::Subdivide(int layer)
{
    Vector3f childDimensions = this->dimensions * 0.5f;
    for (int x = 0; x < 2; ++x)
    {
        for (int y = 0; y < 2; ++y)
        {
            for (int z = 0; z < 2; ++z)
            {
                Vector3f childPosition = position + (childDimensions * Vector3f(x, y, z));
                this->childCells.push_back(new OctreeNode());
                childCells.back()->SetBounds(childDimensions, childPosition);
                for (auto &sphere : containedSpheres)
                {
                    childCells.back()->SetChild(sphere);
                }
                if (childCells.back()->containedSpheres.size() > 2)
                {
                    if (layer < MAX_OCTREE_LAYERS)
                    {
                        childCells.back()->Subdivide(layer + 1);
                    }
                }
            }
        }
    }
}

bool OctreeNode::SetChild(const Sphere& sphere)
{
    bool intersect = AABBSphereIntersection(sphere);
    if (intersect)
    {
        this->containedSpheres.push_back(sphere);
    }

    return intersect;
}

void Swap(float& a, float& b)
{
    float temp = a;
    a = b;
    b = temp;
}

void OctreeNode::RayIntersect(std::unordered_set<Sphere>& possibleSpheres, Vector3f rayOrigin, Vector3f rayDirection, Vector3f invDir)
{
    Vector3f minCoord = position;
    Vector3f maxCoord = position + dimensions;

    float t1 = (minCoord.x - rayOrigin.x) * invDir.x;
    float t2 = (maxCoord.x - rayOrigin.x) * invDir.x;
    float t3 = (minCoord.y - rayOrigin.y) * invDir.y;
    float t4 = (maxCoord.y - rayOrigin.y) * invDir.y;
    float t5 = (minCoord.z - rayOrigin.z) * invDir.z;
    float t6 = (maxCoord.z - rayOrigin.z) * invDir.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    if (tmax < 0)
    {
        return;
    }

    if (tmin > tmax)
    {
        return;
    }

    if (childCells.size() > 0)
    {
        for (auto cell : childCells)
        {
            if (cell->containedSpheres.size() > 0)
            {
                cell->RayIntersect(possibleSpheres, rayOrigin, rayDirection, invDir);
            }
        }
    }
    else
    {
        possibleSpheres.insert(containedSpheres.begin(), containedSpheres.end());
    }
}

bool OctreeNode::AABBSphereIntersection(const Sphere& sphere)
{
    Vector3f AABBMin = position;
    Vector3f AABBMax = position + dimensions;
    float dmin = 0;


    if (sphere.center.x < AABBMin.x)
    {
        dmin += sqrt(sphere.center.x - AABBMin.x);
    }
    else
    {
        if (sphere.center.x > AABBMax.x)
        {
            dmin += sqrt(sphere.center.x - AABBMax.x);
        }
    }

    if (sphere.center.y < AABBMin.y)
    {
        dmin += sqrt(sphere.center.y - AABBMin.y);
    }
    else
    {
        if (sphere.center.y > AABBMax.y)
        {
            dmin += sqrt(sphere.center.y - AABBMax.y);
        }
    }

    if (sphere.center.z < AABBMin.z)
    {
        dmin += sqrt(sphere.center.z - AABBMin.z);
    }
    else
    {
        if (sphere.center.z > AABBMax.z)
        {
            dmin += sqrt(sphere.center.z - AABBMax.z);
        }
    }

    if (dmin <= sphere.radius2)
    {
        return true;
    }
    return false;
}

OctreeNode::~OctreeNode()
{
    for (auto childCell : childCells)
    {
        delete childCell;
    }
}
