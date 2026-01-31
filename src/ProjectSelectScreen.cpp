#include "ProjectSelectScreen.hpp"
#include <iostream>
#include "Session.hpp"
#include "json.hpp"

using namespace std;
using namespace sf;

ProjectSelectScreen::ProjectSelectScreen(Font& font, ParticleSystem& particles, Vector2u windowSize)
    : font(font), particles(particles),
      newProjectNameField(font, "Project Name", {200, 120}, {300, 40}),
      createProjectBtn(font, "Create New", {520, 120}, {150, 40}),
      openFriendsBtn(font, "Open Friends Project", {windowSize.x / 2.0f - 125, windowSize.y - 60.0f}, {250, 30}),
      statusMsg("", font, 16),
      title("Select Project to Open", font, 32),
      yourProjectsLabel("Your Projects", font, 24),
      friendProjectIdField(font, "Project ID", {windowSize.x / 2.0f - 125, windowSize.y / 2.0f - 40}, {250, 40}),
      friendAccessKeyField(font, "Access Key", {windowSize.x / 2.0f - 125, windowSize.y / 2.0f + 20}, {250, 40}),
      friendEnterBtn(font, "Enter", {windowSize.x / 2.0f - 120, windowSize.y / 2.0f + 60}, {240, 40}),
      closePopupBtn(font, "Close", {windowSize.x / 2.0f - 120, windowSize.y / 2.0f + 110}, {240, 30}),
      logoutBtn(font, "Logout", {10, 5}, {80, 30}),
      popupTitle("Join Friend's Project", font, 24)
{
    title.setFillColor(Color::White);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.getSize().x / 2.0f, titleBounds.getSize().y / 2.0f});
    title.setPosition({(float)windowSize.x / 2, 50});

    yourProjectsLabel.setFillColor(Color(200, 200, 200));
    yourProjectsLabel.setPosition({100, 180});

    popupOverlay.setSize({(float)windowSize.x, (float)windowSize.y});
    popupOverlay.setFillColor(Color(0, 0, 0, 150));

    popupCard.setSize({400, 350});
    popupCard.setOrigin({200, 175});
    popupCard.setPosition({(float)windowSize.x / 2, (float)windowSize.y / 2});
    popupCard.setFillColor(Color(30, 41, 59));
    popupCard.setOutlineThickness(1);
    popupCard.setOutlineColor(Color(255, 255, 255, 50));

    FloatRect ptBounds = popupTitle.getLocalBounds();
    popupTitle.setOrigin({ptBounds.getSize().x / 2.0f, ptBounds.getSize().y / 2.0f});
    popupTitle.setPosition({(float)windowSize.x / 2, (float)windowSize.y / 2 - 120});
    popupTitle.setFillColor(Color::White);

    statusMsg.setFillColor(Color::Red);
    statusMsg.setPosition({(float)windowSize.x / 2 - 100, (float)windowSize.y - 120});
}

void ProjectSelectScreen::loadProjects() {
    json request;
    request["action"] = "get_projects";
    request["user_id"] = Session::getInstance().getUserId();
    
    json response = Session::getInstance().getNetworkClient()->sendRequest(request);
    if (response["status"] == "success") {
        projects.clear();
        int i = 0;
        for (auto& projJson : response["projects"]) {
            ProjectItem item;
            item.id = projJson["id"];
            item.name = projJson["name"];
            item.fileCount = projJson.value("fileCount", 0);
            float yPos = 220 + i * 60;
            item.bounds = {{100, yPos}, {600, 50}};
            projects.push_back(item);
            i++;
        }
    }
}

AppState ProjectSelectScreen::run(RenderWindow& window) {
    bool projectsFetched = false;
    while (window.isOpen()) {
        if (!projectsFetched) {
            loadProjects();
            projectsFetched = true;
        }
        Vector2i mousePos = Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return AppState::EXIT;

            if (showFriendPopup) {
                friendProjectIdField.handleEvent(event, window);
                friendAccessKeyField.handleEvent(event, window);
                
                if (friendEnterBtn.isClicked(event, window)) {
                    string projIdStr = friendProjectIdField.getString();
                    string accessKeyStr = friendAccessKeyField.getString();
                    if (!projIdStr.empty() && !accessKeyStr.empty()) {
                        json request;
                        request["action"] = "join_project";
                        request["project_id"] = stoi(projIdStr);
                        request["access_key"] = stoi(accessKeyStr);
                        request["user_id"] = Session::getInstance().getUserId();
                        json response = Session::getInstance().getNetworkClient()->sendRequest(request);
                        if (response["status"] == "success") {
                            statusMsg.setString("Joined Project!");
                            statusMsg.setFillColor(Color::Green);
                            loadProjects();
                            showFriendPopup = false;
                        } else {
                            statusMsg.setString(response.value("message", "Join Failed"));
                            statusMsg.setFillColor(Color::Red);
                        }
                    }
                }
                if (closePopupBtn.isClicked(event, window)) {
                    showFriendPopup = false;
                }
            } else {
                newProjectNameField.handleEvent(event, window);

                if (createProjectBtn.isClicked(event, window)) {
                    string name = newProjectNameField.getString();
                    if (!name.empty()) {
                        json request;
                        request["action"] = "create_project";
                        request["name"] = name;
                        request["user_id"] = Session::getInstance().getUserId();
                        json response = Session::getInstance().getNetworkClient()->sendRequest(request);
                        if (response["status"] == "success") {
                            statusMsg.setString("Project Created!");
                            statusMsg.setFillColor(Color::Green);
                            loadProjects();
                        } else {
                            statusMsg.setString("Creation Failed");
                            statusMsg.setFillColor(Color::Red);
                        }
                    }
                }

                if (openFriendsBtn.isClicked(event, window)) {
                    showFriendPopup = true;
                }

                if (logoutBtn.isClicked(event, window)) {
                    Session::getInstance().clearSession();
                    return AppState::LOGIN;
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        for (const auto& proj : projects) {
                            if (proj.bounds.contains({(float)mousePos.x, (float)mousePos.y})) {
                                Session::getInstance().setCurrentProjectId(proj.id);
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
            logoutBtn.update(window);
            
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
            RectangleShape itemShape({proj.bounds.getSize().x, proj.bounds.getSize().y});
            itemShape.setPosition({proj.bounds.getPosition().x, proj.bounds.getPosition().y});
            itemShape.setFillColor(proj.isHovered ? Color(255, 255, 255, 20) : Color(255, 255, 255, 10));
            itemShape.setOutlineThickness(1);
            itemShape.setOutlineColor(Color(255, 255, 255, 30));
            window.draw(itemShape);

            Text projName(proj.name, font, 20);
            projName.setPosition({proj.bounds.getPosition().x + 20, proj.bounds.getPosition().y + 12});
            projName.setFillColor(Color::White);
            window.draw(projName);

            Text fileCount(to_string(proj.fileCount) + " files", font, 14);
            fileCount.setPosition({proj.bounds.getPosition().x + proj.bounds.getSize().x - 80, proj.bounds.getPosition().y + 16});
            fileCount.setFillColor(Color(200, 200, 200));
            window.draw(fileCount);
        }

        openFriendsBtn.draw(window);
        logoutBtn.draw(window);
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