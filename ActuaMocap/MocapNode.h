#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
struct DecomposedTransform {
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
};
class MocapNode
{
public:
	~MocapNode();
	MocapNode();
	MocapNode(const glm::vec3& pos);
	void UpdateWorldTransform();
	void AddChild(MocapNode* child);
	inline const glm::mat4& GetWorldTransform() {
		return _worldTransform;
	}
	inline const glm::vec3& GetWorldPosition() {
		return getDecomposedTransform().translation;
	}
	void SetLocalPos(const glm::vec3& pos);
	void SetLocalEulers(const glm::vec3& pos);
	const DecomposedTransform& getDecomposedTransform();
private:
	void UpdateLocalTransform();
private:
	bool _isDirty = true;
	glm::vec3 _pos = { 0,0,0 };
	glm::vec3 _eulerAngles = { 0,0,0 };
	glm::mat4 _localTransform;
	glm::mat4 _worldTransform;
	std::vector<MocapNode*> _children;
	MocapNode* _parent;

};

