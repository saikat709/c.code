#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.hpp"
#include "UI.hpp"
#include "ParticleSystem.hpp"

using namespace std;
using namespace sf;

struct ProjectItem {
    string name;
    int fileCount;
    FloatRect bounds;
    bool isHovered = false;
};

class ProjectSelectScreen {
    Font& font;
    ParticleSystem& particles;
    
    // Main UI
    Text title;
    
    // Create New Section
    InputField newProjectNameField;
    Button createProjectBtn;
    
    // Project List
    Text yourProjectsLabel;
    vector<ProjectItem> projects;
    
    // Bottom Actions
    Button openFriendsBtn;
    
    // Friend Project Popup
    bool showFriendPopup = false;
    RectangleShape popupOverlay;
    RectangleShape popupCard;
    Text popupTitle;
    InputField friendProjectIdField;
    InputField friendAccessKeyField;
    Button friendEnterBtn;
    Button closePopupBtn;
    
    Text statusMsg;

public:
    ProjectSelectScreen(Font& font, ParticleSystem& particles, Vector2u windowSize);
    AppState run(RenderWindow& window);
};
