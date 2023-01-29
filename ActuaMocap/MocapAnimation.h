#pragma once
#include "MocapFrame.h"
#include "MocapNode.h"
#include <vector>


class MocapFile;

typedef std::vector < std::pair<size_t, std::vector<size_t>>> SkeletonConnectivity; // parent, vector of children
class MocapAnimation {
public:
	MocapAnimation(const MocapFile* mocapFile, const SkeletonConnectivity& connectivity);
	void Update(double deltaT);
	void SetFps(double newFps);
	void ResetAfterNewFileLoad();
	inline const MocapFrame& GetCurrentFrame() {
		return _currentFrame;
	}
	inline const int GetCurrentFrameNumber() {
		return _previousFrame;
	}
	inline double GetCurrentLengthSeconds() {
		return _fileLengthSeconds;
	}
	double GetAnimationProgressSeconds() {
		return _animationProgressSeconds;
	}

	inline MocapNode* GetSkeletonRoot() {
		return _skeletonRoot;
	}

	void SetToFrame(int frameNumber);
	int GetNumFrames();
	void PopulateSkeleton();
private:
	void SetFileLengthSeconds();
	
private:
	double _fps = 16.0;
	double _inverseFps = 1 / 16.0;
	int _previousFrame = 0;
	int _nextFrame = 1;
	double _counter = 0.0;
	MocapFrame _currentFrame;
	const MocapFile* _mocapFile;
	double _fileLengthSeconds;
	double _animationProgressSeconds;
	MocapNode _skeleton[PlayerPoints+1];
	MocapNode* _skeletonRoot;
	SkeletonConnectivity _connectivity;
};