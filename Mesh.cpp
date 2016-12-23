#include "Mesh.h"

Mesh::Mesh(void)	{
	type = PRIMITIVE_POINTS;

	numVertices = 0;

	vertices = NULL;
	colours = NULL;
	textureCoords = NULL;
}

Mesh::~Mesh(void)	{
	delete[] vertices;
	delete[] colours;
	delete[] textureCoords;
}

/*//////////////////////////////////////////////////////////
//**********	GENERATE LINE	****************************
*///////////////////////////////////////////////////////////

Mesh* Mesh::GenerateLine(const Vector3 &from, const Vector3 &to){
	Mesh* m = new Mesh(); // initiaises a new mesh

	m->numVertices = 2; // sets the vertex count to 2

	m->vertices = new Vector4[m->numVertices]; // initialises enough memory for 2 vertex positions
	m->colours = new Colour[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];

	m->vertices[0] = Vector4(from.x, from.y, from.z, 1.0f); // set vertexes to values sent to them in parameters
	m->vertices[1] = Vector4(to.x, to.y, to.z, 1.0f); // ...

	m->colours[0] = Colour(255, 0, 0, 255);
	m->colours[1] = Colour(0, 0, 255, 255);

	m->type = PRIMITIVE_LINES; //before returning the mesh set it to its primitive type. In this case it is a line. This way the rasterizer knows which pipeline to send it to.

	return m;
}

/*//////////////////////////////////////////////////////////
//**********	GENERATE POINTS	****************************
*///////////////////////////////////////////////////////////

Mesh* Mesh::GeneratePoints(std::vector<Vector3> v){
	Mesh* m = new Mesh();
	m->numVertices = v.size(); // sets the vertex count to 2
	m->vertices = new Vector4[m->numVertices]; // initialises enough memory vertex positions
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Colour[m->numVertices];
	for (int i = 0; i < v.size(); i++){
		m->vertices[i] = Vector4(v[i].x, v[i].y, v[i].z, 1.0f);
	}
	m->type = PRIMITIVE_POINTS; //before returning the mesh set it to its primitive type. In this case it is a line. This way the rasterizer knows which pipeline to send it to.
	return m;
}

/*//////////////////////////////////////////////////////////
//**********	GENERATE LINE STRIP	************************
*///////////////////////////////////////////////////////////

Mesh* Mesh::GenerateLineStrip(std::vector<Vector3> v){
		Mesh* m = new Mesh();
		m->numVertices = v.size(); // sets the vertex count
		m->vertices = new Vector4[m->numVertices]; // initialises enough memory vertex positions
		m->textureCoords = new Vector2[m->numVertices];
		m->colours = new Colour[m->numVertices];
		for (int i = 0; i < v.size(); i++){
			m->vertices[i] = Vector4(v[i].x, v[i].y, v[i].z, 1.0f);
			m->colours[i] = Colour(255, 255, 255, 255);
		}
		m->type = PRIMITIVE_LINE_STRIPS; //before returning the mesh set it to its primitive type. In this case it is a line. This way the rasterizer knows which pipeline to send it to.
		return m;

	
}

/*//////////////////////////////////////////////////////////
//**********	GENERATE LINE LOOP	************************
*///////////////////////////////////////////////////////////

Mesh* Mesh::GenerateLineLoop(std::vector<Vector3> v) {
		Mesh* m = new Mesh();
		m->numVertices = v.size(); // sets the vertex count
		m->vertices = new Vector4[m->numVertices]; // initialises enough memory vertex positions
		m->textureCoords = new Vector2[m->numVertices];
		m->colours = new Colour[m->numVertices];

		for (int i = 0; i < v.size(); i++) {
			m->vertices[i] = Vector4(v[i].x, v[i].y, v[i].z, 1.0f);
			m->colours[i] = Colour(255, 255, 255, 255);
		}
		m->type = PRIMITIVE_LINE_LOOPS; //before returning the mesh set it to its primitive type. In this case it is a line. This way the rasterizer knows which pipeline to send it to.
		return m;
}


/*//////////////////////////////////////////////////////////
//**********	GENERATE SPACE DEBRIS	********************
*///////////////////////////////////////////////////////////
Mesh* Mesh::GenerateDebris() {

	Mesh* m = new Mesh();

	m->numVertices = 9;

	m->vertices = new Vector4[m->numVertices];
	m->colours = new Colour[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];


	m->vertices[0] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	m->vertices[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->vertices[2] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->vertices[3] = Vector4(1.0f, -1.5f, 0.0f, 1.0f);
	m->vertices[4] = Vector4(2.0f, 0.5f, 0.0f, 1.0f);
	m->vertices[5] = Vector4(2.0f, -1.0f, 0.0f, 1.0f);
	m->vertices[6] = Vector4(3.5f, 0.5f, 0.0f, 1.0f);
	m->vertices[7] = Vector4(4.0f, -0.5f, 0.0f, 1.0f);
	m->vertices[8] = Vector4(5.0f, 2.0f, 0.0f, 1.0f);
	
	m->colours[0] = Colour(192, 192, 192, 255); // grey
	m->colours[1] = Colour(192, 192, 192, 255); // grey
	m->colours[2] = Colour(0, 0, 0, 125); // black
	m->colours[3] = Colour(192, 192, 192, 255); // grey
	m->colours[4] = Colour(192, 192, 192, 255); // grey
	m->colours[5] = Colour(0, 0, 0, 125); // black
	m->colours[6] = Colour(192, 192, 192, 255); // grey
	m->colours[7] = Colour(0, 0, 0, 125); // black
	m->colours[8] = Colour(192, 192, 192, 255); // grey

	m->type = PRIMITIVE_TRISTRIP;
	return m;
}

/*//////////////////////////////////////////////////////////
//**********	GENERATE ROCK FAN	************************
*///////////////////////////////////////////////////////////

Mesh* Mesh::GenerateRock() {

	Mesh* m = new Mesh();

	m->numVertices = 10; 	

	m->vertices = new Vector4[m->numVertices];
	m->colours = new Colour[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];

	m->vertices[0] = Vector4(0.0f, 0.0f , 0.0f, 1.0f);
	m->vertices[1] = Vector4(1.0f, 3.0f, 0.0f, 1.0f);

	m->vertices[2] = Vector4(-1.0f, 3.0f, 0.0f, 1.0f);
	m->vertices[3] = Vector4(-3.0f, 1.0f, 0.0f, 1.0f);

	m->vertices[4] = Vector4(-3.0f, -1.0f, 0.0f, 1.0f);
	m->vertices[5] = Vector4(-1.0f, -3.0f, 0.0f, 1.0f);

	m->vertices[6] = Vector4(1.0f,-3.0f, 0.0f, 1.0f);
	m->vertices[7] = Vector4(3.0f, -1.0f, 0.0f, 1.0f);

	m->vertices[8] = Vector4(3.0f, 1.0f, 0.0f, 1.0f);
	m->vertices[9] = Vector4(1.0f, 3.0f, 0.0f, 1.0f);

	m->textureCoords[0] = Vector2(0.5f,0.5f);
	m->textureCoords[1] = Vector2(TWO_THIRD, 0.0f);

	m->textureCoords[2] = Vector2(THIRD, 0.0f);
	m->textureCoords[3] = Vector2(0.0f, THIRD);	
	
	m->textureCoords[4] = Vector2(0.0f, TWO_THIRD);
	m->textureCoords[5] = Vector2(THIRD, 1.0f);

	m->textureCoords[6] = Vector2(TWO_THIRD, 1.0f);
	m->textureCoords[7] = Vector2(1.0f, TWO_THIRD);

	m->textureCoords[8] = Vector2(1.0f, THIRD);
	m->textureCoords[9] = Vector2(TWO_THIRD, 0);

	m->type = PRIMITIVE_TRIFAN;

	return m;
}

/*//////////////////////////////////////////////////////////
//**********	GENERATE SUN FAN	************************
*///////////////////////////////////////////////////////////

Mesh* Mesh::GenerateSun(Colour * c) {

	Mesh* m = new Mesh();

	m->numVertices = 10;

	m->vertices = new Vector4[m->numVertices];
	m->colours = new Colour[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];

	m->vertices[0] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	m->vertices[1] = Vector4(1.0f, 3.0f, 0.0f, 1.0f);

	m->vertices[2] = Vector4(-1.0f, 3.0f, 0.0f, 1.0f);
	m->vertices[3] = Vector4(-3.0f, 1.0f, 0.0f, 1.0f);

	m->vertices[4] = Vector4(-3.0f, -1.0f, 0.0f, 1.0f);
	m->vertices[5] = Vector4(-1.0f, -3.0f, 0.0f, 1.0f);

	m->vertices[6] = Vector4(1.0f, -3.0f, 0.0f, 1.0f);
	m->vertices[7] = Vector4(3.0f, -1.0f, 0.0f, 1.0f);

	m->vertices[8] = Vector4(3.0f, 1.0f, 0.0f, 1.0f);
	m->vertices[9] = Vector4(1.0f, 3.0f, 0.0f, 1.0f);

	for (int i = 0; i < 10; i++) {
		m->colours[i] = (c[i]);
	}
	
	m->type = PRIMITIVE_TRIFAN;

	return m;
}

/*//////////////////////////////////////////////////////////
//**********	GENERATE TRIANGLE	************************
*///////////////////////////////////////////////////////////

Mesh* Mesh::GenerateTriangle() {
	Mesh* m = new Mesh();
	
	m->numVertices = 3;

	m->vertices = new Vector4[m->numVertices];
	m->colours = new Colour[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];

	m->vertices[0] = Vector4(0.5f, -0.5f, 0, 1.0f);
	m->vertices[1] = Vector4(0.0f, 0.5f, 0, 1.0f);
	m->vertices[2] = Vector4(-0.5f, -0.5f, 0, 1.0f);

	m->colours[0] = Colour(255, 0, 0, 255); // red
	m->colours[1] = Colour(0, 255, 0, 255); // Green
	m->colours[2] = Colour(0, 0, 255, 255); // Blue

	m->textureCoords[0] = Vector2(0.0f,0.0f);
	m->textureCoords[1] = Vector2(0.5f, 1.0f);
	m->textureCoords[2] = Vector2(1.0f, 0.0f);

m->type = PRIMITIVE_TRIANGLES;

	return m;
}

/*//////////////////////////////////////////////////////////
//**********	LOAD MESH	********************************
*///////////////////////////////////////////////////////////

Mesh * Mesh::LoadMeshFile(const string &filename) {
	ifstream f(filename);

	if (!f) {
		return NULL;
	}

	Mesh*m = new Mesh();
	m->type = PRIMITIVE_TRIANGLES;
	f >> m->numVertices;

	int hasTex = 0;
	int hasColour = 0;

	f >> hasTex;
	f >> hasColour;

	m->vertices = new Vector4[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Colour[m->numVertices];

	for (int i = 0; i < m->numVertices; i++) {
		f >> m->vertices[i].x;
		f >> m->vertices[i].y;
		f >> m->vertices[i].z;
	}

	if (hasColour) {
		for (int i = 0; i < m->numVertices; ++i) {
			int r, g, b, a;
			f >> r;
			f >> g;
			f >> b;
			f >> a;
			m->colours[i].r = r;
			m->colours[i].g = g;
			m->colours[i].b = b;
			m->colours[i].a = a;
		}
	} else {
		for (uint i = 0; i < m->numVertices; ++i) {
			m->colours[i] = Colour(255, 255, 255, 255);
		}
	}
	if (hasTex) {
		for (unsigned int i = 0; i < m->numVertices; ++i) {
			f >> m->textureCoords[i].x;
			f >> m->textureCoords[i].y;
		}
	}
	return m;

}
