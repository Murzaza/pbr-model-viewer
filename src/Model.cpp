#include "Model.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <GLFW/glfw3.h>

#include "debug.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

glm::vec3 lightPositions[] = {
	glm::vec3(-1.0f,  1.0f, 10.0f),
	glm::vec3( 1.0f,  1.0f, 10.0f),
	glm::vec3(-1.0f, -1.0f, 10.0f),
	glm::vec3( 1.0f, -1.0f, 10.0f)
};

glm::vec3 ligtColors[] = {
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f)
};

Model::Model()
{
	std::string vert = "model.vert";
	std::string frag = "model.frag";

	_shader.add(GL_VERTEX_SHADER, vert);
	_shader.add(GL_FRAGMENT_SHADER, frag);
	_shader.link();
}

Model::~Model()
{
	glDeleteVertexArrays(1, &_vao);
	for (size_t i = 0; i < _vbos.size(); i++)
	{
		glDeleteBuffers(1, &_vbos[i]);
	}
}

std::string Model::getPath()
{
	return _path;
}

bool Model::isLoaded()
{
	return _loaded;
}

void Model::load(std::string& file)
{
	//std::string path = PROJECT_SOURCE_DIR "/models/";
	DEBUG("Loading: %s\n", file.c_str());
	_path += file;

	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool status = loader.LoadASCIIFromFile(&_model, &err, &warn, _path);

	if (!err.empty())
		MPR_PRINT_ERROR("TinyGLTF Error:\n%s\n", err.c_str());

	if (!warn.empty())
		MPR_PRINT_ERROR("TinyGLTF Warning:\n%s\n", warn.c_str());

	if (!status) {
		MPR_PRINT_ERROR("TinyGLTF failed to load model\n");
		return;
	}

	assert(status);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	const tinygltf::Scene& scene = _model.scenes[_model.defaultScene];
	DEBUG("Loading beginning on %zd nodes\n", scene.nodes.size());
	for (auto& scene_node : scene.nodes)
	{
		DEBUG("Loading Node %zd\n", i);
		assert((scene_node >= 0) && (scene_node < _model.nodes.size()));
		setupNode(_model.nodes[scene_node]);
	}
	glBindVertexArray(0);

	for(auto& node : _model.nodes)
	{
		/* Put together the nodes matrix */
		glm::mat4 node_matrix = glm::mat4(1.0f);
		if (node.matrix.size() == 16)
		{
			node_matrix = glm::make_mat4(node.matrix.data());
		}

		if (node.translation.size() == 3)
		{
			node_matrix = glm::translate(node_matrix, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
		}

		if (node.rotation.size() == 4)
		{
			glm::quat rot = glm::quat(node.rotation[3],
									  node.rotation[0],
								  	  node.rotation[1],
								  	  node.rotation[2]);
			node_matrix = glm::mat4_cast(rot) * node_matrix;	
		}

		if (node.scale.size() == 3)
		{
			node_matrix = glm::scale(node_matrix, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
		}

		_node_matricies.push_back(node_matrix);
	}

	//cleanup 
	for (auto it = _vbos.cbegin(); it != _vbos.cend();)
	{
		tinygltf::BufferView bufferView = _model.bufferViews[it->first];
		if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER)
		{
			glDeleteBuffers(1, &_vbos[it->first]);
			_vbos.erase(it++);
		}
		else
		{
			++it;
		}
	}

	_loaded = true;
}

void Model::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection, glm::vec3& camPosition)
{
	_shader.use();
	_shader.setMat4("view", view);
	_shader.setMat4("projection", projection);
	_shader.setMat4("normal_matrix", glm::transpose(glm::inverse(view * projection)));
	_shader.setVec3("sun_position", glm::vec3(1.0f, 1.0f, 1.0f));
	_shader.setVec3("sun_color", glm::vec3(1.0f));
	_shader.setVec3("view_position", camPosition);
	_shader.setBool("has_tangent", _hasTangent);

	glBindVertexArray(_vao);

	const tinygltf::Scene& scene = _model.scenes[_model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); i++)
	{
		tinygltf::Node node = _model.nodes[scene.nodes[i]];
		glm::mat4 node_matrix = _node_matricies[scene.nodes[i]];
		glm::mat4 m = model * node_matrix;
		_shader.setMat4("model", m);
		drawNode(node);
	}

	glBindVertexArray(0);
}

void Model::drawNode(tinygltf::Node& node)
{
	if ((node.mesh >= 0) && (node.mesh < _model.meshes.size()))
	{
		drawMesh(_model.meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		drawNode(_model.nodes[node.children[i]]);
	}
}

void Model::drawMesh(tinygltf::Mesh& mesh)
{
	DEBUG("Drawing mesh %s, prims: %zd \n", mesh.name.empty() ? "N/A" : mesh.name.c_str(), mesh.primitives.size());
	for (size_t i = 0; i < mesh.primitives.size(); i++)
	{
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = _model.accessors[primitive.indices];
		tinygltf::Material& material = _model.materials[primitive.material];

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos.at(indexAccessor.bufferView));
		GLuint albedo = retrieveTex(material.pbrMetallicRoughness.baseColorTexture.index);
		glActiveTexture(GL_TEXTURE0);
		_shader.setInt("material.albedo", 0);
		std::vector<double>& albedoFactor = material.pbrMetallicRoughness.baseColorFactor;
		DEBUG("baseColorFactor (size): %zd\n", albedoFactor.size());
		_shader.setVec4("material.albedoFactor", glm::vec4(albedoFactor[0], albedoFactor[1], albedoFactor[2], albedoFactor[3]));
		_shader.setFloat("material.metallic", material.pbrMetallicRoughness.metallicFactor);
		_shader.setFloat("material.roughness", material.pbrMetallicRoughness.roughnessFactor);
		glBindTexture(GL_TEXTURE_2D, albedo);

		DEBUG("Texture Indices:\n  Albedo: %d\n  Normal: %d\n  Occlusion: %d\n  Emission: %d\n  MetallicRoughness: %d\n",
			material.pbrMetallicRoughness.baseColorTexture.index,
			material.normalTexture.index,
			material.occlusionTexture.index,
			material.emissiveTexture.index,
			material.pbrMetallicRoughness.metallicRoughnessTexture.index);

		GLuint normal =  retrieveTex(material.normalTexture.index);
		glActiveTexture(GL_TEXTURE1);
		_shader.setInt("material.normal", 1);
		_shader.setFloat("material.normalScale", material.normalTexture.scale);
		glBindTexture(GL_TEXTURE_2D, normal);

		GLuint ao = retrieveTex(material.occlusionTexture.index);
		glActiveTexture(GL_TEXTURE2);
		_shader.setInt("material.ao", 2);
		_shader.setFloat("material.occulsionStrength", material.occlusionTexture.strength);
		glBindTexture(GL_TEXTURE_2D, ao);

		GLuint emission = retrieveTex(material.emissiveTexture.index);
		glActiveTexture(GL_TEXTURE3);
		_shader.setInt("material.emission", 3);
		std::vector<double>& emissiveFactor = material.emissiveFactor;
		_shader.setVec3("material.emissiveFactor", glm::vec3(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]));
		glBindTexture(GL_TEXTURE_2D, emission);

		GLuint mr = retrieveTex(material.pbrMetallicRoughness.metallicRoughnessTexture.index);
		glActiveTexture(GL_TEXTURE4);
		_shader.setInt("material.pbrMetallicRoughness", 4);
		glBindTexture(GL_TEXTURE_2D, mr);

		glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
			BUFFER_OFFSET(indexAccessor.byteOffset));
	}
}

void Model::setupNode(tinygltf::Node& node)
{
	DEBUG("This Node has mesh id %d out of %zd and %zd children\n",
		node.mesh,
		_model.meshes.size(),
		node.children.size());
	if ((node.mesh >= 0) && (node.mesh < _model.meshes.size()))
	{
		setupMesh(_model.meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		DEBUG("Loading child node %zd\n", i);
		assert((node.children[i] >= 0) && (node.children[i] < _model.nodes.size()));
		setupNode(_model.nodes[node.children[i]]);
	}
}

void Model::setupMesh(tinygltf::Mesh& mesh)
{
	for (size_t i = 0; i < _model.bufferViews.size(); i++)
	{
		const tinygltf::BufferView bufferView = _model.bufferViews[i];
		if (bufferView.target == 0)
		{
			DEBUG("WARN: bufferView [%zu] is zero", i);
			continue; //Not supported;
		}

		int sparse_acc = -1;
		for (size_t a_i = 0; a_i < _model.accessors.size(); ++a_i) {
			const auto& acc = _model.accessors[a_i];
			if (acc.bufferView == i) {
				MPR_PRINT_ERROR("%zu is used by accessor %zu\n", i, a_i);
				if (acc.sparse.isSparse) {
					MPR_PRINT_ERROR("WARN: We have a sparse accessor at %zu\n", a_i);
					sparse_acc = a_i;
					break;
				}
			}
		}

		const tinygltf::Buffer& buffer = _model.buffers[bufferView.buffer];

		GLuint vbo;
		glGenBuffers(1, &vbo);
		_vbos[i] = vbo;
		glBindBuffer(bufferView.target, vbo);

		DEBUG("Loading Mesh with buffer %s\n\tsize: %zu\n\tbyte offset: %zu\n", bufferView.name.c_str(), bufferView.byteLength, bufferView.byteOffset);

		MPR_PRINT_ERROR("glBufferData(%d, %zu, data_ptr + %zu, GL_STATIC_DRAW)\n", bufferView.target, bufferView.byteLength, bufferView.byteOffset);
		glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}

	for (size_t i = 0; i < mesh.primitives.size(); i++)
	{
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = _model.accessors[primitive.indices];

		for (auto& attrib : primitive.attributes)
		{
			tinygltf::Accessor accessor = _model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(_model.bufferViews[accessor.bufferView]);
			glBindBuffer(GL_ARRAY_BUFFER, _vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
				size = accessor.type;
			}
			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0) vaa = 0;
			if (attrib.first.compare("NORMAL") == 0) vaa = 1;
			if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
			if (attrib.first.compare("TANGENT") == 0) vaa = 3;
			if (vaa > -1)
			{
				DEBUG("Setting up %d\n", vaa);
				if (vaa == 3) _hasTangent = true;
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
					BUFFER_OFFSET(accessor.byteOffset));
			}
			else
			{
				DEBUG("VAA is missing: %s\n", attrib.first.c_str());
			}
		}

		// We want to load textures as described in the material structure.	
		for (size_t i = 0; i < _model.textures.size(); i++)
		{
			tinygltf::Texture& tex = _model.textures[i];

			DEBUG("source: %d, sampler: %d\n", tex.source, tex.sampler);
			if (tex.source > -1)
			{
				int magFilter = GL_LINEAR;
				int minFilter = GL_LINEAR;
				int wrapS = GL_REPEAT;
				int wrapT = GL_REPEAT;

				if (tex.sampler > -1)
				{

					tinygltf::Sampler& sampler = _model.samplers[tex.sampler];
					magFilter = sampler.magFilter > -1 ? sampler.magFilter : GL_LINEAR;
					minFilter = sampler.minFilter > -1 ? sampler.minFilter : GL_LINEAR;
					wrapS = sampler.wrapS > -1 ? sampler.wrapS : GL_REPEAT;
					wrapT = sampler.wrapT > -1 ? sampler.wrapT : GL_REPEAT;

					DEBUG("Sampler available setting Min: %d, Mag: %d, S: %d, T: %d\n",
						minFilter, magFilter, wrapS, wrapT);
				}

				GLuint texId;
				glGenTextures(1, &texId);

				tinygltf::Image& image = _model.images[tex.source];

				DEBUG("Processing texture: %s\n", image.uri.c_str());

				glBindTexture(GL_TEXTURE_2D, texId);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

				GLenum format = GL_RGBA;

				if (image.component == 1)
					format = GL_RED;
				else if (image.component == 2)
					format = GL_RG;
				else if (image.component == 3)
					format = GL_RGB;

				GLenum type = GL_UNSIGNED_BYTE;
				if (image.bits == 16) type = GL_UNSIGNED_SHORT;

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
					format, type, &image.image.at(0));
				glGenerateMipmap(texId);
				_texs.push_back(texId);
			}
			else
			{
				DEBUG("Not loading texture %zu, %s\n", i, _model.images[tex.source].uri.c_str());
				assert(false);
			}
		}
	}

}

unsigned int Model::retrieveTex(size_t idx)
{
	if (idx < 0 || idx > _texs.size()) return 0;
	return _texs[idx];
}