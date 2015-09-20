// CorsairNumber.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

#include "CUESDK.h"

#include <iostream>
#include <thread>
#include <string>
#include <vector>

using std::vector;


const char* toString(CorsairError error)
{
	switch (error) {
	case CE_Success:
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

void highlightKey(CorsairLedId ledId)
{
	for (auto x = .0; x < 2; x += .1) {
		int val = (1 - abs(x - 1)) * 255;
		auto ledColor = CorsairLedColor{ ledId, val, val, val };
		// auto ledColor = CorsairLedColor{ ledId, val, val, val };
		CorsairSetLedsColors(1, &ledColor);
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
}


//CorsairLedId segments[7][3] = {
vector<vector<CorsairLedId>> segments {
	{CLK_NumLock, CLK_KeypadSlash, CLK_KeypadAsterisk},
	{CLK_KeypadAsterisk, CLK_Keypad9, CLK_Keypad6},
	{CLK_Keypad6, CLK_Keypad3, CLK_KeypadPeriodAndDelete},
	{CLK_KeypadPeriodAndDelete, CLK_Keypad0},
	{CLK_Keypad0, CLK_Keypad1, CLK_Keypad4},
	{CLK_Keypad4, CLK_Keypad7, CLK_NumLock},
	{CLK_Keypad4, CLK_Keypad5, CLK_Keypad6}
};

vector<vector<int>> numbers{
	{0, 1, 2, 3, 4, 5},
	{1, 2},
	{0, 1, 6, 4, 3},
	{0, 1, 6, 2, 3},
	{5, 1, 6, 2},
	{0, 5, 6, 2, 3},
	{0, 5, 6, 2, 3, 4},
	{0, 1, 2},
	{0, 1, 2, 3, 4, 5, 6},
	{0, 1, 2, 3, 5, 6}
};

void lightsOff_Keypad() {
	for (auto i = 103; i <= 120; i++) {
		auto color = CorsairLedColor{ static_cast<CorsairLedId>(i) , 0, 0, 0 };
		CorsairSetLedsColors(1, &color);
	}
}

int main()
{
	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}

	CorsairRequestControl(CAM_ExclusiveLightingControl);

	std::cout << "Please, input a number sequence... ";
	auto userInputStr = std::string();
	std::cin >> userInputStr;

	vector<int> nums;
	for (const char c : userInputStr) {
		nums.push_back(c - '0');
	}
	
	for (const int num : nums) {
		std::cout << "Nombre demandé: " << num << std::endl;
	}

	for (const int num : nums) {
		lightsOff_Keypad();
		vector<CorsairLedColor> colors;
		colors.empty();
		for (const auto seg : numbers[num]) {
			for (const auto key_id : segments[seg]) {
				colors.push_back(CorsairLedColor{ key_id, 255, 0, 0 });
			}
		}
		for (auto color : colors) {
			CorsairSetLedsColors(1, &color);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		lightsOff_Keypad;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	return 0;
}