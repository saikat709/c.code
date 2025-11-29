#include "ProjectSelectScreen.hpp"
#include <iostream>

using namespace std;
using namespace sf;

ProjectSelectScreen::ProjectSelectScreen(Font& font, ParticleSystem& particles, Vector2u windowSize)
    : font(font), particles(particles),
      newProjectNameField(font, "Project Name", {200, 120}, {300, 40}),
      createProjectBtn(font, "Create New", {520, 120}, {150, 40}),
      openFriendsBtn(font, "Open Friends Project", {windowSize.x / 2.0f - 125, windowSize.y - 60.0f}, {250, 30}),
      statusMsg(font, "", 16),
      title(font, "Select Project to Open", 32),
      yourProjectsLabel(font, "Your Projects", 24),
      friendProjectIdField(font, "Project ID", {windowSize.x / 2.0f - 125, windowSize.y / 2.0f - 40}, {250, 40}),
      friendAccessKeyField(font, "Access Key", {windowSize.x / 2.0f - 125, windowSize.y / 2.0f + 20}, {250, 40}),
      friendEnterBtn(font, "Enter", {windowSize.x / 2.0f - 125, windowSize.y / 2.0f + 80}, {250, 40}),
      closePopupBtn(font, "Close", {windowSize.x / 2.0f - 125, windowSize.y / 2.0f + 130}, {250, 30}),
      popupTitle(font, "Join Friend's Project", 24)
{
    // Title Setup
    title.setFillColor(Color::White);
    FloatRect titleBounds = title.getLocalBounds();
    // SFML 3: FloatRect has position and size
    title.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    title.setPosition({(float)windowSize.x / 2, 50});

    // Your Projects Label
    yourProjectsLabel.setFillColor(Color(200, 200, 200));
    yourProjectsLabel.setPosition({100, 180});

    // Dummy Projects
    // SFML 3 FloatRect constructor: {{x, y}, {w, h}} or (position, size)
    projects.push_back({"My First Game", 12, {{100, 220}, {600, 50}}});
    projects.push_back({"Chat App", 5, {{100, 280}, {600, 50}}});
    projects.push_back({"Todo List", 3, {{100, 340}, {600, 50}}});

    // Popup Setup
    popupOverlay.setSize({(float)windowSize.x, (float)windowSize.y});
    popupOverlay.setFillColor(Color(0, 0, 0, 150));

    popupCard.setSize({400, 350});
    popupCard.setOrigin({200, 175});
    popupCard.setPosition({(float)windowSize.x / 2, (float)windowSize.y / 2});
    popupCard.setFillColor(Color(30, 41, 59));
    popupCard.setOutlineThickness(1);
    popupCard.setOutlineColor(Color(255, 255, 255, 50));

    FloatRect ptBounds = popupTitle.getLocalBounds();
    popupTitle.setOrigin({ptBounds.size.x / 2.0f, ptBounds.size.y / 2.0f});
    popupTitle.setPosition({(float)windowSize.x / 2, (float)windowSize.y / 2 - 120});
    popupTitle.setFillColor(Color::White);

    statusMsg.setFillColor(Color::Red);
    statusMsg.setPosition({(float)windowSize.x / 2 - 100, (float)windowSize.y - 100});
}

AppState ProjectSelectScreen::run(RenderWindow& window) {
    while (window.isOpen()) {
        Vector2i mousePos = Mouse::getPosition(window);

        while (const auto event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                return AppState::EXIT;

            if (showFriendPopup) {
                friendProjectIdField.handleEvent(*event, window);
                friendAccessKeyField.handleEvent(*event, window);
                
                if (friendEnterBtn.isClicked(*event, window)) {
                    // Handle joining friend's project
                    cout << "Joining project: " << friendProjectIdField.getString() << endl;
                    showFriendPopup = false; // Close for now
                }
                if (closePopupBtn.isClicked(*event, window)) {
                    showFriendPopup = false;
                }
            } else {
                newProjectNameField.handleEvent(*event, window);

                if (createProjectBtn.isClicked(*event, window)) {
                    string name = newProjectNameField.getString();
                    if (!name.empty()) {
                        // Create new project logic
                        float yPos = 220 + projects.size() * 60;
                        projects.push_back({name, 0, {{100, yPos}, {600, 50}}});
                        statusMsg.setString("Project Created!");
                        statusMsg.setFillColor(Color::Green);
                    }
                }

                if (openFriendsBtn.isClicked(*event, window)) {
                    showFriendPopup = true;
                }

                // Handle Project List Clicks
                if (const auto* mouseBtn = event->getIf<Event::MouseButtonPressed>()) {
                    if (mouseBtn->button == Mouse::Button::Left) {
                        for (const auto& proj : projects) {
                            // SFML 3 contains takes Vector2f
                            if (proj.bounds.contains({(float)mousePos.x, (float)mousePos.y})) {
                                cout << "Opening project: " << proj.name << endl;
                                return AppState::CODE_EDITOR;
                            }
                        }
                    }
                }
            }
        }

        // Update
        if (showFriendPopup) {
            friendEnterBtn.update(window);
            closePopupBtn.update(window);
        } else {
            createProjectBtn.update(window);
            openFriendsBtn.update(window);
            
            // Update Hover State for Projects
            for (auto& proj : projects) {
                proj.isHovered = proj.bounds.contains({(float)mousePos.x, (float)mousePos.y});
            }
        }
        
        particles.update();

        window.clear();
        
        // Draw Gradient
        VertexArray gradient(PrimitiveType::TriangleStrip, 4);
        gradient[0].position = {0, 0}; gradient[0].color = Color(15, 23, 42);
        gradient[1].position = {0, 600}; gradient[1].color = Color(30, 41, 59);
        gradient[2].position = {800, 0}; gradient[2].color = Color(15, 23, 42);
        gradient[3].position = {800, 600}; gradient[3].color = Color(51, 65, 85);
        window.draw(gradient);

        // Draw particles
        particles.draw(window);
        
        window.draw(title);
        
        // Draw Create New Section
        newProjectNameField.draw(window);
        createProjectBtn.draw(window);

        // Draw Project List
        window.draw(yourProjectsLabel);
        for (const auto& proj : projects) {
            RectangleShape itemShape({proj.bounds.size.x, proj.bounds.size.y});
            itemShape.setPosition({proj.bounds.position.x, proj.bounds.position.y});
            itemShape.setFillColor(proj.isHovered ? Color(255, 255, 255, 20) : Color(255, 255, 255, 10));
            itemShape.setOutlineThickness(1);
            itemShape.setOutlineColor(Color(255, 255, 255, 30));
            window.draw(itemShape);

            Text projName(font, proj.name, 20);
            projName.setPosition({proj.bounds.position.x + 20, proj.bounds.position.y + 12});
            projName.setFillColor(Color::White);
            window.draw(projName);

            Text fileCount(font, to_string(proj.fileCount) + " files", 14);
            fileCount.setPosition({proj.bounds.position.x + proj.bounds.size.x - 80, proj.bounds.position.y + 16});
            fileCount.setFillColor(Color(200, 200, 200));
            window.draw(fileCount);
        }

        openFriendsBtn.draw(window);
        window.draw(statusMsg);

        // Draw Popup
        if (showFriendPopup) {
            window.draw(popupOverlay);
            window.draw(popupCard);
            window.draw(popupTitle);
            friendProjectIdField.draw(window);
            friendAccessKeyField.draw(window);
            friendEnterBtn.draw(window);
            closePopupBtn.draw(window);
        }

        window.display();
    }
    return AppState::EXIT;
}