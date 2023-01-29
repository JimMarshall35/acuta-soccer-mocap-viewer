#include "MocapNode.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

MocapNode::~MocapNode()
{

}

MocapNode::MocapNode()
{
}

MocapNode::MocapNode(const glm::vec3& pos)
	:_pos(pos),
    _parent(nullptr)
{
}


void MocapNode::UpdateWorldTransform()
{
    if (_isDirty) {
        UpdateLocalTransform();
        if (_parent != nullptr) {
            _worldTransform = _parent->_worldTransform * _localTransform;
        }
        else {
            _worldTransform = glm::mat4() * _localTransform;
        }
        _isDirty = false;
        for (int i = 0; i < _children.size(); i++) {
            _children[i]->_isDirty = true;
            _children[i]->UpdateWorldTransform();
        }
    }
    else {
        for (int i = 0; i < _children.size(); i++) {
            _children[i]->UpdateWorldTransform();
        }
    }
    
}

void MocapNode::AddChild(MocapNode* child)
{
    _children.push_back(child);
    child->_parent = this;
}

void MocapNode::SetLocalPos(const glm::vec3& pos)
{
    _pos = pos;
    _isDirty = true;
}

void MocapNode::SetLocalEulers(const glm::vec3& angles)
{
    _eulerAngles = angles;
    _isDirty = true;
}

void MocapNode::UpdateLocalTransform()
{
    if (_isDirty) {
        auto transform = glm::mat4();
        transform = glm::translate(transform, _pos);
        transform = glm::rotate(transform, _eulerAngles.y, glm::vec3(0.0, 1.0, 0.0));
        transform = glm::rotate(transform, _eulerAngles.x, glm::vec3(1.0, 0.0, 0.0));
        transform = glm::rotate(transform, _eulerAngles.z, glm::vec3(0.0, 0.0, 1.0));
        transform = glm::scale(transform, { 1,1,1 });
        _localTransform = transform;
    }
}

const DecomposedTransform& MocapNode::getDecomposedTransform() {
    static DecomposedTransform decomposed;
    glm::decompose(
        _worldTransform,
        decomposed.scale,
        decomposed.rotation,
        decomposed.translation,
        decomposed.skew,
        decomposed.perspective
    );
    return decomposed;
}