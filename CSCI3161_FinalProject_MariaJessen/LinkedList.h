#pragma once
struct PointNode {
	struct Point point;
	struct PointNode* next;
};

// Add a point to the list of points (used for making faces)
struct PointNode* insertPoint(struct PointNode* list, struct Point newPoint) {
	struct PointNode* newNode = malloc(sizeof(struct PointNode));

	if (newNode == NULL) {
		return list;
	}

	newNode->point.vertex_x = newPoint.vertex_x;
	newNode->point.vertex_y = newPoint.vertex_y;
	newNode->point.vertex_z = newPoint.vertex_z;

	newNode->point.normal_x = newPoint.normal_x;
	newNode->point.normal_y = newPoint.normal_y;
	newNode->point.normal_z = newPoint.normal_z;

	newNode->next = list;

	return newNode;
}

struct FaceNode {
	struct PointNode* pointList;
	struct FaceNode* next;
};

// Add a new face node to a list of faces (used for making components)
struct FaceNode* insertFace(struct FaceNode* list, struct PointNode* pointList) {
	struct FaceNode* newNode = malloc(sizeof(struct FaceNode));

	if (newNode == NULL) {
		return list;
	}

	newNode->pointList = pointList;
	newNode->next = list;

	return newNode;
}