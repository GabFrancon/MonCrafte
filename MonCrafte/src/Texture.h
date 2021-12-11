#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
private:
	std::map<std::string, GLuint> collection;

public:
	Texture() {}
	Texture(GLuint texture)
	{
		collection["side"] = texture;
		collection["top"] = texture;
		collection["bottom"] = texture;
	}
	Texture(GLuint top, GLuint bottom, GLuint side)
	{
		add("top", top);
		add("bottom", bottom);
		add("side", side);
	}

	void add(std::string name, GLuint texture) { collection[name] = texture; }
	GLuint get(std::string name) { return collection[name]; }
};
#endif // !TEXTURE_H

