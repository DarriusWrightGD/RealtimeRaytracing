#pragma once

#include <BoundingBox\BBox.h>
#include <Objects\Object.h>
#include <Objects\Triangle.h>
#include <Objects\Mesh.h>

Object inline createObjectFromTriangle(Triangle triangle, glm::vec3 position)
{
	BBox box  = {glm::min(glm::min(triangle.v0, triangle.v1), triangle.v2),0.0f, glm::max(glm::max(triangle.v0, triangle.v1), triangle.v2), 0.0f};
	Object retObj;

	retObj.box = box;
	//retObj.position = glm::vec4(position,0.0f);


	return retObj;
}

bool inline bboxCollided(BBox b1 , BBox b2)
{
	return (b1.min.x <= b2.min.x + fabs(b2.min.x - b2.max.x) &&
		b2.min.x <= b1.min.x + fabs(b1.min.x - b1.max.x) &&

		b1.min.y <= b2.min.y + fabs(b2.min.y - b2.max.y) &&
		b2.min.y <= b1.min.y + fabs(b1.min.y - b1.max.y) &&

		b1.min.z <= b2.min.z + fabs(b2.min.z - b2.max.z) &&
		b2.min.z <= b1.min.z + fabs(b1.min.z - b1.max.z)) 
	;
}

bool inline objectBoxCollided(BBox b1 , Object b2,const  Mesh m)
{
	b2.box.min[0] +=m.position.s[0];
	b2.box.min[1] +=m.position.s[1];
	b2.box.min[2] +=m.position.s[2];
							  
	b2.box.max[0] +=m.position.s[0];
	b2.box.max[1] +=m.position.s[1];
	b2.box.max[2] +=m.position.s[2];
							  

	return bboxCollided(b1, b2.box);
}




