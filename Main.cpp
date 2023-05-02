#include <TGUI/TGUI.hpp>
#include "SimonGame.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <TGUI/Widgets/MessageBox.hpp>

int main()
{

    bool showExitButton = false;

    // creating window and gui object
    sf::RenderWindow window(sf::VideoMode(800, 600), "Obida Kassif - Simon Game");
    tgui::Gui gui{window};
    // Defining the button colors for simon game
    const std::array<tgui::Color, 4> colors = {
        tgui::Color(128, 255, 128), // Light green
        tgui::Color(255, 128, 128), // Light red
        tgui::Color(255, 255, 128), // Light yellow
        tgui::Color(128, 128, 255)  // Light blue
    };
    // Creating the SimonGame object instance
    SimonGame game;
    // Creating 4 buttons for simonGame
    std::vector<tgui::Button::Ptr> buttons;
    const float buttonSize = std::min(window.getSize().x, window.getSize().y) / 3.5f;
    const float buttonGap = buttonSize / 4.0f;
    const float startX = (window.getSize().x - 2.0f * buttonSize - buttonGap) / 2.0f;
    const float startY = (window.getSize().y - 2.0f * buttonSize - buttonGap) / 2.0f;
    for (int i = 0; i < 4; i++)
    {
        auto button = tgui::Button::create();
        button->setSize(buttonSize, buttonSize);
        button->setPosition(startX + (i % 2) * (buttonSize + buttonGap),
                            startY + (i / 2) * (buttonSize + buttonGap));
        // when mouse on buttons the shadow is same as orginal color
        button->getRenderer()->setBackgroundColorHover(colors[i]);
        button->setVisible(false); // not visible to the frame unitl start new game
        // Disable the focus of the button
        button->setFocusable(false);
        gui.add(button); // add it to frame
        buttons.push_back(button);
    }
    // Create the Start New Game button
    auto startButton = tgui::Button::create();
    startButton->setPosition(20, 20);
    startButton->setSize(200, 50);
    startButton->setText("Start New Game");
    gui.add(startButton);
    // exit button

    gui.add(game.exitButton);
    game.exitButton->onPress.connect([&window]()
                                     { window.close(); });
    game.exitButton->setVisible(false); // not visible to the frame unitl start new game

    // creating action button of starting new game so will display the counter and then make buttons visible
    startButton->onPress.connect([&game, &buttons, &window, &gui]()
                                 {

    // Create starting sound affect
    sf::SoundBuffer startingGameSoundBuffer = game.generateCustomSound(980.0, 0.5);
    sf::Sound startingGameSound(startingGameSoundBuffer);
    startingGameSound.setVolume(5);
    sf::SoundBuffer secondSoundBuffer = game.generateCustomSound(440.0, 0.5);
    sf::Sound secondSound(secondSoundBuffer);
    secondSound.setVolume(5);
    sf::SoundBuffer fourthBuffer = game.generateCustomSound(740.0, 0.5);
    sf::Sound fourth(fourthBuffer);
    fourth.setVolume(5);
    startingGameSound.play();
    game.countingStartGame(gui, window, "Game starting in 3 seconds... Get ready!", 1000);
    secondSound.play();
    game.countingStartGame(gui, window, "Game starting in 2 seconds... Get ready!", 1000);
    fourth.play();
    game.countingStartGame(gui, window, "Game starting in 1 seconds... Get ready!", 1000);

    // after finishing the couinting message will reset the timer for new counter during play catch 
    // if did not click anyhting will lose the game 
    game.restartTimer();
    game.startNewGame();//start game
    //make them visible and enabled
    for (auto button : buttons) {
        button->setEnabled(true);
        button->setVisible(true);
    }
    game.displaySequence(buttons, window, gui); });

    /*
    for (auto button : buttons) {
        button->setEnabled(false);
        button->setVisible(false);
    }*/
    // set the colors to the buttons
    buttons[0]->getRenderer()->setBackgroundColor(tgui::Color(128, 255, 128));
    buttons[1]->getRenderer()->setBackgroundColor(tgui::Color(255, 128, 128));
    buttons[2]->getRenderer()->setBackgroundColor(tgui::Color(255, 255, 128));
    buttons[3]->getRenderer()->setBackgroundColor(tgui::Color(128, 128, 255));
    // looping thorught the simon buttons
    for (int i = 0; i < 4; i++)
    {
        // connect action events of each button from "
        buttons[i]->onPress.connect([&, i]()
                                    {
                                        game.restartTimer(); // restart time
                                        if (game.checkIfTimeExceeded())
                                        { // Check if the time has exceeded
                                            /*
                                            std::string first= "Sorry, wrong color. You made it to round ";
                                            std::string last = " of the game ";
                                            game.displayGameOverMessage(gui, first, game.getSequence().size() - 1, last);//display lost game message
                                                */

                                            game.restartTimer(); // restart the timer
                                            game.displaySequence(buttons, window, gui);
                                        }
                                        else
                                        { // else will continue of corrdct user gussed
                                            if (game.userGuess(static_cast<SimonGame::Color>(i), gui, buttons))
                                            {
                                                game.restartTimer();
                                                game.displaySequence(buttons, window, gui);
                                            }
                                        } });
    }
    // Start the game
    const sf::Time TimePerFrame = sf::seconds(1.f / 60.f); // set the time
    sf::Clock clock;                                       // creating clock
    sf::Time timeSinceLastUpdate = sf::Time::Zero;         // intialize the time to last updated
    sf::Clock timer;                                       // creating timer

    while (window.isOpen())
    {
        // Restart the clockand accumulate the left time
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        // Update the game state while the time since the last update is greater than the time per frame
        while (timeSinceLastUpdate > TimePerFrame)
        {
            // Reduce the amount of time since the last update by the number of frames.
            timeSinceLastUpdate -= TimePerFrame;

            sf::Event event; /// create event object
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                { // if closed exit the game
                    window.close();
                }
                gui.handleEvent(event);
            }
        }
        if (game.gameInProgress && game.checkIfTimeExceeded())
        { // if the time has passed the 5 second, lose the game
            game.gameOverSound.play();
            std::string first = "Sorry, time out. You made it to round ";
            std::string last = " of the game ";
            game.displayGameOverMessage(gui, first, game.getSequence().size() - 1, last, buttons);
        }
        window.clear();
        window.draw(game.backgroundimage);

        gui.updateTime(); // updated the timer
        gui.draw();
        window.display();
    }
    return 0;
}