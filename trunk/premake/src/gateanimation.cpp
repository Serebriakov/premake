#include "gateanimation.h"
#include "glesrenderer.h"
#include "tools.h"


GateAnimation::GateAnimation()
{
	m_Sprite = NULL;
}

GateAnimation::~GateAnimation()
{
	delete m_Sprite;
}

void GateAnimation::Initialize(Uint32 AnimationLength)
{
	if( m_Sprite == NULL )
	{
		RenderState *rs = GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("background"), RenderState::BLENDING_DISABLED));
		m_Sprite = new Sprite();
		m_Sprite->InitializeStaticSprite(GLESRenderer::Instance()->GetImage("UNION_001"), rs, AABB(glm::vec2(), glm::vec2(320, 200)));
	}
	m_AnimationTimer.SetCycleInterval(AnimationLength);
}

float GateAnimation::GetAnimationState() const
{
	return m_AnimationTimer.GetCycleState();
}

void GateAnimation::StartAnimation()
{
	m_AnimationTimer.Reset();
}

void GateAnimation::Update()
{
	float t = m_AnimationTimer.GetCycleState();
	Tools::ClampValue<float>(t, 0.f, 1.f);

	if( t <= 0.5 )
	{
		t *= 2;
		m_Position[0] = glm::vec2(0.f, 200 - t * 100);
		m_Position[1] = glm::vec2(0.f, t * 100 - m_Sprite->GetSize().y);
	}
	else
	{
		t *= 2;
		t -= 1;
		m_Position[0] = glm::vec2(160 + t * 160, 0.f);
		m_Position[1] = glm::vec2(160 - t * 160 - m_Sprite->GetSize().x, 0.f);
	}
}

void GateAnimation::Draw()
{
	GLESRenderer::Instance()->AddSprite2D(m_Sprite, m_Position[0]);
	GLESRenderer::Instance()->AddSprite2D(m_Sprite, m_Position[1]);
	GLESRenderer::Instance()->FlushSprites();
}
