#include "SoftwareRasteriser.h"

#include "Mesh.h"
#include "Texture.h"
#include <vector>
#include <cstdlib>
#include <ctime>.

#define RED_SUN 1
#define BLUE_SUN 2

Colour* getSunColour(int s);
float getRandomFloat(float LOW, float HIGH);

int main() {
	
	/*//////////////////////////////////////////////////////////
	//**********	SCENE VARIABLES	****************************
	*///////////////////////////////////////////////////////////
	const int SCREEN_WIDTH = 320;
	const int SCREEN_HEIGHT = 240;
	const int RATIO = SCREEN_WIDTH / SCREEN_HEIGHT;
	const float MOVE_SCALE = 15.0f;
	const int MAX = 200;
	const int MIN = -200;
	const int ZMIN = -300;
	const int STARS = 1500;
	const float STAR_DISTANCE = -300.0f;
	const int ASTEROIDS = 10;
	const int AST_SIDES = 5;
	const int AST_SIZE = 4;
	const int AST_FIELD_MAX = 200;
	const int AST_FIELD_MIN = -100;
	const int AST_FIELD_DEPTH = -200;
	const int SUN_COLOUR = RED_SUN;
	const int DEBRIS_AMT = 10;
	const float SUN_X = 30;
	const float SUN_Y = -10;
	const float SUN_Z = -100;
	const int SUN_SCALE = 20;
	const int SUN_SIZE = 6 * SUN_SCALE;
	// upper bounds for debris variables
	const float DEBRIS_SPEED_BOUND = 20.0f;
	const float DEBRIS_X_BOUND =20.0f;
	const float DEBRIS_Y_BOUND = 20.0f;
	const float DEBRIS_Z_BOUND = 200.0f;
	const float DEBRIS_ROTATION_BOUND = 5.0f;

	const float COMET_ROTATION = 10.0f;
	const float COMET_SPEED = -5.0f;


	/*//////////////////////////////////////////////////////////
	//**********	SETUP	************************************
	*///////////////////////////////////////////////////////////

	SoftwareRasteriser r(SCREEN_WIDTH, SCREEN_HEIGHT); // make window canvas to draw in
	srand(static_cast <unsigned> (time(0))); // seed the generator 


	/*//////////////////////////////////////////////////////////
	//**********	GENERATE STARS	****************************
	*///////////////////////////////////////////////////////////

	// generates a load of points between defined amount ransomly and puts them in a vector

	std::vector <Vector3> starmap_vectors;
	for (int i = 0; i < STARS; i++) {
		float x = rand() % (MAX - MIN + 1) + MIN;
		float y = rand() % (MAX - MIN + 1) + MIN;
		float z = rand() % (MAX - ZMIN + 1) + ZMIN;
		starmap_vectors.push_back(Vector3(x, y, z));
	}

	Mesh* starmap_mesh = Mesh::GeneratePoints(starmap_vectors);
	RenderObject * starmap = new RenderObject();
	starmap->mesh = starmap_mesh;
	starmap->modelMatrix = Matrix4::Translation(Vector3(0, 0, STAR_DISTANCE));

	

	/*//////////////////////////////////////////////////////////
	//**********	GENERATE SUN	****************************
	*///////////////////////////////////////////////////////////
	Colour * c = getSunColour(SUN_COLOUR);
	Mesh* sun_mesh = Mesh::GenerateSun(c);
	RenderObject * sun = new RenderObject();
	sun->mesh = sun_mesh;


	/*//////////////////////////////////////////////////////////
	//**********	GENERATE ASTEROIDS	************************
	*///////////////////////////////////////////////////////////

	// loops through the degrees of hte asteroid and picks a random number on that axis that is more than the origin and less than the maxiumum size of an asteroid. 
	// Could improve this by programatically dividing 
	//py a defined number of sides and iterating round the circle. You would be able to define how many edges the asteroid would ahve that way.

	RenderObject* arr = new RenderObject[ASTEROIDS];

	for (int i = 0; i < ASTEROIDS; i++) {
		std::vector <Vector3> v;
		// get the centre of new asteroid
		int origin_x = rand() % (AST_FIELD_MAX)+AST_FIELD_MIN;
		int origin_y = rand() % (AST_FIELD_MAX)+AST_FIELD_MIN;
		int origin_z = rand() % (-20) + AST_FIELD_DEPTH;
		float x, y, z;

		// 0 degrees
		y = rand() % ((origin_y + AST_SIZE) - origin_y + 1) + origin_y;
		v.push_back(Vector3(origin_x, y, origin_z));
		// 45 degrees
		x = rand() % ((origin_x + AST_SIZE) - origin_x + 1) + origin_x;
		y = rand() % ((origin_y + AST_SIZE) - origin_y + 1) + origin_y;
		v.push_back(Vector3(x, y, origin_z));
		//90 degrees
		x = rand() % ((origin_x + AST_SIZE) - origin_x + 1) + origin_x;
		v.push_back(Vector3(x, origin_y, origin_z));
		//135 degrees
		x = rand() % ((origin_x + AST_SIZE) - origin_x + 1) + origin_x;
		y = rand() % ((origin_y - 1) - (origin_y - AST_SIZE) + 1) + (origin_y - AST_SIZE);
		v.push_back(Vector3(x, y, origin_z));
		//180 degrees
		y = rand() % ((origin_y - 1) - (origin_y - AST_SIZE) + 1) + (origin_y - AST_SIZE);
		v.push_back(Vector3(origin_x, y, origin_z));
		//225 degrees
		x = rand() % ((origin_x - 1) - (origin_x - AST_SIZE) + 1) + (origin_x - AST_SIZE);
		y = rand() % ((origin_y - 1) - (origin_y - AST_SIZE) + 1) + (origin_y - AST_SIZE);
		v.push_back(Vector3(x, y, origin_z));
		//270 degrees
		x = rand() % ((origin_x - 1) - (origin_x - AST_SIZE) + 1) + (origin_x - AST_SIZE);
		v.push_back(Vector3(x, origin_y, origin_z));
		//315 degrees
		x = rand() % ((origin_x - 1) - (origin_x - AST_SIZE) + 1) + (origin_x - AST_SIZE);
		y = rand() % ((origin_y + AST_SIZE) - origin_y + 1) + origin_y;
		v.push_back(Vector3(x, y, origin_z));

		Mesh* asteroid_mesh = Mesh::GenerateLineLoop(v);
		RenderObject * a = new RenderObject();
		a->mesh = asteroid_mesh;
		arr[i] = *a;
	}

	/*//////////////////////////////////////////////////////////
	//**********	CREATE SPACESHIP	************************
	*///////////////////////////////////////////////////////////
	// create the ship mesh/renderobject
	Mesh* ship_mesh = Mesh::LoadMeshFile("spaceship.mesh");
	RenderObject * ship = new RenderObject();
	ship->mesh = ship_mesh;

	/*//////////////////////////////////////////////////////////
	//**********	CREATE COMETS	****************************
	*///////////////////////////////////////////////////////////
	Mesh *comet = Mesh::GenerateRock();

	RenderObject * c1 = new RenderObject();
	c1->mesh = comet;
	c1->texture = Texture::TextureFromTGA("snow_2_m_gold.tga");

	RenderObject * c2 = new RenderObject();
	c2->mesh = comet;
	c2->texture = Texture::TextureFromTGA("snow_2_m_gold.tga");

	RenderObject * c3 = new RenderObject();
	c3->mesh = comet;
	c3->texture = Texture::TextureFromTGA("snow_2_m_gold.tga");

	/*//////////////////////////////////////////////////////////
	//**********	CREATE DEBRIS	****************************
	*///////////////////////////////////////////////////////////

	Mesh* debris = Mesh::GenerateDebris();
	RenderObject* debris_arr = new RenderObject[DEBRIS_AMT];
	
	for (int i = 0; i < DEBRIS_AMT; i++) {
		RenderObject * d = new RenderObject();
		

		//set up random placement within constraints
		float x = getRandomFloat(-DEBRIS_X_BOUND, DEBRIS_X_BOUND);
		float y = getRandomFloat(-DEBRIS_Y_BOUND, DEBRIS_Y_BOUND);
		float z = -(getRandomFloat( 50.0f, DEBRIS_Z_BOUND));
		
		d->modelMatrix = Matrix4::Translation(Vector3(x, y, z)); // move debris into position
		d->mesh = debris;
		debris_arr[i] = *d;	// add to array to be renderes
		
	}

	/*//////////////////////////////////////////////////////////
	//**********	INITIAL PLACEMENT	************************
	*///////////////////////////////////////////////////////////
	Matrix4 viewMatrix;
	c1->modelMatrix = Matrix4::Translation(Vector3(10, 20, -60));
	c2->modelMatrix = Matrix4::Translation(Vector3(-10, -12, -40));
	c3->modelMatrix = Matrix4::Translation(Vector3(2, 2, 25));
	
	sun->modelMatrix = Matrix4::Translation(Vector3(SUN_X, SUN_Y, SUN_Z)) * Matrix4::Scale(Vector3(SUN_SCALE));
	ship->modelMatrix = Matrix4::Translation(Vector3(-2.0f, 0.0f, -15.0f));
	ship->modelMatrix = ship->modelMatrix * Matrix4::Rotation(90.0f, Vector3(1.0f, 0.0f, 0.0f)); // correct placement of ship
	ship->modelMatrix = ship->modelMatrix * Matrix4::Rotation(180.0f, Vector3(0.0f, 1.0f, 0.0f));
	
	/*//////////////////////////////////////////////////////////
	//**********	GAMELOOP	********************************
	*///////////////////////////////////////////////////////////
	while (r.UpdateWindow()) {

		// *************** SCENE TRANSLATIONS ******************
		//perform some vector calculations to move stuff around
		c3->modelMatrix = c3->modelMatrix * Matrix4::Translation(Vector3(0.0f, 0.0f, COMET_SPEED)) * Matrix4::Rotation(COMET_ROTATION, Vector3(0,0,1)); // move comet c3 quickly through scene with rotation
		ship->modelMatrix = ship->modelMatrix * Matrix4::Translation(Vector3(0.0f, -0.2f, 0.0f)) * Matrix4::Rotation(-5.0f, Vector3(1.0f, 0.0f, 0.0f));	// perform some rotation on the ship
		viewMatrix = viewMatrix * Matrix4::Translation(Vector3(0, 0, 0.01f));			// slowly move forward through the scene 
		for (int i = 0; i < DEBRIS_AMT; i++) { // a bit ineffecient as im already looping through this array to render the objects below. Wanted to keep the calculations seperate form the rendering
			}
		r.SetViewMatrix(viewMatrix);
		r.SetProjectionMatrix(Matrix4::Perspective(1, 500.0f, RATIO, 45.0f));


		// *************** SCENE MOVEMENT / TEXTURE CHANGE ******************
		// enables you to move the camera around the scene with the defined keys
		if (Keyboard::KeyDown(KEY_A)) {
			viewMatrix = viewMatrix *
				Matrix4::Translation(Vector3(-0.1f * MOVE_SCALE, 0, 0));
		};

		if (Keyboard::KeyDown(KEY_D)) {
			viewMatrix = viewMatrix *
				Matrix4::Translation(Vector3(0.1f * MOVE_SCALE, 0, 0));
		}
		if (Keyboard::KeyDown(KEY_W)) {
			viewMatrix = viewMatrix *
				Matrix4::Translation(Vector3(0.0, 0.1f * MOVE_SCALE, 0));
		}
		if (Keyboard::KeyDown(KEY_S)) {
			viewMatrix = viewMatrix *
				Matrix4::Translation(Vector3(0.0, -0.1f * MOVE_SCALE, 0));
		}

		if (Keyboard::KeyDown(KEY_E)) {
			viewMatrix = viewMatrix *
				Matrix4::Translation(Vector3(0.0, 0.0, 0.1 * MOVE_SCALE));
		}
		if (Keyboard::KeyDown(KEY_C)) {
			viewMatrix = viewMatrix *
				Matrix4::Translation(Vector3(0.0, 0.0, -0.1 * MOVE_SCALE));
		}
		if (Keyboard::KeyTriggered(KEY_R)) {
			r.SwitchTextureFiltering();
		}
		

		// clear buffers BEFORE drawing *********
		r.ClearBuffers();
		// *************** DRAW FUNCTIONS ******************
		for (int i = 0; i < ASTEROIDS; i++) {
			RenderObject*obj = &arr[i];
			r.DrawObject(obj);
		}
		for (int i = 0; i < DEBRIS_AMT; i++) {
			RenderObject*obj = &debris_arr[i];
			debris_arr[i].modelMatrix = debris_arr[i].modelMatrix * Matrix4::Translation(Vector3(0.0f, 0.0f, DEBRIS_SPEED_BOUND));
			debris_arr[i].modelMatrix = debris_arr[i].modelMatrix * Matrix4::Rotation(DEBRIS_ROTATION_BOUND, Vector3(1.0f, 0.0f, 0.0f));
			r.DrawObject(obj);
		}

		r.DrawObject(starmap);
		r.DrawObject(sun);
		r.DrawObject(ship);
		r.DrawObject(c1);
		r.DrawObject(c2);
		r.DrawObject(c3);
		//-----------------------------
		r.SwapBuffers();
	}

	// *************** DELETES ******************

	delete starmap;
	delete sun;
	delete c;
	delete ship;
	delete c1;
	delete c2;
	delete c3;
	delete[] debris_arr;
	delete[] arr;

	return 0;
}

// define new colours for the sun here.
Colour* getSunColour(int s) {
	
	Colour *c  = new Colour[10];

	switch (s)
	{
	case 1: {

		c[0] = Colour(255, 255, 0, 255); // yellow
		c[1] = Colour(255, 0, 0, 10); // red
		c[2] = Colour(255, 0, 0, 0); // red
		c[3] = Colour(255, 0, 0, 10); // red
		c[4] = Colour(255, 0, 0, 0); // red
		c[5] = Colour(255, 0, 0, 10); // red
		c[6] = Colour(255, 0, 0, 0); // red
		c[7] = Colour(255, 0, 0, 10); // red
		c[8] = Colour(255, 0, 0, 0); // red
		c[9] = Colour(255, 0, 0, 10); // red


	} break;
	case 2: {

		c[0] = Colour(0, 0, 255, 255); // yellow
		c[1] = Colour(0, 255, 255, 10); // l blue
		c[2] = Colour(0, 255, 255, 0); // l blue
		c[3] = Colour(0, 255, 255, 10); // l blue
		c[4] = Colour(0, 255, 255, 0); // l blue
		c[5] = Colour(0, 255, 255, 10); // l blue
		c[6] = Colour(0, 255, 255, 0); // l blue
		c[7] = Colour(0, 255, 255, 10); // l blue
		c[8] = Colour(0, 255, 255, 0); // l blue
		c[9] = Colour(0, 255, 255, 10); // l blue

	}break;


	}

	return c;
}

float getRandomFloat(float LOW, float HIGH) {
	return LOW + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HIGH - LOW)));
}

