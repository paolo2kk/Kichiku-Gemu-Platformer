#pragma once

#include "Module.h"
#include <list>
#include <vector>
#include <map>

// L10: TODO 2: Define a property to store the Map Orientation and Load it from the map
enum MapOrientation
{
    ORTOGRAPHIC = 0,
    ISOMETRIC
};

// L09: TODO 5: Add attributes to the property structure
struct Properties
{
    struct Property
    {
        std::string name;
        bool value; //We assume that we are going to work only with bool for the moment
    };

    std::list<Property*> propertyList;

    ~Properties()
    {
        for (const auto& property : propertyList)
        {
            delete property;
        }

        propertyList.clear();
    }

    // L09: DONE 7: Method to ask for the value of a custom property
    Property* GetProperty(const char* name);

};

struct MapLayer
{
    // L07: TODO 1: Add the info to the MapLayer Struct
    int id;
    std::string name;
    int width;
    int height;
    std::vector<int> tiles;
    Properties properties;

	float ParallaxSpeedX;
	float ParallaxSpeedY;

    // L07: TODO 6: Short function to get the gid value of i,j
    int Get(int i, int j) const
    {
        return tiles[(j * width) + i];
    }
};
struct AnimationFrame {
    int tileId;       
    int duration;     
};

struct AnimatedTile {
    int gid;                        
    std::vector<AnimationFrame> frames; 
    int totalDuration;          
};
// L06: TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!

struct TileSet
{
    int firstGid;
    std::string name;
    int tileWidth;
    int tileHeight;
    int spacing;
    int margin;
    int tileCount;
    int columns;
    SDL_Texture* texture;
    std::map<int, AnimatedTile> animatedTiles;

    // L07: TODO 7: Implement the method that receives the gid and returns a Rect
    SDL_Rect GetRect(unsigned int gid) {
        SDL_Rect rect = { 0 };

        int relativeIndex = gid - firstGid;
        rect.w = tileWidth;
        rect.h = tileHeight;
        rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
        rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

        return rect;
    }

};

// L06: TODO 1: Create a struct needed to hold the information to Map node
struct MapData
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;
    std::list<TileSet*> tilesets;
    // L10: TODO 2: Define a property to store the Map Orientation and Load it from the map
    MapOrientation orientation;

    // L07: TODO 2: Add the info to the MapLayer Struct
    std::list<MapLayer*> layers;
};

class Map : public Module
{
public:

    Map();

    // Destructor
    virtual ~Map();

    // Called before render is available
    bool Awake();

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update(float dt);

    void UpdateAnimatedTiles(float dt);
    
    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(std::string path, std::string mapFileName);

    // L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
    Vector2D MapToWorld(int x, int y) const;

    // L10: TODO 5: Add method WorldToMap to obtain  map coordinates from screen coordinates 
    Vector2D WorldToMap(int x, int y);

    // L09: TODO 2: Implement function to the Tileset based on a tile id
    TileSet* GetTilesetFromTileId(int gid) const;

    // L09: TODO 6: Load a group of properties 
    bool LoadProperties(pugi::xml_node& node, Properties& properties);

    int GetWidth() {
        return mapData.width;
    }

    int GetHeight() {
        return mapData.height;
    }

    int GetTileWidth() {
        return mapData.tileWidth;
    }

    int GetTileHeight() {
        return mapData.tileHeight;
    }

    MapLayer* GetNavigationLayer();


public: 
    std::string mapFileName;
    std::string mapPath;

private:
    bool mapLoaded;
    // L06: DONE 1: Declare a variable data of the struct MapData
    MapData mapData;
   
    SDL_Texture* BG1;
    SDL_Texture* BG2;
    SDL_Texture* BG3;
    SDL_Texture* BG4;
    float BGvelx = -0.2;
    float BGvely = 0.41;
};