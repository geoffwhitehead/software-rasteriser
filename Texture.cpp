#include "Texture.h"

Texture::Texture(void)	{
	width	= 0;
	height	= 0;

	texels = NULL;
}

Texture::~Texture(void)	{
	delete[] texels;
}

Texture* Texture::TextureFromTGA(const string &filename) {
	
	Texture * t = new Texture();
	
	std::ifstream file;

	std::cout << "Loading TGA from(" << filename << ")" << std::endl;
	file.open(filename.c_str(), std::ios::binary);
	if(!file.is_open()){
		std::cout << "TextureFromTGA file error" << std::endl;
		return t ;
	}

	unsigned char TGAheader[18];

	std::cout << "sizeof(TGAheader) is " << sizeof(TGAheader) << std::endl;

	file.read((char*)TGAheader,sizeof(TGAheader));

	t->width	= (TGAheader[12] + (TGAheader[13] << 8));
	t->height	= (TGAheader[14] + (TGAheader[15] << 8));

	int size = t->width * t->height * (TGAheader[16] / 8);

	t->texels = new Colour[t->width*t->height];

	file.read( (char*) t->texels ,size);
	file.close();
	t->CreateMipMaps();
	return t;
}

const Colour& Texture::NearestTexSample(const Vector3 &coords, int miplevel) {
	miplevel = min(miplevel, mipLevels.size() - 1);
	miplevel = (mipLevels.size() - 1) - miplevel;

	int texWidth = width >> miplevel;
	int texHeight = height >> miplevel;

	int x = (int)(coords.x * (texWidth - 1));
	int y = (int)(coords.y * (texHeight - 1));

	return ColourAtPoint(x, y, miplevel);
}

Colour Texture::BilinearTexSample(const Vector3 &coords, int miplevel) {
	int texWidth = width;
	int texHeight = height;

	int x = (int)(coords.x * texWidth);
	int y = (int)(coords.y * texHeight);

	const Colour &tl = ColourAtPoint(x, y); //our tex point
	const Colour &tr = ColourAtPoint(x+1, y); //one to right
	const Colour &bl = ColourAtPoint(x, y+1); //one below
	const Colour &br = ColourAtPoint(x+1, y+1); //below right

	float fracX = (coords.x * texWidth) - x;
	float fracY = (coords.y * texHeight) - y;

	Colour top = Colour::Lerp(tl, tr, fracX);
	Colour bottom = Colour::Lerp(bl, br, fracX);

	return Colour::Lerp(top, bottom, fracY);

}

void Texture::CreateMipMaps() {
	int tempWidth = width;
	int tempHeight = height;

	mipLevels.push_back(texels);

	int numLevels = 0;

	while (tempWidth > 1 && tempHeight > 1) {
		tempWidth = tempWidth >> 1; //bit shifting halves value;
		tempHeight = tempHeight >> 1;

		Colour* newLevel = new Colour[tempWidth * tempHeight];
		GenerateMipLevel(mipLevels.back(), newLevel, numLevels);

		numLevels++;

		mipLevels.push_back(newLevel);
	}
}


void Texture::GenerateMipLevel(Colour*source, Colour*dest, int level) {
	int sourceWidth = width >> level;
	int sourceHeight = height >> level;

	//destination will be half the size, this shift by +1

	int destWidth = width >> (level + 1);
	int destHeight = height >> (level + 1);

	int outY = 0; // incremenets at half the rate of the for loop

	for (int y = 0; y < sourceHeight; y +=2) {
		int outX = 0; // incremenets at half rate of the for loop
		for (int x = 0; x < sourceWidth; x += 2) {
			Colour out;

			out += source[(y * sourceHeight) + x] * 0.25f;
			out += source[(y * sourceHeight) + x+1] * 0.25f;
			out += source[((y+1) * sourceHeight) + x] * 0.25f;
			out += source[((y+1) * sourceHeight) + x+1] * 0.25f;
		
			dest[outY * destHeight + outX] = out;

			outX++;
		}
		outY++;
	}
}