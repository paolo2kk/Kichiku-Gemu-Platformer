#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Textures.h"

GuiControlButton::GuiControlButton(int id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	if (!isLayoutSet)
	{
		layoutUI = Engine::GetInstance().textures.get()->Load("Assets/UI/GUI.png");
		menuUI = Engine::GetInstance().textures.get()->Load("Assets/UI/menu.png");
		goldCoin = Engine::GetInstance().textures.get()->Load("Assets/Textures/goldCoin.png");
		isLayoutSet = true;
	}
	
	if (state != GuiControlState::DISABLED && !isLayout)
	{
		if (visible)
		{

			// L16: TODO 3: Update the state of the GUiButton according to the mouse position
			Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

			//If the position of the mouse if inside the bounds of the button 
			if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

				state = GuiControlState::FOCUSED;

				if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
					state = GuiControlState::PRESSED;
				}

				if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
					NotifyObserver();
				}
			}
			else {
				state = GuiControlState::NORMAL;
			}

			//L16: TODO 4: Draw the button according the GuiControl State
			switch (state)
			{
			case GuiControlState::DISABLED:
				Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
				break;
			case GuiControlState::NORMAL:

				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
				break;
			case GuiControlState::FOCUSED:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
				break;
			case GuiControlState::PRESSED:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
				isClicked = true;
				break;
			}
			Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h);

		}
		
	}
	else {
		if (visible)
		{
			if (isMenu)
			{
				Engine::GetInstance().render->DrawTexture(menuUI, bounds.x, bounds.y + 90);
			}
			else if (!isMenu)
			{
				state = GuiControlState::NORMAL;
				
				Engine::GetInstance().render->DrawTexture(layoutUI, bounds.x, bounds.y + 90);

				//Dependiendo de las vidas del player se dibujan las monedas

				for (int i = 0; i < Engine::GetInstance().scene->player->lives; i++) {
					Engine::GetInstance().render->DrawTexture(goldCoin, bounds.x + 20, bounds.y + 100);
					bounds.x += 25;
				}
			}
		}
		
		
		
	}

	return false;
}

