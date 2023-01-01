#include <bits/stdc++.h>
#include <ctime>
#include <windows.h>
#include <conio.h>
using namespace std;
using namespace this_thread;
using namespace chrono;

const int sizeY = 29, sizeX=120, gameLength = 60;                                                         // Size of Window
const int gameRow = 6,gameX = 20, gameY = 6, gameXSize = 80, gameYSize = gameY + gameRow*3;
vector<int> defaultColor;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);                                       // Output Handle (Used for WinAPI Functions)
char screen[sizeX][sizeY]={},prvScreen[sizeX][sizeY]={};                                // Screen data (text)
int screenColor[sizeX][sizeY]={},prvScreenColor[sizeX][sizeY]={};                       // Screen data (color)
int currentX = 0, currentY = 0, correctChar = 0, dictionarySize = 10000;
vector<string> dictionary;
string inGame[gameRow],written[gameRow];
vector<int> inGameColor[gameRow],writtenColor[gameRow];
string tmp;ifstream database("database.txt");
clock_t gameTime;

void gotoxy( int x, int y )                                                             // Function for moving cursor
{
    COORD coord;coord.X = x;coord.Y = y;
    SetConsoleCursorPosition(console,coord);
}

bool withinBound (int x, int y)
{
    return (x>1&&x<sizeX-1&&y>0&&y<sizeY);
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

string to_string_with_precision(double n,int precision)
{
	ostringstream out;
	out.precision(precision);
	out<<fixed<<n;
	return out.str();
}

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

bool waitForInput(int delay,char c)                                                            // Waits for specified button being pressed
{
    clock_t stopTime = clock()+delay*CLOCKS_PER_SEC/1000;
    bool hasInput=false;
    while (clock()<=stopTime&&!hasInput)
    {
        if (GetAsyncKeyState(c)) {hasInput=true;break;}
    }
    return hasInput;
}

void putText(string s,int x,int y,vector<int> v)                                                          // Puts string s at start position (X,y)
{
    for (int i=0;i<s.size();i++) {screen[x+i][y] = s[i];screenColor[x+i][y] = v[i];}
}

void generateText(int row)
{
	string rando = dictionary [rand()%dictionarySize];
	while (inGame[row].size()+rando.size()+1<=gameXSize) 
	{
		inGame[row]+=rando+" ";
		rando = dictionary [rand()%dictionarySize];
	}
}

void gamePreStart()
{
	srand(time(NULL));
	for (int i=0;i<sizeX;i++) {defaultColor.push_back(colortranslator(15,0));}
	while (database>>tmp) {dictionary.push_back(tmp);}
}

void gameStart()
{
	char c = 0;
	while (!c)
	{
		putText("Pick your difficulty (E / M / H / X): ",35,17,defaultColor);
		refresh();
		c = _getch();c=toupper(c);
		if (c=='E') {dictionarySize = 50;}
		else if (c=='M') {dictionarySize = 500;}
		else if (c=='H') {dictionarySize = 2500;}
		else if (c=='X') {dictionarySize = 10000;}
		else {c=0;}
	}
	for (int i=0;i<gameRow;i++)
	{
		inGameColor[i] = defaultColor;
		writtenColor[i] = defaultColor;
		generateText(i);
	}
	gameTime = clock();
}

void gameDraw()
{
	double diff = (double)gameTime/CLOCKS_PER_SEC + gameLength - (double)clock()/CLOCKS_PER_SEC;
	putText("Time : "+to_string_with_precision(diff,2),45,2,defaultColor);
	putText("WPM : "+to_string_with_precision(correctChar*12/(60-diff),2),45,28,defaultColor);
	for (int i=currentY;i<currentY+gameRow;i++)
	{
		putText(inGame[i%gameRow],gameX,(i-currentY)*3+gameY,inGameColor[i%gameRow]);
		putText(written[i%gameRow],gameX,(i-currentY)*3+gameY-1,writtenColor[i%gameRow]);
	}
	gotoxy(written[currentY%gameRow].size()+gameX,gameY-1);
}

void endGame()
{
	while (!_kbhit())
	{
		putText("Your WPM : "+to_string_with_precision((double)correctChar/5,2),54,18,defaultColor);
		putText("Press any key to continue.",48,24,defaultColor);
		refresh();
	}
}

void procInput(char c)
{
	if (c==8)
	{
		if (written[currentY].size()) 
		{
			if (written[currentY].back()!=' ')
			{
				inGameColor[currentY][written[currentY].size()-1] = colortranslator(15,0);
				written[currentY].pop_back();
			}
		}
	}
	else
	{
		if (c==32)
		{
			int i;
			for (i=written[currentY].size();i<inGame[currentY].size();i++)
			{
				if (inGame[currentY][i]==' ') {break;}
			}
			for (int j=written[currentY].size();j<i;j++)
			{
				written[currentY].push_back(' ');
				inGameColor[currentY][j] = colortranslator(15,4);
			}
		}
		written[currentY].push_back(c);
		if (written[currentY].back()==inGame[currentY][written[currentY].size()-1])
		{
			inGameColor[currentY][written[currentY].size()-1] = colortranslator(10,0);
			correctChar++;
		}
		else {inGameColor[currentY][written[currentY].size()-1] = colortranslator(15,4);}
		if (written[currentY].size()>=inGame[currentY].size()) 
		{
			written[currentY] = "";
			inGame[currentY] = "";
			writtenColor[currentY] = defaultColor;
			inGameColor[currentY] = defaultColor;
			generateText(currentY);
			currentY++;
			currentY%=gameRow;
		}
	}
}

int main()
{
	gamePreStart();
	while (true)
	{
		gameStart();
		while (clock()<=gameTime+gameLength*CLOCKS_PER_SEC)
		{
			gameDraw();
			refresh();
			while (_kbhit())
			{
				char c = _getch();
				procInput(c);
			}
		}
		endGame();
	}	
}
