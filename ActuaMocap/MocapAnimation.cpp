#include "MocapAnimation.h"
#include "MocapFile.h"
#include <iostream>

MocapAnimation::MocapAnimation(const MocapFile* mocapFile, const SkeletonConnectivity& connectivity)
	:_mocapFile(mocapFile),
	_connectivity(connectivity),
	_skeletonRoot(_skeleton)
{
	memcpy(_currentFrame.points, &_mocapFile->CGetFrames()[0], PlayerPoints * sizeof(glm::vec3)); // load first frame
	SetFileLengthSeconds();
	_skeletonRoot->SetLocalPos({ 0,0,0 });
	_skeletonRoot->SetLocalEulers({ 0,0,0 });

}

void MocapAnimation::Update(double deltaT)
{
	const auto& frames = _mocapFile->CGetFrames();
	int numFrames = frames.size();
	if (numFrames == 0) {
		return;
	}

	_animationProgressSeconds += deltaT;
	_counter += deltaT;
	if (_counter >= _inverseFps) {
		_counter = 0;
		_previousFrame++;
		_nextFrame++;
		if (_previousFrame >= numFrames) {
			_previousFrame = 0;
			
		}
		if (_nextFrame >= numFrames) {
			_nextFrame = 0;
			_animationProgressSeconds = 0.0;
		}
	}
	// set _currentFrame - interpolate each frame between two frames in the loaded file
	double t = _counter / _inverseFps;
	for (int i = 0; i < PlayerPoints; i++) {
		_currentFrame.points[i] = glm::mix(
			frames[_previousFrame].points[i],
			frames[_nextFrame].points[i],
			t
		);
	}
}

void MocapAnimation::SetFps(double newFps)
{
	_fps = newFps;
	_inverseFps = 1 / newFps;
	SetFileLengthSeconds();
}

void MocapAnimation::ResetAfterNewFileLoad()
{
	_previousFrame = 0;
	_nextFrame = 1;
	_counter = 0.0;
	_animationProgressSeconds = 0.0;
	SetFileLengthSeconds();

}


void MocapAnimation::SetToFrame(int frameNumber)
{
	const auto& frames = _mocapFile->CGetFrames();
	if (frameNumber < 0 || frameNumber >= frames.size()) {
		std::cout << "invalid frame number " << frameNumber << " anumation has " << frames.size() << " frames\n";
	}
	const auto& frame2Set = frames[frameNumber];
	for (int i = 0; i < PlayerPoints; i++) {
		_currentFrame.points[i] = frame2Set.points[i];
	}
	_previousFrame = frameNumber;
	_nextFrame = frameNumber + 1;

	int numFrames = frames.size();
	if (_nextFrame >= numFrames) {
		_nextFrame = 0;
	}
	_counter = 0;
	PopulateSkeleton();
}

int MocapAnimation::GetNumFrames()
{
	const auto& frames = _mocapFile->CGetFrames();
	int numFrames = frames.size();
	return numFrames;
}

void MocapAnimation::PopulateSkeleton()
{
	for (int i = 0; i < _connectivity.size(); i++) {
		auto parentIndex = _connectivity[i].first;
		const auto& children = _connectivity[i].second;
		auto& parent =_skeleton[parentIndex];
		for (int j = 0; j < children.size(); j++) {
			auto childIndex = children[j];
			auto& child = _skeleton[childIndex];
			const auto& childPos = _currentFrame.points[childIndex - 1];
			const auto& parentPos = parent.GetWorldPosition();
			
			child.SetLocalPos(parentPos - childPos);
			parent.AddChild(&child);
		}
	}
}

void MocapAnimation::SetFileLengthSeconds()
{
	const auto& frames = _mocapFile->CGetFrames();
	int numFrames = frames.size();
	_fileLengthSeconds = (double)numFrames / _fps;
}
