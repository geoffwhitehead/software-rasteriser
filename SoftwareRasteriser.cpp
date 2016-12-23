#include "SoftwareRasteriser.h"
#include <cmath>
#include <math.h>
/*
While less 'neat' than just doing a 'new', like in the tutorials, it's usually
possible to render a bit quicker to use direct pointers to the drawing area
that the OS gives you. For a bit of a speedup, you can uncomment the define below
to switch to using this method.

For those of you new to the preprocessor, here's a quick explanation:

Preprocessor definitions like #define allow parts of a file to be selectively enabled
or disabled at compile time. This is useful for hiding parts of the codebase on a
per-platform basis: if you have support for linux and windows in your codebase, obviously
the linux platform won't have the windows platform headers available, so compilation will
fail. So instead you can hide away all the platform specific stuff:

#if PLATFORM_WINDOWS
 DoSomeWindowsStuff();
#elif PLATFORM_LINUX
 DoSomeLinuxStuff();
 #else
 #error Unsupported Platform Specified!
 #endif

 As in our usage, it also allows you to selectively compile in some different functionality
 without any 'run time' cost - if it's not enabled by the preprocessor, it won't make it to
 the compiler, so no assembly will be generated.

Also, I've implemented the Resize method for you, in a manner that'll behave itself
no matter which method you use. I kinda forgot to do that, so there was a chance you'd
get exceptions if you resized to a bigger screen area. Sorry about that.
*/
//#define USE_OS_BUFFERS

SoftwareRasteriser::SoftwareRasteriser(uint width, uint height)	: Window(width, height){
	currentDrawBuffer	= 0;
	currentTexture = NULL; //TODO check this is correct!!

#ifndef USE_OS_BUFFERS
	//Hi! In the tutorials, it's mentioned that we need to form our front + back buffer like so:
	for (int i = 0; i < 2; ++i) {
		buffers[i] = new Colour[screenWidth * screenHeight];
	}
#else
	//This works, but we can actually save a memcopy by rendering directly into the memory the 
	//windowing system gives us, which I've added to the Window class as the 'bufferData' pointers
	for (int i = 0; i < 2; ++i) {
		buffers[i] = (Colour*)bufferData[i];
	}
#endif

	depthBuffer		=	new unsigned short[screenWidth * screenHeight];

	float zScale	= (pow(2.0f,16) - 1) * 0.5f;

	Vector3 halfScreen = Vector3((screenWidth - 1) * 0.5f, (screenHeight - 1) * 0.5f, zScale);

	portMatrix = Matrix4::Translation(halfScreen) * Matrix4::Scale(halfScreen);
}

SoftwareRasteriser::~SoftwareRasteriser(void)	{
#ifndef USE_OS_BUFFERS
	for(int i = 0; i < 2; ++i) {
		delete[] buffers[i];
	}
#endif
	delete[] depthBuffer;
}

void SoftwareRasteriser::Resize() {
	Window::Resize(); //make sure our base class gets to do anything it needs to

#ifndef USE_OS_BUFFERS
	for (int i = 0; i < 2; ++i) {
		delete[] buffers[i];
		buffers[i] = new Colour[screenWidth * screenHeight];
	}
#else
	for (int i = 0; i < 2; ++i) {
		buffers[i] = (Colour*)bufferData[i];
	}
#endif

	delete[] depthBuffer;
	depthBuffer = new unsigned short[screenWidth * screenHeight];

	float zScale = (pow(2.0f, 16) - 1) * 0.5f;

	Vector3 halfScreen = Vector3((screenWidth - 1) * 0.5f, (screenHeight - 1) * 0.5f, zScale);

	portMatrix = Matrix4::Translation(halfScreen) * Matrix4::Scale(halfScreen);
}

Colour*	SoftwareRasteriser::GetCurrentBuffer() {
	return buffers[currentDrawBuffer];
}

void	SoftwareRasteriser::ClearBuffers() {
	
	//NOTE: This was slightly different to his tutorial code, may cause errors???

	Colour* buffer = GetCurrentBuffer();

	unsigned int clearVal = 0xFF000000;
	unsigned int depthVal = ~0;

	for(uint y = 0; y < screenHeight; ++y) {
		for(uint x = 0; x < screenWidth; ++x) {
			buffer[(y * screenWidth) + x].c  = clearVal;
			depthBuffer[(y * screenWidth) + x] = depthVal;
		}
	}
}

void	SoftwareRasteriser::SwapBuffers() {
	PresentBuffer(buffers[currentDrawBuffer]);
	currentDrawBuffer = !currentDrawBuffer;
}

/*//////////////////////////////////////////////////////////
//**********	DRAW OBJECT		****************************
*///////////////////////////////////////////////////////////

void	SoftwareRasteriser::DrawObject(RenderObject*o) {
	currentTexture = o->texture;
	switch (o->GetMesh()->GetType())
	{
	case PRIMITIVE_POINTS: {
							   RasterisePointsMesh(o);
	} break;
	case PRIMITIVE_LINES: {
							  RasteriseLinesMesh(o);
	} break;
	case PRIMITIVE_LINE_STRIPS: {
									RasteriseLineStripMesh(o);
	} break;
	case PRIMITIVE_LINE_LOOPS: {
								   RasteriseLineLoopMesh(o);
	} break;
	case PRIMITIVE_TRIANGLES: {
									RasteriseTriMesh(o);
	} break;
	case PRIMITIVE_TRIFAN: {
								RasteriseTriFanMesh(o);
	} break;
	case PRIMITIVE_TRISTRIP: {
								RasteriseTriStripMesh(o);
	} break;
	}
}

void	SoftwareRasteriser::RasterisePointsMesh(RenderObject*o) {
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();

	for (uint i = 0; i < o->GetMesh()->numVertices; i++){
		Vector4 vertexPos = mvp * o->GetMesh()->vertices[i];
		vertexPos.SelfDivisionByW();

		Vector4 screenPos = portMatrix * vertexPos;
		BlendPixel((uint)screenPos.x, (uint)screenPos.y, Colour::White);
	}
}

// GEOFF MODIFICATION

/*//////////////////////////////////////////////////////////
//**********	RASTERISE LINES MESH	********************
*///////////////////////////////////////////////////////////

void	SoftwareRasteriser::RasteriseLinesMesh(RenderObject*o) {
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();
	Mesh*m = o->GetMesh();

	for (uint i = 0; i < m->numVertices; i += 2){
		Vector4 v0 = mvp * m->vertices[i];
		Vector4 v1 = mvp * m->vertices[i+1];
		
		Colour c0 = m->colours[i];
		Colour c1 = m->colours[i+1];

		Vector3 t0 = Vector3(
			m->textureCoords[i].x,
			m->textureCoords[i].y, 1.0f);

		Vector3 t1 = Vector3(
			m->textureCoords[i+1].x,
			m->textureCoords[i+1].y, 1.0f);

		if (!CohenSutherlandLine(v0, v1, c0, c1, t0, t1)) {
			continue;
		}

		t0.z = 1.0f;
		t1.z = 1.0f;

		t0 /= v0.w;
		t1 /= v1.w;

		v0.SelfDivisionByW();
		v1.SelfDivisionByW();

		RasteriseLine(v0, v1, c0, c1, t0, t1); 
	}
}

/*//////////////////////////////////////////////////////////
//**********	RASTERISE TRI MESH	************************
*///////////////////////////////////////////////////////////

void SoftwareRasteriser::RasteriseTriMesh(RenderObject *o) {
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();

	Mesh*m = o->GetMesh();
	
	for (uint i = 0; i < m->numVertices; i += 3) { // loop through the render object in groups of 3 vertices
		Vector4 v0 = mvp * m->vertices[i]; // add the 3 vertices of the triangle
		Vector4 v1 = mvp * m->vertices[i + 1];
		Vector4 v2 = mvp * m->vertices[i + 2];

		//added on tut 10
		Vector3 t0 = Vector3(
			m->textureCoords[i].x,
			m->textureCoords[i].y, 1.0f);// / v0.w;
		Vector3 t1 = Vector3(
			m->textureCoords[i + 1].x,
			m->textureCoords[i + 1].y, 1.0f);// / v1.w;
		Vector3 t2 = Vector3(
			m->textureCoords[i + 2].x,
			m->textureCoords[i + 2].y, 1.0f);// / v2.w;

		SutherlandHodgmanTri(v0, v1, v2,
			m->colours[i], m->colours[i + 1], m->colours[i + 2], t0, t1, t2);
		}
}

/*//////////////////////////////////////////////////////////
//**********	RASTERISE TRI FANMESH	********************
*///////////////////////////////////////////////////////////

void SoftwareRasteriser::RasteriseTriFanMesh(RenderObject *o) {

		Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();

		Mesh*m = o->GetMesh();

	if (m->numVertices > 2) { // need atleast 3 vert to make a triangle

		Vector4 v0 = mvp * m->vertices[0];
		Vector4 v1, v2;
		Vector3 t0 = Vector3(m->textureCoords[0].x,
			m->textureCoords[0].y, 1.0f);
		Vector3 t1, t2;

		for (uint i = 1; i < m->numVertices - 1; i++) {

			v1 = mvp * m->vertices[i];

			t1 = Vector3(m->textureCoords[i].x,
				m->textureCoords[i].y, 1.0f);

			v2 = mvp * m->vertices[i + 1];

			t2 = Vector3(m->textureCoords[i + 1].x,
				m->textureCoords[i + 1].y, 1.0f);

			SutherlandHodgmanTri(v0, v1, v2,
				m->colours[0], m->colours[i], m->colours[i + 1], t0, t1, t2);
		}
	}
}

/*//////////////////////////////////////////////////////////
//**********	RASTERISE TRI STRIP MESH	****************
*///////////////////////////////////////////////////////////

void SoftwareRasteriser::RasteriseTriStripMesh(RenderObject *o) {

	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();

	Mesh*m = o->GetMesh();

	if (m->numVertices > 2) { // need atleast 3 vert to make a triangle

		Vector4 v0, v1, v2;
		Vector3 t0, t1, t2;

		for (uint i = 0; i < m->numVertices - 2; i++) {

			v0 = mvp * m->vertices[i];
			t0 = Vector3(m->textureCoords[i].x,
				m->textureCoords[i].y, 1.0f);

			v1 = mvp * m->vertices[i+1];
			t1 = Vector3(m->textureCoords[i+1].x,
				m->textureCoords[i+1].y, 1.0f);

			v2 = mvp * m->vertices[i + 2];
			t2 = Vector3(m->textureCoords[i + 2].x,
				m->textureCoords[i + 2].y, 1.0f);

			if (i % 2 == 0) {
				SutherlandHodgmanTri(v0, v1, v2,
					m->colours[i], m->colours[i+1], m->colours[i + 2], t0, t1, t2);
			}
			else {
				SutherlandHodgmanTri(v2, v1, v0,
					m->colours[i+2], m->colours[i+1], m->colours[i], t2, t1, t0);

			}
		}
	}
}

/*//////////////////////////////////////////////////////////
//**********	RASTERISE LINE STRIP MESH	****************
*///////////////////////////////////////////////////////////

void SoftwareRasteriser::RasteriseLineStripMesh(RenderObject*o){

	Mesh* m = o->GetMesh();

	if (m->numVertices > 2){
		Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();
		Vector4 v0, v1;
		Colour c0, c1;
		Vector3 t0, t1;

		for (uint i = 0; i < m->numVertices - 1; ++i){
			v0 = mvp * m->vertices[i];
			v1 = mvp * m->vertices[i + 1];

			c0 = m->colours[i];
			c1 = m->colours[i + 1];

			 t0 = Vector3(
				m->textureCoords[i].x,
				m->textureCoords[i].y, 1.0f);

			 t1 = Vector3(
				m->textureCoords[i + 1].x,
				m->textureCoords[i + 1].y, 1.0f);

			if (!CohenSutherlandLine(v0, v1, c0, c1, t0, t1)) {
				continue;
			}

			t0.z = 1.0f;
			t1.z = 1.0f;

			t0 /= v0.w;
			t1 /= v1.w;

			v0.SelfDivisionByW();
			v1.SelfDivisionByW();

			RasteriseLine(v0, v1, c0, c1, t0, t1);
		}
	}
}

/*//////////////////////////////////////////////////////////
//**********	RASTERISE LINE LOOP MESH	****************
*///////////////////////////////////////////////////////////


void SoftwareRasteriser::RasteriseLineLoopMesh(RenderObject*o){
	
	Mesh* m = o->GetMesh();

	if (m->numVertices > 2){
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();
	Vector4 v0, v1;
	Colour c0, c1;
	Vector3 t0, t1;

	for (uint i = 0; i < m->numVertices; ++i){
		v0 = mvp * m->vertices[i];
		v1 = mvp * m->vertices[(i + 1) % m->numVertices];

		c0 = m->colours[i];
		c1 = m->colours[(i + 1) % m->numVertices];

		t0 = Vector3(
			m->textureCoords[i].x,
			m->textureCoords[i].y, 1.0f);

		t1 = Vector3(
			m->textureCoords[(i + 1) % m->numVertices].x,
			m->textureCoords[(i + 1) % m->numVertices].y, 1.0f);

		if (!CohenSutherlandLine(v0,v1, c0 ,c1, t0, t1)){
			continue;
		}

		t0.z = 1.0f;
		t1.z = 1.0f;
		t0 /= v0.w;
		t1 /= v1.w;

		v0.SelfDivisionByW();
		v1.SelfDivisionByW();

		RasteriseLine(v0, v1, c0, c1, t0, t1);

	}
}}
	
/*//////////////////////////////////////////////////////////
//**********	RASTERISE LINE 	****************************
*///////////////////////////////////////////////////////////

void SoftwareRasteriser::RasteriseLine(
	const Vector4 & vertA, const Vector4 & vertB, 
	const Colour &colA, const Colour &colB , 
	const Vector3 &texA , const Vector3 &texB){

	//transform our ndc coords ito screen coords
	Vector4 v0 = portMatrix * vertA;
	Vector4 v1 = portMatrix * vertB;
	Vector4 dir = v1 - v0; // what direction is the line going?

	int xDir = (dir.x < 0.0f) ? -1 : 1;//move left or right?
	int yDir = (dir.y < 0.0f) ? -1 : 1; //move up or down?
	int x = (int)v0.x; // current x axis plot point
	int y = (int)v0.y; //current y axis plot point

	int* target = NULL; //increment on error overflow, x  or y?
	int* scan = NULL; //increment every iteration, x or y?
	int scanVal = 0;
	int targetVal = 0;

	float slope = 0.0f;

	int range = 0; // Bound or loop by this value; how many times for the rasterisation loop to run. Equal to the length of the longer line axis.

	// if we are steep, we have to scan over the y axis
	if (abs(dir.y) > abs(dir.x)) {
		slope = (dir.x / dir.y);
		range = (int)abs(dir.y);
		target = &x; //error on x
		scan = &y; //iterate over y
		scanVal = yDir;
		targetVal = xDir;
	}
	else { //not steep? scan over the x axis;
		slope = (dir.y / dir.x);
		range = (int)abs(dir.x);
		target = &y; //error on x
		scan = &x; //iterate over y
		scanVal = xDir;
		targetVal = yDir;
	}
	float absSlope = abs(slope);
	float error = 0.0f;
	float reciprocalRange = 1.0f / range; //save a divide ...

		for (int i = 0; i < range; ++i){
			float t = i*reciprocalRange;
			Colour currentCol = colB*t + colA*(1.0f - t);

			// added on tut9
			Vector3 subTex = texA * (t) + texB * (1.0f - (t));
			subTex.x /= subTex.z;
			subTex.y /= subTex.z;
			//end mod from tut9

			// added on tut8
			float zVal = v1.z*(t)+v0.z*(1.0f - (t));

			if (DepthFunc((int)x, (int)y, zVal)) {
				BlendPixel(x, y, currentCol);
			}
			// end mod from tut 8

			BlendPixel(x, y, currentCol);
			error += absSlope;
			if (error > 0.5f) {
				error -= 1.0f;
				(*target) += targetVal; // Either advance along x or y //TARGET
			}
			(*scan) += scanVal; //Either iterate along x or y //SCAN
		}
	}

/*//////////////////////////////////////////////////////////
//**********	SHADE PIXEL		****************************
*///////////////////////////////////////////////////////////

inline void SoftwareRasteriser::ShadePixel(uint x, uint y, const Colour&c) {
	

	if (y >= screenHeight){
		return;
	}
	if (x >= screenWidth){
		return;
	}
	int index = (y * screenWidth) + x;
	buffers[currentDrawBuffer][index] = c;
}

/*//////////////////////////////////////////////////////////
//**********	CALCULATE BOX FOR TRI	********************
*///////////////////////////////////////////////////////////

SoftwareRasteriser::BoundingBox SoftwareRasteriser::CalculateBoxForTri(const Vector4 &a, const Vector4 &b, const Vector4 &c) {
	
	BoundingBox box;

	box.topLeft.x = a.x; // start with the first vertex value
	box.topLeft.x = min(box.topLeft.x, b.x); //swap to second if less
	box.topLeft.x = min(box.topLeft.x, c.x); // swap to second if less
	box.topLeft.x = max(box.topLeft.x, 0.0f); //screen bound

	box.topLeft.y = a.y; // start with the first vertex value
	box.topLeft.y = min(box.topLeft.y, b.y); //swap to second if less
	box.topLeft.y = min(box.topLeft.y, c.y); // swap to second if less
	box.topLeft.y = max(box.topLeft.y, 0.0f); //screen bound

	box.bottomRight.x = a.x; // start with the first vertex value
	box.bottomRight.x = min(box.bottomRight.x, b.x); //swap to second if less
	box.bottomRight.x = min(box.bottomRight.x, c.x); // swap to second if less
	box.bottomRight.x = max(box.bottomRight.x, screenWidth); //screen bound

	box.bottomRight.y = a.y; // start with the first vertex value
	box.bottomRight.y = min(box.bottomRight.y, b.y); //swap to second if less
	box.bottomRight.y = min(box.bottomRight.y, c.y); // swap to second if less
	box.bottomRight.y = max(box.bottomRight.y, screenWidth); //screen bound

	return box;
}

/*//////////////////////////////////////////////////////////
//**********	SCREEN AREA OF TRI	************************
*///////////////////////////////////////////////////////////

float SoftwareRasteriser::ScreenAreaOfTri(const Vector4 &a, const Vector4 &b, const Vector4 &c) {
	float area = ((a.x*b.y) + (b.x*c.y) + (c.x* a.y)) - //first diagonals
		((b.x*a.y) + (c.x*b.y) + (a.x*c.y));			//second column diagonals

	return area * 0.5f;
}

/*//////////////////////////////////////////////////////////
//**********	RASTERISE TRIANGLE	************************
*///////////////////////////////////////////////////////////

void SoftwareRasteriser::RasteriseTri(const Vector4 &triA, const Vector4 &triB, const Vector4 &triC,
	const Colour &colA, const Colour &colB, const Colour &colC,
	const Vector3 &texA, const Vector3 &texB, const Vector3 &texC) {

	// incoming triangles are in NDC space
	Vector4 v0 = portMatrix * triA; // Now in viewport space!
	Vector4 v1 = portMatrix * triB; // Now in viewport space!
	Vector4 v2 = portMatrix * triC; // Now in viewport space!
	BoundingBox b = CalculateBoxForTri(v0, v1, v2);
	float triArea = ScreenAreaOfTri(v0, v1, v2);
	float areaRecip = 1.0f / triArea;
	float subTriArea[3];
	Vector4 screenPos(0, 0, 0, 1);


	for (float y = b.topLeft.y; y < b.bottomRight.y; ++y) {
		for (float x = b.topLeft.x; x < b.bottomRight.x; ++x) {
			screenPos.x = x; //create vertex 'p'
			screenPos.y = y; //create vertex 'p'

			// back face culling 
			if (triArea < 0.0f) {
				return;
			}

			subTriArea[0] = abs(ScreenAreaOfTri(v0, screenPos, v1));
			subTriArea[1] = abs(ScreenAreaOfTri(v1, screenPos, v2));
			subTriArea[2] = abs(ScreenAreaOfTri(v2, screenPos, v0));

			float triSum = subTriArea[0] + subTriArea[1] + subTriArea[2];

			if (triSum > (triArea + 1.0f)) {
				continue; // current pixel is NOT IN this triangle!!
			}

			if (triSum < 1.0f) { // tiny triangle we dont care about
				continue;
			}
			
			float alpha = subTriArea[1] * areaRecip;
			float beta = subTriArea[2] * areaRecip;
			float gamma = subTriArea[0] * areaRecip;

			// start mods tut 8
			float zVal = (v0.z * alpha) + (v1.z * beta) + (v2.z * gamma);
 			if (!DepthFunc((int)x, (int)y, zVal)) {
				continue;
			}
			//end mods tut 8


			//mod tut10
			if (currentTexture) { 
				//interpolate in screen linear space
				Vector3 subTex = (texA * alpha) + (texB * beta) + (texC * gamma);
				//convert the coordinates back into world linear space.
				subTex.x /= subTex.z;
				subTex.y /= subTex.z;
				if (texSampleState == SAMPLE_BILINEAR) {
					BlendPixel((int)x, (int)y, currentTexture->BilinearTexSample(subTex));
				} 
				else if (texSampleState == SAMPLE_NEAREST) {
					BlendPixel((int)x, (int)y, currentTexture->NearestTexSample(subTex));
				}
				else if (texSampleState == SAMPLE_MIPMAP_NEAREST) {
					float xAlpha, xBeta, xGamma;
					float yAlpha, yBeta, yGamma;

					CalculateWeights(v0, v1, v2, screenPos + Vector4(1, 0, 0, 0), xAlpha, xBeta, xGamma);
					CalculateWeights(v0, v1, v2, screenPos + Vector4(0, 1, 0, 0), yAlpha, yBeta, yGamma);

					Vector3  xDerivs = (texA * xAlpha) + (texB * xBeta) + (texC * xGamma);
					Vector3  yDerivs = (texA * yAlpha) + (texB * yBeta) + (texC * yGamma);
						
					xDerivs.x /= xDerivs.z;			//return to linear texture space
					xDerivs.y /= xDerivs.z;

					yDerivs.x /= yDerivs.z;
					yDerivs.y /= yDerivs.z;

					xDerivs = xDerivs - subTex; // get the rate of change on the x axis
					yDerivs = yDerivs - subTex; // get the rate of change on the y axis

					//mpw have the rates of change ont he x and y acis for the tex u and v;

					float maxU = max(abs(xDerivs.x), abs(yDerivs.x));
					float maxV = max(abs(xDerivs.y), abs(yDerivs.y));

					float maxChange = abs(max(maxU, maxV));

					int lambda = abs(log(maxChange) / log(2.0));

					//sample form the usual texture coords, wicth the new LOD

					BlendPixel((int)x, (int)y, currentTexture->NearestTexSample(subTex, lambda));


				}
			}
			else {
				Colour subColour = ((colA * alpha) + (colB * beta) + (colC * gamma));
				BlendPixel((uint)x, (uint)y, subColour);
			}
			//end mod 10
		}
	}
}


// OUTCODE CONSTANTS
const int INSIDE_CS = 0;	//000000
const int LEFT_CS = 1;	//000001
const int RIGHT_CS = 2;	//000010
const int BOTTOM_CS = 4;	//000100
const int TOP_CS = 8;	//001000
const int NEAR_CS = 16;	//010000
const int FAR_CS = 32;	//100000

/*//////////////////////////////////////////////////////////
//**********	HOMOGENOUS OUTCODE	************************
*///////////////////////////////////////////////////////////

int SoftwareRasteriser::HomogenousOutcode(const Vector4 &in) {

	int outCode = INSIDE_CS;

	if (in.x < -in.w) {
		outCode |= LEFT_CS;
	}
	else if (in.x >in.w) {
		outCode |= RIGHT_CS;
	}
	if (in.y < -in.w) {
		outCode |= BOTTOM_CS;
	}
	else if (in.y >in.w) {
		outCode |= TOP_CS;
	}
	if (in.z < -in.w) {
		outCode |= NEAR_CS;
	}
	else if (in.z > in.w) {
		outCode |= FAR_CS;
	}
	return outCode;
}

/*//////////////////////////////////////////////////////////
//**********	CLIP EDGE	********************************
*///////////////////////////////////////////////////////////

float SoftwareRasteriser::ClipEdge(const Vector4 &inA, const Vector4 &inB, int axis) {
	float ratio = 0.0f;
	switch (axis) {
		//negative w' planes subtract the w of a and add the w of b
		//positive w' planes add the w of a and subtract the w of b

	case LEFT_CS: ratio = 
		(-inA.w - inA.x) / ((inB.x - inA.x) + inB.w - inA.w); break;

	case RIGHT_CS: ratio =
		(inA.w - inA.x) / ((inB.x - inA.x) - inB.w + inA.w); break;

	case BOTTOM_CS: ratio =
		(-inA.w - inA.y) / ((inB.y - inA.y) + inB.w - inA.w); break;

	case TOP_CS: ratio =
		(inA.w - inA.y) / ((inB.y - inA.y) - inB.w + inA.w); break;

	case NEAR_CS: ratio =
		(-inA.w - inA.z) / ((inB.z - inA.z) + inB.w - inA.w); break;

	case FAR_CS: ratio =
		(inA.w - inA.z) / ((inB.z - inA.z) - inB.w + inA.w); break;
	}
	return min(1.0f, ratio);
}

/*//////////////////////////////////////////////////////////
//**********	COHENSUTHERLAND LINE	********************
*///////////////////////////////////////////////////////////

bool SoftwareRasteriser::CohenSutherlandLine(Vector4 &inA, Vector4 &inB, Colour &colA, Colour &colB, Vector3 &texA, Vector3 &texB) {
	
	for (int i = 0; i < 6; ++i) {
		int planeCode = 1 << i;
		int outsideA = (HomogenousOutcode(inA) & planeCode);
		int outsideB = (HomogenousOutcode(inB) & planeCode);

		if (outsideA && outsideB) {
			return false; //both on wrong side of the plane, continue
		}

		if (!outsideA && !outsideB) {
			continue; // both on side of this plane, discard
		}
		else {
			float clipRatio = ClipEdge(inA, inB, planeCode);

			if (outsideA) {
				texA = Vector3::Lerp(texA, texB, clipRatio);
				colA = Colour::Lerp(colA, colB, clipRatio);
				inA = Vector4::Lerp(inA, inB, clipRatio);
			}
			else {
				texB = Vector3::Lerp(texA, texB, clipRatio);
				colB = Colour::Lerp(colA, colB, clipRatio);
				inB = Vector4::Lerp(inA, inB, clipRatio);
			}
		}
	}
	return true;
}

/*//////////////////////////////////////////////////////////
//**********	SUTHERLAND HODGEMAN TRI	********************
*///////////////////////////////////////////////////////////

#define MAX_VERTS 15

void SoftwareRasteriser::SutherlandHodgmanTri(Vector4 &v0, Vector4 &v1, Vector4 &v2,
	const Colour &c0,
	const Colour &c1,
	const Colour &c2,
	const Vector3 &t0,
	const Vector3 &t1,
	const Vector3 &t2) {

	Vector4 posIn[MAX_VERTS];
	Colour colIn[MAX_VERTS];
	Vector3 texIn[MAX_VERTS];

	Vector4 posOut[MAX_VERTS];
	Colour colOut[MAX_VERTS];
	Vector3 texOut[MAX_VERTS];

	posIn[0] = v0;
	posIn[1] = v1;
	posIn[2] = v2;

	colIn[0] = c0;
	colIn[1] = c1;
	colIn[2] = c2;

	texIn[0] = Vector3(t0.x, t0.y, 1); // throws away z value .... ???
	texIn[1] = Vector3(t1.x, t1.y, 1);
	texIn[2] = Vector3(t2.x, t2.y, 1);

	int inSize = 3; //keep track of the input list size...

	for (int i = 0; i <= 6; i++) {
		int planeCode = 1 << i;

		Vector4 prevPos = posIn[inSize - 1];
		Colour prevCol = colIn[inSize - 1];
		Vector3 prevTex = texIn[inSize - 1];

		int outSize = 0; //keep track of the output list size

		for (int j = 0; j < inSize; ++j) {
			int outsideA = (HomogenousOutcode(posIn[j]) & planeCode);
			int outsideB = (HomogenousOutcode(prevPos) & planeCode);

			if (outsideA ^ outsideB) {
				float clipRatio = ClipEdge(posIn[j], prevPos, planeCode);

				colOut[outSize] = Colour::Lerp(colIn[j], prevCol, clipRatio);
				texOut[outSize] = Vector3::Lerp(texIn[j], prevTex, clipRatio);
				posOut[outSize] = Vector4::Lerp(posIn[j], prevPos, clipRatio);

				outSize++;
			}

			if (!outsideA) {
				posOut[outSize] = posIn[j];
				colOut[outSize] = colIn[j];
				texOut[outSize] = texIn[j];
				outSize++;
			}
			prevPos = posIn[j];
			prevCol = colIn[j];
			prevTex = texIn[j];

		} // end of vertex processing loop
		for (int j = 0; j < outSize; ++j) {
			posIn[j] = posOut[j];
			colIn[j] = colOut[j];
			texIn[j] = texOut[j];
		}
		inSize = outSize;
	} // end of plane clipping loop
	for (int i = 0; i < inSize; ++i) {
		texIn[i] = Vector3(texIn[i].x, texIn[i].y, 1.0f) / posIn[i].w;
		posIn[i].SelfDivisionByW();
	}
	for (int i = 2; i < inSize; ++i) {
		RasteriseTri(
			posIn[0], posIn[i - 1], posIn[i],
			colIn[0], colIn[i - 1], colIn[i],
			texIn[0], texIn[i - 1], texIn[i]
			);
	}

} //end of function

  /*//////////////////////////////////////////////////////////
  //**********	CALCULATE WEIGHTS	**************************
  *///////////////////////////////////////////////////////////

void SoftwareRasteriser::CalculateWeights(const Vector4 &v0, const Vector4 &v1, const Vector4 &v2, const Vector4 &p, float &alpha, float &beta, float &gamma) {
	float triArea = ScreenAreaOfTri(v0, v1, v2);
	float areaRecip = 1.0f / triArea;

	float subTriArea[3];

	subTriArea[0] = abs(ScreenAreaOfTri(v0, p, v1));
	subTriArea[1] = abs(ScreenAreaOfTri(v1, p, v2));
	subTriArea[2] = abs(ScreenAreaOfTri(v2, p, v0));

	alpha = subTriArea[1] * areaRecip;
	beta = subTriArea[2] * areaRecip;
	gamma = subTriArea[0] * areaRecip;


}

// GEOFF MODIFICATION END