#include "SceneManager.h"


SceneManager::SceneManager(SceneBase* start_scene) : _now_scene(start_scene) {

	_transGraph_hdl = LoadGraph("graphics/FadeFxImage/black.bmp");
}


SceneManager* SceneManager::GetInstance(SceneBase* startScene) {

	static SceneManager* instance = nullptr;

	if (instance == nullptr) {

		instance = new SceneManager(startScene);
	}

	return instance;
}


void SceneManager::ChangeScene(SceneBase* next_scene, float trans_time) {

	_next_scene = next_scene;
	_transTime = trans_time;
	_sequence.change(&SceneManager::SeqTransOut);
}


void SceneManager::Render(float deltaTime) {

	if (_now_scene)
	{
		_now_scene->Render(deltaTime);
	}

	_sequence.update(deltaTime);
}


void SceneManager::Update(float deltaTime) {

	if (_now_scene)
	{
		_now_scene->Update(deltaTime);
	}

	_sequence.update(deltaTime);
}


bool SceneManager::SeqTransIn(const float delta_time) {

	float alpha = 255.f - (_sequence.getProgressTime() / _transTime * 255.0f);

	if (alpha <= 0.f)
	{
		_sequence.change(&SceneManager::SeqRunScene);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha));
	DrawExtendGraph(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, _transGraph_hdl, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	return true;
}


bool SceneManager::SeqTransOut(const float delta_time) {

	float alpha = (_sequence.getProgressTime() / _transTime * 255.0f);

	if (alpha >= 255.f)
	{
		_sequence.change(&SceneManager::SeqTransIn);
		_now_scene->ReleaseMem();
		delete _now_scene;
		_now_scene = _next_scene;
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha));
	DrawExtendGraph(0, 0, DXE_WINDOW_WIDTH, DXE_WINDOW_HEIGHT, _transGraph_hdl, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	return true;
}


bool SceneManager::SeqRunScene(const float delta_time) {

	return true;
}


void SceneManager::ReleaseMem() {

	delete GetInstance();
}