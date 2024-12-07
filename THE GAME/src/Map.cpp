
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    float cameraX = Engine::GetInstance().render.get()->camera.x;
    float cameraY = Engine::GetInstance().render.get()->camera.y;

    Engine::GetInstance().render.get()->DrawTexture(BG, -cameraX, -cameraY);

    Engine::GetInstance().render.get()->DrawTexture(BG1, -cameraX * parallaxFactor1, -cameraY * parallaxFactor1);
    Engine::GetInstance().render.get()->DrawTexture(BG2, -cameraX * parallaxFactor2, -cameraY * parallaxFactor2);
    Engine::GetInstance().render.get()->DrawTexture(BG3, -cameraX * parallaxFactor3, -cameraY * parallaxFactor3);
    Engine::GetInstance().render.get()->DrawTexture(BG4, -cameraX * parallaxFactor4, -cameraY * parallaxFactor4);


    if (mapLoaded) {

        // L07 TODO 5: Prepare the loop to draw all tiles in a layer + DrawTexture()
        // iterate all tiles in a layer
        for (const auto& mapLayer : mapData.layers) {
            //Check if the property Draw exist get the value, if it's true draw the lawyer
            if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {

                        // L07 TODO 9: Complete the draw function

                        //Get the gid from tile
                        int gid = mapLayer->Get(i, j);
                        //Check if the gid is different from 0 - some tiles are empty
                        if (gid != 0) {
                            //L09: TODO 3: Obtain the tile set using GetTilesetFromTileId
                            TileSet* tileSet = GetTilesetFromTileId(gid);
                            if (tileSet != nullptr) {
                                //Get the Rect from the tileSetTexture;
                                SDL_Rect tileRect = tileSet->GetRect(gid);
                                //Get the screen coordinates from the tile coordinates
                                Vector2D mapCoord = MapToWorld(i, j);
                                //Draw the texture
                                Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
                            }
                        }
                    }
                }
            }
        }
    }
    UpdateAnimatedTiles(dt);

    if (mapLoaded) {
        for (const auto& mapLayer : mapData.layers) {
            if (mapLayer->properties.GetProperty("Draw") &&
                mapLayer->properties.GetProperty("Draw")->value) {
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {
                        int gid = mapLayer->Get(i, j);
                        if (gid != 0) {
                            TileSet* tileSet = GetTilesetFromTileId(gid);
                            if (tileSet != nullptr) {
                                if (tileSet->animatedTiles.count(gid)) {
                                    gid = tileSet->animatedTiles[gid].gid;
                                }
                                SDL_Rect tileRect = tileSet->GetRect(gid);
                                Vector2D mapCoord = MapToWorld(i, j);
                                Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
                            }
                        }
                    }
                }
            }
        }
    }
    return ret;
}

// L09: TODO 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
	TileSet* set = nullptr;

    for (const auto& tileset : mapData.tilesets) {
    	if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
			set = tileset;
			break;
		}
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L06: TODO 2: Make sure you clean up any memory allocated from tilesets/map
    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
    }
    mapData.tilesets.clear();

    // L07 TODO 2: clean up all layer data
    for (const auto& layer : mapData.layers)
    {
        delete layer;
    }
    mapData.layers.clear();

    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
    bool ret = false;

    BG = Engine::GetInstance().textures.get()->Load("Assets/Maps/BG.png");

    BG1 = Engine::GetInstance().textures.get()->Load("Assets/Maps/1.png");
    BG2 = Engine::GetInstance().textures.get()->Load("Assets/Maps/2.png");
    BG3 = Engine::GetInstance().textures.get()->Load("Assets/Maps/3.png");
    BG4 = Engine::GetInstance().textures.get()->Load("Assets/Maps/4.png");



    // Assigns the name of the map file and the path
    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapFileName;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		ret = false;
    }
    else {

        // L06: TODO 3: Implement LoadMap to load the map properties
        // retrieve the paremeters of the <map> node and store the into the mapData struct
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L10: TODO 2: Define a property to store the MapType and Load it from the map
        std::string orientationStr = mapFileXML.child("map").attribute("orientation").as_string();
        if (orientationStr == "orthogonal") {
            mapData.orientation = MapOrientation::ORTOGRAPHIC;
        }
        else if (orientationStr == "isometric") {
            mapData.orientation = MapOrientation::ISOMETRIC;
        }
        else {
            LOG("Map orientation not found");
            ret = false;
        }

        // L06: TODO 4: Implement the LoadTileSet function to load the tileset properties
       
        //Iterate the Tileset
        for(pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode!=NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
		{
            //Load Tileset attributes
			TileSet* tileSet = new TileSet();
            tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
            tileSet->name = tilesetNode.attribute("name").as_string();
            tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileSet->spacing = tilesetNode.attribute("spacing").as_int();
            tileSet->margin = tilesetNode.attribute("margin").as_int();
            tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
            tileSet->columns = tilesetNode.attribute("columns").as_int();
            for (pugi::xml_node tileNode = tilesetNode.child("tile"); tileNode; tileNode = tileNode.next_sibling("tile")) {
                int localId = tileNode.attribute("id").as_int();
                int gid = tileSet->firstGid + localId;

                pugi::xml_node animationNode = tileNode.child("animation");
                if (animationNode) {
                    AnimatedTile animTile;
                    animTile.gid = gid;
                    animTile.totalDuration = 0;

                    for (pugi::xml_node frameNode = animationNode.child("frame"); frameNode; frameNode = frameNode.next_sibling("frame")) {
                        AnimationFrame frame;
                        frame.tileId = frameNode.attribute("tileid").as_int() + tileSet->firstGid;
                        frame.duration = frameNode.attribute("duration").as_int();
                        animTile.frames.push_back(frame);
                        animTile.totalDuration += frame.duration;
                    }

                    tileSet->animatedTiles[gid] = animTile;
                }
            }
			//Load the tileset image
			std::string imgName = tilesetNode.child("image").attribute("source").as_string();
            tileSet->texture = Engine::GetInstance().textures->Load((mapPath+imgName).c_str());

			mapData.tilesets.push_back(tileSet);
		}

        // L07: TODO 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            // L07: TODO 4: Implement the load of a single layer 
            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L09: TODO 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Iterate over all the tiles and assign the values in the data array
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
            }

            //add the layer to the map
            mapData.layers.push_back(mapLayer);
        }

        // L08 TODO 3: Create colliders
        // L08 TODO 7: Assign collider type
        // Later you can create a function here to load and create the colliders from the map

        //Iterate the layer and create colliders
        for (const auto& mapLayer : mapData.layers) {
            if (mapLayer->name == "Collisions") {
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {
                        int gid = mapLayer->Get(i, j);
                        if (gid == 49) {
                            Vector2D mapCoord = MapToWorld(i, j);
                            PhysBody* c1 = Engine::GetInstance().physics.get()->CreateRectangle(mapCoord.getX()+ mapData.tileWidth/2, mapCoord.getY()+ mapData.tileHeight/2, mapData.tileWidth, mapData.tileHeight, STATIC);
                            c1->ctype = ColliderType::PLATFORM;
                        }
                    }
                }
            }
        }
        

        for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup"); objectGroupNode != NULL; objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {

            std::string objectLayerName = objectGroupNode.attribute("name").as_string();

            ColliderType colliderType = ColliderType::PLATFORM;  

            for (pugi::xml_node propertyNode = objectGroupNode.child("properties").child("property"); propertyNode != NULL; propertyNode = propertyNode.next_sibling("property")) {
                std::string propertyName = propertyNode.attribute("name").as_string();
                std::string propertyValue = propertyNode.attribute("value").as_string();

                if (propertyName == "type" && propertyValue == "triangle") {
                    colliderType = ColliderType::UNKNOWN;  
                    break; 
                }
            }

            for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {
                Vector2D position;
                position.setX(objectNode.attribute("x").as_float());
                position.setY(objectNode.attribute("y").as_float());
                float width = objectNode.attribute("width").as_float();
                float height = objectNode.attribute("height").as_float();

                if (colliderType == ColliderType::UNKNOWN) {
                    std::string polylinePoints = objectNode.child("polyline").attribute("points").as_string();

                    float objectX = position.getX(); 
                    float objectY = position.getY(); 

                    size_t start = 0;
                    size_t end = polylinePoints.find(" ");
                    std::vector<b2Vec2> vertices;

                    while (end != std::string::npos) {
                        std::string point = polylinePoints.substr(start, end - start);
                        size_t commaPos = point.find(",");
                        float x = std::stof(point.substr(0, commaPos));  
                        float y = std::stof(point.substr(commaPos + 1)); 

                        vertices.push_back(b2Vec2(objectX + x, objectY + y));

                        start = end + 1;
                        end = polylinePoints.find(" ", start);
                    }

                    if (vertices.size() == 3) {
                        b2Vec2 v1 = vertices[0]; 
                        b2Vec2 v2 = vertices[1]; 
                        b2Vec2 v3 = vertices[2]; 

                        PhysBody* triangle = Engine::GetInstance().physics->CreateTriangle(
                            0,
                            0,
                            v1, v2, v3, bodyType::STATIC
                        );

                        triangle->ctype = ColliderType::UNKNOWN;  
                    }
                }
                else {
                    PhysBody* platform = Engine::GetInstance().physics->CreateRectangle(
                        position.getX() + width / 2,
                        position.getY() + height / 2,
                        width, height,
                        bodyType::STATIC
                    );

                    platform->ctype = colliderType;
                }
            }
        }
        ret = true;
        
        // L06: TODO 5: LOG all the data loaded iterate all tilesetsand LOG everything
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", fileName.c_str());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

            LOG("Tilesets----");

            //iterate the tilesets
            for (const auto& tileset : mapData.tilesets) {
                LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstGid);
                LOG("tile width : %d tile height : %d", tileset->tileWidth, tileset->tileHeight);
                LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
            }
            			
            LOG("Layers----");

            for (const auto& layer : mapData.layers) {
                LOG("id : %d name : %s", layer->id, layer->name.c_str());
				LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
            }   
        }
        else {
            LOG("Error while parsing map file: %s", mapPathName.c_str());
        }

        if (mapFileXML) mapFileXML.reset();

    }

    mapLoaded = ret;
    return ret;
}
void Map::UpdateAnimatedTiles(float dt) {
    static int elapsedTime = 0;
    elapsedTime += static_cast<int>(dt) * 10;

    for (auto& tileset : mapData.tilesets) {
        for (auto& animatedTile : tileset->animatedTiles) {
            int cycleTime = elapsedTime % animatedTile.second.totalDuration;
            int timeAccumulator = 0;

            for (const auto& frame : animatedTile.second.frames) {
                timeAccumulator += frame.duration;
                if (cycleTime < timeAccumulator) {
                    animatedTile.second.gid = frame.tileId;
                    break;
                }
            }
        }
    }
}
// L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
Vector2D Map::MapToWorld(int x, int y) const
{
    Vector2D ret;

    // L09: TODO 3: Get the screen coordinates of tile positions for isometric maps 
    if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
        ret.setX(x * mapData.tileWidth);
        ret.setY(y * mapData.tileHeight);
    }
	else if (mapData.orientation == MapOrientation::ISOMETRIC) {
        ret.setX(x * mapData.tileWidth / 2 - y * mapData.tileWidth / 2);
        ret.setY(x * mapData.tileHeight / 2 + y * mapData.tileHeight / 2);
    }

    return ret;
}
// L10: TODO 5: Add method WorldToMap to obtain  map coordinates from screen coordinates 
Vector2D Map::WorldToMap(int x, int y) {

    Vector2D ret(0, 0);

    if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
        ret.setX(x / mapData.tileWidth);
        ret.setY(y / mapData.tileHeight);
    }

    if (mapData.orientation == MapOrientation::ISOMETRIC) {
        float half_width = mapData.tileWidth / 2;
        float half_height = mapData.tileHeight / 2;
        ret.setX(int((x / half_width + y / half_height) / 2));
        ret.setY(int((y / half_height - (x / half_width)) / 2));
    }

    return ret;
}

// L09: TODO 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.push_back(p);
    }

    return ret;
}

MapLayer* Map::GetNavigationLayer() {
    for (const auto& layer : mapData.layers) {
		if (layer->properties.GetProperty("Navigation") != NULL && 
            layer->properties.GetProperty("Navigation")->value) {
			return layer;
		}
	}

	return nullptr;
}

// L09: TODO 7: Implement a method to get the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    for (const auto& property : propertyList) {
        if (property->name == name) {
			return property;
		}
    }

    return nullptr;
}



