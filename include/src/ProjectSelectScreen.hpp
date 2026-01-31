#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.hpp"
#include "UI.hpp"
#include "ParticleSystem.hpp"

struct ProjectItem {
    int id;
    std::string name;
    int fileCount;
    sf::FloatRect bounds;
    bool isHovered = false;
};

class ProjectSelectScreen {
    sf::Font& font;
    ParticleSystem& particles;
    
    // Main UI
    sf::Text title;
    
    // Create New Section
    InputField newProjectNameField;
    Button createProjectBtn;
    
    // Project List
    sf::Text yourProjectsLabel;
    std::vector<ProjectItem> projects;
    
    // Bottom Actions
    Button openFriendsBtn;
    
    // Friend Project Popup
    bool showFriendPopup = false;
    sf::RectangleShape popupOverlay;
    sf::RectangleShape popupCard;
    sf::Text popupTitle;
    InputField friendProjectIdField;
    InputField friendAccessKeyField;
    Button friendEnterBtn;
    Button closePopupBtn;
    Button logoutBtn;
    
    sf::Text statusMsg;
    void loadProjects();

public:
    ProjectSelectScreen(sf::Font& font, ParticleSystem& particles, sf::Vector2u windowSize);
    AppState run(sf::RenderWindow& window);
};
