#ifndef _IMAGE_SEQUENCE_H_
#define _IMAGE_SEQUENCE_H_

#include <vector>

#include "image.h"


/*
Stores an array of pointers to Image objects (as frames of an animation)
*/
class ImageSequence
{
public :
	// type of animation
	enum PLAYBACK_TYPE
	{
		LOOP,																	// 0,1,2,0,1,2,...
		FORWARD_BACKWARD,														// 0,1,2,1,0,1,...
		ONCE																	// 0,1,2
	};

	// direction of playback
	enum PLAYBACK_DIRECTION
	{
		FORWARD,																// 0,1,2...
		BACKWARD																// ...,2,1,0
	};


protected :
	PLAYBACK_TYPE				m_PlaybackType;									// determines the order of frames
	mutable PLAYBACK_DIRECTION	m_PlaybackDirection;							// the current direction of playback, used only if playback type is forward_backward
	std::vector<Image *>		m_Images;										// frames of the sequence
	size_t						m_NumberOfImages;								// number of image pointers in the array
	unsigned					m_FrameTime;									// duration of 1 frame, in milliseconds


public :
	ImageSequence(PLAYBACK_TYPE PlaybackType, unsigned FrameTime);

	void AddImage(Image *NewImage);
	void RemoveImages();

	size_t GetNextImageIndex(size_t CurrentImageIndex) const;
	size_t GetNumberOfImages() const;

	const Image *GetImage(size_t Index) const;

	PLAYBACK_TYPE GetPlaybackType() const;

	unsigned GetFrameTime() const;
};

#endif
