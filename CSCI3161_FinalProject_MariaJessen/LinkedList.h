#pragma once
/*

CSCI3161 FINAL PROJECT
MARIA JESSEN B00743170

Linkedlist class. Used to store points and faces

*/
struct PointNode {
	struct Point point;
	struct PointNode* next;
};

/// <summary>
/// Add a point to the list of points (used for making faces)
/// </summary>
struct PointNode* insertPoint(struct PointNode* list, struct Point newPoint) {
	struct PointNode* newNode = malloc(sizeof(struct PointNode));

	if (newNode == NULL) {
		return list;
	}

	// store the vertex values
	newNode->point.vertex_x = newPoint.vertex_x;
	newNode->point.vertex_y = newPoint.vertex_y;
	newNode->point.vertex_z = newPoint.vertex_z;

	// store the normal values
	newNode->point.normal_x = newPoint.normal_x;
	newNode->point.normal_y = newPoint.normal_y;
	newNode->point.normal_z = newPoint.normal_z;

	newNode->next = list; // insert point node at the front of the list

	return newNode;
}

struct FaceNode {
	struct PointNode* pointList;
	struct FaceNode* next;
};

/// <summary>
/// Add a new face node to a list of faces (used for making components)
/// </summary>
struct FaceNode* insertFace(struct FaceNode* list, struct PointNode* pointList) {
	struct FaceNode* newNode = malloc(sizeof(struct FaceNode));

	if (newNode == NULL) {
		return list;
	}

	newNode->pointList = pointList; // store linkedlist of points inside this node
	newNode->next = list; // insert this node at the head of the list

	return newNode;
}