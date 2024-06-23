#include"GameState.h"


using namespace SpringEngine;
using namespace SpringEngine::Core;
using namespace SpringEngine::Graphics;
using namespace SpringEngine::Math;
using namespace SpringEngine::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f,3.0f,-3.0f });
	mCamera.SetLookAt({ 0.0f,1.0f,0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f,-1.0f,1.0f });
	mDirectionalLight.ambient = { 0.5f,0.5f,0.5f,1.0f };
	mDirectionalLight.diffuse = { 0.8f,0.8f,0.8f,1.0f };
	mDirectionalLight.specular = { 1.0f,1.0f,1.0f,1.0f };


	mStandardEffect.Initialize(L"../../Assets/Shaders/Standard.fx");
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);



	Mesh Ground = MeshBuilder::CreateGroundPlane(10, 10, 1.0f);
	mGround1.meshBuffer.Initialize(Ground);
	mGround1.diffuseMapId = TextureManager::Get()->LoadTexture("misc/concrete.jpg");
	
	mCharacterId1 = ModelManager::Get()->LoadModel("../../Assets/Models/Test/up.model");
	mCharacter1 = CreateRenderGroup(mCharacterId1,&mCharacterAnimator1);
	mCharacterAnimator1.Initialize(mCharacterId1);
	mCharacterAnimator1.AddGlobalKeyFrame(0, [&]() { mCharacterAnimator1.Stop(); });
	
	
	mCharacterAnimator1.PlayAnimation(0, true);

	mCharacterId2 = ModelManager::Get()->LoadModel("../../Assets/Models/Jumping/Jumping.model");
	mCharacter2 = CreateRenderGroup(mCharacterId2, &mCharacterAnimator2);
	mCharacterAnimator2.Initialize(mCharacterId2);
	
	mCharacterAnimator2.PlayAnimation(0, true);
	mCharacterAnimator2.AddKeyFrame(0, [&]() { mCharacterAnimator2.ChangeSpeed(1.0f); });
	mCharacterAnimator2.AddKeyFrame(mCharacterAnimator2.GetFrameCount() / 2, [&]() { mCharacterAnimator2.ChangeSpeed(0.1f); });
	mCharacterAnimator2.AddKeyFrame(mCharacterAnimator2.GetFrameCount() * 3 / 4, [&]() { mCharacterAnimator2.ChangeSpeed(0.5f); });
	mCharacterAnimator2.AddKeyFrame(mCharacterAnimator2.GetFrameCount() - 1, [&]() { 
		Transform tempt = Transform();
		tempt.scale = Math::Vector3::Zero;
		RenderGroupSet(mCharacter2, tempt);
		mCharacterAnimator2.Stop();
		mCamera.SetPosition({ 0.0f,6.0f,-6.0f });
	});
	mCharacterAnimator1.AddGlobalKeyFrame(
		mCharacterAnimator2.GetFrameCount() / 2 + 
		(mCharacterAnimator2.GetFrameCount() * 3 / 4 - mCharacterAnimator2.GetFrameCount() / 2) * 10,
		[&]() {
			mAnimationTime = 1.0f;
			mAnimation = AnimationBuilder()
				.AddPositionKey({ 0.0f, 5.0f, 0.0f }, 0.0f, EaseType::EaseInOutQuad)
				.AddPositionKey({ 0.0f, 0.5f, 0.0f }, 1.0f, EaseType::EaseInQuad)
				.AddPositionKey({ 0.0f, 0.5f, 0.0f }, 1.1f)
				.AddPositionKey({ 0.0f, 5.0f, 0.0f }, 2.0f, EaseType::EaseOutQuad)
				.AddRotationKey({ 0.0f,0.0f,0.0f,1.0f }, 0.0f)
				.AddRotationKey(Math::Quaternion::Normalize({ 6.28f,0.0f,0.0f,1.0f }), 2.0f)
				.AddScaleKey({ 1.0f, 1.0f, 1.0f }, 0.0f)
				.AddScaleKey({ 1.0f, 1.0f, 1.0f }, 0.8f)
				.AddScaleKey({ 1.0f, 0.25f, 1.0f }, 1.0f)
				.AddScaleKey({ 1.0f, 0.25f, 1.0f }, 1.1f)
				.AddScaleKey({ 1.0f, 1.5f, 1.0f }, 1.25f)
				.AddScaleKey({ 1.0f, 1.0f, 1.0f }, 2.0f)
				.Build();
			mParticleStop = false;
			mCharacterAnimator1.Start();
			
		});
	// mCharacterId = ModelManager::Get()->LoadModel("../../Assets/Models/Jumping/Jumping.model");
	RenderGroupModify(mCharacter2, { 0.0f, 0.0f, 4.0f });

	EventManager::Get()->AddListener(EventType::SpeacePressed, std::bind(&GameState::OnSpacePressedEvent, this, std::placeholders::_1));
	ParticleInit();

	mAnimation = AnimationBuilder()
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 0.0f, EaseType::EaseInOutQuad)
		.AddPositionKey({ 0.0f, 0.0f, 0.0f }, 2.0f, EaseType::EaseOutQuad)
		.Build();
	mAnimationTime = 0.0f;
}

void GameState::Terminate()
{
	CleanupRenderGroup(mCharacter1);
	mGround1.Terminate();
	CleanupRenderGroup(mCharacter2);
	mStandardEffect.Terminate();
	mParticleSystem.Terminate();
	mParticleEffect.Terminate();
}
void GameState::Update(const float deltaTime)
{
	if (TimeUtil::GetGlobalTime() > 45.0f) {
		return;
	}
	if (!mParticleStop) {
		mParticleSystem.Update(deltaTime);
	}
	
	UpdateCameraControl(deltaTime);

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::SPACE))
	{
		SpacePressedEvent event;
		EventManager::Broadcast(&event);
	}

	mCharacterAnimator1.Update(deltaTime);
	mCharacterAnimator2.Update(deltaTime);

	mAnimationTime += deltaTime;
	while (mAnimationTime > mAnimation.GetDuration())
	{
		mAnimationTime -= mAnimation.GetDuration();
	}

	mParticleSystem.SetPosition(mCharacter1.back().transform.position);
}
void GameState::Render()
{
	SimpleDraw::AddGroundPlane(10.0f, Colors::White);
	SimpleDraw::Render(mCamera);
	RenderGroupSet(mCharacter1, mAnimation.GetTransform(mAnimationTime));

	mStandardEffect.Begin();
		if (mDrawSkeleton)
		{
			AnimationUtil::BoneTransforms boneTransforms;
			AnimationUtil::ComputeBoneTransforms(mCharacterId1, boneTransforms,&mCharacterAnimator1);
			AnimationUtil::ComputeBoneTransforms(mCharacterId2, boneTransforms,&mCharacterAnimator2);
			AnimationUtil::DrawSkeleton(mCharacterId1, boneTransforms);
			AnimationUtil::DrawSkeleton(mCharacterId2, boneTransforms);
		}
		else
		{
			DrawRenderGroup(mStandardEffect, mCharacter1);
			DrawRenderGroup(mStandardEffect, mCharacter2);
		}
		mStandardEffect.Render(mGround1);
	mStandardEffect.End();

	mParticleEffect.Begin();
		mParticleSystem.Render(mParticleEffect);
	mParticleEffect.End();

}
void GameState::DebugUI()
{
	ImGui::Begin("Debug control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text(("Time: " + std::to_string(TimeUtil::GetGlobalTime()) + "s").c_str());
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Direction", &mDirectionalLight.direction.x, 0.1f))
		{
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}
	ImGui::Checkbox("DrawSkeleton", &mDrawSkeleton);
	mStandardEffect.DebugUI();
	ImGui::End();
}
void GameState::UpdateCameraControl(float deltaTime)
{
	auto input = Input::InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 1.0f;
	const float turnSpeed = 0.1f;

	if (input->IsKeyDown(KeyCode::W))
	{
		mCamera.Walk(moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::S))
	{
		mCamera.Walk(-moveSpeed * deltaTime);
	}
	if (input->IsKeyDown(KeyCode::D))
	{
		mCamera.Strafe(moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::A))
	{
		mCamera.Strafe(-moveSpeed * deltaTime);
	}
	if (input->IsKeyDown(KeyCode::E))
	{
		mCamera.Rise(moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::Q))
	{
		mCamera.Rise(-moveSpeed * deltaTime);
	}
	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}

void GameState::RenderGroupModify(SpringEngine::Graphics::RenderGroup& renderGroup, SpringEngine::Math::Vector3 moffset)
{
	for (SpringEngine::Graphics::RenderObject &renderObject : renderGroup) {
		renderObject.transform.position += moffset;
	}
}

void GameState::RenderGroupSet(SpringEngine::Graphics::RenderGroup& renderGroup, SpringEngine::Graphics::Transform transform)
{
	for (SpringEngine::Graphics::RenderObject& renderObject : renderGroup) {
		renderObject.transform = transform;
	}
}

void GameState::OnSpacePressedEvent(const SpringEngine::Event* event)
{
	SpacePressedEvent* spEvent = (SpacePressedEvent*)event;
	RenderGroupModify(mCharacter2, { 0.0f, 0.0f, 1.0f });
}

void GameState::ParticleInit()
{
	mParticleEffect.Initialize();
	mParticleEffect.SetCamera(mCamera);

	ParticleSystemInfo info;
	info.spawnPosition = Math::Vector3::Zero;
	info.spawnDirection = Math::Vector3::YAxis;
	info.spawnDelay = 0.0f;
	info.systemLifeTime = 99999.0f;
	info.minParticlePerEmit = 3;
	info.maxParticlePerEmit = 10;
	info.minTimeBetweenEmit = 0.0f;
	info.maxTimeBetweenEmit = 0.05f;
	info.minSpawnAngle = -10.0f * 3.141592 / 180.0f;
	info.maxSpawnAngle = 10.0f * 3.141592 / 180.0f;
	info.minSpeed = 5.0f;
	info.maxSpeed = 10.0f;
	info.minLifeTime = 0.3f;
	info.maxLifeTime = 1.0f;
	info.minStartColor = Colors::White;
	info.maxStartColor = Colors::White;
	info.minEndColor = Colors::White;
	info.maxEndColor = Colors::White;
	info.minStartScale = Math::Vector3::One * 0.02f;
	info.maxStartScale = Math::Vector3::One * 0.02f;
	info.minEndScale = Math::Vector3::Zero;
	info.maxEndScale = Math::Vector3::Zero;
	info.maxParticles = 100;
	info.particleTextureId = TextureManager::Get()->LoadTexture("sun.jpg");
	mParticleSystem.Initialize(info);
	mParticleSystem.SetCamera(mCamera);
}