/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description: Class to represent the geometric data that makes up the meshes
we render on screen.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector4.h"
#include "Colour.h"
#include <vector>
#include "Vector3.h"
#include "Vector2.h"
#include "Common.h"

#include <string>
#include <fstream>

#define THIRD 0.33333333f		// used when wrapping textures in tri fan
#define TWO_THIRD 0.66666666f

using std::ifstream;
using std::string;

enum PrimitiveType {
	PRIMITIVE_POINTS,
	PRIMITIVE_LINES,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_TRIFAN,
	PRIMITIVE_LINE_STRIPS,
	PRIMITIVE_LINE_LOOPS,
	PRIMITIVE_TRISTRIP
};

class Mesh	{
	friend class SoftwareRasteriser;
public:
	Mesh(void);
	~Mesh(void);

	
	static Mesh*	GenerateLine(const Vector3 &from, const Vector3 &to);
	static Mesh*    GeneratePoints(std::vector<Vector3> v);

	// MODIFICATION GEOFF

	static Mesh*	GenerateTriangle();
	static Mesh*	GenerateFanTriangles(std::vector<Vector3> v);
	static Mesh*    GenerateLineStrip(std::vector<Vector3> v);
	static Mesh*    GenerateLineLoop(std::vector<Vector3> v);
	static Mesh*	Mesh::LoadMeshFile(const string &filename);
	static Mesh*	GenerateRock();
	static Mesh*	GenerateDebris();
	static Mesh*	GenerateSun(Colour *);
	// MODIFICATION GEOFF END



PrimitiveType	GetType() { return type;}

protected:
	PrimitiveType	type;

	uint			numVertices;

	Vector4*		vertices;
	Colour*			colours;
	Vector2*		textureCoords;	//We get onto what to do with these later on...

	
};

