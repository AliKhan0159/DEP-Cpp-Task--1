#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Location.h" 
#include "WeatherVariable.h"
#include "WeatherForecastingSystem.h"
#include "HistoricalWeatherSystem.h"
#include "ExportUtils.h" 

class Location {
private:
	std::string name;
	double latitude;
	double longitude;

public:
	Location(const std::string& name, double lat, double lon)
		: name(name), latitude(lat), longitude(lon) {}

	std::string getName() const { return name; }
	double getLatitude() const { return latitude; }
	double getLongitude() const { return longitude; }
};

class LocationManager {
private:
	std::vector<Location> locations;

public:
	void addLocation(const Location& loc) {
		locations.push_back(loc);
	}

	void removeLocation(const std::string& name) {
		auto it = std::remove_if(locations.begin(), locations.end(),
			[&](const Location& loc) { return loc.getName() == name; });
		if (it != locations.end()) {
			locations.erase(it, locations.end());
			std::cout << "Location Removed Successfully." << std::endl;
		}
		else {
			std::cout << "Location Not Found." << std::endl;
		}
	}

	void listLocations() const {
		if (locations.empty()) {
			std::cout << "No Locations Defined." << std::endl;
			return;
		}
		for (const auto& loc : locations) {
			std::cout << "Name: " << loc.getName()
				<< ", Latitude: " << loc.getLatitude()
				<< ", Longitude: " << loc.getLongitude()
				<< std::endl;
		}
	}
};

class WeatherVariable {
private:
	std::unordered_map<std::string, double> variables;

public:
	void defineVariable(const std::string& name, double value) {
		variables[name] = value;
	}

	double getVariable(const std::string& name) const {
		auto it = variables.find(name);
		if (it != variables.end()) {
			return it->second;
		}
		else {
			std::cerr << "Variable " << name << " not found." << std::endl;
			return 0.0; // or throw an exception
		}
	}

	void removeVariable(const std::string& name) {
		auto it = variables.find(name);
		if (it != variables.end()) {
			variables.erase(it);
			std::cout << "Variable removed successfully." << std::endl;
		}
		else {
			std::cerr << "Variable " << name << " not found, cannot remove." << std::endl;
		}
	}

	void listVariables() const {
		if (variables.empty()) {
			std::cout << "No weather variables defined." << std::endl;
			return;
		}
		for (const auto& var : variables) {
			std::cout << "Variable: " << var.first << ", Value: " << var.second << std::endl;
		}
	}

	void menu() {
		int choice;
		do {
			std::cout << "\nWeather Variable Manager Menu\n";
			std::cout << "1. Add/Update Variable\n";
			std::cout << "2. Retrieve Variable\n";
			std::cout << "3. Remove Variable\n";
			std::cout << "4. List All Variables\n";
			std::cout << "5. Exit\n";
			std::cout << "Enter your choice: ";
			std::cin >> choice;

			switch (choice) {
			case 1: {
						std::string name;
						double value;
						std::cout << "Enter variable name: ";
						std::cin >> name;
						std::cout << "Enter variable value: ";
						std::cin >> value;
						defineVariable(name, value);
						std::cout << "Variable added/updated successfully." << std::endl;
						break;
			}
			case 2: {
						std::string name;
						std::cout << "Enter variable name to retrieve: ";
						std::cin >> name;
						double value = getVariable(name);
						if (value != 0.0) {
							std::cout << "Variable value: " << value << std::endl;
						}
						break;
			}
			case 3: {
						std::string name;
						std::cout << "Enter variable name to remove: ";
						std::cin >> name;
						removeVariable(name);
						break;
			}
			case 4:
				listVariables();
				break;
			case 5:
				std::cout << "Exiting the program." << std::endl;
				break;
			default:
				std::cout << "Invalid choice. Please try again." << std::endl;
				break;
			}
		} while (choice != 5);
	}
};

class WeatherForecastingSystem {
public:
	std::string fetchWeather(const std::string& location) {
		// Replace with your API endpoint and parameters
		std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" + location + "&longitude=...";

		CURL* curl;
		CURLcode res;
		std::string response;

		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();

		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
			return "";
		}

		return response;
	}

	void displayWeather(const std::string& weatherData) {
		std::cout << "Weather Data: " << weatherData << std::endl;
	}

private:
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}
};

class HistoricalWeatherSystem {
public:
	std::string fetchHistoricalData(const std::string& location, const std::string& date) {
		// Replace with your API endpoint and parameters
		std::string url = "https://api.open-meteo.com/v1/history?latitude=" + location + "&longitude=..." + "&date=" + date;

		CURL* curl;
		CURLcode res;
		std::string response;

		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();

		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
			return "";
		}

		return response;
	}

	void displayHistoricalData(const std::string& historicalData) {
		std::cout << "Historical Data: " << historicalData << std::endl;
	}

private:
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}
};

void exportToCSV(const std::string& filename, const std::unordered_map<std::string, double>& data) {
	std::ofstream file(filename);
	if (file.is_open()) {
		file << "Variable,Value\n";
		for (const auto& pair : data) {
			file << pair.first << "," << pair.second << "\n";
		}
		file.close();
	}
	else {
		std::cerr << "Unable to open file for writing." << std::endl;
	}
}

void exportToJSON(const std::string& filename, const std::unordered_map<std::string, double>& data) {
	nlohmann::json j;
	for (const auto& pair : data) {
		j[pair.first] = pair.second;
	}

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(4); // Pretty print with 4 spaces
		file.close();
	}
	else {
		std::cerr << "Unable to open file for writing." << std::endl;
	}
}

int main() {
	LocationManager locManager;
	WeatherVariable weatherVars;
	WeatherForecastingSystem weatherFS;
	HistoricalWeatherSystem histWS;

	// Example Usage
	locManager.addLocation(Location("CityA", 35.0, -78.0));
	locManager.listLocations();

	weatherVars.defineVariable("Temperature", 75.0);
	weatherVars.listVariables();

	std::string weatherData = weatherFS.fetchWeather("35.0,-78.0");
	weatherFS.displayWeather(weatherData);

	std::string historicalData = histWS.fetchHistoricalData("35.0,-78.0", "2024-09-10");
	histWS.displayHistoricalData(historicalData);

	exportToCSV("weather_data.csv", { { "Temperature", 75.0 } });
	exportToJSON("weather_data.json", { { "Temperature", 75.0 } });

	return 0;
}


