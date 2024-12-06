#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlButton : public GuiControl
{

public:

	GuiControlButton(int id, SDL_Rect bounds, const char* text);
	virtual ~GuiControlButton();

	// Called each loop iteration
	bool Update(float dt);
	bool isLayout = false;
private:

	bool canClick = true;
	bool isLayoutSet = false;
	bool drawBasic = false;
	SDL_Texture* layoutUI;
	
};

#pragma once