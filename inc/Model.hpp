#ifndef MODEL
#define MODEL

#include "Shader.hpp"
#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include <vector>
#include <string>

class Model
{
public:
	Model();
	~Model();

	void load(std::string& file);
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec3& camPosition);
	void useShader() { _shader.use(); }
	Shader *getShader() { return &_shader; }
	std::string getPath();
	bool isLoaded();

private:
	Shader _shader;
	unsigned int _vao;
	std::map<size_t, unsigned int> _vbos;
	std::vector<unsigned int> _texs;
	tinygltf::Model _model;
	glm::mat4 _matrix;
	bool _hasTangent = false;

	std::string _path = "";
	bool _loaded = false;

	void setupNode(tinygltf::Node& node);
	void setupMesh(tinygltf::Mesh& mesh);
	void drawNode(tinygltf::Node& node);
	void drawMesh(tinygltf::Mesh& mesh);
};
#endif