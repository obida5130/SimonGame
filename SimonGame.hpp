// SimonGame.hpp
#ifndef SIMONGAME_HPP
#define SIMONGAME_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <chrono>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/System/Clock.hpp>
class SimonGame {
    sf::Clock timer;
public:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundimage;
    enum class Color {  GREEN, RED, BLUE, YELLOW };
    bool checkIfTimeExceeded();
    bool gameInProgress;
     tgui::Button::Ptr exitButton;

     sf::SoundBuffer gameOverSoundBuffer;
     sf::Sound gameOverSound;
    SimonGame();
    void startNewGame();
    bool userGuess(Color color, tgui::Gui& gui, std::vector<tgui::Button::Ptr>& buttons);
    void displaySequence(const std::vector<tgui::Button::Ptr>& buttons, sf::RenderWindow& window, tgui::Gui& gui);
    static sf::SoundBuffer generateCustomSound(double frequency, double duration);
    void restartTimer();

    void buttonBlink(SimonGame::Color color, const std::vector<tgui::Button::Ptr>& buttons, sf::RenderWindow& window, tgui::Gui& gui);
    const std::vector<Color>& getSequence() const;
    void displayGameOverMessage(tgui::Gui& gui, const std::string& first, int level, const std::string& last, std::vector<tgui::Button::Ptr>& buttons);
    void countingStartGame(tgui::Gui& gui, sf::RenderWindow& window, const std::string& message, unsigned int duration);
    std::chrono::steady_clock::time_point lastUserInputTime;
private:
    void generateColor();
    std::vector<Color> sequence;
    size_t userGuessIndex;
    std::default_random_engine rng;
    std::vector<sf::SoundBuffer> soundBuffers;
    std::vector<sf::Sound> sounds;
    sf::Int16 generateSound(double time, double frequency, double amplitude);

};

#endif // SIMONGAME_HPP