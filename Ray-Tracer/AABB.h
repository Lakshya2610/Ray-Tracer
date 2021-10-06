#pragma once
#include "Shape.h"

#if USING( BVH )


enum AABBTreeState
{
	AABBTREE_NOT_BUILT = 0,
	AABBTREE_BUILDING,
	AABBTREE_BUILT,

	AABBTREE_STATE_COUNT,
	AABBTREE_BUILD_FAILED = -1
};

class AABB
{
public:
	AABB();
	~AABB() {};

	vec3 GetCenter();
	Shape* GetShape();
	void SetShape( Shape* shape );
	void Insert( Shape* shape );
	bool Intersect( Ray* ray, float* dist, Intersection *in );

private:

	bool IntersectBox( Ray* ray );

	Shape* m_shape;
	vec3 m_bottomLeft;
	vec3 m_topRight;
	uint32_t m_numShapes;
};

class AABBTree
{
public:
	AABBTree();
	~AABBTree() {};

	bool Build( Shape** shapes, uint32_t numShapes );
	uint32_t Split( Shape** shapes, uint32_t numShapes );
	bool Intersect( Ray* ray, float* dist, Intersection* in );
	AABBTreeState GetState();
	AABB GetBoundingBox();
	AABBTree* GetLeftSubtree();
	AABBTree* GetRightSubtree();

private:


	AABBTree* m_left;
	AABBTree* m_right;
	AABB m_box;
	AABBTreeState m_state;
};

#endif // USING( BVH )
