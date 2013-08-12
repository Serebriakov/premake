#include "sprite.h"


Sprite::Sprite() : m_CurrentImage(NULL), m_RenderState(NULL), m_SizedByCurrentImage(false), m_FlipHorizontally(false)
{
}

/*
Initialize a static sprite
*/
void Sprite::InitializeStaticSprite(Image *CurrentImagePtr, RenderState *RenderStatePtr, const AABB &SpriteAABB)
{
	m_CurrentImage = CurrentImagePtr;
	m_RenderState = RenderStatePtr;
	m_AABB = SpriteAABB;

	m_Animated = false;
	m_FinishedCurrentSequence = false;
	m_CurrentSequenceIndex = 0;
	m_CurrentFrameNumber = 0;
}

/*
Initialize an animated sprite
*/
void Sprite::InitializeAnimatedSprite(ImageSequence *FirstImageSequencePtr, RenderState *RenderStatePtr, const AABB &SpriteAABB)
{
	m_ImageSequences.clear();
	AddImageSequence(FirstImageSequencePtr);
	JumpToSequence(0);
	m_RenderState = RenderStatePtr;
	m_AABB = SpriteAABB;

	m_Animated = true;
}

const Image *Sprite::GetCurrentImage() const
{
	return m_CurrentImage;
}


void Sprite::SetRenderState(RenderState *RenderStatePtr)
{
	m_RenderState = RenderStatePtr;
}

void Sprite::SetPosition(const glm::vec2 &Position)
{
	m_AABB.m_Position = Position;
}

void Sprite::SetSize(const glm::vec2 &Size)
{
	m_AABB.m_Size = Size;
}

const RenderState *Sprite::GetRenderState() const
{
	return m_RenderState;
}

const glm::vec2 &Sprite::GetPosition() const
{
	return m_AABB.m_Position;
}

const glm::vec2 &Sprite::GetSize() const
{
	return m_SizedByCurrentImage ? m_CurrentImage->GetSize() : m_AABB.m_Size;
}

const AABB &Sprite::GetAABB() const
{
	return m_AABB;
}

void Sprite::FlipHorizontally(bool Flip)
{
	m_FlipHorizontally = Flip;
}

bool Sprite::IsFlippedHorizontally() const
{
	return m_FlipHorizontally;
}

void Sprite::Animate()
{
	if( m_Animated )
	{
		Uint32 frame_steps = m_AnimationTimer.GetCycles();
		size_t last_frame = m_CurrentFrameNumber;
		for( Uint32 step = 0; step < frame_steps; ++step )
		{
			m_CurrentFrameNumber = m_ImageSequences[m_CurrentSequenceIndex]->GetNextImageIndex(m_CurrentFrameNumber);
		}
		m_CurrentImage = m_ImageSequences[m_CurrentSequenceIndex]->GetImage(m_CurrentFrameNumber);

		m_FinishedCurrentSequence = false;
		if( m_ImageSequences[m_CurrentSequenceIndex]->GetPlaybackType() == ImageSequence::ONCE )
		{
			if( m_CurrentFrameNumber == m_ImageSequences[m_CurrentSequenceIndex]->GetNumberOfImages() - 1)
			{
				m_FinishedCurrentSequence = true;
			}
		}
		else if( m_ImageSequences[m_CurrentSequenceIndex]->GetPlaybackType() == ImageSequence::LOOP )
		{
			if( (m_CurrentFrameNumber != last_frame) && (m_CurrentFrameNumber == 0) )
			{
				m_FinishedCurrentSequence = true;
			}
		}
	}
}

bool Sprite::IsAnimated() const
{
	return m_Animated;
}

void Sprite::AddImageSequence(ImageSequence *NewSequence)
{
	m_ImageSequences.push_back(NewSequence);
	m_Animated = true;
}

const ImageSequence *Sprite::GetCurrentImageSequence() const
{
	return m_ImageSequences[m_CurrentSequenceIndex];
}

size_t Sprite::GetCurrentImageSequenceIndex() const
{
	return m_CurrentSequenceIndex;
}

size_t Sprite::GetCurrentFrameNumber() const
{
	return m_CurrentFrameNumber;
}

size_t Sprite::GetNumberOfSequences() const
{
	return m_ImageSequences.size();
}

bool Sprite::FinishedCurrentSequence() const
{
	return (m_Animated == false) || (m_FinishedCurrentSequence);
}

void Sprite::JumpToSequence(size_t SequenceIndex)
{
	if( SequenceIndex != m_CurrentSequenceIndex )
	{
		m_CurrentSequenceIndex = SequenceIndex;
		m_CurrentFrameNumber = 0;
		m_AnimationTimer.SetCycleInterval(m_ImageSequences[m_CurrentSequenceIndex]->GetFrameTime());
		m_AnimationTimer.Reset();
		m_FinishedCurrentSequence = false;

		m_CurrentImage = m_ImageSequences[m_CurrentSequenceIndex]->GetImage(m_CurrentFrameNumber);
	}
}

void Sprite::JumpToFrame(size_t FrameNumber)
{
	if( FrameNumber < m_ImageSequences[m_CurrentSequenceIndex]->GetNumberOfImages() )
	{
		m_CurrentFrameNumber = FrameNumber;
		m_CurrentImage = m_ImageSequences[m_CurrentSequenceIndex]->GetImage(m_CurrentFrameNumber);
	}
}

void Sprite::JumpToSequence(size_t SequenceIndex, size_t FrameNumber)
{
	JumpToSequence(SequenceIndex);
	JumpToFrame(FrameNumber);
}

bool Sprite::IsSizedByCurrentImage() const
{
	return m_SizedByCurrentImage;
}

void Sprite::UseSizeOfCurrentImage(bool Toggle)
{
	m_SizedByCurrentImage = Toggle;
}


// --- vertex attributes ---

SpriteVertexAttributes::SpriteVertexAttributes(float PosX, float PosY, float TexCoordX, float TexCoordY) : PositionX(PosX), PositionY(PosY), TextureCoordinateX(TexCoordX), TextureCoordinateY(TexCoordY)
{
}


// --- sprite render modes ---

SpriteRenderMode::SpriteRenderMode(const TextureObject *TexturePtr, const RenderState *RenderStatePtr) : Texture(const_cast<TextureObject *>(TexturePtr)), RenderStatePtr(const_cast<RenderState *>(RenderStatePtr))
{
}

/*
operator< for ordering SpriteRenderMode objects
*/
bool SpriteRenderMode::operator<(const SpriteRenderMode &Other) const
{
	if( Texture == Other.Texture )
	{
		return RenderStatePtr < Other.RenderStatePtr;
	}
	else
	{
		return Texture < Other.Texture;
	}
}
