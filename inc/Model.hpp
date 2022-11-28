#ifndef MODEL
#define MODEL

#include "Shader.hpp"
#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include <vector>

class Model
{
public:
	Model();
	~Model();

	void load(std::string& file);
	void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec3& camPosition);

private:
	Shader _shader;
	unsigned int _vao;
	unsigned int _texId;
	std::map<size_t, unsigned int> _vbos;
	//std::map<size_t, unsigned int> _texs;
	std::vector<unsigned int> _texs;
	tinygltf::Model _model;

	void setupNode(tinygltf::Node& node);
	void setupMesh(tinygltf::Mesh& mesh);
	void drawNode(tinygltf::Node& node);
	void drawMesh(tinygltf::Mesh& mesh);
};
#endif