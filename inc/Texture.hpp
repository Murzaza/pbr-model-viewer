#ifndef TEXTURE
#define TEXTURE

#include <string>

class Texture
{
public:
	Texture();
	Texture(std::string file, bool flip = false);
	~Texture();
	void load(std::string file, bool flip = false);
	unsigned int _id;

};

#endif