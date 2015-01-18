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
static int MAX_CHILD_START ;
static int DEPTH_REACHED;
struct Octree
{

	Octree() : numberOfObjects(0), depth(0)
	{
		childrenStart = -1;
	
		for (int i = 0; i < OCTREE_MAX_OBJECTS; i++)
		{
			treeObjects[i] = -1;
		}
	}
	
	Octree(glm::vec3 origin, BBox boundingBox, int index)
		: origin(origin), boundingBox(boundingBox), numberOfObjects(0), index(index),depth(0)
	{
		childrenStart = -1;
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
		return childrenStart < 0;
	}
	
	BBox boundingBox;
	int treeObjects[OCTREE_MAX_OBJECTS];
	glm::vec3 origin;
	int index;
	int numberOfObjects;
	int childrenStart;
	int depth;


	/*
	typedef struct
{
	float3 direction;
	float3 position;
	float3 color;
	float distance;
	int objectIndex;
	int type;
}IntersectionInfo;


typedef struct 
{
	float minValue;
	float maxValue;
	bool hit;
}HitReturn;
	*/


	void add(const Object object,const std::vector<Object> objects, const std::vector<Mesh> & meshes,std::vector<Octree> & nodes)
	{
		int nodeIndex = index;
		nodes[nodeIndex].numberOfObjects++;
		if(nodes[nodeIndex].isLeaf())
		{
			if(nodes[nodeIndex].numberOfObjects-1 >= OCTREE_MAX_OBJECTS)
			{
				int previousSize = nodes.size();
				//nodes.reserve(nodes.size() + 8);

				//int j = previousSize;

				nodes[nodeIndex].childrenStart = previousSize;
				MAX_CHILD_START =glm::max(nodes[nodeIndex].childrenStart, MAX_CHILD_START);//(nodes[nodeIndex].childrenStart > MAX_CHILD_START)  ? nodes[nodeIndex].childrenStart : MAX_CHILD_START;
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
					o.depth = nodes[previousSize-1].depth + 1;
					o.index = previousSize + i;
					DEPTH_REACHED = glm::max(DEPTH_REACHED,o.depth);
					//cout<< "Push Back" << endl; 
					nodes.push_back(o);
					
					//cout<< "If Collision" << endl; 

					for (int objectI = 0; objectI < OCTREE_MAX_OBJECTS; objectI++)
					{
						int objectIndex = nodes[nodeIndex].treeObjects[objectI];
						if(objectBoxCollided(nodes[nodes.size()-1].boundingBox,objects[objectIndex] , meshes[objects[objectIndex].meshIndex]))
						{
							nodes[nodes.size()-1].add(objects[objectIndex], objects,meshes,nodes);
						}
					}

					if(objectBoxCollided(nodes[previousSize + i].boundingBox,object , meshes[object.meshIndex]))
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
				if(objectBoxCollided(nodes[nodes[nodeIndex].childrenStart + i].boundingBox,object,meshes[object.meshIndex]))
				{
					nodes[nodes[nodeIndex].childrenStart + i].add(object,objects,meshes,nodes);
				}
			}
		}
	}

	void addObject(Object o)
	{
		treeObjects[numberOfObjects++] = o.index;
	}

	void add(const std::vector<Object> & objects , const std::vector<Mesh> & meshes , std::vector<Octree> & nodes)
	{

		for (int i = 0; i < objects.size(); i++)
		{
			add(objects[i], objects,meshes, nodes);
		}
	}

};

struct OctreeManager
{
	OctreeManager(Octree root) //: root(root)
	{
		octreeNodes.push_back(root);
	}

	OctreeManager() //: root(root)
	{
		
	}

	//Octree root;
	std::vector<Octree> octreeNodes;
	void insert(const std::vector<Object> & objects , const std::vector<Mesh> & meshes )
	{
		for (int i = 0; i < objects.size(); i++)
		{
			octreeNodes[0].add(objects[i],objects, meshes,octreeNodes);
		}	
	}
};