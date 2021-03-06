#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include <algorithm>
#include <string>

#define sizeX 100
#define sizeY 50
#define timePause 1000 // ms

using namespace std;
using namespace sf;

int field[sizeY][sizeX];
int pastField[sizeY][sizeX];

int zeros(int f[sizeY][sizeX])
{
    for(int i = 0; i < sizeY; i++)
        for(int j = 0; j < sizeX; j++)
            f[i][j] = 0;
}

int getLiveCount(int field[sizeY][sizeX])
{
    int count = 0;
    for (int i = 0; i < sizeY; i++)
        for (int j = 0; j < sizeX; j++)
            count += field[i][j];
    return count;
}

void coordinatsNeighbors(int nb[8][2], int x, int y)
{
    int k = 0;
    for (int i = x - 1; i <= x + 1; i++)
        for (int j = y - 1; j <= y + 1; j++)
            if (i != x || j != y) {
                nb[k][0] = i;
                nb[k][1] = j;
                k++;
            }
}

int countNeighbors(int field[sizeY][sizeX],int x, int y)
{
    int count = 0;
    int nb[8][2];

    coordinatsNeighbors(nb, x, y);

    for(int i = 0; i < 8; i++)
        {if (nb[i][0] >= 0 && nb[i][1] >= 0 && nb[i][0] < sizeX && nb[i][0] < sizeY)
            count += field[nb[i][0]][nb[i][1]];}
    return count;
}

void newGen(int newField[sizeY][sizeX], int oldField[sizeY][sizeX]){
    for (int i = 0; i < sizeY; i++)
        for (int j = 0; j < sizeX; j++){
            int nb = countNeighbors(oldField, i, j);

            if (oldField[i][j] == 0){
                if (nb == 3)
                    newField[i][j] = 1;
            }else
                if (nb < 2 || nb > 3)
                    newField[i][j] = 0;
        }
}

void copyField(int field[sizeY][sizeX], int cpField[sizeY][sizeX])
{
    for(int i = 0; i < sizeY; i++)
        for(int j = 0; j < sizeX; j++)
            cpField[i][j] = field[i][j];
}

void beginField(int field[sizeY][sizeX], vector <pair<int, int>> *beginNum)
{
    for(int i = 0; i < (*beginNum).size(); i++)
        field[(*beginNum)[i].second][(*beginNum)[i].first] = 1;
}

bool compField(int field1[sizeY][sizeX], int field2[sizeY][sizeX])
{
    for (int i = 0; i < sizeY; i++)
        for (int j = 0; j < sizeX; j++)
            if (field1[i][j] != field2[i][j])
                return 0;
    return 1;
}

void fieldToVector(int field[sizeY][sizeX], vector <pair<int, int>>* v)
{
    for (int i = 0; i < sizeY; i++)
        for (int j = 0; j < sizeX; j++)
            if (field[i][j] == 1)
                (*v).push_back({j, i});
}

int main()
{
    Font font;
    font.loadFromFile("CyrilicOld.ttf");
    vector <pair<int, int>> beginNum;
    vector <pair<int, int>> posCellField;
    vector <int> populat;
    zeros(field);
    int generation = 0;

    cout << "Left mouse button-create life in the cell.\nRight mouse button-kill the life in the cage.\n";
    cout << "Specify the starting positions of the live cells, and press Enter to start the process.\n";
    cout << "During the game, you can pause it by clicking on the Space button.\n";
    cout << "To continue the game, click Enter.\n";
    cout << "For exit press Escape.\n";
    cout << "\n\nSize field: " << sizeX << " x " << sizeY << '\n';
    Sleep(1000);
    RenderWindow window(VideoMode(sizeX * 10, sizeY * 10), "Life!");

    //////////////// INPUT ///////////////////////////////////////////////////////////////////////////////////

    while (window.isOpen())
    {
        Texture t;
        t.loadFromFile("image/cell.png");
        Sprite s(t);

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();
        }

        window.clear(Color::White);
        if (sf :: Mouse :: isButtonPressed ( sf :: Mouse :: Left ))
            {
                Vector2i pos = Mouse::getPosition(window);
                Vector2f mousePos = window.mapPixelToCoords(pos);
                pair<int, int> posCell((int(mousePos.x) - (int(mousePos.x) % 10))/10, (int(mousePos.y) - (int(mousePos.y) % 10))/10);
                if(find(beginNum.begin(), beginNum.end(), posCell) == beginNum.end())
                    beginNum.push_back(posCell);
                //Sleep(500);
            }
        if (sf :: Mouse :: isButtonPressed ( sf :: Mouse :: Right ))
            {
                Vector2i pos = Mouse::getPosition(window);
                Vector2f mousePos = window.mapPixelToCoords(pos);
                pair<int, int> posCell((int(mousePos.x) - (int(mousePos.x) % 10))/10, (int(mousePos.y) - (int(mousePos.y) % 10))/10);
                auto poss = find(beginNum.begin(), beginNum.end(), posCell);
                if(poss != beginNum.end())
                    beginNum.erase(poss);
                    //beginNum.push_back(posCell);
                //Sleep(500);
            }
        for(int i = 0; i < beginNum.size(); i++)
        {
            s.setPosition(beginNum[i].first * 10, 10 * beginNum[i].second);
            window.draw(s);
        }

        Text text("", font, 20);
        text.setColor(Color::Black);
        text.setPosition(0, 0);
        text.setString("Generation: 0");
        window.draw(text);

        window.display();

        if (Keyboard::isKeyPressed(Keyboard::Enter))
            break;


    }

/////////////////////////////////////////////////////////////////////////////////////////////////////

    beginField(field, &beginNum);
    bool f = 1;

    cout << "Initial number of cells: " << beginNum.size() << '\n';

    int time = 0;
    int dt = 10;

    while (window.isOpen())
    {
        Texture t;
        t.loadFromFile("image/cell.png");
        Sprite s(t);

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();
        }
        window.clear(Color::White);

        for(int i = 0; i < sizeY; i++)
        {
            for(int j = 0; j < sizeX; j++)
                if(field[i][j] == 1)
                {
                    s.setPosition(j * 10, i * 10);
                    window.draw(s);
                }
        }
        if(time > timePause)
        {
            copyField(field, pastField);
            newGen(field, pastField);
            if(compField(field, pastField))
                break;
            generation++;
            time = 0;
        }

        Text textGen("", font, 20);
        Text textPause("PAUSE", font, 50);
        Text textPop("", font, 20);
        textGen.setColor(Color::Black);
        textPause.setColor(Color::Red);
        textPop.setColor(Color::Black);
        string strGen = "Generation: ";
        string strPop = "Population: ";
        string num = to_string(generation);
        string countCell = to_string(getLiveCount(field));
        populat.push_back(getLiveCount(field));
        textGen.setString(strGen + num);
        textPop.setString(strPop + countCell);
        textGen.setPosition(0, 0);
        textPop.setPosition(0, 20);
        window.draw(textGen);
        window.draw(textPop);



        posCellField = {};
        fieldToVector(field, &posCellField);
        if (Keyboard::isKeyPressed(Keyboard::Space))
            f = 0;
        if (Keyboard::isKeyPressed(Keyboard::Enter))
            f = 1;

        if (f)  time+= dt;

        if (!f)
        {
            textPause.setPosition(sizeX * 10 / 2 - 100, 0);
            window.draw(textPause);
        }

        if (sf :: Mouse :: isButtonPressed ( sf :: Mouse :: Left ))
        {
            Vector2i pos = Mouse::getPosition(window);
            Vector2f mousePos = window.mapPixelToCoords(pos);
            pair<int, int> posCell((int(mousePos.x) - (int(mousePos.x) % 10))/10, (int(mousePos.y) - (int(mousePos.y) % 10))/10);
            if(find(posCellField.begin(), posCellField.end(), posCell) == posCellField.end())
                posCellField.push_back(posCell);
        }
        if (sf :: Mouse :: isButtonPressed ( sf :: Mouse :: Right ))
        {
            Vector2i pos = Mouse::getPosition(window);
            Vector2f mousePos = window.mapPixelToCoords(pos);
            pair<int, int> posCell((int(mousePos.x) - (int(mousePos.x) % 10))/10, (int(mousePos.y) - (int(mousePos.y) % 10))/10);
            auto poss = find(posCellField.begin(), posCellField.end(), posCell);
            if(poss != posCellField.end())
                posCellField.erase(poss);
        }

        for(int i = 0; i < posCellField.size(); i++)
        {
            s.setPosition(posCellField[i].first * 10, 10 * posCellField[i].second);
            window.draw(s);
        }

        Sleep(dt);
        zeros(field);
        beginField(field, &posCellField);
        window.display();




        if (Keyboard::isKeyPressed(Keyboard::Escape))
            break;
    }
    Sleep(350);
    cout << "Life lasted: " << generation;
    cout << '\n';

    int maxPopul = *max_element(populat.begin(), populat.end());
    int minPopul = *min_element(populat.begin(), populat.end());

    cout << "\nmaximum of population: " << maxPopul << '\n';
    cout << "\nminimum of population: " << minPopul << '\n';



/////////////////////////////// Grapher ////////////////////////////////////////////////////////////////////


    RenderWindow windowGraph(VideoMode(sizeX * 10, sizeY * 10), "Grapher");
    while (windowGraph.isOpen())
    {
        Event e;
        while (windowGraph.pollEvent(e))
        {
            if (e.type == Event::Closed)
                windowGraph.close();
        }

            windowGraph.clear(Color::White);
            RectangleShape ox(Vector2f(2, sizeY*10));
            RectangleShape oy(Vector2f(sizeX*10, 2));
            ox.setFillColor(Color::Black);
            oy.setFillColor(Color::Black);
            ox.setPosition(10, 0);
            oy.setPosition(0, sizeY * 10 - 30);
            windowGraph.draw(ox);
            windowGraph.draw(oy);

            Text xLabel("", font, 20);
            Text yLabel("", font, 20);
            xLabel.setColor(Color::Black);
            yLabel.setColor(Color::Black);
            xLabel.setPosition(15, 5);
            yLabel.setPosition(sizeX * 10/2, sizeY * 10 - 25);
            xLabel.setString("Population");
            yLabel.setString("Generation");
            windowGraph.draw(xLabel);
            windowGraph.draw(yLabel);
            CircleShape point(3, 30);
            point.setFillColor(Color::Black);
            for(int i = 0; i < populat.size(); i++)
            {
                point.setPosition(int(20 + double(i) / double(populat.size()) * (sizeX * 10 - 12)), abs(int(sizeY * 10 - 30 - double(populat[i])/double(maxPopul - minPopul - 10)*(sizeY * 10 - 50))));
                windowGraph.draw(point);
            }
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                break;
            windowGraph.display();

    }
    return 0;
}
