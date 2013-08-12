#include "imagesequence.h"
#include "logger.h"


ImageSequence::ImageSequence(PLAYBACK_TYPE PlaybackType, unsigned FrameTime) : m_PlaybackType(PlaybackType), m_FrameTime(FrameTime)
{
	m_PlaybackDirection = FORWARD;
	m_NumberOfImages = 0;
}

void ImageSequence::AddImage(Image *NewImage)
{
	m_Images.push_back(NewImage);
	m_NumberOfImages++;
}

void ImageSequence::RemoveImages()
{
	m_Images.clear();
	m_NumberOfImages = 0;
}

size_t ImageSequence::GetNextImageIndex(size_t CurrentImageIndex) const
{
	switch( m_PlaybackType )
	{
		case LOOP :
		{
			++CurrentImageIndex %= m_NumberOfImages;

			break;
		}

		case FORWARD_BACKWARD :
		{
			if( m_PlaybackDirection == FORWARD )
			{
				if( CurrentImageIndex < m_NumberOfImages - 1 )
				{
					++CurrentImageIndex;
				}
				else
				{
					--CurrentImageIndex;
					m_PlaybackDirection = BACKWARD;
				}
			}
			else
			{
				if( CurrentImageIndex > 0 )
				{
					--CurrentImageIndex;
				}
				else
				{
					++CurrentImageIndex;
					m_PlaybackDirection = FORWARD;
				}
			}

			break;
		}

		case ONCE :
		{
			CurrentImageIndex = (CurrentImageIndex < m_NumberOfImages - 1) ? (CurrentImageIndex + 1) : (m_NumberOfImages - 1);

			break;
		}
	}

	return CurrentImageIndex;
}

size_t ImageSequence::GetNumberOfImages() const
{
	return m_NumberOfImages;
}

const Image *ImageSequence::GetImage(size_t Index) const
{
	assert((Index < m_NumberOfImages) && ("ERROR: ImageSequence indexed out of range."));
	return m_Images[Index];
}

ImageSequence::PLAYBACK_TYPE ImageSequence::GetPlaybackType() const
{
	return m_PlaybackType;
}

unsigned ImageSequence::GetFrameTime() const
{
	return m_FrameTime;
}
