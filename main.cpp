#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace sf;
using namespace std;

RenderWindow window(VideoMode(1000, 800), "Bricksy");
long long highscore = 0, highscore_update = 0;
long trans = 255;
CircleShape x;

Sound Menu_sound, shooting_sound, lose_life, paddle_ball, powerGainedSound, brick_ball, unbreakable_ball;
Texture paddle_tex, cursorshape, brick_tex[7], powerUp_tex[7], ball_tex, bullet_tex, logo, sound;
Text GameOver, scorelable, score, highscore_txt, nothighscore, credits_txt, play_button, back_button, marwan, mina, joe, amr, nourhan, mo5tar, credits_txt1;
Font FredokaOne, Revamped;
Sprite cursor, bricksy;

struct button
{
	CircleShape circle;
	bool isEnabled = 0;
	Texture texture;
}mute;

struct PowerShapes
{
	RectangleShape rectangle;
	CircleShape circle;
}power[4];

struct Background
{

	CircleShape circle;

	Texture texture;

	Sound music;

	SoundBuffer buffer;

}background;

struct Levels
{
	ifstream file;
	int x = 0, y = 0, lvl = 0, color, non_breakables = 0, num = 20;
	string name[20];
	Text txt;
}level;

struct Options
{
	Text txt;
	bool isSelected = 0;

}Start_Game, Exit, Resume_game, Return_to_main_menu, Retry, Credits, back, lvlmaker;

struct Ball
{
	CircleShape circle;
	bool isReleased = 1;
	float x = 0, y = 0.9;
};
vector<Ball> balls;

struct Bricks
{
	RectangleShape rectangle;
	bool isShown = 1;
	//0 for nothing
	//1 for adding 3 balls
	//2 for adding 1 life
	//3 for fire ball
	//4 for shooter
	//5 for enlarging the paddle
	//6 for shrinking the paddle

	int powertype = 0;
	CircleShape powershape;
	bool powerActive = 0, isBreakabel = 1;

	int level = 2;
	int points = 10;

}; vector<Bricks> bricks;

struct  lvlMakerBricks
{
	RectangleShape rectangle;
	bool isSelected = 0;
	float color = 4;
} lvl_maker_bricks[304];

struct Paddle
{
	RectangleShape rectangle;
	//0 for nothing
	//1 for adding 3 balls
	//2 for adding 1 life
	//3 for fire ball
	//4 for shooter
	//5 for enlarging the paddle
	//6 for shrinking the paddle


	bool powerGained[7] = {};
	Clock powerTimer[7], shootingTime;

	int lifeCounter = 4;
	CircleShape lifeShape[4];

	long long score = 0;
}paddle;

struct Shooter
{
	RectangleShape bullets[2];
	bool isShown[2] = { 1, 1 };

};
vector<Shooter> shooter;

void bouncing_balls() {
	//multi_balls
	if (paddle.powerGained[1]) {
		for (int i = 0; i < 3; i++)
		{
			balls.push_back(Ball());
			balls.back().circle.setRadius(15);
			balls.back().circle.setOrigin(15, 15);
			balls.back().circle.setTexture(&ball_tex);
			balls.back().circle.setPosition(paddle.rectangle.getPosition().x + (i - 1) * 20, paddle.rectangle.getPosition().y - balls.back().circle.getRadius());
		}

		paddle.powerGained[1] = 0;
	}

	for (int i = 0; i < balls.size(); i++)
	{
		if (balls[i].isReleased)
		{
			balls[i].circle.move(balls[i].x, balls[i].y);
			if (paddle.powerGained[3])
				balls[i].circle.setTextureRect(IntRect(0, 125, 125, 125));
			else
				balls[i].circle.setTextureRect(IntRect(0, 0, 125, 125));

			//with walls
			if (balls[i].circle.getPosition().y <= balls[i].circle.getRadius() || balls[i].circle.getPosition().y >= 800 - balls[i].circle.getRadius())
				balls[i].y *= -1;

			if (balls[i].circle.getPosition().x >= 1000 - balls[i].circle.getRadius() || balls[i].circle.getPosition().x <= balls[i].circle.getRadius())
				balls[i].x *= -1;


			//with paddle
			if (paddle.rectangle.getGlobalBounds().intersects(balls[i].circle.getGlobalBounds()) && balls[i].circle.getPosition().y <= 700 - balls[i].circle.getRadius() + 2)
			{
				balls[i].x = (balls[i].circle.getPosition().x - paddle.rectangle.getPosition().x) / (paddle.rectangle.getSize().x / 1.5);
				balls[i].y = sqrt(1 - balls[i].x * balls[i].x);
				balls[i].y *= -1;
				paddle_ball.play();
				balls[i].circle.setPosition(balls[i].circle.getPosition().x, 700 - balls[i].circle.getRadius() - paddle.rectangle.getOrigin().y);

				/*paddle.rectangle.rotate(-atan((balls[i].circle.getPosition().x - paddle.rectangle.getPosition().x) /
					(balls[i].circle.getPosition().y - paddle.rectangle.getPosition().y)));
				cout << -atan((balls[i].circle.getPosition().x - paddle.rectangle.getPosition().x) /
					(balls[i].circle.getPosition().y - paddle.rectangle.getPosition().y)) << endl;

				paddle.rotation_animation.restart();*/
			}



			//with bricks
			for (int j = 0; j < bricks.size(); j++)
				if (balls[i].circle.getGlobalBounds().intersects(bricks[j].rectangle.getGlobalBounds()) && bricks[j].isShown)
				{
					if (!paddle.powerGained[3] || !bricks[j].isBreakabel)
					{
						if (bricks[j].isBreakabel)
							brick_ball.play();
						else
							unbreakable_ball.play();
						if (abs(balls[i].circle.getPosition().y - bricks[j].rectangle.getPosition().y) >= bricks[j].rectangle.getSize().y - 1)
						{
							if (balls[i].circle.getPosition().y - bricks[j].rectangle.getPosition().y > 0)
								balls[i].circle.setPosition(balls[i].circle.getPosition().x,
									bricks[j].rectangle.getPosition().y + (bricks[j].rectangle.getSize().x / 2));

							else
								balls[i].circle.setPosition(balls[i].circle.getPosition().x,
									bricks[j].rectangle.getPosition().y - (bricks[j].rectangle.getSize().x / 2));
							balls[i].y *= -1;
						}
						else
						{
							if (balls[i].circle.getPosition().x - bricks[j].rectangle.getPosition().x > 0)
								balls[i].circle.setPosition(bricks[j].rectangle.getPosition().x + (bricks[j].rectangle.getSize().x / 2 + balls[i].circle.getRadius()),
									balls[i].circle.getPosition().y);

							else
								balls[i].circle.setPosition(bricks[j].rectangle.getPosition().x - (bricks[j].rectangle.getSize().x / 2 + balls[i].circle.getRadius()),
									balls[i].circle.getPosition().y);
							balls[i].x *= -1;
						}
						if (bricks[j].isBreakabel)
							bricks[j].level--;
					}
					else
						if (bricks[j].isBreakabel)
							bricks[j].level -= 10;
				}

		}
		else
			balls[i].circle.setPosition(paddle.rectangle.getPosition().x, 700 - balls[i].circle.getRadius() - paddle.rectangle.getOrigin().y);

		window.draw(balls[i].circle);
		if (balls[i].circle.getPosition().y >= 800 - balls[i].circle.getRadius())
		{
			balls.erase(balls.begin() + i);
		}

	}


}

void paddle_movement() {

	paddle.rectangle.setPosition(Mouse::getPosition(window).x, 700);

	if (paddle.rectangle.getPosition().x + paddle.rectangle.getSize().x / 2 > 1000)
		paddle.rectangle.setPosition(1000 - paddle.rectangle.getSize().x / 2, 700);

	if (paddle.rectangle.getPosition().x - paddle.rectangle.getSize().x / 2 < 0)
		paddle.rectangle.setPosition(paddle.rectangle.getSize().x / 2, 700);

	window.draw(paddle.rectangle);
}

void bricks_and_power() {

	for (int i = 0; i < bricks.size(); i++)
	{
		if (bricks[i].level <= 0 && !bricks[i].powerActive)
		{
			bricks[i].rectangle.setPosition(2000, 2000);
			bricks[i].isShown = 0;
			bricks[i].powerActive = 1;
			paddle.score += bricks[i].points;
		}
		if (bricks[i].powerActive && bricks[i].powertype > 0)
		{
			bricks[i].powershape.move(0, .8);
			if (bricks[i].powershape.getGlobalBounds().intersects(paddle.rectangle.getGlobalBounds()))
			{
				paddle.powerGained[bricks[i].powertype] = 1;
				paddle.score += 10;
				bricks[i].powershape.setPosition(2000, 2000);
				paddle.powerTimer[bricks[i].powertype].restart();
				powerGainedSound.play();
			}
		}
		if (bricks[i].powershape.getPosition().y >= 800)
		{
			bricks[i].powerActive = 0;
			bricks[i].powershape.setPosition(2000, 2000);
		}
		if (bricks[i].isBreakabel)
			bricks[i].rectangle.setTextureRect(IntRect(0, 187 * (3 - bricks[i].level), 376, 187));

		if (bricks[i].isShown)
			window.draw(bricks[i].rectangle);
		if (bricks[i].powerActive && bricks[i].powertype > 0)
			window.draw(bricks[i].powershape);

		if ((!bricks[i].powerActive || bricks[i].powertype == 0) && !bricks[i].isShown)
		{
			bricks.erase(bricks.begin() + i);
		}
	}
	score.setString(to_string(paddle.score));
	score.setOrigin(score.getGlobalBounds().width, 0);
	window.draw(score);

	//power timing
	int r = 1, c = 1;
	for (int i = 3; i < 7; i++)
	{

		if (paddle.powerGained[i] && paddle.powerTimer[i].getElapsedTime().asSeconds() >= 5)
			paddle.powerGained[i] = 0;
		if (paddle.powerGained[i])
		{
			power[i - 3].rectangle.setSize(Vector2f(150 - paddle.powerTimer[i].getElapsedTime().asSeconds() * 30, 10));
			power[i - 3].rectangle.setOrigin(0, 5);
			power[i - 3].rectangle.setPosition(220 * r, 740 + 24 * c);
			power[i - 3].circle.setPosition(200 * r, 740 + 24 * c);
			c++;
			if (c == 3)
			{
				r += 1.5;
				c = 1;

			}


			window.draw(power[i - 3].circle);
			window.draw(power[i - 3].rectangle);
		}

	}



	/*for (int i = 0; i < 3; i++)
	{
		power[i].rank = power[i].rectangle.getSize().x;

		power[i].circle.setPosition(200, 760+24* (i - 1) );

		window.draw(power[i].circle);
	}*/

}

void shooting()
{

	if (paddle.shootingTime.getElapsedTime().asSeconds() >= .4 && paddle.powerGained[4])
	{
		shooter.push_back(Shooter());
		shooter.back().bullets[0].setSize(Vector2f(20, 20));
		shooter.back().bullets[0].setOrigin(10, 10);
		shooter.back().bullets[0].setTexture(&bullet_tex);
		shooter.back().bullets[0].setRotation(-45);
		shooter.back().bullets[0].setPosition(paddle.rectangle.getPosition().x - paddle.rectangle.getSize().x * 0.375, paddle.rectangle.getPosition().y);
		shooter.back().bullets[1].setSize(Vector2f(20, 20));
		shooter.back().bullets[1].setOrigin(10, 10);
		shooter.back().bullets[1].setTexture(&bullet_tex);
		shooter.back().bullets[1].setRotation(-45);
		shooter.back().bullets[1].setPosition(paddle.rectangle.getPosition().x + paddle.rectangle.getSize().x * 0.375, paddle.rectangle.getPosition().y);
		paddle.shootingTime.restart();
		shooting_sound.play();
	}
	for (int i = 0; i < shooter.size(); i++)
	{
		shooter[i].bullets[0].move(0, -1);
		shooter[i].bullets[1].move(0, -1);

		for (int j = 0; j < bricks.size(); j++)
		{
			if (shooter[i].bullets[0].getGlobalBounds().intersects(bricks[j].rectangle.getGlobalBounds()) && bricks[j].isShown)
			{
				shooter[i].isShown[0] = 0;
				shooter[i].bullets[0].setPosition(-100, -100);

				if (bricks[j].isBreakabel)
					bricks[j].level--;
			}
			if (shooter[i].bullets[1].getGlobalBounds().intersects(bricks[j].rectangle.getGlobalBounds()) && bricks[j].isShown)
			{
				shooter[i].isShown[1] = 0;
				shooter[i].bullets[1].setPosition(-100, -100);

				if (bricks[j].isBreakabel)
					bricks[j].level--;
			}
		}

		if (shooter[i].bullets[0].getPosition().y <= 0)
			shooter[i].isShown[0] = 0;

		if (shooter[i].bullets[1].getPosition().y <= 0)
			shooter[i].isShown[1] = 0;


		window.draw(shooter[i].bullets[0]);
		window.draw(shooter[i].bullets[1]);

		if (!shooter[i].isShown[0] && !shooter[i].isShown[1])
			shooter.erase(shooter.begin() + i);
	}

}

void enlarging_shrinking() {
	if (paddle.powerTimer[5].getElapsedTime() < paddle.powerTimer[6].getElapsedTime() && paddle.powerGained[5])
	{
		paddle.rectangle.setSize(Vector2f(150 * 1.5, 30));
		paddle.rectangle.setOrigin(paddle.rectangle.getSize().x / 2, paddle.rectangle.getSize().y / 2);
		paddle.rectangle.setTextureRect(IntRect(0, 0, 638, 85));
		paddle.powerGained[6] = 0;

	}

	else if (paddle.powerTimer[5].getElapsedTime() > paddle.powerTimer[6].getElapsedTime() && paddle.powerGained[6])
	{
		paddle.rectangle.setSize(Vector2f(150 / 1.5, 30));
		paddle.rectangle.setOrigin(paddle.rectangle.getSize().x / 2, paddle.rectangle.getSize().y / 2);
		paddle.rectangle.setTextureRect(IntRect(0, 171, 285, 86));
		paddle.powerGained[5] = 0;
	}

	else
	{
		paddle.rectangle.setSize(Vector2f(150, 30));
		paddle.rectangle.setOrigin(paddle.rectangle.getSize().x / 2, paddle.rectangle.getSize().y / 2);
		paddle.rectangle.setTextureRect(IntRect(0, 86, 425, 84));
	}

	paddle.rectangle.setOrigin(paddle.rectangle.getSize().x / 2, paddle.rectangle.getSize().y / 2);
}

void life_counting() {

	for (int i = 0; i < paddle.lifeCounter; i++)
	{
		window.draw(paddle.lifeShape[i]);
	}
	if (paddle.powerGained[2] && paddle.lifeCounter < 3)
	{
		paddle.lifeCounter++;
		paddle.powerGained[2] = 0;
	}
	if (balls.empty())
	{
		balls.push_back(Ball());
		balls.back().circle.setRadius(15);
		balls.back().circle.setOrigin(15, 15);
		balls.back().circle.setTexture(&ball_tex);
		balls.back().circle.setTextureRect(IntRect(0, 0, 125, 125));
		balls.back().isReleased = 0;

		if (paddle.lifeCounter < 4)
			lose_life.play();

		paddle.lifeCounter--;
		for (int i = 0; i < 7; i++)
			paddle.powerGained[i] = 0;
	}
}

void nlevel() {

	if ((!(bricks.size() - level.non_breakables) || bricks.empty()) && level.txt.getCharacterSize() > 350)
	{
		bricks.clear();
		level.txt.setCharacterSize(20);
		trans = 255;
		level.num = 0;
	}

	if (level.txt.getCharacterSize() >= 350)
	{
		if ((!(bricks.size() - level.non_breakables) || bricks.empty()) && level.lvl < 9)
		{

			balls.clear();
			balls.push_back(Ball());
			balls.back().circle.setRadius(15);
			balls.back().circle.setOrigin(15, 15);
			balls.back().circle.setTexture(&ball_tex);
			balls.back().circle.setTextureRect(IntRect(0, 0, 125, 125));
			balls.back().isReleased = 0;
			shooter.clear();

			for (int i = 0; i < 7; i++)
				paddle.powerGained[i] = 0;


			level.non_breakables = 0;
			bricks.clear();
			level.file.open(level.name[level.lvl]);
			while (level.file >> level.x && level.file >> level.y && level.file >> level.color)
			{
				bricks.push_back(Bricks());
				bricks.back().rectangle.setSize(Vector2f(60, 30));
				bricks.back().rectangle.setOrigin(30, 15);
				bricks.back().rectangle.setPosition(level.x, level.y);
				bricks.back().powertype = rand() % 40;

				if (bricks.back().powertype > 6)
					bricks.back().powertype = 0;

				if (level.color == 6)
				{
					level.non_breakables++;
					bricks.back().isBreakabel = 0;
					bricks.back().level = 3;
				}
				else
					bricks.back().level = rand() % 3 + 1;
				bricks.back().rectangle.setTexture(&brick_tex[level.color]);
				bricks.back().points = 10 * bricks.back().level;
				bricks.back().powershape.setRadius(15);
				bricks.back().powershape.setOrigin(15, 15);
				bricks.back().powershape.setPosition(bricks.back().rectangle.getPosition());
				bricks.back().powershape.setTexture(&powerUp_tex[bricks.back().powertype]);

			}
			level.file.close();

			level.txt.setCharacterSize(15);
			level.lvl++;
		}
	}
	else
	{
		level.txt.setCharacterSize(level.num += 2);
		//level.txt.setScale(level.num / 10, level.num / 10);
		level.txt.setOrigin((level.txt.getGlobalBounds().width / 2), (level.txt.getGlobalBounds().height));
		level.txt.setFillColor(sf::Color(255, 255, 255, trans));
		if (trans > 0)
		{
			trans--;
		}
		level.txt.setPosition(500, 400);
		level.txt.setString(to_string(level.lvl + 1));
		if (level.lvl < 6)
			window.draw(level.txt);

	}
}

bool gameOver_menu() {
	if (paddle.lifeCounter)
	{
		return 1;
	}
	else {

		Texture window_capture;
		window_capture.create(1000, 800);
		window_capture.update(window);

		Sprite window_capture_sprite;
		window_capture_sprite.setTexture(window_capture);

		Return_to_main_menu.txt.setPosition(500, 400);
		while (true)
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();

				if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Retry.txt.getGlobalBounds())) {
					paddle.lifeCounter = 3;
					bricks.clear();
					level.lvl = 0;
					paddle.score = 0;
					highscore = highscore_update;
					score.setPosition(1000, 747);
					return 1;
				}
				if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Return_to_main_menu.txt.getGlobalBounds())) {
					paddle.lifeCounter = 4;
					bricks.clear();
					level.lvl = 0;
					paddle.score = 0;
					highscore = highscore_update;
					score.setPosition(1000, 747);
					return 0;
				}
			}

			if (cursor.getGlobalBounds().intersects(Retry.txt.getGlobalBounds()))
			{
				Retry.txt.setCharacterSize(90);
				Retry.txt.setOrigin((Retry.txt.getGlobalBounds().width / 2), (Retry.txt.getGlobalBounds().height * 1.5));
				Retry.txt.setFillColor(Color::Red);
			}
			else
			{
				Retry.txt.setCharacterSize(80);
				Retry.txt.setOrigin((Retry.txt.getGlobalBounds().width / 2), (Retry.txt.getGlobalBounds().height * 1.5));
				Retry.txt.setFillColor(Color::Magenta);
			}

			if (cursor.getGlobalBounds().intersects(Return_to_main_menu.txt.getGlobalBounds()))
			{
				Return_to_main_menu.txt.setCharacterSize(80);
				Return_to_main_menu.txt.setOrigin((Return_to_main_menu.txt.getGlobalBounds().width / 2), (Return_to_main_menu.txt.getGlobalBounds().height * 1.5));
				Return_to_main_menu.txt.setFillColor(Color::Red);
			}
			else
			{
				Return_to_main_menu.txt.setCharacterSize(70);
				Return_to_main_menu.txt.setOrigin((Return_to_main_menu.txt.getGlobalBounds().width / 2), (Return_to_main_menu.txt.getGlobalBounds().height * 1.5));
				Return_to_main_menu.txt.setFillColor(Color::Magenta);
			}

			cursor.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);

			window.clear();
			window.draw(window_capture_sprite);
			window.draw(GameOver);
			window.draw(Retry.txt);
			window.draw(Return_to_main_menu.txt);
			if (paddle.score > highscore)
			{

				window.draw(highscore_txt);
				score.setPosition(550, 500);
				window.draw(score);
				highscore_update = paddle.score;
			}
			if (paddle.score <= highscore)
			{
				score.setPosition(550, 500);
				window.draw(nothighscore);
				window.draw(score);
			}

			window.draw(score);

			window.draw(cursor);
			window.display();
		}
	}
}

bool pause_menu() {

	Texture window_capture;
	window_capture.create(1000, 800);
	window_capture.update(window);

	Sprite window_capture_sprite;
	window_capture_sprite.setTexture(window_capture);
	Return_to_main_menu.txt.setPosition(500, 420);

	Exit.txt.setPosition(500, 540);

	while (true)
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Resume_game.txt.getGlobalBounds()))
			{
				Mouse::setPosition(Vector2i(paddle.rectangle.getPosition()), window);
				Menu_sound.play();
				return 1;
			}
			if (Keyboard::isKeyPressed(Keyboard::M) || Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(mute.circle.getGlobalBounds()))
			{
				if (mute.isEnabled == 1)
				{
					mute.isEnabled = 0;
					background.music.play();
				}
				else if (mute.isEnabled == 0)
				{
					mute.isEnabled = 1;
					background.music.pause();
				}

			}
			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Return_to_main_menu.txt.getGlobalBounds()))
			{
				Mouse::setPosition(Vector2i(10, 0), window);
				//Exit.txt.setPosition(500, 350 + 150 + 150);
				Exit.txt.setPosition(500, 800);
				Menu_sound.play();
				return 0;
			}
			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Exit.txt.getGlobalBounds()))
			{

				Menu_sound.play();
				window.close();
			}
		}

		if (cursor.getGlobalBounds().intersects(Resume_game.txt.getGlobalBounds()))
		{
			Resume_game.txt.setCharacterSize(90);
			Resume_game.txt.setOrigin((Resume_game.txt.getGlobalBounds().width / 2), (Resume_game.txt.getGlobalBounds().height * 1.5));
			Resume_game.txt.setFillColor(Color::Red);

		}
		else
		{
			Resume_game.txt.setCharacterSize(80);
			Resume_game.txt.setOrigin((Resume_game.txt.getGlobalBounds().width / 2), (Resume_game.txt.getGlobalBounds().height * 1.5));
			Resume_game.txt.setFillColor(Color::Magenta);
		}

		if (cursor.getGlobalBounds().intersects(Exit.txt.getGlobalBounds()))
		{
			Exit.txt.setCharacterSize(90);
			Exit.txt.setOrigin((Exit.txt.getGlobalBounds().width / 2), (Exit.txt.getGlobalBounds().height * 1.5));
			Exit.txt.setFillColor(Color::Red);
		}
		else
		{
			Exit.txt.setCharacterSize(80);
			Exit.txt.setOrigin((Exit.txt.getGlobalBounds().width / 2), (Exit.txt.getGlobalBounds().height * 1.5));
			Exit.txt.setFillColor(Color::Magenta);
		}
		if (cursor.getGlobalBounds().intersects(Return_to_main_menu.txt.getGlobalBounds()))
		{
			Return_to_main_menu.txt.setCharacterSize(80);
			Return_to_main_menu.txt.setOrigin((Return_to_main_menu.txt.getGlobalBounds().width / 2), (Return_to_main_menu.txt.getGlobalBounds().height * 1.5));
			Return_to_main_menu.txt.setFillColor(Color::Red);

		}
		else
		{
			Return_to_main_menu.txt.setCharacterSize(70);
			Return_to_main_menu.txt.setOrigin((Return_to_main_menu.txt.getGlobalBounds().width / 2), (Return_to_main_menu.txt.getGlobalBounds().height * 1.5));
			Return_to_main_menu.txt.setFillColor(Color::Magenta);
		}


		cursor.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);


		if (mute.isEnabled)
		{
			mute.circle.setTextureRect(IntRect(0, 0, 191, 192));
		}
		else {
			mute.circle.setTextureRect(IntRect(192, 0, 192, 192));
		}

		window.clear();
		window.draw(window_capture_sprite);
		window.draw(Resume_game.txt);
		window.draw(Exit.txt);
		window.draw(Return_to_main_menu.txt);
		window.draw(mute.circle);
		window.draw(cursor);
		window.display();
	}
}

void play() {

	background.music.setVolume(2);
	bool keep = 1;
	while (keep)
	{
		keep = gameOver_menu();
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (Keyboard::isKeyPressed(Keyboard::Escape)) {
				keep = pause_menu();
				lvlmaker.txt.setPosition(500, 500);

			}
			if (Mouse::isButtonPressed(Mouse::Left))
				balls.front().isReleased = 1;


			if (Keyboard::isKeyPressed(Keyboard::X))
			{
				paddle.powerGained[1] = 1;
				paddle.powerTimer[1].restart();
			}
			if (Keyboard::isKeyPressed(Keyboard::Numpad2))
			{
				paddle.powerGained[2] = 1;
				paddle.powerTimer[2].restart();
			}
			if (Keyboard::isKeyPressed(Keyboard::C))
			{
				paddle.powerGained[3] = 1;
				paddle.powerTimer[3].restart();
			}
			if (Keyboard::isKeyPressed(Keyboard::Numpad4))
			{
				paddle.powerGained[4] = 1;
				paddle.powerTimer[4].restart();
			}
			if (Keyboard::isKeyPressed(Keyboard::Numpad5))
			{
				paddle.powerGained[5] = 1;
				paddle.powerTimer[5].restart();
			}
			if (Keyboard::isKeyPressed(Keyboard::Numpad6))
			{
				paddle.powerGained[6] = 1;
				paddle.powerTimer[6].restart();
			}


		}


		if (level.lvl == 7) {

			amr.setPosition(500, 150);
			mo5tar.setPosition(500, 450);
			marwan.setPosition(500, 250);
			mina.setPosition(500, 350);
			nourhan.setPosition(500, 550);
			joe.setPosition(500, 650);
			score.setPosition(550, 900);
			nothighscore.setPosition(500, 800);




			while (window.isOpen()) {
				Event event;
				while (window.pollEvent(event))
				{
					if (event.type == Event::Closed)
						window.close();
				}

				cursor.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
				amr.move(0, -0.1);
				joe.move(0, -0.1);
				marwan.move(0, -0.1);
				mina.move(0, -0.1);
				mo5tar.move(0, -0.1);
				nourhan.move(0, -0.1);
				credits_txt1.move(0, -0.1);
				if (nothighscore.getPosition().y >= 350)
					nothighscore.move(0, -0.1);
				if (score.getPosition().y >= 450)
					score.move(0, -0.1);
				if (joe.getPosition().y <= -250 || Keyboard::isKeyPressed(Keyboard::Escape))
					return;
				background.circle.rotate(.0025);

				window.clear();
				window.draw(background.circle);
				window.draw(marwan);
				window.draw(amr);
				window.draw(joe);
				window.draw(mo5tar);
				window.draw(mina);
				window.draw(credits_txt1);
				window.draw(nourhan);
				window.draw(score);
				window.draw(nothighscore);
				window.draw(cursor);
				window.display();
			}
		}

		window.clear();

		background.circle.rotate(.0025);

		window.draw(background.circle);

		nlevel();

		bouncing_balls();

		bricks_and_power();

		enlarging_shrinking();

		paddle_movement();

		shooting();

		life_counting();

		window.display();
	}

}

void credits() {
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(back.txt.getGlobalBounds()))
			{
				return;

			}
		}

		cursor.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);

		if (cursor.getGlobalBounds().intersects(back.txt.getGlobalBounds()))
		{
			back.txt.setCharacterSize(90);
			back.txt.setOrigin((back.txt.getGlobalBounds().width / 2), (back.txt.getGlobalBounds().height * 1.5));
			back.txt.setFillColor(Color::Red);
		}
		else
		{
			back.txt.setCharacterSize(80);
			back.txt.setOrigin((back.txt.getGlobalBounds().width / 2), (back.txt.getGlobalBounds().height * 1.5));
			back.txt.setFillColor(Color::Magenta);
		}
		window.clear();
		window.draw(background.circle);
		window.draw(back.txt);
		window.draw(marwan);
		window.draw(amr);
		window.draw(joe);
		window.draw(mo5tar);
		window.draw(mina);
		window.draw(nourhan);
		window.draw(credits_txt1);
		window.draw(cursor);
		window.display();
	}
}

void level_maker() {

	CircleShape cursor1(10);
	cursor1.setOrigin(10, 10);
	cursor1.setFillColor(Color::Yellow);
	background.music.setVolume(100);
	ofstream saveing_file;
	saveing_file.open("level.txt");

	Texture level_maker_colors;
	level_maker_colors.loadFromFile("Textures/bricks.png");

	int c = 1, r = 0;
	for (int i = 0; i < 304; i++)
	{
		if (r % 16 == 0 && r != 0)
		{
			c++;
			r = 0;
		}

		lvl_maker_bricks[i].rectangle.setSize(Vector2f(60, 30));
		lvl_maker_bricks[i].rectangle.setOrigin(30, 15);
		lvl_maker_bricks[i].rectangle.setPosition(61 * r + 42, 31 * c);
		lvl_maker_bricks[i].rectangle.setTexture(&level_maker_colors);
		lvl_maker_bricks[i].rectangle.setTextureRect(IntRect(0, 0, 530, 266));
		lvl_maker_bricks[i].color = 4;
		lvl_maker_bricks[i].isSelected = 0;
		r++;

	}

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{

			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::MouseWheelMoved)
			{


				for (int i = 0; i < 304; i++)
				{
					if (cursor1.getGlobalBounds().intersects(lvl_maker_bricks[i].rectangle.getGlobalBounds()))
					{

						lvl_maker_bricks[i].color += (float)event.mouseWheel.delta;

						if (lvl_maker_bricks[i].color >= 7)
							lvl_maker_bricks[i].color = 0;
						else if (lvl_maker_bricks[i].color < 0)
							lvl_maker_bricks[i].color = 6;
						if (!lvl_maker_bricks[i].isSelected)
						{
							for (int j = 0; j < 304; j++)
								if (!lvl_maker_bricks[j].isSelected)
									lvl_maker_bricks[j].color = lvl_maker_bricks[i].color;
						}

					}
				}
			}

			if (cursor1.getGlobalBounds().intersects(play_button.getGlobalBounds()) && Mouse::isButtonPressed(Mouse::Left))
			{


				for (int i = 0; i < 304; i++)
				{
					if (cursor.getGlobalBounds().intersects(lvl_maker_bricks[i].rectangle.getGlobalBounds()))
					{

						lvl_maker_bricks[i].color += (float)event.mouseWheel.delta / 5;

						if (lvl_maker_bricks[i].color >= 7)
							lvl_maker_bricks[i].color = 0;
						else if (lvl_maker_bricks[i].color < 0)
							lvl_maker_bricks[i].color = 6;
						if (!lvl_maker_bricks[i].isSelected)
						{
							for (int j = 0; j < 304; j++)
								if (!lvl_maker_bricks[j].isSelected)
									lvl_maker_bricks[j].color = lvl_maker_bricks[i].color;
						}

					}
				}
			}


		}



		cursor1.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
		background.circle.rotate(.0025);



		window.clear();
		window.draw(background.circle);
		if (cursor1.getGlobalBounds().intersects(play_button.getGlobalBounds()))
		{
			play_button.setCharacterSize(60);
			play_button.setOrigin(play_button.getGlobalBounds().width / 2, play_button.getGlobalBounds().height * 1.5);
			play_button.setFillColor(Color::Red);

			if (Mouse::isButtonPressed(Mouse::Left))
			{

				for (int i = 0; i < 304; i++)
				{
					if (lvl_maker_bricks[i].isSelected)
					{
						saveing_file << lvl_maker_bricks[i].rectangle.getPosition().x << ' ' << lvl_maker_bricks[i].rectangle.getPosition().y << ' ' << (int)lvl_maker_bricks[i].color << endl;
					}


				}

				level.non_breakables = 0;
				bricks.clear();
				level.file.open("level.txt");
				while (level.file >> level.x && level.file >> level.y && level.file >> level.color)
				{
					bricks.push_back(Bricks());
					bricks.back().rectangle.setSize(Vector2f(60, 30));
					bricks.back().rectangle.setOrigin(30, 15);
					bricks.back().rectangle.setPosition(level.x, level.y);
					bricks.back().powertype = rand() % 40;

					if (bricks.back().powertype > 6)
						bricks.back().powertype = 0;

					if (level.color == 6)
					{
						level.non_breakables++;
						bricks.back().isBreakabel = 0;
						bricks.back().level = 3;
					}
					else
						bricks.back().level = rand() % 3 + 1;
					bricks.back().rectangle.setTexture(&brick_tex[level.color]);
					bricks.back().points = 10 * bricks.back().level;
					bricks.back().powershape.setRadius(15);
					bricks.back().powershape.setOrigin(15, 15);
					bricks.back().powershape.setPosition(bricks.back().rectangle.getPosition());
					bricks.back().powershape.setTexture(&powerUp_tex[bricks.back().powertype]);

				}
				level.file.close();

				play();
				background.music.setVolume(100);
			}


		}
		else
		{
			play_button.setCharacterSize(50);
			play_button.setOrigin(play_button.getGlobalBounds().width / 2, play_button.getGlobalBounds().height * 1.5);
			play_button.setFillColor(Color::Magenta);
		}

		if (cursor1.getGlobalBounds().intersects(back_button.getGlobalBounds()))
		{
			back_button.setCharacterSize(60);
			back_button.setOrigin(back_button.getGlobalBounds().width / 2, back_button.getGlobalBounds().height * 1.5);
			back_button.setFillColor(Color::Red);
			if (Mouse::isButtonPressed(Mouse::Left))
				return;
		}
		else
		{
			back_button.setCharacterSize(50);
			back_button.setOrigin(back_button.getGlobalBounds().width / 2, back_button.getGlobalBounds().height * 1.5);
			back_button.setFillColor(Color::Magenta);
		}


		for (int i = 0; i < 304; i++)
		{


			if (cursor1.getGlobalBounds().intersects(lvl_maker_bricks[i].rectangle.getGlobalBounds()))
			{
				if (Mouse::isButtonPressed(Mouse::Left))
				{
					lvl_maker_bricks[i].rectangle.setFillColor(Color::White);
					lvl_maker_bricks[i].isSelected = 1;
				}
				else if (Mouse::isButtonPressed(Mouse::Right))
				{
					lvl_maker_bricks[i].isSelected = 0;

				}
				if (!lvl_maker_bricks[i].isSelected)
					lvl_maker_bricks[i].rectangle.setFillColor(sf::Color(255, 255, 255, 200));

			}
			else if (!lvl_maker_bricks[i].isSelected)
				lvl_maker_bricks[i].rectangle.setFillColor(sf::Color(255, 255, 255, 128));

			lvl_maker_bricks[i].rectangle.setTextureRect(IntRect(530 * (int)lvl_maker_bricks[i].color, 0, 530, 266));

			window.draw(lvl_maker_bricks[i].rectangle);
		}
		window.draw(play_button);
		window.draw(back_button);
		window.draw(cursor1);
		window.display();
	}


}

void menu() {

	Exit.txt.setPosition(500, 800);

	background.music.play();
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (Keyboard::isKeyPressed(Keyboard::M) || Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(mute.circle.getGlobalBounds()))
			{
				if (mute.isEnabled == 1)
				{
					mute.isEnabled = 0;
					background.music.play();
				}
				else if (mute.isEnabled == 0)
				{
					mute.isEnabled = 1;
					background.music.pause();
				}

			}
			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Start_Game.txt.getGlobalBounds()))
			{
				if (!mute.isEnabled)
				{
					Menu_sound.play();
				}
				paddle.lifeCounter = 4;
				bricks.clear();
				level.lvl = 0;
				balls.clear();
				life_counting();
				paddle.score = 0;
				play();
				background.music.setVolume(100);
				lvlmaker.txt.setPosition(500, 500);
				score.setPosition(1000, 747);
				amr.setPosition(500, 150);
				mo5tar.setPosition(500, 450);
				marwan.setPosition(500, 250);
				mina.setPosition(500, 350);
				nourhan.setPosition(500, 550);
				joe.setPosition(500, 650);
				credits_txt1.setPosition(410, 50);
				Credits.txt.setPosition(500, 650);
				nothighscore.setPosition(500, 500);
			}
			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Credits.txt.getGlobalBounds()))
			{
				Menu_sound.play();
				credits();
			}
			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(lvlmaker.txt.getGlobalBounds()))
			{
				Menu_sound.play();
				Mouse::setPosition(Vector2i(500, 700), window);
				level_maker();
				background.music.setVolume(100);
			}
			if (Mouse::isButtonPressed(Mouse::Left) && cursor.getGlobalBounds().intersects(Exit.txt.getGlobalBounds()))
			{
				Menu_sound.play();
				Menu_sound.play();
				window.close();
			}
			if (Mouse::isButtonPressed(Mouse::Right))
			{
				cout << Mouse::getPosition(window).x << ' ' << Mouse::getPosition(window).y << endl;
			}

		}

		if (cursor.getGlobalBounds().intersects(Start_Game.txt.getGlobalBounds()))
		{
			Start_Game.txt.setCharacterSize(90);
			Start_Game.txt.setOrigin((Start_Game.txt.getGlobalBounds().width / 2), (Start_Game.txt.getGlobalBounds().height * 1.5));
			Start_Game.txt.setFillColor(Color::Red);
		}
		else
		{
			Start_Game.txt.setCharacterSize(80);
			Start_Game.txt.setOrigin((Start_Game.txt.getGlobalBounds().width / 2), (Start_Game.txt.getGlobalBounds().height * 1.5));
			Start_Game.txt.setFillColor(Color::Magenta);
		}
		if (cursor.getGlobalBounds().intersects(Exit.txt.getGlobalBounds()))
		{
			Exit.txt.setCharacterSize(90);
			Exit.txt.setOrigin((Exit.txt.getGlobalBounds().width / 2), (Exit.txt.getGlobalBounds().height * 1.5));
			Exit.txt.setFillColor(Color::Red);
		}
		else
		{
			Exit.txt.setCharacterSize(80);
			Exit.txt.setOrigin((Exit.txt.getGlobalBounds().width / 2), (Exit.txt.getGlobalBounds().height * 1.5));
			Exit.txt.setFillColor(Color::Magenta);
		}
		if (cursor.getGlobalBounds().intersects(Credits.txt.getGlobalBounds()))
		{
			Credits.txt.setCharacterSize(90);
			Credits.txt.setOrigin((Credits.txt.getGlobalBounds().width / 2), (Credits.txt.getGlobalBounds().height * 1.5));
			Credits.txt.setFillColor(Color::Red);
		}
		else
		{
			Credits.txt.setCharacterSize(80);
			Credits.txt.setOrigin((Credits.txt.getGlobalBounds().width / 2), (Credits.txt.getGlobalBounds().height * 1.5));
			Credits.txt.setFillColor(Color::Magenta);
		}
		if (cursor.getGlobalBounds().intersects(lvlmaker.txt.getGlobalBounds()))
		{
			lvlmaker.txt.setCharacterSize(90);
			lvlmaker.txt.setOrigin((lvlmaker.txt.getGlobalBounds().width / 2), (lvlmaker.txt.getGlobalBounds().height * 1.5));
			lvlmaker.txt.setFillColor(Color::Red);
		}
		else
		{
			lvlmaker.txt.setCharacterSize(80);
			lvlmaker.txt.setOrigin((lvlmaker.txt.getGlobalBounds().width / 2), (lvlmaker.txt.getGlobalBounds().height * 1.5));
			lvlmaker.txt.setFillColor(Color::Magenta);
		}


		cursor.setPosition(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
		background.circle.rotate(.0025);


		if (mute.isEnabled)
		{
			mute.circle.setTextureRect(IntRect(0, 0, 191, 192));
		}
		else {
			mute.circle.setTextureRect(IntRect(192, 0, 192, 192));
		}

		window.clear();
		window.draw(background.circle);
		window.draw(mute.circle);
		window.draw(Start_Game.txt);
		window.draw(Exit.txt);
		window.draw(lvlmaker.txt);
		window.draw(Credits.txt);
		window.draw(bricksy);
		window.draw(cursor);
		window.display();
	}
}

void fonts_stuff() {

	FredokaOne.loadFromFile("Fonts/FredokaOne.ttf");
	Revamped.loadFromFile("Fonts/Revamped.otf");

	Start_Game.txt.setFont(FredokaOne);
	Exit.txt.setFont(FredokaOne);
	Resume_game.txt.setFont(FredokaOne);
	Return_to_main_menu.txt.setFont(FredokaOne);
	Retry.txt.setFont(FredokaOne);

	Start_Game.txt.setString("START");
	Exit.txt.setString("EXIT");
	Resume_game.txt.setString("RESUME");
	Return_to_main_menu.txt.setString("RETURN TO MAIN MENU");
	Retry.txt.setString("RETRY");

	Start_Game.txt.setOutlineThickness(5);
	Start_Game.txt.setOutlineColor(Color::Yellow);
	Exit.txt.setOutlineThickness(5);
	Exit.txt.setOutlineColor(Color::Yellow);
	Resume_game.txt.setOutlineThickness(5);
	Resume_game.txt.setOutlineColor(Color::Yellow);
	Return_to_main_menu.txt.setOutlineThickness(5);
	Return_to_main_menu.txt.setOutlineColor(Color::Yellow);
	Retry.txt.setOutlineThickness(5);
	Retry.txt.setOutlineColor(Color::Yellow);


	Exit.txt.setStyle(Start_Game.txt.getStyle());
	Resume_game.txt.setStyle(Start_Game.txt.getStyle());
	Return_to_main_menu.txt.setStyle(Start_Game.txt.getStyle());
	Retry.txt.setStyle(Start_Game.txt.getStyle());

	Start_Game.txt.setCharacterSize(80);
	Exit.txt.setCharacterSize(80);
	Resume_game.txt.setCharacterSize(80);
	Return_to_main_menu.txt.setCharacterSize(80);
	Retry.txt.setCharacterSize(80);

	Start_Game.txt.setOrigin((Start_Game.txt.getGlobalBounds().width / 2), (Start_Game.txt.getGlobalBounds().height * 1.5));
	Exit.txt.setOrigin((Exit.txt.getGlobalBounds().width / 2), (Exit.txt.getGlobalBounds().height * 1.5));
	Resume_game.txt.setOrigin((Resume_game.txt.getGlobalBounds().width / 2), (Resume_game.txt.getGlobalBounds().height * 1.5));
	Return_to_main_menu.txt.setOrigin((Return_to_main_menu.txt.getGlobalBounds().width / 2), (Return_to_main_menu.txt.getGlobalBounds().height * 1.5));
	Retry.txt.setOrigin((Retry.txt.getGlobalBounds().width / 2), (Retry.txt.getGlobalBounds().height * 1.5));

	Start_Game.txt.setPosition(500, 350);
	Retry.txt.setPosition(500, 300);
	Resume_game.txt.setPosition(500, 300);


	GameOver.setFont(Revamped);
	GameOver.setString("GAME OVER !");
	GameOver.setCharacterSize(100);
	GameOver.setOutlineThickness(5);
	GameOver.setOutlineColor(Color::Yellow);
	GameOver.setPosition(500, 100);
	GameOver.setFillColor(Color::Red);
	GameOver.setOrigin(GameOver.getGlobalBounds().width / 2, GameOver.getGlobalBounds().height / 2);

	Credits.txt.setFont(FredokaOne);
	Credits.txt.setString("CREDITS");
	Credits.txt.setCharacterSize(80);
	Credits.txt.setPosition(500, 650);
	Credits.txt.setFillColor(Color::Red);
	Credits.txt.setOrigin(Credits.txt.getGlobalBounds().width / 2, Credits.txt.getGlobalBounds().height / 2);
	Credits.txt.setOutlineThickness(5);
	Credits.txt.setOutlineColor(Color::Yellow);

	lvlmaker.txt.setFont(FredokaOne);
	lvlmaker.txt.setString("LEVEL MAKER");
	lvlmaker.txt.setCharacterSize(80);
	lvlmaker.txt.setPosition(500, 500);
	lvlmaker.txt.setFillColor(Color::Red);
	lvlmaker.txt.setOrigin(Credits.txt.getGlobalBounds().width / 2, Credits.txt.getGlobalBounds().height / 2);
	lvlmaker.txt.setOutlineThickness(5);
	lvlmaker.txt.setOutlineColor(Color::Yellow);

	play_button.setFont(FredokaOne);
	play_button.setCharacterSize(50);
	play_button.setString("PLAY");
	play_button.setPosition(900, 750);
	play_button.setOrigin(play_button.getGlobalBounds().width / 2, play_button.getGlobalBounds().height);
	play_button.setOutlineColor(Color::Yellow);
	play_button.setOutlineThickness(5);
	back_button.setOutlineThickness(5);
	back_button.setOutlineColor(Color::Yellow);
	back_button.setFont(FredokaOne);
	back_button.setCharacterSize(50);
	back_button.setString("BACK");
	back_button.setPosition(100, 750);
	back_button.setOrigin(back_button.getGlobalBounds().width / 2, back_button.getGlobalBounds().height);


	back.txt.setFont(FredokaOne);
	back.txt.setString("Back");
	back.txt.setCharacterSize(80);
	back.txt.setPosition(150, 780);
	back.txt.setFillColor(Color::Red);
	back.txt.setOrigin(Credits.txt.getGlobalBounds().width / 2, Credits.txt.getGlobalBounds().height / 2);
	back.txt.setOutlineThickness(5);
	back.txt.setOutlineColor(Color::Yellow);

	scorelable.setFont(FredokaOne);
	scorelable.setString("score");
	scorelable.setCharacterSize(50);
	scorelable.setPosition(0, 780);
	scorelable.setFillColor(Color::Red);

	score.setFont(FredokaOne);
	score.setCharacterSize(50);
	score.setPosition(1000, 747);
	score.setFillColor(Color::White);

	highscore_txt.setString("NEW HIGH SCORE !!!");
	highscore_txt.setFont(FredokaOne);
	highscore_txt.setPosition(500, 500);
	highscore_txt.setCharacterSize(70);
	highscore_txt.setFillColor(Color::Green);
	highscore_txt.setOrigin((highscore_txt.getGlobalBounds().width / 2), (highscore_txt.getGlobalBounds().height * 1.5));

	nothighscore.setString("YOUR SCORE IS");
	nothighscore.setFont(FredokaOne);
	nothighscore.setPosition(500, 500);
	nothighscore.setCharacterSize(70);
	nothighscore.setFillColor(Color::Green);
	nothighscore.setOrigin((nothighscore.getGlobalBounds().width / 2), (nothighscore.getGlobalBounds().height * 1.5));

	level.txt.setFont(FredokaOne);
	level.txt.setFillColor(Color::White);
	level.txt.setCharacterSize(20);



	marwan.setString("Marwan Ezzat");
	marwan.setFont(FredokaOne);
	marwan.setPosition(500, 250);
	marwan.setCharacterSize(50);
	marwan.setFillColor(Color::Black);
	marwan.setOrigin((marwan.getGlobalBounds().width / 2), 0);
	marwan.setOutlineThickness(3);
	marwan.setOutlineColor(Color::Yellow);

	mina.setString("Mina Philips");
	mina.setFont(FredokaOne);
	mina.setPosition(500, 350);
	mina.setCharacterSize(50);
	mina.setFillColor(Color::Black);
	mina.setOrigin((mina.getGlobalBounds().width / 2), 0);
	mina.setOutlineThickness(3);
	mina.setOutlineColor(Color::Yellow);

	joe.setString("Yousef Mohamed");
	joe.setFont(FredokaOne);
	joe.setPosition(500, 650);
	joe.setCharacterSize(50);
	joe.setFillColor(Color::Black);
	joe.setOrigin((joe.getGlobalBounds().width / 2), 0);
	joe.setOutlineThickness(3);
	joe.setOutlineColor(Color::Yellow);

	mo5tar.setString("Mohamed Mokhtar");
	mo5tar.setFont(FredokaOne);
	mo5tar.setPosition(500, 450);
	mo5tar.setCharacterSize(50);
	mo5tar.setFillColor(Color::Black);
	mo5tar.setOrigin((mo5tar.getGlobalBounds().width / 2), 0);
	mo5tar.setOutlineThickness(3);
	mo5tar.setOutlineColor(Color::Yellow);

	amr.setString("Amr Akram");
	amr.setFont(FredokaOne);
	amr.setPosition(500, 150);
	amr.setCharacterSize(50);
	amr.setFillColor(Color::Black);
	amr.setOrigin((amr.getGlobalBounds().width / 2), 0);
	amr.setOutlineThickness(3);
	amr.setOutlineColor(Color::Yellow);

	nourhan.setString("Nourhan Ayoub");
	nourhan.setFont(FredokaOne);
	nourhan.setPosition(500, 550);
	nourhan.setCharacterSize(50);
	nourhan.setFillColor(Color::Black);
	nourhan.setOrigin((nourhan.getGlobalBounds().width / 2), 0);
	nourhan.setOutlineThickness(3);
	nourhan.setOutlineColor(Color::Yellow);

	credits_txt1.setString("Credits");
	credits_txt1.setFont(FredokaOne);
	credits_txt1.setPosition(410, 50);
	credits_txt1.setCharacterSize(50);
	credits_txt1.setFillColor(Color::Red);
	credits_txt1.setOrigin((credits_txt.getGlobalBounds().width / 2), 0);
	credits_txt1.setOutlineThickness(3);
	credits_txt1.setOutlineColor(Color::Yellow);



}

int main()
{

	//some settings
	window.setMouseCursorVisible(0);
	window.setFramerateLimit(500);
	srand(time(0));

	mute.circle.setPosition(940, 20);
	mute.circle.setRadius(20);



	//menu sound
	SoundBuffer buffer_for_menu;
	SoundBuffer buffer_for_paddle_ball;
	SoundBuffer losing_life_buffer;
	SoundBuffer powergain_buff;
	SoundBuffer shooter_buff;
	SoundBuffer unbreakable_buff;
	SoundBuffer brick_buff;


	unbreakable_buff.loadFromFile("Sounds/unbreakable.wav");
	brick_buff.loadFromFile("Sounds/bricksball.wav");

	buffer_for_paddle_ball.loadFromFile("Sounds/paddle&ball.wav");
	losing_life_buffer.loadFromFile("Sounds/losing life.wav");
	buffer_for_menu.loadFromFile("Sounds/menu.wav");
	powergain_buff.loadFromFile("Sounds/power gained.wav");
	shooter_buff.loadFromFile("Sounds/shooter.wav");
	background.buffer.loadFromFile("Sounds/background.wav");

	powerGainedSound.setBuffer(powergain_buff);
	Menu_sound.setBuffer(buffer_for_menu);
	paddle_ball.setBuffer(buffer_for_paddle_ball);
	lose_life.setBuffer(losing_life_buffer);
	shooting_sound.setBuffer(shooter_buff);
	background.music.setBuffer(background.buffer);
	brick_ball.setBuffer(brick_buff);
	unbreakable_ball.setBuffer(unbreakable_buff);


	background.music.setLoop(true);

	//textures
	ball_tex.loadFromFile("Textures/ball.png");
	paddle_tex.loadFromFile("Textures/paddles.png");
	brick_tex[0].loadFromFile("Textures/brick (1).png");
	brick_tex[1].loadFromFile("Textures/brick (2).png");
	brick_tex[2].loadFromFile("Textures/brick (3).png");
	brick_tex[3].loadFromFile("Textures/brick (4).png");
	brick_tex[4].loadFromFile("Textures/brick (5).png");
	brick_tex[5].loadFromFile("Textures/brick (6).png");
	brick_tex[6].loadFromFile("Textures/brick (7).png");


	powerUp_tex[1].loadFromFile("Textures/power-up (1).png");
	powerUp_tex[2].loadFromFile("Textures/power-up (2).png");
	powerUp_tex[3].loadFromFile("Textures/power-up (3).png");
	powerUp_tex[4].loadFromFile("Textures/power-up (4).png");
	powerUp_tex[5].loadFromFile("Textures/power-up (5).png");
	powerUp_tex[6].loadFromFile("Textures/power-up (6).png");
	mute.texture.loadFromFile("Textures/sound.png");

	mute.circle.setTexture(&mute.texture);

	Texture level_maker_colors;
	level_maker_colors.loadFromFile("Textures/bricks.png");

	bullet_tex.loadFromFile("Textures/bullet.png");
	cursorshape.loadFromFile("Textures/cursor.png");
	logo.loadFromFile("Textures/bricksy.png");
	background.texture.loadFromFile("Textures/background.png");

	fonts_stuff();

	//background
	background.circle.setRadius(1300);
	background.circle.setOrigin(1300, 1300);
	background.circle.setPosition(1000, 800);

	background.circle.setTexture(&background.texture);



	//paddle stuff
	paddle.rectangle.setSize(Vector2f(150, 30));
	paddle.rectangle.setOrigin(paddle.rectangle.getSize().x / 2, paddle.rectangle.getSize().y / 2);
	paddle.rectangle.setTexture(&paddle_tex);

	//power up stuff
	for (int i = 0; i < 4; i++)
	{
		power[i].circle.setRadius(12);
		power[i].circle.setOrigin(12, 12);
		power[i].circle.setTexture(&powerUp_tex[i + 3]);
	}


	//life stuff
	for (int i = 0; i < 4; i++)
	{
		paddle.lifeShape[i].setRadius(20);
		paddle.lifeShape[i].setOrigin(20, 20);
		paddle.lifeShape[i].setPosition(1000 - (i * 42 + 20), 700 + 15 + 20 + 5);
		paddle.lifeShape[i].setTexture(&powerUp_tex[2]);
	}




	//cursor stuff
	cursor.setTexture(cursorshape);
	cursor.setScale(.1, .1);
	cursor.setOrigin(0, 0);
	logo.setSmooth(true);

	bricksy.setTexture(logo);
	bricksy.setOrigin(bricksy.getGlobalBounds().width / 2, bricksy.getGlobalBounds().height / 2);
	bricksy.setPosition(500, 130);
	bricksy.setScale(.14, .14);

	level.name[0] = "Levels/Joe4.txt"; //1
	level.name[1] = "Levels/Joe3.txt";//2
	level.name[2] = "Levels/marwan2.txt"; //3
	level.name[3] = "Levels/marwan1.txt"; //4
	level.name[4] = "Levels/Joe2.txt"; //5
	level.name[5] = "Levels/Joe1.txt"; //6
	level.name[6] = "Levels/marwan1.txt"; //7





	menu();

	return 0;
}