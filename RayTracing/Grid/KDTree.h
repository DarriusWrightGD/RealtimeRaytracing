#include <vector>
#include <Objects\Object.h>
#include <Objects\Mesh.h>
#include <Helper.h>

static const int NUMBER_OF_OBJECTS = 5;
static const int DIMENSIONS = 3;
static const float COST_OF_TRAVERSAL = 1.0;
static const float COST_OF_INTERSECTION = 1.5;
static const int MAX_KD_DEPTH = 20;


struct KDTreeInfo
{
	std::vector<int> objects;
};

struct KDTreeNode
{
	BBox boundingBox;
	int nodes[2];
	int startingIndex; // if internal node then starting index == -1
	int numberOfObjects;
	float d[1];
	int parent;
	int index;
	int depth;

	KDTreeNode(BBox boundingBox, int depth, int index) : numberOfObjects(0) , depth(depth) , boundingBox(boundingBox), index(index)
	{
		startingIndex = -1;
		nodes[0] = nodes[1] = -1;
	}

	void buildMidPoint(std::vector<KDTreeNode> & nodes,std::vector<KDTreeInfo> & nodeInfo, 
		const std::vector<Mesh> meshes,const std::vector<Object> & objects , std::vector<int> & leafIndices,
		int nodeIndex)
	{
		int index = nodes.size();

		BBox box;

		if(nodeInfo[index].objects.size() == 0) return;
		if(nodeInfo[index].objects.size() == 1) 
		{

			return;
		}


		glm::vec3 midPoint;
		for (int i = 0; i < nodeInfo[index].objects.size(); i++)
		{
			box.expand(objects[nodeInfo[index].objects[i]].box);
			
		}

		nodes.push_back(KDTreeNode(box,depth,index));
	}

	void buildSAH(std::vector<KDTreeNode> & nodes,std::vector<KDTreeInfo> & nodeInfo, 
		const std::vector<Mesh> meshes,const std::vector<Object> & objects , std::vector<int> & leafIndices,
		int nodeIndex)
	{
		//if stop met done

		if(stopCreation())
		{
			startingIndex = leafIndices.size();
			for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
			{
				leafIndices.push_back(nodeInfo[nodeIndex].objects[i]);
			}

			return;
		}
		
		// find optimal split position
		float bestPosition = -1;
		float bestCost = 10000000.0f;
		int axis = depth%DIMENSIONS;

		for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
		{
			float min = objects[nodeInfo[nodeIndex].objects[i]].box.min[axis];
			float max = objects[nodeInfo[nodeIndex].objects[i]].box.max[axis];

			float cost = findCost(min,objects, nodeInfo[nodeIndex].objects,meshes);
			if(cost < bestCost){bestCost = cost; bestPosition = min;}
			cost = findCost(max,objects, nodeInfo[nodeIndex].objects, meshes);
			if(cost < bestCost){bestCost = cost; bestPosition = max;}
		}

		//glm::vec3 halfDimension;

		//for (int i = 0; i < DIMENSIONS; i++)
		//{
		//	if(axis != i)
		//		halfDimension[i] = boundingBox.max[i];
		//}

		//halfDimension[axis] = bestPosition;

		glm::vec3 minHalf = boundingBox.max;
		glm::vec3 maxHalf = boundingBox.min;

		minHalf[axis] = bestPosition;
		maxHalf[axis] = bestPosition;


		BBox left = {boundingBox.min , 0.0f , minHalf};
		BBox right = {maxHalf , 0.0f , boundingBox.max};
		KDTreeNode leftNode(left,depth + 1, nodes.size() ); 
		KDTreeNode rightNode(right,depth + 1, nodes.size() + 1);

		KDTreeInfo leftInfo;
		KDTreeInfo rightInfo;

		leftInfo.objects.reserve(nodeInfo[nodeIndex].objects.size());
		rightInfo.objects.reserve(nodeInfo[nodeIndex].objects.size());

		for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
		{

			Object currentObject = objects[nodeInfo[nodeIndex].objects[i]];

			if(objectBoxCollided(left,currentObject,meshes[currentObject.meshIndex]))
			{
				leftInfo.objects.push_back(nodeInfo[nodeIndex].objects[i]);
			}

			if(objectBoxCollided(right,currentObject,meshes[currentObject.meshIndex]))
			{
				rightInfo.objects.push_back(nodeInfo[nodeIndex].objects[i]);
			}
		}

		leftNode.numberOfObjects = leftInfo.objects.size();
		rightNode.numberOfObjects = rightInfo.objects.size();

		nodeInfo.push_back(leftInfo);
		nodeInfo.push_back(rightInfo);
		
		int leftIndex = nodes.size();
		nodes.push_back(leftNode);
		int rightIndex = nodes.size(); 
		nodes.push_back(rightNode);

		
		nodes[nodeIndex].nodes[0] =  leftIndex;
		nodes[nodeIndex].nodes[1] =  rightIndex;

		nodes[leftIndex].build(nodes,nodeInfo,meshes,objects,leafIndices ,leftIndex);//(objects,nodes,nodeInfo);
		nodes[rightIndex].build(nodes,nodeInfo,meshes,objects, leafIndices,rightIndex);//.build(objects,nodes,nodeInfo);
	}


	void build(std::vector<KDTreeNode> & nodes,std::vector<KDTreeInfo> & nodeInfo, 
		const std::vector<Mesh> meshes,const std::vector<Object> & objects , std::vector<int> & leafIndices,
		int nodeIndex)
	{
		//if stop met done

		if(stopCreation(nodes[nodeIndex]))
		{
			nodes[nodeIndex].startingIndex = leafIndices.size();
			for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
			{
				leafIndices.push_back(nodeInfo[nodeIndex].objects[i]);
			}

			return;
		}
		
	
		int axis = depth%DIMENSIONS;


		glm::vec3 minHalf = boundingBox.max;
		glm::vec3 maxHalf = boundingBox.min;

		float halfPoint = ((boundingBox.max[axis] - boundingBox.min[axis]) /2.0f) + boundingBox.min[axis];


		minHalf[axis] = halfPoint;
		maxHalf[axis] = halfPoint;

		BBox left = {boundingBox.min , 0.0f , minHalf};
		BBox right = {maxHalf , 0.0f , boundingBox.max};
		KDTreeNode leftNode(left,depth + 1, nodes.size() ); 
		KDTreeNode rightNode(right,depth + 1, nodes.size() + 1);

		KDTreeInfo leftInfo;
		KDTreeInfo rightInfo;
		
		leftInfo.objects.reserve(nodeInfo[nodeIndex].objects.size());
		rightInfo.objects.reserve(nodeInfo[nodeIndex].objects.size());

		for (int i = 0; i < nodeInfo[nodeIndex].objects.size(); i++)
		{

			Object currentObject = objects[nodeInfo[nodeIndex].objects[i]];

			if(objectBoxCollided(left,currentObject,meshes[currentObject.meshIndex]))
			{
				leftInfo.objects.push_back(nodeInfo[nodeIndex].objects[i]);
			}

			if(objectBoxCollided(right,currentObject,meshes[currentObject.meshIndex]))
			{
				rightInfo.objects.push_back(nodeInfo[nodeIndex].objects[i]);
			}
		}

		leftNode.numberOfObjects = leftInfo.objects.size();
		rightNode.numberOfObjects = rightInfo.objects.size();

		leftNode.parent = nodeIndex;
		rightNode.parent = nodeIndex;

		nodeInfo.push_back(leftInfo);
		nodeInfo.push_back(rightInfo);

		int leftIndex = nodes.size();
		nodes.push_back(leftNode);
		int rightIndex = nodes.size(); 
		nodes.push_back(rightNode);

		
		nodes[nodeIndex].nodes[0] =  leftIndex;
		nodes[nodeIndex].nodes[1] =  rightIndex;

		nodes[leftIndex].build(nodes,nodeInfo,meshes,objects,leafIndices ,leftIndex);///(objects,nodes,nodeInfo);
		nodes[rightIndex].build(nodes,nodeInfo,meshes,objects, leafIndices,rightIndex);//.build(objects,nodes,nodeInfo);
	}



	bool stopCreation()
	{
		return numberOfObjects <= NUMBER_OF_OBJECTS || depth >= MAX_KD_DEPTH;
	}

	bool stopCreation(KDTreeNode node)
	{
		return node.numberOfObjects <= NUMBER_OF_OBJECTS || node.depth >= MAX_KD_DEPTH;
	}

	float leftCost(float splitPosition, const std::vector<Object> & objects , 
		const std::vector<int> & objectIndices, const std::vector<Mesh> meshes)
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

		int nObjects = 0;
		for (int i = 0; i < objectIndices.size(); i++)
		{
			

			if(objectBoxCollided(box,objects[objectIndices[i]],meshes[objects[objectIndices[i]].meshIndex]))
			{
				nObjects++;
			}
		}

		//formula correct...
		return  area * nObjects ;
	}

	float rightCost(float splitPosition, const std::vector<Object> & objects
		, const std::vector<int> & objectIndices, const std::vector<Mesh> meshes)
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

		int nObjects = 0;
		for (int i = 0; i < objectIndices.size(); i++)
		{
			if(objectBoxCollided(box,objects[objectIndices[i]],meshes[objects[objectIndices[i]].meshIndex] ))
			{
				nObjects++;
			}
		}


		return area * nObjects ;
	}

	float findCost(float splitPosition , const std::vector<Object> & objects,
		const std::vector<int> & objectIndices, const std::vector<Mesh> meshes)
	{
		return COST_OF_TRAVERSAL + COST_OF_INTERSECTION * (leftCost(splitPosition, objects, objectIndices,meshes) + rightCost(splitPosition,objects, objectIndices, meshes));
	}

};



struct KDTreeManager
{
	KDTreeManager(KDTreeNode root)
	{
		root.index = 0;
		root.depth = 0;
		root.parent = -1;
		nodes.push_back(root);
	}

	KDTreeManager()
	{

	}


	std::vector<KDTreeNode> nodes;
	std::vector<KDTreeInfo> nodesInfo;
	std::vector<int> leafObjectIndices;

	void insert(const  std::vector<Object> & objects , const std::vector<Mesh> & mesh)
	{
		KDTreeInfo node;
		node.objects.reserve(objects.size());
		for (int i = 0; i < objects.size(); i++)
		{
			node.objects.push_back(i);
		}

		nodesInfo.push_back(node);

		nodes[0].numberOfObjects = objects.size();
		nodes[0].build(nodes,nodesInfo,mesh,objects,leafObjectIndices,0);
	}
};