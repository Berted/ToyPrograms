#include <bits/stdc++.h>
#include <windows.h>
#include <ctime>
#define ll long long
using namespace std;
using namespace this_thread;
using namespace chrono;

const int gradient[14]={10,11,9,1,3,2,6,4,5,12,13,14,7,8};                              // Used Colors
struct stackData {int x, lg, color;};                                                   // Structure to keep stack data
struct anim {char c;int x,y,movementX,movementY,maxFrame,curFrame,color;};
const int sizeY = 29,sizeX=120;                                                         // Size of Window

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);                                       // Output Handle (Used for WinAPI Functions)
char screen[sizeX][sizeY]={},prvScreen[sizeX][sizeY]={};                                // Screen data (text)
int screenColor[sizeX][sizeY]={},prvScreenColor[sizeX][sizeY]={};                       // Screen data (color)
vector<stackData> currentStack;                                                         // Vector for stack data
vector<anim> currentAnimation;
int currentX = 1,currentMovement=1,currentY=sizeY-2,currentLength = 10,currentColor;    // Initialization of current block data variables
ll score=0,combo=1,inaRow=1,dly=20,mcmb=1,tdly=20;                                      // Initialization of score data variables

void gotoxy( int x, int y )                                                             // Function for moving cursor
{
    COORD coord;coord.X = x;coord.Y = y;
    SetConsoleCursorPosition(console,coord);
}

bool withinBound (int x, int y)
{
    if (x>1&&x<sizeX-1&&y>0&&y<sizeY) {return true;}
}

int colortranslator(int textcolor,int backgroundcolor)                                  // Translates color to an 8-bit number
{
                                                                                        //    0 = Black       8 = Gray
                                                                                        //    1 = Blue        9 = Light Blue
                                                                                        //    2 = Green       10 = Light Green
                                                                                        //    3 = Aqua        11 = Light Aqua
                                                                                        //    4 = Red         12 = Light Red
                                                                                        //    5 = Purple      13 = Light Purple
                                                                                        //    6 = Yellow      14 = Light Yellow
                                                                                        //    7 = White       15 = Bright White
    return backgroundcolor*16+textcolor;
}
void setcolor(int num) {SetConsoleTextAttribute(console,num);}                          // Sets color

void refresh()                                                                          // Refreshes Screen (New Frame)
{
    for (int i=0;i<sizeX;i++)                                                               // Output
    {
        for (int j=0;j<sizeY;j++)
        {
            if (screen[i][j]!=prvScreen[i][j]||screenColor[i][j]!=prvScreenColor[i][j])
            {
                gotoxy(i,j);
                setcolor(screenColor[i][j]);
                cout<<screen[i][j];
            }
        }
    }

    for (int i=0;i<sizeX;i++)                                                               // CurrentScreen Reset
    {
        for (int j=0;j<sizeY;j++)
        {
            prvScreen[i][j]=screen[i][j];
            prvScreenColor[i][j]=screenColor[i][j];
            screen[i][j]=' ';screenColor[i][j]=7;
        }
    }
}

void delay(int a) {sleep_for(nanoseconds(a*1000000));}                                  // Pause for a miliseconds

void blok(int x,int y,int l,int clr)                                                    // Draws a block
{
    for (int i=0;i<l;i++)
    {
        screen[x+i][y]=(char)219;
        screenColor[x+i][y]=gradient[clr];
    }
}

bool waitForInput(int delay)                                                            // Waits for space button being pressed
{
    clock_t stopTime = clock()+delay*CLOCKS_PER_SEC/1000;
    bool hasInput=false;
    while (clock()<=stopTime&&!hasInput)
    {
        if (GetAsyncKeyState(VK_SPACE)) {hasInput=true;break;}
    }
    return hasInput;
}

int setBlock(int x,int l)                                                               // Insert a block into the vector
{
    if (currentStack.size()<1)                                                              // Used at beginning of game, if stack is empty
    {
        currentStack.push_back({x,l,currentColor});
        int n = currentColor;
        while (currentColor==n) {n=rand()%14;}
        currentColor = n;
        return l;
    }

    int lef = max(currentStack.back().x,x),                                                 // Calculation of new left and right of the block
        rgh = min(currentStack.back().x+currentStack.back().lg,x+l);

    if (rgh-lef>0)                                                                          // Used in most cases.
    {
        currentStack.push_back({lef,rgh-lef,currentColor});
        int n = currentColor;
        while (currentColor==n) {n=rand()%14;}
        currentColor = n;
        if (currentStack.size()>sizeY) {currentStack.erase(currentStack.begin());}
    }
    return rgh-lef;
}

void putinBlocks()                                                                          // Draws blocks in the CurrentStack vector
{
    int thisY = sizeY-1;
    for (int i=max((int)currentStack.size()-(sizeY/2),0);i<currentStack.size();i++) {blok(currentStack[i].x,thisY,currentStack[i].lg,currentStack[i].color);thisY--;}
}
void setAnims(char c,int x,int y,int movementX,int movementY,int maxFrame = 4, int curFrame = 0,int color = 7)
{
    if (withinBound(x,y)) {currentAnimation.push_back({c,x,y,movementX,movementY,maxFrame,curFrame,color});}
}
void drawAnims()
{
    anim x;
    for (int i=0;i<currentAnimation.size();i++)
    {
        x = currentAnimation[i];
        screen[x.x][x.y] = x.c;
        screenColor[x.x][x.y] = gradient[x.color];
        x.x += x.movementX; x.y += x.movementY; x.curFrame++;
        if (x.curFrame<x.maxFrame&&withinBound(x.x,x.y)) {currentAnimation[i] = x;}
        else {currentAnimation.erase(currentAnimation.begin()+i);}
    }
}
void putText(string s,int x,int y)                                                          // Puts string s at start position (X,y)
{
    for (int i=0;i<s.size();i++) {screen[x+i][y] = s[i];}
}
void drawGame()                                                                             // Game Draw Function
{
    putinBlocks();
    putText("SCORE : "+to_string(score),1,0);
    putText("COMBO : "+to_string(combo)+"x",81,0);
    drawAnims();
}

bool gameOver()                                                                             // Game Over Function
{
    string inp;
    for (int i=0;i<sizeX;i++)                                                               // Screen Reset
    {
        for (int j=0;j<sizeY;j++)
        {
            screen[i][j]=' ';
            screenColor[i][j]=7;
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    refresh();gotoxy(46,11);cout<<"GAME OVER\n";                                            // Output
    gotoxy(30,13);cout<<"Your Score : "<<score<<" with highest combo : "<<mcmb<<"\n";
    gotoxy (30,15); cout<<"Do you want to play again ( Y / N ) ?  ";cin>>inp;
    for (int i=0;i<sizeX;i++)                                                               // Screen Reset
    {
        for (int j=0;j<sizeY;j++)
        {
            prvScreen[i][j]='X';
            prvScreenColor[i][j]=7;
        }
    }
    refresh();
    if (inp[0]!='N'&&inp[0]!='n') {return true;} else {return false;}
}

int main()                                                                                  // Main
{
    gotoxy(40,15);cout<<"Input Delay : ";cin>>tdly;dly=tdly;
    srand(time(NULL));currentColor = rand()%14;                                                 // Variable initialization
    setBlock(sizeX/2-currentLength/2,currentLength);                                            // Variable initialization (pt. 2)

    while (true)                                                                                // Game Process
    {
        currentX+=currentMovement;                                                                  // Block Movement
        if (currentX>=120-currentLength||currentX<=1) {currentMovement=-currentMovement;}           // Block Movement
        blok(currentX,currentY,currentLength,currentColor);                                         // Block Movement
        drawGame();                                                                                 // Game Draw Process
        refresh();                                                                                  // Game Draw Process

        if (waitForInput(dly))                                                                      // If space is pressed
        {
            int whatLength = setBlock(currentX,currentLength);
            if (whatLength>0)                                                                       // If block fits in stack
            {
                if (currentLength==whatLength)                                                          // Score Calculation
                {
                    if ((combo++)%inaRow==0)                                                                // Lengthening of block if few in a row
                    {
                        currentStack[currentStack.size()-1].lg++;
                        setAnims(178,currentStack.back().x-2,currentY-2,-1,-1,12,0,currentStack.back().color);
                        for (int i=0;i<currentStack.back().lg;i++)
                        {
                            setAnims(178,currentStack.back().x+i,currentY-2,0,-1,12,0,currentStack.back().color);
                        }
                        setAnims(178,currentStack.back().x+currentStack.back().lg+1,currentY-2,1,-1,12,0,currentStack.back().color);
                        whatLength++;
                    }
                    setAnims(178,currentStack.back().x-1,currentY-1,-1,-1,8,0,currentStack.back().color);
                    for (int i=0;i<currentStack.back().lg;i++)
                    {
                        setAnims(178,currentStack.back().x+i,currentY-1,0,-1,8,0,currentStack.back().color);
                    }
                    setAnims(178,currentStack.back().x+currentStack.back().lg,currentY-1,1,-1,8,0,currentStack.back().color);
                }
                else                                                                                    // Score Calculation
                {
                    if (currentX != currentStack.back().x)
                    {
                        for (int i=currentX;i<currentStack.back().x;i++)
                        {
                            setAnims(219,i,currentY+(currentY-1<sizeY/2),-1,0,8,0,currentStack.back().color);
                        }
                    }
                    else
                    {
                        for (int i=currentStack.back().x+currentStack.back().lg;i<currentX+currentLength;i++)
                        {
                            setAnims(219,i,currentY+(currentY-1<sizeY/2),1,0,8,0,currentStack.back().color);
                        }
                    }
                    combo=1;
                }
                if (currentY-1>=sizeY/2) {currentY--;}                                                  // Block Variable Reset
                currentX = 1;currentMovement = 1;                                                       // Block Variable Reset
                currentLength = whatLength;                                                             // Block Variable Reset
                score+=whatLength*combo;                                                                // Score Calculation
                mcmb=max(mcmb,combo);
                while (waitForInput(dly)) {drawGame();refresh();delay(dly);}                            // Waits player to stop pressing space
            }
            else                                                                                    // If not
            {
                if (gameOver())                                                                         // If player wants to play again
                {
                    srand(time(NULL));                                                                  // Change seed for rand()
                    currentStack.clear();                                                               // Resets currentStack vector
                    currentAnimation.clear();
                    currentX = 1;currentMovement=1;                                                     // Resets other variables
                    currentY=sizeY-2;currentLength = 10;                                                // Resets other variables
                    score=0;combo=1;currentColor=rand()%14;                                             // Resets other variables
                    setBlock(sizeX/2-currentLength/2,currentLength);                                    // Resets other variables
                    mcmb=1;dly=tdly;
                }
                else {return 0;}                                                                    // If player wants to stop playing
            }
        }

    }
    return 0;
}

