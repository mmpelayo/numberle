// NUMBERLE WITH MAIN MENU 
// WITH PLAY AGAIN 
// WITH BACK TO MAIN MENU BUTTON


#include <SFML/Graphics.hpp>

#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>   
#include <cstdlib>
#include <ctime> 
#include <cmath>
#include <fstream>
#include <string>
#include <list>

using namespace std;
// Constants for window dimensions and box properties
const int WindowWidth = 1920;
const int maxAttempts = 5;
const int WindowHeight = 1080;
const float BoxSize = 80.f;
const float BoxSpacing = 20.f;
const sf::Vector2f BoxesPosition(700.f, 130.f);
const sf::Vector2f KeysPosition(650.f, 750.f);

// Function to check if the guessed number matches the target number
bool checkNumber(int target, int guess) {
    return target == guess;
}

// GAME STATE // GAME PAGES
enum class GameState {
    splash,
    Menu,
    Game,
    Mechanics,
    SureNo,
    WinLetter,
    LoseLetter,
    Leaderboard,
    EnterName
};

class lplayer {
public:
    std::string name;
    float victoryTime;
    // Constructor
    lplayer();
    lplayer(std::string, float);
};
lplayer::lplayer() {
    name = "";
    victoryTime = 0.0;
}
lplayer::lplayer(std::string n, float t) {
    name += n;
    victoryTime = t;
}

void leaderboardToFile(const std::string& filename, list<lplayer> l) {
    std::ofstream outputFile(filename);

    if (outputFile.is_open()) {
        for (auto it = l.begin(); it != l.end(); ++it) {
            lplayer tmp = *it;
            outputFile << tmp.victoryTime << std::endl << tmp.name << std::endl;
        }
        outputFile.close(); // Close the file
        
    }
    else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

list<lplayer> leaderboardFromFile(const std::string& filename) {
    list<lplayer> l;
    std::ifstream InputFile(filename);
    if (InputFile.is_open()) {
        string line, name;
        float time;

        lplayer tmp;
        int i = 0;
        while (std::getline(InputFile, line)) {
            if (i % 2 == 0)
                time = std::stof(line);
            else {
                name = line;
                tmp = lplayer(name, time);
                l.push_back(tmp);
            }
            i++;
        }
        InputFile.close();
        return l;
    }
    else
        std::cerr << "Unable to open file: " << filename << std::endl;

    return l;
}

void toLeaderboard(list<lplayer>& l, lplayer p) {
    for (auto player = l.begin(); player != l.end(); ++player) {
        if (p.victoryTime <= (*player).victoryTime) {
            l.insert(player, p);
            return;
        }
    }
    l.push_back(p);
}

// TEXTURE FUNCTION 
sf::Texture loadTexture(const std::string& filePath) {
    sf::Texture texture;
    if (!texture.loadFromFile(filePath)) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        // Return an empty texture or an invalid texture state
        return sf::Texture();
    }
    return texture;
}

bool gameOver = false; // Flag to control input scanning loop
bool Win = false; //Flag to see if it wins or not
//bool Lose = false;



//bool comparePlayers(const player& a, const player& b) {
//    return a.victoryTime < b.victoryTime;
//}

// MAIN FUNCTION
int main() {

    // WINDOW 
    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "Numberle");

    //NAME OF THE PLAYERS
    //std::string name;

    // FONT INITIALIZATION
    sf::Font font;
    if (!font.loadFromFile("media/font/Pixeboy.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return 1;
    }

    // TOP 3 INITIALIZATIONS


    // TIME CLOCK
    sf::Clock clock;
    sf::Text timerText("", font, 48); // Text object to display the timer
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(WindowWidth - 250.f, 20.f);

    //WINNING TIME
    sf::Time winningTime;

    // TEXTURE AND SPRITES
    // loading of ...

    // BACKGROUNDS and BUTTONS
    //main menu
    sf::Texture mainMenuBGTexture = loadTexture("media/backgrounds/menuBackground.png");
    sf::Sprite mainMenuBGSprite(mainMenuBGTexture);
    mainMenuBGSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );

    // BUTTONS INSIDE:
    // "play" button 
    sf::Texture playButtonTexture = loadTexture("media/buttons/playButton.png");
    sf::Sprite playButtonSprite(playButtonTexture);
    playButtonSprite.setPosition((window.getSize().x - playButtonTexture.getSize().x) / 2.f, 500.f);

    // "leaderboard" button 
    sf::Texture leaderboardButtonTexture = loadTexture("media/buttons/leaderBoardButton.png");
    sf::Sprite leaderboardButtonSprite(leaderboardButtonTexture);
    leaderboardButtonSprite.setPosition((window.getSize().x - leaderboardButtonTexture.getSize().x) / 2.f, 640.f);

    // "How to play" button
    sf::Texture howToPlayButtonTexture = loadTexture("media/buttons/howToPlayButton.png");
    sf::Sprite howToPlayButtonSprite(howToPlayButtonTexture);
    howToPlayButtonSprite.setPosition((window.getSize().x - howToPlayButtonTexture.getSize().x) / 2.f, 760.f);

    // "exit" button
    sf::Texture exitMenuButtonTexture = loadTexture("media/buttons/exitButton.png");
    sf::Sprite exitMenuButtonSprite(exitMenuButtonTexture);
    exitMenuButtonSprite.setPosition((window.getSize().x - exitMenuButtonTexture.getSize().x) / 2.f, 890.f);

    //game
    sf::Texture inGameBackgroundTexture = loadTexture("media/backgrounds/gameBackground.png");
    sf::Sprite inGameBackgroundSprite(inGameBackgroundTexture);
    inGameBackgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );
    //win
    sf::Texture WinLetterTexture = loadTexture("media/backgrounds/winBackground.png");
    sf::Sprite WinLetterSprite(WinLetterTexture);
    WinLetterSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );

    //try again (lose)
    sf::Texture LoseLetterTexture = loadTexture("media/backgrounds/tryAgainBackground.png");
    sf::Sprite LoseLetterSprite(LoseLetterTexture);
    LoseLetterSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );


    //BUTTONS:
    // playagain 
    sf::Texture PlayAgainTexture = loadTexture("media/buttons/playAgainButton.png");
    sf::Sprite PlayAgainSprite(PlayAgainTexture);
    PlayAgainSprite.setPosition((window.getSize().x - PlayAgainTexture.getSize().x) / 2.f, 675.f);

    // menu
    sf::Texture menuTexture = loadTexture("media/buttons/menuButton.png");
    sf::Sprite menuSprite(menuTexture);
    menuSprite.setPosition((window.getSize().x - menuTexture.getSize().x) / 2.f, 800.f);

    //sureNo
    sf::Texture SureNoBackgroundTexture = loadTexture("media/backgrounds/sureBackground.png");
    sf::Sprite SureNoBackgroundSprite(SureNoBackgroundTexture);
    SureNoBackgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );
    // BUTTONS
    // Load SureNo YES button
    sf::Texture YESSureNoBackgroundTexture = loadTexture("media/buttons/yesButton.png");
    sf::Sprite YESSureNoBackgroundSprite(YESSureNoBackgroundTexture);
    YESSureNoBackgroundSprite.setPosition((window.getSize().x - YESSureNoBackgroundTexture.getSize().x) / 2.f, 550.f);

    // Load SureNo NO button
    sf::Texture NOSureNoBackgroundTexture = loadTexture("media/buttons/noButton.png");
    sf::Sprite NOSureNoBackgroundSprite(NOSureNoBackgroundTexture);
    NOSureNoBackgroundSprite.setPosition((window.getSize().x - NOSureNoBackgroundTexture.getSize().x) / 2.f, 675.f);

    //Leaderboard
    sf::Texture leaderboardBackgroundTexture = loadTexture("media/backgrounds/leaderboardBackground.png");
    sf::Sprite leaderboardBackgroundSprite(leaderboardBackgroundTexture);
    leaderboardBackgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );

    //Enter Name
    sf::Texture enterNameBackgroundTexture = loadTexture("media/backgrounds/nameInputBackground.png");
    sf::Sprite enterNameBackgroundSprite(enterNameBackgroundTexture);
    enterNameBackgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );

    // how to play
    sf::Texture howToPlayBackgroundTexture = loadTexture("media/backgrounds/howToPlayBackground.png");
    sf::Sprite howToPlayBackgroundSprite(howToPlayBackgroundTexture);
    howToPlayBackgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );
    // BUTTONS:
    // "exit icon" button
    sf::Texture exitIconButtonTexture = loadTexture("media/buttons/exitIconButton.png");
    sf::Sprite exitIconButtonSprite(exitIconButtonTexture);
    exitIconButtonSprite.setPosition(((window.getSize().x) / 4) + 55.f, 95.f);
    //"exit icon" button for in game
    sf::Texture exitIconInGameButtonTexture = loadTexture("media/buttons/exitIconButton.png");
    sf::Sprite exitIconInGameButtonSprite(exitIconInGameButtonTexture);
    exitIconInGameButtonSprite.setPosition(((window.getSize().x) / 4) + 15.f, 50.f);


    // ----------------------------------------------------------------------------------------------


    srand(static_cast<unsigned>(time(0)));
    int attempts = maxAttempts;
    int targetNumber = 0;

    do {
        targetNumber = rand() % 10;
    } while (targetNumber == 0);
    for (int i = 1; i < 5; ++i) {
        targetNumber = targetNumber * 10 + rand() % 10;
    }

    std::vector<std::vector<sf::RectangleShape>> guessBoxes;
    std::vector<std::vector<int>> guessedNumbers;
    for (int i = 0; i < maxAttempts; i++) {
        std::vector<sf::RectangleShape> line;
        std::vector<int> digits(5, -1);
        for (int j = 0; j < 5; j++) {
            sf::RectangleShape box(sf::Vector2f(BoxSize, BoxSize));
            box.setFillColor(sf::Color::White);
            box.setOutlineColor(sf::Color::Black);
            box.setOutlineThickness(2.f);
            box.setPosition(BoxesPosition.x + j * (BoxSize + BoxSpacing),
                BoxesPosition.y + i * (BoxSize + BoxSpacing));
            line.push_back(box);
        }
        guessBoxes.push_back(line);
        guessedNumbers.push_back(digits);
    }

    std::vector<sf::RectangleShape> keys;
    std::vector<std::string> keyLabels = { "1", "2", "3", "4", "5", "Delete",
                                           "6", "7", "8", "9", "0", "Enter" };
    for (size_t i = 0; i < keyLabels.size(); ++i) {
        sf::RectangleShape key(sf::Vector2f(BoxSize, BoxSize));
        key.setFillColor(sf::Color(236, 240, 243));
        key.setOutlineColor(sf::Color::Black);
        key.setOutlineThickness(2.f);
        key.setPosition(KeysPosition.x + (i % 6) * (BoxSize + BoxSpacing), KeysPosition.y + (i / 6) * (BoxSize + BoxSpacing));
        keys.push_back(key);
    }

    std::vector<int> guessDigits(5, -1);
    int currentDigit = 0;

    bool gameOver = false; // Flag to control input scanning loop
    int guessNumber = 0;
    std::string name;

    // PRINT THE TARGET NUMBER
    sf::Text TARGET;
    TARGET.setFont(font);
    TARGET.setCharacterSize(60);
    TARGET.setFillColor(sf::Color::Red);
    TARGET.setString("The target number was : " + std::to_string(targetNumber));
    TARGET.setPosition((window.getSize().x - TARGET.getLocalBounds().width) / 2.f, 500.f);

    
    // clicking play, mechanics, back, play again, and main menu button

        // Splash screen
    sf::Texture splashTexture;
    if (!splashTexture.loadFromFile("media/backgrounds/splashBackground.png"))
    {
        std::cout << "Failed to load splash logo texture!" << std::endl;
        return 1;
    }
    sf::Sprite splashSprite;
    splashSprite.setTexture(splashTexture);
    splashSprite.setScale(
        static_cast<float>(window.getSize().x) / mainMenuBGSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / mainMenuBGSprite.getLocalBounds().height
    );
    sf::Clock splashClock;
    sf::Time splashTimer;
    splashTimer = sf::seconds(3); // 3 seconds

    GameState gameState = GameState::splash;

    std::vector<std::string> players_Names;
    std::vector<float> players_Times;
    sf::Text playerName("", font, 150);
    playerName.setFillColor(sf::Color::Black);
    playerName.setPosition(700, 450);

    list<lplayer> leaderboard = leaderboardFromFile("leaderboard.txt");
    float timeinSeconds = 0.0;
    sf::Text ScoreText, ScoreTime;
    /*
    sf::Music music;
    if (!music.openFromFile("resource/menu_music.OGG")) {
        // Error: Failed to load music
    }
    else {
        music.setVolume(100.0);
        music.play();
    }
    */ 

    //NOTES TO SIR:
    // sir we really tried putting music pero wala talaga ayaw ni file </333

    // Inside the main event loop
    while (window.isOpen()) {
        sf::Event event;

       
        while (window.pollEvent(event)) {
            //button1.play();

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            else if (event.type == sf::Event::MouseButtonPressed) {

                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePositionF(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

                    // BACK BUTTON 
                    if (exitIconButtonSprite.getGlobalBounds().contains(mousePositionF)) {
                        // Start the game
                        if (gameState == GameState::Mechanics || gameState == GameState::Leaderboard)
                            gameState = GameState::Menu;
                        else if (gameState == GameState::Game)
                            gameState = GameState::SureNo;

                    }

                    // SURE or NO BUTTONS
                    if (gameState == GameState::SureNo) {
                        if (YESSureNoBackgroundSprite.getGlobalBounds().contains(mousePositionF)) {
                            // Go back to the main menu
                            gameState = GameState::Menu;
                            gameOver = true;
                            Win = false;
                            attempts = maxAttempts;

                            // Reset the guessed numbers and guess digits
                            guessedNumbers.clear();
                            guessedNumbers.resize(maxAttempts, std::vector<int>(5, -1));

                            guessDigits.assign(5, -1);
                            currentDigit = 0;

                            // Reset the colors of guess boxes
                            for (int i = 0; i < maxAttempts; ++i) {
                                for (int j = 0; j < 5; ++j) {
                                    guessBoxes[i][j].setFillColor(sf::Color::White);
                                    guessBoxes[i][j].setOutlineColor(sf::Color::Black);
                                }
                            }
                            targetNumber = 0;
                            do {
                                targetNumber = rand() % 10;
                            } while (targetNumber == 0);
                            for (int i = 1; i < 5; ++i) {
                                targetNumber = targetNumber * 10 + rand() % 10;
                            }
                        }
                        else if (NOSureNoBackgroundSprite.getGlobalBounds().contains(mousePositionF)) {
                            // Go back to the game
                            gameState = GameState::Game;
                        }
                    }
                    // PLAY AGAIN & MAIN MENU BUTTTON
                    else
                        if ((gameState == GameState::WinLetter) || (gameState == GameState::LoseLetter)) {
                            if (PlayAgainSprite.getGlobalBounds().contains(mousePositionF)) {

                                // PLAY AGAIN FUNCTION... RESET GAME DATA...SAME AS PLAY BUTTON 
                                gameState = GameState::Game;

                                gameOver = false;
                                Win = false;
                                attempts = maxAttempts;

                                // Reset the guessed numbers and guess digits
                                guessedNumbers.clear();
                                guessedNumbers.resize(maxAttempts, std::vector<int>(5, -1));

                                guessDigits.assign(5, -1);
                                currentDigit = 0;

                                // Reset the colors of guess boxes
                                for (int i = 0; i < maxAttempts; ++i) {
                                    for (int j = 0; j < 5; ++j) {
                                        guessBoxes[i][j].setFillColor(sf::Color::White);
                                        guessBoxes[i][j].setOutlineColor(sf::Color::Black);
                                    }
                                }
                                targetNumber = 0;
                                do {
                                    targetNumber = rand() % 10;
                                } while (targetNumber == 0);
                                for (int i = 1; i < 5; ++i) {
                                    targetNumber = targetNumber * 10 + rand() % 10;
                                }
                                clock.restart();
                            }
                            else if (menuSprite.getGlobalBounds().contains(mousePositionF)) {
                                gameState = GameState::Menu;
                                gameOver = false;
                                Win = false;
                                attempts = maxAttempts;

                                // Reset the guessed numbers and guess digits
                                guessedNumbers.clear();
                                guessedNumbers.resize(maxAttempts, std::vector<int>(5, -1));

                                guessDigits.assign(5, -1);
                                currentDigit = 0;

                                // Reset the colors of guess boxes
                                for (int i = 0; i < maxAttempts; ++i) {
                                    for (int j = 0; j < 5; ++j) {
                                        guessBoxes[i][j].setFillColor(sf::Color::White);
                                        guessBoxes[i][j].setOutlineColor(sf::Color::Black);
                                    }
                                }
                                targetNumber = 0;
                                do {
                                    targetNumber = rand() % 10;
                                } while (targetNumber == 0);
                                for (int i = 1; i < 5; ++i) {
                                    targetNumber = targetNumber * 10 + rand() % 10;
                                }
                            }
                        }
                    // MAIN MENU BUTTONS
                        else
                            if (gameState == GameState::Menu) {
                                // Check if the start button is clicked
                                if (exitMenuButtonSprite.getGlobalBounds().contains(mousePositionF)) {
                                    // close window
                                    window.close();
                                }
                                if (playButtonSprite.getGlobalBounds().contains(mousePositionF)) {
                                    // Start the game
                                    gameState = GameState::Game;
                                    gameOver = false;
                                    Win = false;
                                    attempts = maxAttempts;

                                    // Reset the guessed numbers and guess digits
                                    guessedNumbers.clear();
                                    guessedNumbers.resize(maxAttempts, std::vector<int>(5, -1));

                                    guessDigits.assign(5, -1);
                                    currentDigit = 0;

                                    // Reset the colors of guess boxes
                                    for (int i = 0; i < maxAttempts; ++i) {
                                        for (int j = 0; j < 5; ++j) {
                                            guessBoxes[i][j].setFillColor(sf::Color::White);
                                            guessBoxes[i][j].setOutlineColor(sf::Color::Black);
                                        }
                                    }
                                    targetNumber = 0;
                                    do {
                                        targetNumber = rand() % 10;
                                    } while (targetNumber == 0);
                                    for (int i = 1; i < 5; ++i) {
                                        targetNumber = targetNumber * 10 + rand() % 10;
                                    }
                                    clock.restart();
                                }

                                // Check if the mechanics button is clicked
                                if ((howToPlayButtonSprite.getGlobalBounds().contains(mousePositionF)) && gameState == GameState::Menu) {
                                    // Show the mechanics overlay

                                    gameState = GameState::Mechanics;
                                }

                                if ((leaderboardButtonSprite.getGlobalBounds().contains(mousePositionF)) && gameState == GameState::Menu) {
                                    // Show the leaderboard

                                    gameState = GameState::Leaderboard;
                                }

                            }




                    // GAME BUTTONS

                            else if (gameState == GameState::Game) {



                                if (gameOver) {
                                    continue; // Skip input scanning if the game is over
                                }
                                if (exitIconButtonSprite.getGlobalBounds().contains(mousePositionF)) {
                                    // Start the game
                                    if (gameState == GameState::Mechanics)
                                        gameState = GameState::Menu;
                                    else if (gameState == GameState::Game)
                                        gameState = GameState::SureNo;

                                }
                                for (const auto& key : keys) {
                                    if (key.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
                                        std::string label = keyLabels[&key - &keys[0]];
                                        if (label == "Delete") {
                                            if (currentDigit > 0) {
                                                guessDigits[currentDigit - 1] = -1;
                                                currentDigit--;
                                            }
                                        }
                                        else if (label == "Enter") {
                                            if (currentDigit == 5) {
                                                int guessNumber = 0;
                                                for (int i = 0; i < 5; ++i) {
                                                    guessNumber = guessNumber * 10 + guessDigits[i];
                                                }
                                                guessedNumbers[maxAttempts - attempts] = guessDigits;
                                                attempts--;

                                                // Reset the guess digits
                                                guessDigits.assign(5, -1);
                                                currentDigit = 0;

                                                // Update the colors of the boxes for feedback
                                                for (int i = 0; i < 5; ++i) {
                                                    int guessedDigit = guessedNumbers[maxAttempts - attempts - 1][i];
                                                    if (guessedDigit != -1) {
                                                        bool foundDigit = false;
                                                        bool foundPosition = false;
                                                        bool duplicateFound = false;  // Added flag for duplicate digit
                                                        for (int j = 0; j < 5; ++j) {
                                                            int targetDigit = (targetNumber / static_cast<int>(pow(10, 4 - j))) % 10;
                                                            if (guessedDigit == targetDigit && i == j) {
                                                                foundPosition = true;
                                                                foundDigit = true;
                                                                break;
                                                            }

                                                            if (guessedDigit == targetDigit) {
                                                                foundDigit = true;
                                                            }
                                                        }
                                                        if (foundDigit) {
                                                            if (foundPosition) {
                                                                guessBoxes[maxAttempts - attempts - 1][i].setFillColor(sf::Color(54, 238, 224)); // Correct digit and position
                                                                guessBoxes[maxAttempts - attempts - 1][i].setOutlineColor(sf::Color(54, 238, 224));
                                                            }
                                                            else {
                                                                guessBoxes[maxAttempts - attempts - 1][i].setFillColor(sf::Color(246, 82, 160)); // Correct digit but wrong position
                                                                guessBoxes[maxAttempts - attempts - 1][i].setOutlineColor(sf::Color(246, 82, 160));
                                                            }
                                                        }
                                                        else {
                                                            guessBoxes[maxAttempts - attempts - 1][i].setFillColor(sf::Color(158, 145, 168)); // Incorrect digit
                                                            guessBoxes[maxAttempts - attempts - 1][i].setOutlineColor(sf::Color(158, 145, 168));
                                                        }
                                                    }
                                                }


                                                if (checkNumber(targetNumber, guessNumber)) {
                                                    gameOver = true; // Set the flag to stop input scanning
                                                    Win = true;
                                                    winningTime = clock.getElapsedTime(); // Store the winning time
                                                    timeinSeconds = winningTime.asSeconds();
                                                    // INSERT TIME
                                                    // tmpNode.victoryTime = timeinSeconds;
                                                }
                                                else if (attempts == 0) {
                                                    gameOver = true;

                                                }

                                            }
                                        }
                                        else {
                                            if (currentDigit < 5) {
                                                guessDigits[currentDigit] = std::stoi(label);
                                                currentDigit++;
                                            }
                                        }
                                    }
                                }
                            }
                }
            }
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::TextEntered) {
                if (gameState == GameState::EnterName) {
                    lplayer newPlayer;
                    newPlayer.victoryTime = timeinSeconds;
                    //Handle the case where key is pressed
                    if (event.key.code == sf::Keyboard::Enter) {
                        newPlayer.name = name;
                        toLeaderboard(leaderboard, newPlayer);
                        name.clear();
                        gameState = GameState::WinLetter;
                    }

                    else if (event.type == sf::Event::TextEntered && event.text.unicode < 128 && event.text.unicode != '\b')
                        if (name.size() < 10) {
                            name += event.text.unicode;
                        }
                    //name += static_cast<char>(event.text.unicode);
                    playerName.setString(name);
                }
                if (gameState == GameState::Game) {

                    if (gameOver) {
                        continue; // Skip input scanning if the game is over
                    }

                    if (event.key.code == sf::Keyboard::Backspace) {
                        // Handle pressing the Backspace key (for deleting)
                        if (currentDigit > 0) {
                            guessDigits[currentDigit - 1] = -1;
                            currentDigit--;
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        // Handle pressing the Enter key
                        if (currentDigit == 5) {
                            int guessNumber = 0;

                            // if mag input number next digit
                            for (int i = 0; i < 5; ++i) {
                                guessNumber = guessNumber * 10 + guessDigits[i];
                            }
                            //guessed number is an Array ex. 12345

                            // guessdigit  is yung 1, 2, 3, 4, 5, 
                            guessedNumbers[maxAttempts - attempts] = guessDigits;
                            attempts--;

                            // Reset the guess digits
                            guessDigits.assign(5, -1);
                            currentDigit = 0;

                            // Update the colors of the boxes for feedback

                            // CHECKS EACH DIGIT FROM RIGHT TO LEFT... NESTED LOOP
                            for (int i = 0; i < 5; ++i) {
                                int guessedDigit = guessedNumbers[maxAttempts - attempts - 1][i];
                                if (guessedDigit != -1) {
                                    bool foundDigit = false;
                                    bool foundPosition = false;  // Added flag for correct position

                                    // THE DIGIT THAT TRAVERSE TO CHECK THE iTH ELEMENT
                                    for (int j = 0; j < 5; ++j) {
                                        int targetDigit = (targetNumber / static_cast<int>(pow(10, 4 - j))) % 10;  // Reverse the order of digits
                                        if (guessedDigit == targetDigit && i == j) {
                                            foundPosition = true;
                                            foundDigit = true;
                                            break;
                                        }

                                        if (guessedDigit == targetDigit) {
                                            foundDigit = true;
                                        }
                                    }
                                    // UPDATES COLOR OF THE BOXES
                                    if (foundDigit) {
                                        if (foundPosition) {
                                            guessBoxes[maxAttempts - attempts - 1][i].setFillColor(sf::Color::Green); // Correct digit and position
                                            guessBoxes[maxAttempts - attempts - 1][i].setOutlineColor(sf::Color(54, 238, 224));
                                        }
                                        else {
                                            guessBoxes[maxAttempts - attempts - 1][i].setFillColor(sf::Color::Yellow); // Correct digit but wrong position
                                            guessBoxes[maxAttempts - attempts - 1][i].setOutlineColor(sf::Color(246, 82, 160));
                                        }
                                    }
                                    else {
                                        guessBoxes[maxAttempts - attempts - 1][i].setFillColor(sf::Color::Red); // Incorrect digit
                                        guessBoxes[maxAttempts - attempts - 1][i].setOutlineColor(sf::Color(158, 145, 168));
                                    }
                                }
                            }

                            if (checkNumber(targetNumber, guessNumber)) {
                                gameOver = true; // Set the flag to stop input scanning
                                Win = true;
                                winningTime = clock.getElapsedTime(); // Store the winning time
                                timeinSeconds = winningTime.asSeconds();
                                players_Times.push_back(timeinSeconds);
                            }
                            else if (attempts == 0) {
                                gameOver = true;

                            }
                        }
                    }
                    else if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
                        // Handle number key presses
                        int numKeyPressed = event.key.code - sf::Keyboard::Num0;
                        if (currentDigit < 5) {
                            guessDigits[currentDigit] = numKeyPressed;
                            currentDigit++;
                        }
                    }

                }
            }
        }

        // Outside the main event loop, before the window.clear() call

        if (gameState == GameState::splash) {
            window.clear();

            sf::Time elapsedTime = splashClock.getElapsedTime();

            if (elapsedTime >= splashTimer) {
                gameState = GameState::Menu;
            }
            else {
                window.draw(splashSprite);
            }

            window.display();
        }

        else if (gameState == GameState::WinLetter) {
            // Draw the main menu
            window.clear();

            // tmpNODE insertion to the leaderBoardNode


            window.draw(WinLetterSprite);
            window.draw(PlayAgainSprite);
            window.draw(menuSprite);

            // Format the winning time as MM:SS
            int winningMinutes = static_cast<int>(winningTime.asSeconds()) / 60;
            int winningSeconds = static_cast<int>(winningTime.asSeconds()) % 60;
            std::ostringstream winningTimeString;
            winningTimeString << std::setfill('0') << std::setw(2) << winningMinutes << ":" << std::setw(2) << winningSeconds;

            // Display the winning time
            sf::Text timeText("Time: " + winningTimeString.str(), font, 100);
            timeText.setFillColor(sf::Color::Red);
            timeText.setPosition((WindowWidth - timeText.getLocalBounds().width) / 2.f, 400.f);
            window.draw(timeText);


            window.display(); // Display the main menu


        }

        else if (gameState == GameState::LoseLetter) {
            // Draw the main menu
            window.clear();
            window.draw(LoseLetterSprite);
            window.draw(PlayAgainSprite);
            window.draw(menuSprite);
            TARGET.setString("The target number was : " + std::to_string(targetNumber));
            window.draw(TARGET);

            window.display(); // Display the main menu

        }
        else if (gameState == GameState::SureNo) {
            // Draw the main menu
            window.clear();
            window.draw(SureNoBackgroundSprite);
            window.draw(NOSureNoBackgroundSprite);
            window.draw(YESSureNoBackgroundSprite);
            window.display(); // Display the main menu

        }
        else if (gameState == GameState::Menu) {
            // Draw the main menu
            window.clear();
            window.draw(mainMenuBGSprite);
            // window.draw(titleSprite);
            window.draw(playButtonSprite);
            window.draw(howToPlayButtonSprite);
            window.draw(leaderboardButtonSprite);
            window.draw(exitMenuButtonSprite);
            window.display(); // Display the main menu
        }
        else if (gameState == GameState::Mechanics) {
            // Draw the mechanics overlay
            window.clear();
            window.draw(howToPlayBackgroundSprite);

            window.draw(exitIconButtonSprite);
            window.display(); // Display the mechanics overlay
        }
        else if (gameState == GameState::Leaderboard) {
            // Draw the leaderboard
            window.clear();
            window.draw(leaderboardBackgroundSprite);
            window.draw(exitIconButtonSprite);
            int i = 500;
            int count = 1;
            for (auto it = leaderboard.begin(); it != leaderboard.end() && count <= 5; ++it) {
                int minutes = static_cast<int>(std::floor((*it).victoryTime / 60));
                int remainingSeconds = static_cast<int>(std::fmod((*it).victoryTime, 60));

                std::ostringstream ss;
                ss << std::setfill('0') << std::setw(2) << minutes << ":";
                ss << std::setfill('0') << std::setw(2) << remainingSeconds;
                std::string tmpname = (*it).name;
                
                tmpname.resize(10, ' ');


                ScoreTime.setFont(font);
                ScoreTime.setCharacterSize(50);
                ScoreTime.setFillColor(sf::Color::Red);
                ScoreTime.setString(ss.str());

                ScoreTime.setPosition(1100, i);

                ScoreText.setFont(font);
                ScoreText.setCharacterSize(50);
                ScoreText.setFillColor(sf::Color::Red);
                ScoreText.setString(std::to_string(count) + "     " + (*it).name);

                ScoreText.setPosition(700, i);
                i = i + 80;
                window.draw(ScoreText);

                window.draw(ScoreTime);
                count++;
            }
            window.display();
        }
        else if (gameState == GameState::EnterName) {
            window.clear();
            window.draw(WinLetterSprite);
            window.draw(PlayAgainSprite);
            window.draw(menuSprite);
            window.draw(enterNameBackgroundSprite);
            window.draw(playerName);
            window.display();
        }
        else if (gameState == GameState::Game) {


            // Draw the game
            window.clear(sf::Color::White);
            window.draw(inGameBackgroundSprite);
            // ... game drawing code ...
            for (size_t i = 0; i < guessBoxes.size(); ++i) {
                for (size_t j = 0; j < guessBoxes[i].size(); ++j) {
                    window.draw(guessBoxes[i][j]);

                    // Check if there is a digit in the box
                    int guessedDigit = guessedNumbers[i][j];
                    if (guessedDigit != -1) {
                        // Convert the digit to a string
                        std::string digitString = std::to_string(guessedDigit);

                        // Create the digit text object
                        sf::Text digitText(digitString, font, 48);
                        digitText.setFillColor(sf::Color::Black);

                        // Calculate the position to center the digit
                        float xPos = BoxesPosition.x + j * (BoxSize + BoxSpacing) + (BoxSize - digitText.getGlobalBounds().width) / 2.f;
                        float yPos = BoxesPosition.y + i * (BoxSize + BoxSpacing) + (BoxSize - digitText.getGlobalBounds().height) / 2.f;
                        digitText.setPosition(xPos, yPos);

                        // Draw the digit text
                        window.draw(digitText);
                    }
                }
            }

            //Timer text
            sf::Time elapsed = clock.getElapsedTime();
            int seconds = static_cast<int>(elapsed.asSeconds());

            // Format the time as MM:SS
            int minutes = seconds / 60;
            seconds %= 60;
            std::ostringstream timerStream;
            timerStream << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds;

            timerText.setString("Time: " + timerStream.str());

            for (int i = 0; i < maxAttempts - attempts; ++i) {
                for (int j = 0; j < 5; ++j) {
                    if (guessedNumbers[i][j] != -1) {
                        sf::Text digitText(std::to_string(guessedNumbers[i][j]), font, 48);
                        digitText.setFillColor(sf::Color::Black);
                        digitText.setPosition(BoxesPosition.x + j * (BoxSize + BoxSpacing) + (BoxSize - digitText.getGlobalBounds().width) / 2.f,
                            BoxesPosition.y + i * (BoxSize + BoxSpacing) + (BoxSize - digitText.getGlobalBounds().height) / 2.f);
                        window.draw(digitText);
                    }
                }
            }

            for (size_t i = 0; i < currentDigit; ++i) {
                sf::Text digitText(std::to_string(guessDigits[i]), font, 48);
                digitText.setFillColor(sf::Color::Black);
                digitText.setPosition(BoxesPosition.x + i * (BoxSize + BoxSpacing) + (BoxSize - digitText.getGlobalBounds().width) / 2.f,
                    BoxesPosition.y + (maxAttempts - attempts) * (BoxSize + BoxSpacing) + (BoxSize - digitText.getGlobalBounds().height) / 2.f);
                window.draw(digitText);
            }

            for (const auto& key : keys) {
                window.draw(key);

                // Draw key label inside the key box
                std::string label = keyLabels[&key - &keys[0]];
                sf::Text labelText(label, font, 24);
                labelText.setFillColor(sf::Color::Black);
                labelText.setPosition(key.getPosition().x + (BoxSize - labelText.getGlobalBounds().width) / 2.f,
                    key.getPosition().y + (BoxSize - labelText.getGlobalBounds().height) / 2.f);
                window.draw(labelText);
            }
            //window.draw(messageText);

            //
            window.draw(timerText);
            window.draw(exitIconInGameButtonSprite);

            if (Win && gameOver) {

                gameState = GameState::EnterName;
                //gameState = GameState::WinLetter;
            }
            if (gameOver && !Win) {
                gameState = GameState::LoseLetter;
            }



            window.display(); // Display the game
        }



    }

    // MAKE ANOTHER RANDOM NUMBER TO GUESS FOR NEXT GAME
    targetNumber = 0;
    do {
        targetNumber = rand() % 10;
    } while (targetNumber == 0);
    for (int i = 1; i < 5; ++i) {
        targetNumber = targetNumber * 10 + rand() % 10;
    }

    leaderboardToFile("leaderboard.txt", leaderboard);

    return 0;
}


