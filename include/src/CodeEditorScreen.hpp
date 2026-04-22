#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include "ParticleSystem.hpp"
#include "AppState.hpp"
#include "UI.hpp"
#include "json.hpp"

using namespace sf;
using namespace std;


class CodeEditorScreen {
    Font& font;
    ParticleSystem& particles;
    
    // Window dimensions
    float windowWidth;
    float windowHeight;
    
    // UI Components
    RectangleShape editorBox;
    RectangleShape outputBox;
    Text editorLabel;
    Text outputLabel;
    Text codeText;
    Text outputText;
    
    // Code content
    string code;
    string output;
    
    // Cursor and editing
    size_t cursorPos = 0;
    RectangleShape cursor;
    Clock cursorBlink;
    
    // Text selection
    size_t selectionStart = 0;
    size_t selectionEnd = 0;
    bool isSelecting = false;
    int preferredColumn = 0;  // For up/down navigation
    
    // Resizable panels state
    float sidebarWidth = 200.0f;
    float chatWidth = 160.0f;
    float outputHeight = 280.0f;
    
    bool isResizingSidebar = false;
    bool isResizingChat = false;
    bool isResizingOutput = false;
    
    RectangleShape sidebarHandle;
    RectangleShape chatHandle;
    RectangleShape outputHandle;

    // Scrolling
    int codeScrollOffset = 0;
    int outputScrollOffset = 0;
    
    // Sidebar components
    RectangleShape sidebarBox;
    vector<string> fileList;
    vector<int> fileIds;
    int selectedFileIndex = 0;
    Button newFileBtn;
    int sidebarScrollOffset = 0;
    Text sidebarTitle;
    
    // File naming input
    bool isNamingFile = false;
    string newFileName;
    Text fileNameInputText;
    RectangleShape fileNameInputBox;
    
    // Top menu bar components
    RectangleShape topMenuBar;
    Button fileMenuBtn;
    Button editMenuBtn;
    Button viewMenuBtn;
    Button runMenuBtn;
    Button toggleOutputBtn;
    Button chatBtn;  
    Button shareBtn;
    Button backBtn;
    Button logoutBtn;
    
    // Output panel toggle
    bool outputVisible = false;

    // Chat Screen
    bool chatVisible = false;
    RectangleShape chatBox;
    Text chatTitle;
    vector<Text> messageDisplay;
    InputField chatInputField;
    Button sendChatBtn;
    void updateChatDisplay();
    void loadMessages();
    void sendMessage();
    
    // Share Popup
    bool showSharePopup = false;
    RectangleShape popupOverlay;
    RectangleShape sharePopupCard;
    Text sharePopupTitle;
    Text sharePopupInfo;
    Button closeShareBtn;

    // Debounce Save
    Clock lastEditClock;
    bool needsSave = false;
    int currentFileId = -1;
    void saveFile();
    
    // Real-time updates
    Clock pollClock;
    int lastMessageId = 0;
    void pollForUpdates();
    void handleServerBroadcast(const json& broadcast);

    
    // Helper methods
    void executeCode();
    void updateCodeDisplay();
    void updateOutputDisplay();
    vector<string> splitLines(const string& text);
    void loadFiles();
    void fetchFileContent(int fileId);
    
    // Cursor and selection helpers
    size_t getCursorPosFromClick(Vector2f mousePos);
    void moveCursorUp();
    void moveCursorDown();
    void deleteSelection();
    string getSelectedText();
    bool hasSelection();
    void clearSelection();
    void drawSelection(RenderWindow& window);
    
    // New UI helpers
    void drawSidebar(RenderWindow& window);
    void drawTopMenu(RenderWindow& window);
    void handleFileSelection(Vector2f mousePos);
    void createNewFile(const string& name);
    void toggleOutputPanel();
    void updateLayout();

public:
    CodeEditorScreen(Font& font, ParticleSystem& particles, Vector2u windowSize);
    AppState run(RenderWindow& window);
};
