#pragma once


class SceneTitle_SelectMenu : public SceneBase
{
public:

	SceneTitle_SelectMenu();

	void Update(float deltaTime) override;
	void Render(float deltaTime) override;
	void ReleaseMem() override;

private:

	void ShowMenu();

	void SelectMenuByInput();

public:

	typedef struct {

		int  menu_x;          // 文字列座標 X
		int  menu_y;          // 文字列座標 Y
		char menu_name[128];  // 文字列名
	}SelectMenu;

private:

	int       _bgImage_hdl{};
	int       _menu_index{};
};