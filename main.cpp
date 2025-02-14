#include <iostream>
#include <string>
#include <filesystem>
#include <regex>
#include <cctype>
#include <fstream>
using namespace std;



vector<string> std_strtok(const string& s, const string& regex_s)
{
	vector<string> tokens;

	regex r(regex_s);

	sregex_token_iterator iter(s.begin(), s.end(), r, -1);
	sregex_token_iterator end;

	while (iter != end)
	{
		if (*iter != "")
			tokens.push_back(*iter);

		iter++;
	}

	return tokens;
}


int main(void)
{
	std::string path = "Y:/home/sjhalayka/ldak_min";

	vector<string> filenames;

	for (const auto& entry : filesystem::directory_iterator(path))
	{
		string s = entry.path().string();

		vector<string> tokens = std_strtok(s, "[.]\\s*");

		for (size_t i = 0; i < tokens.size(); i++)
			for (size_t j = 0; j < tokens[i].size(); j++)
				tokens[i][j] = tolower(tokens[i][j]);

		if (tokens.size() > 0 &&
			(tokens[tokens.size() - 1] == "c" ||
				tokens[tokens.size() - 1] == "cpp"))
		{
			filenames.push_back(s);
		}
	}


	vector<string> types;
	types.push_back("short");
	types.push_back("long");
	types.push_back("int");
	types.push_back("char");
	types.push_back("unsigned");
	types.push_back("signed");
	types.push_back("size_t");
	types.push_back("float");
	types.push_back("double");
	types.push_back("FILE");
	types.push_back("DIR");
	types.push_back("gzFile");
	types.push_back("struct");

	for (size_t i = 0; i < filenames.size(); i++)
	{
		ifstream infile(filenames[i]);

		if (infile.fail())
			continue;

		string line;

		cout << endl << endl << endl;
		cout << filenames[i] << endl << endl;

		while (getline(infile, line))
		{
			if (line == "")
				continue;

			vector<string> tokens = std_strtok(line, "[ \t]\\s*");

			bool found_type = false;

			if (tokens.size() > 0 &&
				types.end() != find(
					types.begin(),
					types.end(),
					tokens[0]))
			{
				found_type = true;
			}

			if (found_type &&
				tokens.size() > 0 &&
				tokens[tokens.size() - 1].length() > 0 &&
				';' == tokens[tokens.size() - 1][tokens[tokens.size() - 1].length() - 1])
			{
				string type = tokens[0];

				if (tokens.size() > 1 && tokens[0] == "unsigned")
				{
					type = "unsigned ";

					for (size_t i = 0; i < tokens.size(); i++)
					{
						if (tokens[i] == "short" ||
							tokens[i] == "long" ||
							tokens[i] == "int" ||
							tokens[i] == "char")
						{
							if(tokens[i] != "unsigned")
								type += tokens[i] + " ";

							tokens.erase(tokens.begin() + i);
							i = 0;
						}
					}
				}
				else if (tokens.size() > 1 && tokens[0] == "signed")
				{
					type = "signed ";

					for (size_t i = 1; i < tokens.size(); i++)
					{
						if (
							tokens[i] == "short" ||
							tokens[i] == "long" ||
							tokens[i] == "int" ||
							tokens[i] == "char")
						{
							if (tokens[i] != "signed")
								type += tokens[i] + " ";

							tokens.erase(tokens.begin() + i);
							i = 0;
						}
					}
				}
				else if (tokens.size() > 1 && tokens[0] == "short")
				{
					type = "short ";

					for (size_t i = 0; i < tokens.size(); i++)
					{
						if (
							tokens[i] == "unsigned" ||
							tokens[i] == "signed" ||
							tokens[i] == "int")
						{
							if (tokens[i] != "short")
								type += tokens[i] + " ";

							tokens.erase(tokens.begin() + i);
							i = 0;
						}
					}
				}
				else if (tokens.size() > 1 && tokens[0] == "long")
				{
					type = "long ";

					for (size_t i = 0; i < tokens.size(); i++)
					{
						if (
							tokens[i] == "unsigned" ||
							tokens[i] == "signed" ||
							tokens[i] == "int")
						{
							if (tokens[i] != "long")
								type += tokens[i] + " ";
				
							tokens.erase(tokens.begin() + i);
							i = 0;
						}
					}
				}
				else if (tokens.size() > 1 && tokens[0] == "struct")
				{
					type = "struct " + tokens[1];
				}

				cout << type << endl;

				for (size_t j = 1; j < tokens.size(); j++)
					cout << tokens[j] << " ";
				
				cout << endl;

				//cout << type << endl;
				//cout << line << endl;
			}
		}


	}

	return 0;
}