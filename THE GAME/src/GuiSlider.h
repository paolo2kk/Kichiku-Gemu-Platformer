#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiSlider : public GuiControl
{
public:
    GuiSlider(int id, SDL_Rect bounds, const char* text, int minValue, int maxValue, int defaultValue);
    virtual ~GuiSlider();

    bool Update(float dt);
    bool Draw(std::shared_ptr<Render> render);

    int GetValue() const { return currentValue; }

    bool canClick = true;
    bool drawBasic = false;
    bool visible = true;

private:
    int minValue;
    int maxValue;
    int currentValue;

    Vector2D mousePos;

    SDL_Rect sliderBar;
    SDL_Rect sliderThumb;
};
