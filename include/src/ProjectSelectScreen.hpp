#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.hpp"
#include "UI.hpp"
#include "ParticleSystem.hpp"

using namespace sf;
using namespace std;

struct ProjectItem {
    int id;
    string name;
    int fileCount;
    FloatRect bounds;
    bool isHovered = false;
};

class ProjectSelectScreen {
    Font& font;
    ParticleSystem& particles;
    
    Text title;
    
    InputField newProjectNameField;
    Button createProjectBtn;
    
    Text yourProjectsLabel;
    vector<ProjectItem> projects;
    
    Button openFriendsBtn;
    
    bool showFriendPopup = false;
    RectangleShape popupOverlay;
    RectangleShape popupCard;
    Text popupTitle;
    InputField friendProjectIdField;
    InputField friendAccessKeyField;
    Button friendEnterBtn;
    Button closePopupBtn;
    Button logoutBtn;
    
    Text statusMsg;
    void loadProjects();

public:
    ProjectSelectScreen(Font& font, ParticleSystem& particles, Vector2u windowSize);
    AppState run(RenderWindow& window);
};
