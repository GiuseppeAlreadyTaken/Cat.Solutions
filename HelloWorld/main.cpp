#include <iostream>
#include <thread>
#include "ui.h"
#include "includes.h"
#include <filesystem>

std::string version = "1.0";

KeyAuth::api KeyAuthApp("Test", "cK9PelFX9Z", "ce3b4257e67b23fa06361290e118a158e2d7285d1da9e7dc2957d1ab4feb1878", version, "https://keyauth.win/api/1.2/"); // Oh no leak :sob:

int main() {

	Utils::Log("Initializating KeyAuth...\n", 0);

	KeyAuthApp.init();

	if (KeyAuthApp.checkblack()) {

		Utils::Log("Black PC detected!", 2);
		Sleep(3000);
		std::exit(__TIME__[4]);

	}

	if (version != KeyAuthApp.version) {

		Utils::Log("Outdated Loader!", 2);
		Sleep(3000);
		std::exit(__TIME__[4]);

	}

	Utils::Log("KeyAuth Succefully Initializated!\n", 1);

	Utils::Log("Initializating UI...", 0);

	std::thread T1(ui::Init);

	while (ui::initialized == false) {

		std::cout << ".";
		Sleep(100);

	}

	Utils::Art();

	while (true) {

		SetConsoleTitleA(R"(Cat.solutions - \O_O/)");
		Sleep(300);
		SetConsoleTitleA(R"(Cat.solutions - /O_O/)");
		Sleep(300);
		SetConsoleTitleA(R"(Cat.solutions - \O_O\)");
		Sleep(300);
		SetConsoleTitleA(R"(Cat.solutions - /O_O\)");
		Sleep(300);
		SetConsoleTitleA(R"(Cat.solutions - :uwu:)");
		Sleep(300);

	}

	return 1;

}
