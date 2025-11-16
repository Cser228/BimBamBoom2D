#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
//#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <windows.h>
#include <fstream>
#include <math.h>
#include <vector>

#define GRAVITY_CONST 0
#define GRAVITY_DISC 0.0001

#define BULLET_SPEED 0.5

#define PLAYER_HEIGHT 80
#define PLAYER_WIDTH 80

using namespace std;
using namespace sf;

class Bullet {
private:
    float x;
    float y;
    float angle;
    bool active;
public:
    Bullet() {
        x = -1000;
        y = -1000;
        angle = 0;
        active = false;
    }
    Bullet(int x, int y) {
        this->x = x;
        this->y = y;
        angle = 0;
        active = false;
    }
    Bullet(int x, int y, float angle) {
        this->x = x;
        this->y = y;
        this->angle = angle;
    }

    float getX() {
        return x;
    }

    float getY() {
        return y;
    }

    float getAngle() {
        return angle;
    }

    bool getActive() {
        return active;
    }

    void setX(float x) {
        this->x = x;
    }

    void setY(float y) {
        this->y = y;
    }

    void setAngle(float angle) {
        this->angle = angle;
    }

    void setActive(bool active) {
        this->active = active;
    }

    void move() {
        x += BULLET_SPEED * cos(angle);
        y += BULLET_SPEED * sin(angle);
    }
};

int main() {
    //Player
    Texture texture;
    if (!texture.loadFromFile("images/player.png")) {
        return -2;
    }

    Texture gunTexture;
    if (!gunTexture.loadFromFile("images/gun.png")) {
        return -2;
    }

    Sprite player(texture);
    player.setScale(Vector2f(5, 5));
    player.setPosition(Vector2f(520, 400));

    Sprite gun(gunTexture);
    gun.setScale(Vector2f(1.5, 1.5));
    gun.setPosition(player.getPosition());
    //End

    Vector2f secondPlayerPos(720, 400);

    //MAP
    RectangleShape map[150];
    ifstream file("map.txt");
    short yMap = 0;
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            for (size_t i = 0; i < 15; i++) {
                if (line[i] == '#') {
                    RectangleShape square(Vector2f(80, 80));
                    square.setPosition(Vector2f(i * 80, yMap * 80));
                    square.setFillColor(Color(0, 153, 51));
                    map[i + 15 * yMap] = square;
                }
                else if (line[i] == '_') {
                    RectangleShape square(Vector2f(1, 1));
                    square.setPosition(Vector2f(-100, -100));
                    square.setFillColor(Color(0, 153, 51));
                    map[i + 15 * yMap] = square;
                }
                else if (line[i] == 'r') {
                    player.setPosition(Vector2f(i * 80, yMap * 80));
                }
                else if (line[i] == 'b') {
                    secondPlayerPos = Vector2f(i * 80, yMap * 80);
                }
            }

            yMap++;
        }
        file.close();
    }
    //END

    //Varuables
    bool playerOnGround = false;
    float gravity = -GRAVITY_CONST;
    bool facing_right = true;
    Bullet bullets[50];
    unsigned short bullets_count = 0;
    //End

    //TCP Varuables
    
    //End

    sf::RenderWindow window(sf::VideoMode({ 1200, 800 }), "BimBamBoom2D");

    //Explosions and Shaders
    Shader explosionShader;
    if (!explosionShader.loadFromFile("exp.frag", Shader::Type::Fragment)) {
        return -2;
    }
    explosionShader.setUniform("screen_size", Vector2f(1200, 800));
    CircleShape explosion;
    explosion.setPosition(Vector2f(-1000, -1000));
    explosion.setRadius(50);
    //End

    Texture bulletTexture;
    if (!bulletTexture.loadFromFile("images/bullet.png")) {
        return -2;
    }

    while (window.isOpen()) {
        //EVENTS
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto& key = event->getIf<Event::MouseButtonPressed>()) {
                if (key->button == Mouse::Button::Left) {
                    if (bullets_count < 50) {
                        Vector2i mousePositio = Mouse::getPosition(window);
                        Vector2f mousePosition(mousePositio.x - 16, mousePositio.y - 16);
                        bullets[bullets_count].setX(gun.getPosition().x);
                        bullets[bullets_count].setY(gun.getPosition().y);
                        bullets[bullets_count].setAngle(atan2(mousePosition.y - gun.getPosition().y, mousePosition.x - gun.getPosition().x));
                        bullets[bullets_count].setActive(true);
                        bullets_count++;
                    }
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left)) {
            facing_right = false;
            bool canImove = true;
            for (size_t i = 0; i < 150; i++) {
                FloatRect groundBound(map[i].getPosition(), map[i].getSize());

                if (groundBound.contains(Vector2f(player.getPosition().x - 0.1, player.getPosition().y + PLAYER_HEIGHT - 2))) {
                    canImove = false;
                    break;
                }
            }
            if (canImove) {
                player.move(Vector2f(-0.1, 0));
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right)) {
            facing_right = true;
            bool canImove = true;
            for (size_t i = 0; i < 150; i++) {
                FloatRect groundBound(map[i].getPosition(), map[i].getSize());

                if (groundBound.contains(Vector2f(player.getPosition().x + PLAYER_WIDTH + 0.1, player.getPosition().y + PLAYER_HEIGHT - 2))) {
                    canImove = false;
                    break;
                }
            }
            if (canImove) {
                player.move(Vector2f(0.1, 0));
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Space) || Keyboard::isKeyPressed(Keyboard::Key::Up) || Keyboard::isKeyPressed(Keyboard::Key::W)) {
            if (playerOnGround) {
                gravity = -0.15;
            }
        }

        //END

        //GRAVITY
        player.setPosition(Vector2f(player.getPosition().x, player.getPosition().y + gravity));
        if (facing_right) {
            gun.setPosition(Vector2f(player.getPosition().x + PLAYER_WIDTH - 10, player.getPosition().y + (PLAYER_HEIGHT / 2)));
        }
        else {
            gun.setPosition(Vector2f(player.getPosition().x - 10, player.getPosition().y + (PLAYER_HEIGHT / 2)));
        }
        gravity += GRAVITY_DISC;

        bool vremeno_onGround = false;
        for (size_t i = 0; i < 150; i++) {
            RectangleShape ground = map[i];
            if (player.getPosition().y + PLAYER_HEIGHT >= ground.getPosition().y && (ground.getPosition().x <= player.getPosition().x + PLAYER_WIDTH && player.getPosition().x <= ground.getPosition().x + ground.getSize().x) && ground.getPosition().y >= player.getPosition().y) {
                vremeno_onGround = true;
                if (gravity >= 0) {
                    gravity = GRAVITY_CONST;
                }
                break;
            }
        }
        playerOnGround = vremeno_onGround;
        //END

        //Bullets
        if (bullets_count == 50) {
            bullets_count = 0;
        }

        for (int i = 0; i < 50; i++) {
            if (bullets[i].getActive() == true) {
                bullets[i].move();
            }

            if (bullets[i].getX() + 32 <= 0 || bullets[i].getX() >= 1200 || bullets[i].getY() + 32 <= 0 || bullets[i].getY() >= 800) {
                bullets[i].setActive(false);
            }

            for (size_t g = 0; g < 150; g++) {
                FloatRect groundBound = map[g].getGlobalBounds();
                FloatRect bulletBounds(Vector2f(bullets[i].getX(), bullets[i].getY()), Vector2f(32, 32));

                if (groundBound.findIntersection(bulletBounds) && bullets[i].getX() >= 0 && bullets[i].getY() >= 0) {
                    explosion.setPosition(Vector2f(bullets[i].getX() - 50, bullets[i].getY() - 50));
                    explosionShader.setUniform("center", Vector2f(explosion.getPosition().x + 50, explosion.getPosition().y + 50));

                    bullets[i].setActive(false);
                    bullets[i].setX(-1000);
                    bullets[i].setY(-1000);
                }
            }
        }
        //End

        window.clear(sf::Color(153, 204, 255));
        //draw map
        for (size_t i = 0; i < 150; i++) {
            window.draw(map[i]);
        }
        //end
        //bullets
        for (size_t i = 0; i < 50; i++) {
            if (bullets[i].getActive() == true) {
                Sprite bullet(bulletTexture);
                bullet.setPosition(Vector2f(bullets[i].getX(), bullets[i].getY()));
                bullet.setScale(Vector2f(2, 2));

                window.draw(bullet);
            }
        }
        //end
        //explosions
        window.draw(explosion, &explosionShader);
        //end
        window.draw(player);
        window.draw(gun);
        window.display();
    }

    return 0;
}