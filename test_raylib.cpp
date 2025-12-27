#include <iostream>
#include <raylib.h>
#include <raymath.h>

using namespace std;

Color green = {17, 204, 96, 255};
Color darkGreen = {4, 51, 24, 255};

// when you multiply cellsize * cell count = 750 pixels.
int cellSize = 30;
int cellCount = 25;
int offSet = 75;

double lastUpdateTIme = 0;

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if  (currentTime - lastUpdateTIme >= interval)
    {
        lastUpdateTIme = currentTime;
        return true;
    }
    return false;
};

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i=0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }

    return false;
};

class Snake 
{
    public:
        deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        Vector2 direction = {1,0};
        bool addSegment = false;

    void Draw(){
        for(unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            // Rectange
            Rectangle segment = Rectangle{
               offSet + x * cellSize, offSet+ y*cellSize, (float)cellSize, (float)cellSize
            };
            DrawRectangleRounded(segment, 0.7, 0.7, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0] , direction));

        if (addSegment == true){
            addSegment = false;
        }
        else{
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {6,9};
    }
};

class Food {

public:
    Vector2 position = {5,6};
    Texture2D texture;

    // constructor
    Food(deque<Vector2> snakeBody){
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPosition(snakeBody);
    }

    // destructor
    ~Food()
    {
        UnloadTexture(texture);
    }
    

    void Draw()
    {
        DrawTexture(texture, offSet + position.x * cellSize, offSet + position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0, cellCount -1);
        float y = GetRandomValue(0, cellCount -1);

        return Vector2{x,y};
    }

    Vector2 GenerateRandomPosition(deque<Vector2> snakeBody)
    {
        
        Vector2 position = GenerateRandomCell();
        while(ElementInDeque(position, snakeBody)){
            position = GenerateRandomCell();
        }
        return position;
    }
};


class Game
{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        int score = 0;
        Sound eatSound;
        Sound wallSound;

        Game()
        {
            InitAudioDevice();
            eatSound = LoadSound("Sounds/eat.mp3");
            wallSound = LoadSound("Sounds/wall.mp3");
        }

        ~Game()
        {
            UnloadSound(eatSound);
            UnloadSound(wallSound);
            CloseAudioDevice();
        }

        void Draw()
        {
            food.Draw();
            snake.Draw();
        }

        void Update()
        {   
            if (running)
            {
            snake.Update();
            CheckCollistionWithFood();
            CheckCollisionWithEdge();
            CheckCollistionWithTail();
            }
        }

        void CheckCollistionWithFood(){
            if (Vector2Equals(snake.body[0], food.position))
            {
                food.position = food.GenerateRandomPosition(snake.body);
                snake.addSegment = true;
                score ++;
                PlaySound(eatSound);
            }
        }

        void CheckCollisionWithEdge()
        {
            if(snake.body[0].x == cellCount || snake.body[0].x == -1)
            {
                GameOver();
            }
            if(snake.body[0].y == cellCount || snake.body[0].y == -1)
            {
                GameOver();
            }
        }

        void CheckCollistionWithTail(){
            deque<Vector2> headlessBody = snake.body;
            headlessBody.pop_front();
            if(ElementInDeque(snake.body[0], headlessBody))
            {
                GameOver();
            }
        }

        void GameOver()
        {   
            running = false;
            snake.Reset();
            score = 0;
            food.position  = food.GenerateRandomPosition(snake.body);
            PlaySound(wallSound);
        }
};
int main () {

    cout << "Starting the game ..." << endl;
    // start window
    InitWindow(2 * offSet + cellSize * cellCount, 2 * offSet + cellSize * cellCount, "Nokia Snake");
    SetTargetFPS(60);

    Game game = Game();

    while (WindowShouldClose() == false){

        BeginDrawing();

        if (eventTriggered(0.2)){
            game.Update();
        }

        // keyboard shortcuts
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y !=1){
            game.snake.direction = {0, -1};
            game.running = true;
        }

        // keyboard shortcuts
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0, 1};
            game.running = true;
        }

        // keyboard shortcuts
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x!=1){
            game.snake.direction = {-1, 0};
            game.running = true;
        }

        // keyboard shortcuts
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x!=-1){
            game.snake.direction = {1, 0};
            game.running = true;
        }

        

        // Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offSet-5, (float)offSet-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, darkGreen);
        DrawText("Retro Snake", offSet - 5, 20,40, darkGreen);
        DrawText(TextFormat("%i", game.score), offSet - 5, offSet + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }

    // close window
    CloseWindow();
}