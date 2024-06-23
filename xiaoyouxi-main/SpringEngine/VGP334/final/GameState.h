#pragma once
#include <SpringEngine/Inc/SpringEngine.h>

class GameState : public SpringEngine::AppState
{
public:
	~GameState() = default;
	void Initialize()  override;
	void Terminate()  override;
	void Update(const float deltaTime) override;
	void Render() override;
	void DebugUI() override;
	void OnSpacePressedEvent(const SpringEngine::Event* event);

	void RenderGroupModify(SpringEngine::Graphics::RenderGroup &renderGroup, SpringEngine::Math::Vector3 moffset);
	void RenderGroupSet(SpringEngine::Graphics::RenderGroup& renderGroup, SpringEngine::Graphics::Transform transform);

	void ParticleInit();


private:
	void UpdateCameraControl(float deltaTime);

	SpringEngine::Graphics::Camera mCamera;
	SpringEngine::Graphics::DirectionalLight mDirectionalLight;
	SpringEngine::Graphics::StandardEffect mStandardEffect;

	SpringEngine::Graphics::ModelId mCharacterId1;
	SpringEngine::Graphics::Animator mCharacterAnimator1;
	SpringEngine::Graphics::RenderGroup mCharacter1;
	SpringEngine::Graphics::RenderObject mGround1;

	SpringEngine::Graphics::ModelId mCharacterId2;
	SpringEngine::Graphics::Animator mCharacterAnimator2;
	SpringEngine::Graphics::RenderGroup mCharacter2;
	
	SpringEngine::Graphics::ParticleEffect mParticleEffect;
	SpringEngine::FireworkParticleSystem mParticleSystem;

	bool mDrawSkeleton = false;

	//anmation
	SpringEngine::Graphics::Animation mAnimation;
	float mAnimationTime = 0.0f;
	bool mParticleStop = true;
};