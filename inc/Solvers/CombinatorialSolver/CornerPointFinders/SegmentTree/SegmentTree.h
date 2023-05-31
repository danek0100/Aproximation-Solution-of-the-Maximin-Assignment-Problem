#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H

#include <vector>
#include <string>

enum class Status
{
	EMPTY = 0,
	PARTIAL,
	FULL
};

class SegmentTreeNode 
{
public:
	std::unique_ptr<SegmentTreeNode> leftChild;
	std::unique_ptr<SegmentTreeNode> rightChild;
	long l, r;
	int C;
	Status status;

	SegmentTreeNode( long l, long r ) :
		l( l ), r( r ), C( 0 ), status( Status::EMPTY ),
		leftChild( nullptr ), rightChild( nullptr ) {}

	void insert( long left, long right ) 
	{
		if ( left <= l && r <= right ) 
		{
			C++;
		}
		else 
		{
			if ( left < ( l + r ) / 2 && leftChild )
			{
				leftChild->insert( left, right );
			}
			if ( ( l + r ) / 2 < right && rightChild )
			{
				rightChild->insert( left, right );
			}
		}
		update();
	}

	void remove( long left, long right )
	{
		if ( left <= l && r <= right ) 
		{
			C--;
		}
		else
		{
			if ( left < ( l + r ) / 2 && leftChild )
			{
				leftChild->remove( left, right );
			}
			if ( ( l + r ) / 2 < right && rightChild )
			{
				rightChild->remove( left, right );
			}
		}
		update();
	}

	void update() 
	{
		if ( C > 0 )
		{
			status = Status::FULL;
		}
		else
		{
			if ( leftChild && rightChild )
			{
				if ( leftChild->status == Status::EMPTY && rightChild->status == Status::EMPTY )
				{
					status = Status::EMPTY;
				}
				else
				{
					status = Status::PARTIAL;
				}
			}
			else
			{
				status = Status::EMPTY;
			}
		}
	}

	void contr( long left, long right, std::vector<long>& stack )
	{
		if ( status != Status::FULL )
		{
			if ( left <= l && r <= right && status == Status::EMPTY )
			{
				if ( !stack.empty() && l == stack.back() )
				{
					stack.pop_back();
				}
				else 
				{
					stack.push_back( l );
				}
				stack.push_back( r );
			}
			else
			{
				if ( leftChild && left < ( l + r ) / 2 )
				{
					leftChild->contr( left, right, stack );
				}
				if (rightChild && ( l + r ) / 2 < right )
				{
					rightChild->contr( left, right, stack );
				}
			}
		}
	}
};

class SegmentTree
{
public:
	long minValue, maxValue;
	std::unique_ptr<SegmentTreeNode> root;

	SegmentTree( long minValue, long maxValue ) : minValue(minValue), maxValue(maxValue)
	{
		root = buildSegmentTree( minValue, maxValue );
	}

	std::unique_ptr<SegmentTreeNode> buildSegmentTree( long l, long r )
	{
		auto subtree = std::make_unique<SegmentTreeNode>( l, r );
		if ( ( r - l ) > 1 )
		{
			subtree->leftChild = buildSegmentTree( l, ( l + r ) / 2 );
			subtree->rightChild = buildSegmentTree( ( l + r ) / 2  , r );
		}
		return std::move(subtree);
	}

	void extendTree(long newMinValue, long newMaxValue)
	{
		if (newMinValue < minValue || newMaxValue > maxValue)
		{
			minValue = std::min(newMinValue, minValue);
			maxValue = std::max(newMaxValue, maxValue);

			extendTreeHelper(root, minValue, maxValue);
		}
	}

private:
	void extendTreeHelper(std::unique_ptr<SegmentTreeNode>& node, long l, long r)
	{
		if (node->leftChild == nullptr || node->rightChild == nullptr)
		{
			if (l < node->l || r > node->r)
			{
				// Rebuild the current node with new bounds
				// TODO only extend_boards before checking childrens
				node = buildSegmentTree(l, r);
				return;
			}
		}

		if (l < node->l || r > node->r)
		{
			// Extend the left child if necessary
			if (node->leftChild != nullptr && l < node->leftChild->l)
			{
				extendTreeHelper(node->leftChild, l, std::min(r, node->leftChild->r));
			}

			// Extend the right child if necessary
			if (node->rightChild != nullptr && r > node->rightChild->r)
			{
				extendTreeHelper(node->rightChild, std::max(l, node->rightChild->l), r);
			}
		}
	}
};

#endif // SEGMENT_TREE_H