#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <vector>

#include "imagesequence.h"
#include "renderstate.h"
#include "aabb.h"
#include "stopwatch.h"


/*
2D sprite
*/
class Sprite
{
protected :
	std::vector<ImageSequence*> m_ImageSequences;								// pointers to the image sequences if the sprite is animated
	bool						m_Animated;										// true if the sprite is animated (has pointers in the m_ImageSequences array)
	size_t						m_CurrentSequenceIndex;							// index of the image sequence currently in use
	size_t						m_CurrentFrameNumber;							// index of the current frame within the current sequence
	bool						m_FinishedCurrentSequence;						// the animation has reached the last frame of the current image sequence
	Stopwatch					m_AnimationTimer;								// timer used for changing frames
	bool						m_FlipHorizontally;								// if true, sprite is mirrored about axis Y
	const Image					*m_CurrentImage;								// the image currently used by the sprite
	RenderState					*m_RenderState;									// pointer to the renderstate used by the sprite
	AABB						m_AABB;											// the sprite's upper left corner and width and height in world-space coordinates
	bool						m_SizedByCurrentImage;							// always use the current image's size instead of the sprite's size


public :
	Sprite();
		
	// initialize the sprite's properties
	void InitializeStaticSprite(Image *CurrentImagePtr, RenderState *RenderStatePtr, const AABB &SpriteAABB);
	void InitializeAnimatedSprite(ImageSequence *FirstImageSequencePtr, RenderState *RenderStatePtr, const AABB &SpriteAABB);

	const Image *GetCurrentImage() const;

	void SetRenderState(RenderState *RenderStatePtr);
	void SetPosition(const glm::vec2 &Position);
	void SetSize(const glm::vec2 &Size);

	const RenderState *GetRenderState() const;
	const glm::vec2 &GetPosition() const;
	const glm::vec2 &GetSize() const;
	const AABB &GetAABB() const;

	void FlipHorizontally(bool Flip);
	bool IsFlippedHorizontally() const;

	void Animate();
	bool IsAnimated() const;

	void AddImageSequence(ImageSequence *NewSequence);
	const ImageSequence *GetCurrentImageSequence() const;

	size_t GetCurrentImageSequenceIndex() const;
	size_t GetCurrentFrameNumber() const;
	size_t GetNumberOfSequences() const;

	bool FinishedCurrentSequence() const;

	void JumpToSequence(size_t SequenceIndex);
	void JumpToFrame(size_t FrameNumber);
	void JumpToSequence(size_t SequenceIndex, size_t FrameNumber);

	bool IsSizedByCurrentImage() const;
	void UseSizeOfCurrentImage(bool Toggle);
};


/*
Per-vertex attributes of a 2D sprite
*/
class SpriteVertexAttributes
{
public :
	float						PositionX;
	float						PositionY;
	float						TextureCoordinateX;
	float						TextureCoordinateY;

	SpriteVertexAttributes(float PosX = 0, float PosY = 0, float TexCoordX = 0, float TexCoordY = 0);
};

/*
Settings for rendering 2D sprites
*/
class SpriteRenderMode
{
public :
	TextureObject				*Texture;										// pointer to the texture object used in this mode
	RenderState					*RenderStatePtr;								// pointer to the renderstate used in this mode

	SpriteRenderMode(const TextureObject *TexturePtr, const RenderState *RenderStatePtr);

	// operator< for ordering SpriteRenderMode objects
	bool operator<(const SpriteRenderMode &Other) const;
};

#endif
