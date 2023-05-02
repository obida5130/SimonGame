#include "SimonGame.hpp"
#ifndef M_PI
#define M_PI (3.14159265358979323846) // Define M_PI
#endif
/**
 * siminGame class constructor
 * constructor respnsible for loading image from file
 * creating sounds wave and setting the frequency
 * create exit button and lost sound
 */
SimonGame::SimonGame()
{
	/***************loading image background********************/
	if (!backgroundTexture.loadFromFile("simon.jpg"))
	{
		std::cerr << "Error loading background image" << std::endl;
	}
	backgroundimage.setTexture(backgroundTexture);
	/************Seed the random number generator****************/
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	rng.seed(seed);
	/*****************seeting up the sounds waves*****************/
	// Initialize frequencies for sounds
	const std::vector<double> frequencies = {
		222.22, // Green button
		523.25, // Red button
		659.25, // Yellow button
		783.99	// Blue button
	};
	const double amplitude = 30000.0;  // set wave
	const unsigned sampleRate = 44100; // set rate for sound
	const double duration = 0.5;	   // set time for sound

	// Generate sound buffer for each color
	for (const auto &frequency : frequencies)
	{
		sf::SoundBuffer buffer;
		std::vector<sf::Int16> samples(sampleRate * duration);
		// Generate sine wave samples for the sound buffer
		for (size_t i = 0; i < samples.size(); ++i)
		{
			samples[i] = generateSound(static_cast<double>(i) / sampleRate, frequency, amplitude);
		}

		buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate);
		soundBuffers.push_back(buffer);
	}

	float desiredVolume = 5.0f; // set volume of the sound
	// Set the volume
	for (const auto &soundBuffer : soundBuffers)
	{
		sf::Sound sound(soundBuffer);
		sound.setVolume(desiredVolume);
		sounds.push_back(sound);
	}
	/********************************************************************/
	gameInProgress = false; // initialize game state not started yet
	/*************************exit button when game over************************/
	exitButton = tgui::Button::create();
	exitButton->setPosition(300, 20);
	exitButton->setSize(200, 50);
	exitButton->setText("Exit");
	/*************************sound game over***********************/
	gameOverSoundBuffer = generateCustomSound(1000.0, 1.0);
	gameOverSound = sf::Sound(gameOverSoundBuffer);
}
/**
 * Generate sine wave sample for given time, frequency, and amplitude
 */
sf::Int16 SimonGame::generateSound(double time, double frequency, double amplitude)
{
	return static_cast<sf::Int16>(amplitude * sin(2.0 * M_PI * frequency * time));
}
/**
 * starting new game which reset sequebce and timer and user guess
 */
void SimonGame::startNewGame()
{
	exitButton->setVisible(false); // remove exit button while playing
	// here will set last user input time to current time
	lastUserInputTime = std::chrono::steady_clock::now();
	sequence.clear();	   // reset squence
	userGuessIndex = 0;	   // reset the user guess index
	generateColor();	   // generate new game
	gameInProgress = true; // set the game state as active to starting counting for 5 sec
}
/**
 * ButtonBlink method for make buttons bigger when blinked and set current colors to darker
 * Also play the correct sound to correct index buttons
 */
void SimonGame::buttonBlink(SimonGame::Color color, const std::vector<tgui::Button::Ptr> &buttons, sf::RenderWindow &window, tgui::Gui &gui)
{
	// here will get the buttons colors and stored them here
	auto originalColor = buttons[static_cast<int>(color)]->getRenderer()->getBackgroundColor();
	// here when button gets bigger the color of the button get darkker
	tgui::Color darkerColor = tgui::Color(std::max(originalColor.getRed() - 100, 0),
										  std::max(originalColor.getGreen() - 100, 0),
										  std::max(originalColor.getBlue() - 100, 0));
	// set the darker color to the buttons
	buttons[static_cast<int>(color)]->getRenderer()->setBackgroundColor(darkerColor);
	// increase the button size during the blink
	auto originalSize = buttons[static_cast<int>(color)]->getSize();
	auto newSize = originalSize * 1.05f;				// new size of the button
	buttons[static_cast<int>(color)]->setSize(newSize); // set the size which make the button bigger
	// here will play sounds depends on index of the array with corrects colors
	int soundIndex = static_cast<int>(color);
	sounds[soundIndex].stop();	  // stop any exist sound
	sounds[soundIndex].play();	  // start music
	window.clear();				  // clear frame
	window.draw(backgroundimage); // add background image to the frame
	gui.draw();					  // draw everything on frame
	window.display();			  // make frame visible
	sf::sleep(sf::milliseconds(300));
	// set back normal size of the buttons when finish plink
	buttons[static_cast<int>(color)]->setSize(originalSize); // seeting back orginal size of the buttons
	buttons[static_cast<int>(color)]->getRenderer()->setBackgroundColor(originalColor);
	/*
	window.clear();
	gui.draw();
	window.display();
	sf::sleep(sf::milliseconds(300));*/
}
/**
 * restart timer every new game or new round
 */
void SimonGame::restartTimer()
{
	lastUserInputTime = std::chrono::steady_clock::now();
}
/**
 * here check if user passed the 5 second durring the game will return false otherwise true which continue the game
 */
bool SimonGame::checkIfTimeExceeded()
{
	auto currentTime = std::chrono::steady_clock::now();
	auto timeSinceLastInput = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUserInputTime);

	if (timeSinceLastInput.count() > 5)
	{
		return true;
	}
	return false;
}
/**
 * here will display sequence durng game play
 */
void SimonGame::displaySequence(const std::vector<tgui::Button::Ptr> &buttons, sf::RenderWindow &window, tgui::Gui &gui)
{

	for (auto color : sequence)
	{
		buttonBlink(color, buttons, window, gui);
		sf::sleep(sf::milliseconds(300));
	}
}
/**
 * Get the current sequence of colors
 */
const std::vector<SimonGame::Color> &SimonGame::getSequence() const
{
	return sequence;
}
/**
 * display dialog message when lost or time out
 */
void SimonGame::displayGameOverMessage(tgui::Gui &gui, const std::string &first, int level, const std::string &last, std::vector<tgui::Button::Ptr> &buttons)
{

	auto gameOverMessage = tgui::MessageBox::create();
	gameOverMessage->setTitle("Game Over");
	gameOverMessage->setText(first + std::to_string(level) + last);
	gameOverMessage->setSize("50%", "20%");

	gameOverMessage->addButton("OK");
	gameOverMessage->onButtonPress([&gui, gameOverMessage, &buttons]()
								   {
		gui.remove(gameOverMessage);
	for (const auto& button : buttons) {
		button->setVisible(false);
	} });
	exitButton->setVisible(true);

	gameInProgress = false;

	gui.add(gameOverMessage);
}
/**
 * this method diplay counting message when start new game button clicked, and calling this methon in main class cpp
 */
void SimonGame::countingStartGame(tgui::Gui &gui, sf::RenderWindow &window, const std::string &message, unsigned int duration)
{
	auto label = tgui::Label::create();
	label->setText(message);
	label->setTextSize(24);
	label->setPosition(window.getSize().x * 0.5f - label->getSize().x * 0.5f, window.getSize().y * 0.5f - label->getSize().y * 0.5f);
	label->getRenderer()->setTextColor(tgui::Color(128, 0, 128));
	label->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
	auto highlightPanel = tgui::Panel::create();
	highlightPanel->setSize(label->getSize().x + 10, label->getSize().y + 10);
	highlightPanel->setPosition(label->getPosition().x - 5, label->getPosition().y - 5);
	highlightPanel->getRenderer()->setBackgroundColor(tgui::Color(255, 255, 0, 100));

	gui.add(highlightPanel);
	gui.add(label);

	sf::Clock clock;
	while (clock.getElapsedTime().asMilliseconds() < duration)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			gui.handleEvent(event);
		}

		window.clear();
		window.draw(backgroundimage);
		gui.draw();
		window.display();
	}

	gui.remove(highlightPanel);
	gui.remove(label);
}
/**
 * here generating sounds affects when starting the game
 */
sf::SoundBuffer SimonGame::generateCustomSound(double frequency, double duration)
{
	const double amplitude = 30000.0;
	const unsigned sampleRate = 44100;
	sf::SoundBuffer buffer;
	std::vector<sf::Int16> samples(sampleRate * duration);

	for (size_t i = 0; i < samples.size(); ++i)
	{
		samples[i] = static_cast<sf::Int16>(amplitude * sin(2.0 * M_PI * frequency * static_cast<double>(i) / sampleRate));
	}

	buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate);
	return buffer;
}
/**
 * user guess saved in vector and later we checking if match the current sequence will return true otherwite false which lost the game
 */
bool SimonGame::userGuess(Color color, tgui::Gui &gui, std::vector<tgui::Button::Ptr> &buttons)
{
	auto currentTime = std::chrono::steady_clock::now();
	auto timeSinceLastInput = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUserInputTime);

	if (timeSinceLastInput.count() > 5)
	{
		return false;
	}

	lastUserInputTime = currentTime;

	if (color == sequence[userGuessIndex])
	{
		userGuessIndex++;
		if (userGuessIndex >= sequence.size())
		{
			userGuessIndex = 0;
			generateColor();
			return true;
		}
	}
	else
	{
		gameOverSound.play();

		std::string first = "Sorry, wrong color. You made it to round ";
		std::string last = " of the game ";

		displayGameOverMessage(gui, first, sequence.size() - 1, last, buttons);
		return false;
	}

	return false;
}
/**
 * this method generate color for sequence
 */
void SimonGame::generateColor()
{
	std::uniform_int_distribution<int> dist(0, 3);
	sequence.push_back(static_cast<Color>(dist(rng)));
}
