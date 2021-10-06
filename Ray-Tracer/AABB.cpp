#include "AABB.h"
#if USING( BVH )

AABB::AABB()
{
	m_shape = nullptr;
	m_bottomLeft = vec3( 0 );
	m_topRight = vec3( 0 );
	m_numShapes = 0;
}

vec3 AABB::GetCenter()
{
	return ( m_bottomLeft + m_topRight ) / 2.0;
}

Shape* AABB::GetShape()
{
	return m_shape;
}

void AABB::SetShape( Shape* shape )
{
	m_shape = shape;
}

bool AABB::IntersectBox( Ray* ray )
{
	float tmin = ( m_bottomLeft.x - ray->pos.x ) / ray->direction.x;
	float tmax = ( m_topRight.x - ray->pos.x ) / ray->direction.x;

	if ( tmin > tmax ) SwapFloat( &tmin, &tmax );

	float tymin = ( m_bottomLeft.y - ray->pos.y ) / ray->direction.y;
	float tymax = ( m_topRight.y - ray->pos.y ) / ray->direction.y;

	if ( tymin > tymax ) SwapFloat( &tymin, &tymax );

	if ( ( tmin > tymax ) || ( tymin > tmax ) )
		return false;

	if ( tymin > tmin )
		tmin = tymin;

	if ( tymax < tmax )
		tmax = tymax;

	float tzmin = ( m_bottomLeft.z - ray->pos.z ) / ray->direction.z;
	float tzmax = ( m_topRight.z - ray->pos.z ) / ray->direction.z;

	if ( tzmin > tzmax ) SwapFloat( &tzmin, &tzmax );

	if ( ( tmin > tzmax ) || ( tzmin > tmax ) )
		return false;

	if ( tzmin > tmin )
		tmin = tzmin;

	if ( tzmax < tmax )
		tmax = tzmax;

	return true;
}

bool AABB::Intersect( Ray* ray, float* dist, Intersection* in )
{
	if ( !IntersectBox( ray ) )
	{
		return false;
	}

	PerfTracker::GetInstance()->IncrementIntersections();

	if ( !m_shape )
	{
		return true;
	}

	PerfTracker::GetInstance()->IncrementIntersections();
	if ( m_shape->intersect( *ray, dist, in->localGeo ) )
	{
		in->shape = m_shape;
		return true;
	}

	return false;
}

AABBTree::AABBTree()
{
	m_box = AABB();
	m_state = AABBTREE_NOT_BUILT;
	m_left = nullptr;
	m_right = nullptr;
}

AABB AABBTree::GetBoundingBox()
{
	return m_box;
}

void AABB::Insert( Shape* shape )
{
	if ( !shape )
	{
		return;
	}

	vec3 min = shape->GetMinCoords();
	vec3 max = shape->GetMaxCoords();
	m_bottomLeft.x = fminf( m_bottomLeft.x, min.x );
	m_bottomLeft.y = fminf( m_bottomLeft.y, min.y );
	m_bottomLeft.z = fminf( m_bottomLeft.z, min.z );

	m_topRight.x = fmaxf( m_topRight.x, max.x );
	m_topRight.y = fmaxf( m_topRight.y, max.y );
	m_topRight.z = fmaxf( m_topRight.z, max.z );
	m_numShapes++;
}

AABBTree* AABBTree::GetLeftSubtree()
{
	return m_left;
}

AABBTree* AABBTree::GetRightSubtree()
{
	return m_right;
}

AABBTreeState AABBTree::GetState()
{
	return m_state;
}

// searches & returns for the index to split this box on (non-inclusive)
uint32_t AABBTree::Split( Shape** shapes, uint32_t numShapes )
{
	float midpt = m_box.GetCenter().x;
	for ( uint32_t i = 0; i < numShapes; i++ )
	{
		if ( shapes[i]->GetCenter().x > midpt )
		{
			return i;
		}
	}
	return numShapes;
}

bool AABBTree::Build( Shape** shapes, uint32_t numShapes )
{
	if ( !shapes )
	{
		m_state = AABBTREE_BUILD_FAILED;
		return false;
	}

	if ( numShapes == 1 )
	{
		m_box.Insert( shapes[0] );
		m_box.SetShape( shapes[0] );
		return true;
	}

	// only need to sort once
	if ( m_state == AABBTREE_NOT_BUILT )
	{
		auto compareFunc = []( const void* _s1, const void* _s2 )
		{
			Shape* s1 = *(Shape**)_s1;
			Shape* s2 = *(Shape**)_s2;

			return (int)roundf( s1->GetMinX() - s2->GetMinX() );
		};

		qsort( shapes, numShapes, sizeof( Shape* ), compareFunc );
#if USING( DEBUG_BUILD )
		PRINT_INFO( COM_CHANNEL_AABB, "Done sorting geo, building tree now...." );
#endif // USING( DEBUG_BUILD )
	}
	m_state = AABBTREE_BUILDING;

	for ( uint32_t i = 0; i < numShapes; i++ )
	{
		m_box.Insert( shapes[i] );
	}

	uint32_t splitIndex = Split( shapes, numShapes );
	if ( splitIndex == numShapes || splitIndex == 0 )
	{
		splitIndex = (uint32_t) numShapes / 2;
	}
	m_left = new AABBTree();
	m_right = new AABBTree();

	bool success = true;

	// build left subtree
	m_left->m_state = AABBTREE_BUILDING;
	success = success && m_left->Build( shapes, splitIndex );

	// build right subtree
	m_right->m_state = AABBTREE_BUILDING;
	success = success && m_right->Build( shapes + splitIndex, numShapes - splitIndex );

	if ( !success )
	{
		m_state = AABBTREE_BUILD_FAILED;
	}

	return success;
}

bool AABBTree::Intersect( Ray* ray, float* dist, Intersection* in )
{
	if ( !m_box.Intersect( ray, dist, in ) )
	{
		return false;
	}

	bool hit = false;
	if ( m_left )
	{
		hit = m_left->Intersect( ray, dist, in );
	}

	if ( m_right )
	{
		hit = hit || m_right->Intersect( ray, dist, in );
	}

	if ( !m_left && !m_right )
	{
		// leaf, alreay passed intersection so just return true
		return true;
	}

	return hit;
}

#endif // #if USING( BVH )