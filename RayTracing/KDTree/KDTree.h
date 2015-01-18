#pragma once 

#include <Objects\Object.h>
#include <Helper.h>
#include <vector>

static const int NUMBER_OF_OBJECTS = 3;
static const int DIMENSIONS = 3;
static const float COST_OF_TRAVERSAL = 1.0;
static const float COST_OF_INTERSECTION = 1.5;


struct KDInfo
{
	std::vector<int> objects;
};

struct KDTree
{

	KDTree(BBox boundingBox, int depth, int index) : numberOfObjects(0)
	{
		for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
		{
			objects[i] = -1;
		}
		nodes[0] = nodes[1] = -1;
	}


	BBox boundingBox;
	int objects[NUMBER_OF_OBJECTS];
	int nodes[2];
	int depth;
	int numberOfObjects;
	int index;

	void insert(const std::vector<Object> & objects , std::vector<KDTree> & nodes, const std::vector<KDInfo> & nodeInfo )
	{
		int nodeIndex = index;
		int nodeDepth = depth;

		if(numberOfObjects  > NUMBER_OF_OBJECTS)
		{
			float bestPosition = -1;
			float bestCost = 10000000.0f;
			for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
			{
				float min = objects[nodeInfo[nodeIndex].objects[i]].box.min[depth % DIMENSIONS];
				float max = objects[nodeInfo[nodeIndex].objects[i]].box.max[depth % DIMENSIONS];

				float cost = findCost(min,objects);
				if(cost < bestCost){bestCost = cost; bestPosition = min;}
				cost = findCost(max,objects);
				if(cost < bestCost){bestCost = cost; bestPosition = max;}

			}

			glm::vec3 halfDimension;
			int axis = depth%DIMENSIONS;

			for (int i = 0; i < DIMENSIONS; i++)
			{
				if(axis != i)
					halfDimension[i] = boundingBox.max[i];
			}

			halfDimension[axis] = bestCost;

			BBox left = {boundingBox.min , 0.0f , halfDimension};
			BBox right = {halfDimension , 0.0f , boundingBox.max};
			KDTree leftNode(left,depth + 1, nodes.size() + 1); 
			KDTree rightNode(right,depth + 1, nodes.size() + 2);

			nodes.push_back(leftNode);
			nodes.push_back(rightNode);

			nodes[nodes.size() - 1].insert(objects,nodes,nodeInfo);
			nodes[nodes.size() - 2].insert(objects,nodes,nodeInfo);
		}
		else
		{

		}
	}


	void insert(Object object , std::vector<KDTree> & nodes , const std::vector<Object> & objects)
	{
		numberOfObjects++;
		int nodeIndex = index;
		int nodeDepth = depth;
		if(numberOfObjects > NUMBER_OF_OBJECTS)
		{
			if(nodes[nodes[nodeIndex].nodes[0]].boundingBox.max[depth %DIMENSIONS] > object.box.max[depth % DIMENSIONS])
			{
				nodes[nodes[nodeIndex].nodes[0]].insert( object , nodes ,  objects);
			}
			else
			{
				nodes[nodes[nodeIndex].nodes[1]].insert( object , nodes ,  objects);
			}
		}
		else if(numberOfObjects - 1 < NUMBER_OF_OBJECTS)
		{
			nodes[nodeIndex].objects[numberOfObjects - 1] = object.index;
		}
		else
		{
			float bestPosition = -1;
			float bestCost = 10000000.0f;
			for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
			{
				float min = object.box.min[depth % DIMENSIONS];
				float max = object.box.max[depth % DIMENSIONS];

				float cost = findCost(min,objects);
				if(cost < bestCost){bestCost = cost; bestPosition = min;}
				cost = findCost(max,objects);
				if(cost < bestCost){bestCost = cost; bestPosition = max;}

			}

			glm::vec3 halfDimension;
			int axis = depth%DIMENSIONS;

			for (int i = 0; i < DIMENSIONS; i++)
			{
				if(axis != i)
					halfDimension[i] = boundingBox.max[i];
			}

			halfDimension[axis] = bestCost;

			BBox left = {boundingBox.min , 0.0f , halfDimension};
			BBox right = {halfDimension , 0.0f , boundingBox.max};
			KDTree leftNode(left,depth + 1, nodes.size() + 1); 
			KDTree rightNode(right,depth + 1, nodes.size() + 2);

			nodes.push_back(leftNode);
			nodes.push_back(rightNode);
		}
	}


	float leftCost(float splitPosition, const std::vector<Object> & objects)
	{
		glm::vec3 dimensions;
		int axis = depth%DIMENSIONS;

		for (int i = 0; i < DIMENSIONS; i++)
		{
			if(axis != i)
				dimensions[i] = boundingBox.max[i];
		}

		dimensions[axis] = splitPosition;

		glm::vec3 absDimensions = dimensions - boundingBox.min;

		float area = 2.0f * absDimensions.x * absDimensions.y * absDimensions.z;


		BBox box = {glm::min(dimensions, boundingBox.min), 0.0, glm::max(dimensions,boundingBox.min )};

		int numberOfObjects = 0;
		for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
		{
			

			if(objectBoxCollided(box,objects[this->objects[i]]))
			{
				numberOfObjects++;
			}
		}


		return COST_OF_TRAVERSAL + area * numberOfObjects * COST_OF_INTERSECTION;
	}

	float rightCost(float splitPosition, const std::vector<Object> & objects)
	{
		glm::vec3 dimensions;
		int axis = depth%DIMENSIONS;

		for (int i = 0; i < DIMENSIONS; i++)
		{
			if(axis != i)
				dimensions[i] = boundingBox.max[i];
		}

		dimensions[axis] = splitPosition;

		glm::vec3 absDimensions = glm::abs( boundingBox.max - dimensions ) ;

		float area = 2.0f * absDimensions.x * absDimensions.y * absDimensions.z;


		BBox box = {glm::min(dimensions, boundingBox.max), 0.0, glm::max(dimensions,boundingBox.max )};

		int numberOfObjects = 0;
		for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
		{
			if(objectBoxCollided(box,objects[this->objects[i]]))
			{
				numberOfObjects++;
			}
		}


		return COST_OF_TRAVERSAL + area * numberOfObjects * COST_OF_INTERSECTION;
	}

	float findCost(float splitPosition , const std::vector<Object> & objects)
	{
		return leftCost(splitPosition, objects) + rightCost(splitPosition,objects);
	}
};


struct KDTreeManager
{
	KDTreeManager(KDTree root)
	{
		root.index = 0;
		root.depth = 0;
		nodes.push_back(root);
	}

	std::vector<KDTree> nodes;
	std::vector<KDInfo> nodeInfo;


	void insert(const  std::vector<Object> & objects)
	{
		nodes[0].insert(objects,nodes,nodeInfo);
	}

	void insert(Object object , const  std::vector<Object> & objects)
	{
		nodes[0].insert(object, nodes, objects);
	}
};