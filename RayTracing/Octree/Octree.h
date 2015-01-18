#pragma once
#include <glm.hpp>

#include <vector>
#include <Helper.h>
#include <iostream>


/*
An easy way to keep track of what object has moved is once it has moved remove this object and then see if it can be inserted completely into the parent,
if not continue up until there is a parent that the object can completely fix inside of.

*/

using std::cout;
using std::endl;

static const int OCTREE_MAX_OBJECTS = 5;
static const int MAX_DEPTH = 20;
struct Octree
{

	Octree() : numberOfObjects(0)
	{
		for (int i = 0; i < 8; i++)
		{
			childrenIndices[i] = -1;
		}
		for (int i = 0; i < OCTREE_MAX_OBJECTS; i++)
		{
			treeObjects[i] = -1;
		}
	}

	Octree(glm::vec3 origin, BBox boundingBox, int index)
		: origin(origin), boundingBox(boundingBox), numberOfObjects(0), index(index)
	{
		for (int i = 0; i < 8; i++)
		{
			childrenIndices[i] = -1;
		}
		for (int i = 0; i < OCTREE_MAX_OBJECTS; i++)
		{
			treeObjects[i] = -1;
		}
	}

	~Octree()
	{

	}

	inline bool isLeaf()
	{
		return childrenIndices[0] == -1;
	}
	
	BBox boundingBox;
	int childrenIndices[8];
	int treeObjects[OCTREE_MAX_OBJECTS];
	glm::vec3 origin;
	int index;
	int numberOfObjects;



	void add(Object object, std::vector<Octree> & nodes)
	{
		int nodeIndex = index;
		nodes[nodeIndex].numberOfObjects++;
		if(nodes[nodeIndex].isLeaf())
		{
			if(nodes[nodeIndex].numberOfObjects-1 >= OCTREE_MAX_OBJECTS)
			{
				int previousSize = nodes.size();
				//nodes.reserve(nodes.size() + 8);

				int j = previousSize;
				for (int i = 0; i < 8; i++)
				{

					glm::vec3 octantHalfDimension = glm::abs(nodes[nodeIndex].boundingBox.max - nodes[nodeIndex].boundingBox.min);
					octantHalfDimension /= 2.0f;
					
					octantHalfDimension.x = octantHalfDimension.x *  (i&4 ? 1.0f : -1.0f);
					octantHalfDimension.y = octantHalfDimension.y *  (i&2 ? 1.0f : -1.0f);
					octantHalfDimension.z = octantHalfDimension.z *  (i&1 ? 1.0f : -1.0f);

					glm::vec3 newBoxDimension = nodes[nodeIndex].origin + octantHalfDimension;

					BBox box = {glm::min(nodes[nodeIndex].origin,newBoxDimension) ,0.0f,glm::max(nodes[nodeIndex].origin,newBoxDimension), 0.0f};
					glm::vec3 newOrigin = nodes[nodeIndex].origin + glm::abs(box.max - box.min)/2.0f;
					Octree o(newOrigin,box,previousSize + i);
					//cout<< "Push Back" << endl; 
					nodes.push_back(o);
					nodes[nodeIndex].childrenIndices[i] = previousSize + i;
					//cout<< "If Collision" << endl; 

					if(objectBoxCollided(nodes[previousSize + i].boundingBox,object))
					{
						//cout<< "Add Object" << endl; 

						nodes[previousSize + i].addObject(object);
					}
				}

			}
			else
			{

				nodes[nodeIndex].treeObjects[nodes[nodeIndex].numberOfObjects-1] = object.index;
			}
		}
		else
		{
			for (int i = 0; i < 8; i++)
			{
				if(objectBoxCollided(nodes[nodes[nodeIndex].childrenIndices[i]].boundingBox,object))
				{
					nodes[nodes[nodeIndex].childrenIndices[i]].add(object,nodes);
				}
			}
		}
	}

	void addObject(Object o)
	{
		treeObjects[numberOfObjects++] = o.index;
	}

	void add(const std::vector<Object> & objects , std::vector<Octree> & nodes)
	{

		for (int i = 0; i < objects.size(); i++)
		{
			add(objects[i], nodes);
		}
	}

};

struct OctreeManager
{
	OctreeManager(Octree root) //: root(root)
	{
		octreeNodes.push_back(root);
	}

	//Octree root;
	std::vector<Octree> octreeNodes;
	void insert(const std::vector<Object> & objects )
	{
		for (int i = 0; i < objects.size(); i++)
		{
			octreeNodes[0].add(objects[i],octreeNodes);
		}	
	}
};