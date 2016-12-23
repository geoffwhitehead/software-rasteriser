/******************************************************************************
Class:SoftwareRasteriser
Implements:Window
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description: Class to encapsulate the various rasterisation techniques looked
at in the course material.

This is the class you'll be modifying the most!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix4.h"
#include "Mesh.h"
#include "Texture.h"
#include "RenderObject.h"
#include "Common.h"
#include "Window.h"

#include <vector>

using std::vector;

class RenderObject;
class Texture;

class SoftwareRasteriser : public Window	{ 
	
	
public://***********************************************************PUBLIC
	SoftwareRasteriser(uint width, uint height);
	~SoftwareRasteriser(void);

	void	ClearBuffers();
	void	SwapBuffers();
	void DrawObject(RenderObject * o);

	inline void	SetViewMatrix(const Matrix4 &m) {
		viewMatrix		= m;
		viewProjMatrix	= projectionMatrix * viewMatrix;
	}
	
	void	SetProjectionMatrix(const Matrix4 &m) {
		projectionMatrix	= m;
		viewProjMatrix		= projectionMatrix * viewMatrix;
	}
	 
	// GEOFF MODIFICATION
	
	static float ScreenAreaOfTri(const Vector4 &a, const Vector4 &b, const Vector4 &c);
	enum SampleState {
		SAMPLE_NEAREST,
		SAMPLE_BILINEAR,
		SAMPLE_MIPMAP_NEAREST,
		SAMPLE_MIPMAP_BILINEAR
	};

	void SwitchTextureFiltering() {
		if (texSampleState == SAMPLE_NEAREST) {
			texSampleState = SAMPLE_BILINEAR;
		}
		else if (texSampleState == SAMPLE_BILINEAR) {
			texSampleState = SAMPLE_MIPMAP_NEAREST;
		} 
		else {
			texSampleState = SAMPLE_NEAREST;
		}
	}

	// GEOFF MODIFICATION END

	
protected://***********************************************************PROTECTED
	
	
	
	SampleState texSampleState = SAMPLE_NEAREST;
	Colour*	GetCurrentBuffer();
	Texture* currentTexture;
	void	RasterisePointsMesh(RenderObject*o);
	void	RasteriseLinesMesh(RenderObject*o);

	virtual void Resize();

	void	RasteriseLine(const Vector4 &v0, const Vector4 &v1, 
		const Colour &colA = Colour(), const Colour &colB = Colour(), 
		const Vector3 &texA = Vector3() , const Vector3 &texB = Vector3());

	inline void	ShadePixel(uint x, uint y, const Colour&c);

	void	RasteriseTriMesh(RenderObject*o);
	void	RasteriseTriFanMesh(RenderObject *o);
	void	RasteriseTriStripMesh(RenderObject *o);

	int		currentDrawBuffer;

	Colour*	buffers[2];

	unsigned short*	depthBuffer;

	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
	Matrix4 textureMatrix;

	Matrix4	viewProjMatrix;

	Matrix4	portMatrix;

	
	struct BoundingBox {
		Vector2 topLeft;
		Vector2 bottomRight;
	};
	void	RasteriseLineStripMesh(RenderObject*o);
	void	RasteriseLineLoopMesh(RenderObject*o);
	
	BoundingBox CalculateBoxForTri(const Vector4 &a, const Vector4 &b, const Vector4 &c);

	void RasteriseTri(const Vector4 &triA, const Vector4 &triB, const Vector4 &triC,
		const Colour &colA = Colour(), const Colour &colB = Colour(), const Colour &colC = Colour(),
		const Vector3 &texA = Vector3(), const Vector3 &texB = Vector3(), const Vector3 &texC = Vector3());

	bool CohenSutherlandLine( Vector4 &inA, Vector4 &inB, Colour &colA, Colour &colB, Vector3 &texA, Vector3 &texB ) ;

	void SutherlandHodgmanTri(Vector4 &v0, Vector4 &v1, Vector4 &v2,
		const Colour &c0 = Colour(),
		const Colour &c1 = Colour(), 
		const Colour &c2 = Colour(), 
		const Vector3 &t0 = Vector3(),
		const Vector3 &t1 = Vector3(), 
		const Vector3 &t2 = Vector3());

	float ClipEdge(const Vector4 &inA, const Vector4 &inB, int axis);

	int HomogenousOutcode(const Vector4 &in);

	void CalculateWeights(const Vector4 &a, const Vector4 &b, const Vector4 &c, const Vector4 &p, float &alpha, float &beta, float &gamma);
	
	/*//////////////////////////////////////////////////////////
	//**********	INLINE: BLENDPIXEL	********************
	*///////////////////////////////////////////////////////////

	inline void BlendPixel(int x, int y, const Colour &source) {
		if (y >= screenHeight || y < 0) {
			return;
		}

		if (x >= screenWidth || x < 0) {
			return;
		}

		int index = (y*screenWidth) + x;

		Colour &dest = buffers[currentDrawBuffer][index];
		unsigned char sFactor = source.a;
		unsigned char dFactor = (255 - source.a);

		dest.r = ((source.r * sFactor) + (dest.r * dFactor)) / 255;
		dest.g = ((source.g * sFactor) + (dest.g * dFactor)) / 255;
		dest.b = ((source.b * sFactor) + (dest.b * dFactor)) / 255;
		dest.a = ((source.a * sFactor) + (dest.a * dFactor)) / 255;


	}


	/*//////////////////////////////////////////////////////////
	//**********	INLINE: DEPTH FUNC	********************************
	*///////////////////////////////////////////////////////////

	inline bool DepthFunc(int x, int y, float depthValue) {
		int index = (y * screenWidth) + x;

		unsigned int castVal = (unsigned int)depthValue;

		if (castVal > depthBuffer[index]) {
			return false;
		}
		depthBuffer[index] = castVal;
		return true;
	}
};

