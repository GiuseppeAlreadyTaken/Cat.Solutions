#include "includes.h"
#include "colors.hpp"

void Utils::Art() {

	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;
	CONSOLE_CURSOR_INFO     cursorInfo;
	CONSOLE_SCREEN_BUFFER_INFOEX csbi{ sizeof csbi };
	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA( console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	FillConsoleOutputAttribute( console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written );
	SetConsoleCursorPosition(console, topLeft);
	GetConsoleCursorInfo(console, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(console, &cursorInfo);

	std::cout << dye::aqua_on_black ( R"(
      _.---.._             _.---...__  
   .-'   /\   \          .'  /\     /      ____      _     ____        _       _   _                 
   `.   (  )   \        /   (  )   /      / ___|__ _| |_  / ___|  ___ | |_   _| |_(_) ___  _ __  ___ 
     `.  \/   .'\      /`.   \/  .'      | |   / _` | __| \___ \ / _ \| | | | | __| |/ _ \| '_ \/ __|
       ``---''   )    (   ``---''        | |__| (_| | |_ _ ___) | (_) | | |_| | |_| | (_) | | | \__ \
               .';.--.;`.                 \____\__,_|\__(_)____/ \___/|_|\__,_|\__|_|\___/|_| |_|___/
             .' /_...._\ `.                                   
           .'   `.a  a.'   `.
          (        \/        )                       
           `.___..-'`-..___.'
              \          /
               `-.____.-'               )");

	std::cout << "\n\n\n\n\n";

}

void Utils::Log(std::string message, int type) {

	if (type != 0 && type != 1 && type != 2)
		std::cout << dye::vanilla("[") + dye::black_on_light_red(" Error ") + dye::vanilla("] ") + "Invalid type";

	if (type == 0)
		std::cout << dye::vanilla("[") + dye::aqua(" Debug ") + dye::vanilla("] ") + message;;
	
	if (type == 1)
		std::cout << dye::vanilla("[") + dye::light_green(" Success ") + dye::vanilla("] ") + message;;

	if (type == 2)
		std::cout << dye::vanilla("[") + dye::black_on_light_red(" Error ") + dye::vanilla("] ") + message;;

};