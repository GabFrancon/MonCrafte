#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
private:
	std::map<std::string, GLuint> collection;

public:
	Texture() : collection(std::map<std::string, GLuint>{ {"right", 0}, { "left", 0 }, { "top", 0 }, { "bottom", 0 }, { "front", 0 }, { "back", 0 }}) {}

	void addToCollection(std::string name, GLuint texture) { collection[name] = texture; }
	GLuint getFromCollection(std::string name) { return collection[name]; }
};
#endif // !TEXTURE_H

