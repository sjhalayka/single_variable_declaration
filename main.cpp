#include <iostream>
#include <string>
#include <filesystem>
#include <regex>
#include <cctype>
#include <fstream>
using namespace std;

string trim_left_whitespace(const string& s)
{
	string ret;
	bool found_non_whitespace = false;

	for (size_t i = 0; i < s.size(); i++)
	{
		if(false == found_non_whitespace && (s[i] == ' ' || s[i] == '\t'))
		{
			continue;
		}
		else
		{
			found_non_whitespace = true;
			ret += s[i];
		}
	}

	return ret;
}

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
	types.push_back("static");
	types.push_back("const");
	types.push_back("short");
	types.push_back("long");
	types.push_back("int");
	types.push_back("char");
	types.push_back("unsigned");
	types.push_back("signed");
	types.push_back("float");
	types.push_back("double");
	types.push_back("size_t");
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
		string prev_lines;

		cout << endl << endl << endl;
		cout << filenames[i] << endl << endl;

		ostringstream output;

		while (getline(infile, line))
		{
			if (line == "")
				continue;

			if (line[line.size() - 1] == '\\')
			{
				prev_lines = prev_lines + line;
				continue;
			}
			else
			{
				prev_lines = line;
			}

			bool finished_with_semi_colon = false;

			if (prev_lines[prev_lines.size() - 1] == ';')
				finished_with_semi_colon = true;

			bool inside_quotes = false;

			for (size_t j = 0; j < prev_lines.size(); j++)
			{
				if (prev_lines[j] == '\"')
					inside_quotes = !inside_quotes;

				if (prev_lines[j] == ';' && inside_quotes)
					prev_lines[j] = ':';
			}




			vector<string> statements = std_strtok(prev_lines, "[;]\\s*");

			for (size_t j = 0; j < statements.size(); j++)
				statements[j] += ';';

			for (size_t s = 0; s < statements.size(); s++)
			{
				//statements[s] = trim_left_whitespace(statements[s]);

				vector<string> tokens = std_strtok(statements[s], "[ \t]\\s*");

				if (tokens.size() == 0)
					continue;

				for (size_t j = 0; j < tokens.size(); j++)
				{
					tokens[j] = trim_left_whitespace(tokens[j]);
				}

				bool found_type = false;
				bool is_struct = false;
				bool is_const = false;

				// Found a # or / as the first character
				if ((tokens[0][0] == '#' || tokens[0][0] == '/')
					||
					(tokens[0][0] == 'f' && 
					tokens[0][1] == 'o' &&
					tokens[0][2] == 'r'))
				{
					cout << tokens[0] << endl;

					for(size_t x = 0; x < statements.size(); x++)
					for (size_t j = 0; j < statements[x].size(); j++)
						if (statements[x][j] == '\\')
							statements[x][j] = ' ';

					for (size_t x = 0; x < statements.size(); x++)
					{
						for (size_t j = 0; j < statements[x].size() - 1; j++)
							output << statements[x][j];

						if(x < statements.size() - 1)
						output << ';';
					}

					output << endl;

					break;
				}



				// Is known type?
				if (types.end() != find(
						types.begin(),
						types.end(),
						tokens[0]))
				{
					found_type = true;

					cout << "FOUND_TYPE " << tokens[0] << endl;
				}



				// This is not a variable declaration statement
				if(false == found_type)
				{
					// Not a variable declaration
					if (finished_with_semi_colon)
					{
						output << statements[s];
					}
					else
					{
						statements[s].pop_back();
						output << statements[s];
					}

					output << endl;

					continue;
				}

				bool found_initializer = false;

				for (size_t j = 0; j < statements[s].size(); j++)
				{
					if (statements[s][j] == '{' || statements[s][j] == '}')
					{
						found_initializer = true;
					}
				}

				if (found_initializer)
				{
					

					if (finished_with_semi_colon)
						output << statements[s] << endl;
					else
					{
						statements[s].pop_back();
						output << statements[s] << endl;
					}

					continue;
				}

				if (tokens[tokens.size() - 1].length() > 0 &&
					';' == tokens[tokens.size() - 1][tokens[tokens.size() - 1].length() - 1])
				{
					string type = tokens[0];

					if (tokens.size() > 1 && tokens[0] == "static")
					{
						type = "static ";

						for (size_t i = 0; i < tokens.size(); i++)
						{
							if (tokens[i] == "size_t" ||
								tokens[i] == "FILE" ||
								tokens[i] == "DIR" ||
								tokens[i] == "gzFile" ||
								tokens[i] == "double" ||
								tokens[i] == "float" ||
								tokens[i] == "unsigned" ||
								tokens[i] == "signed" ||
								tokens[i] == "short" ||
								tokens[i] == "long" ||
								tokens[i] == "int" ||
								tokens[i] == "char")
							{
								if (tokens[i] != "static")
									type += tokens[i] + " ";

								tokens.erase(tokens.begin() + i);
								i = 0;
							}
						}
					}
					if (tokens.size() > 1 && tokens[0] == "const")
					{
						type = "const ";

						for (size_t i = 0; i < tokens.size(); i++)
						{
							if (tokens[i] == "size_t" ||
								tokens[i] == "FILE" ||
								tokens[i] == "DIR" ||
								tokens[i] == "gzFile" ||
								tokens[i] == "double" ||
								tokens[i] == "float" ||
								tokens[i] == "unsigned" ||
								tokens[i] == "signed" ||
								tokens[i] == "short" ||
								tokens[i] == "long" ||
								tokens[i] == "int" ||
								tokens[i] == "char")
							{
								if (tokens[i] != "const")
									type += tokens[i] + " ";

								tokens.erase(tokens.begin() + i);
								i = 0;
							}
						}
					}
					else if (tokens.size() > 1 && tokens[0] == "unsigned")
					{
						type = "unsigned ";

						for (size_t i = 0; i < tokens.size(); i++)
						{
							if (tokens[i] == "short" ||
								tokens[i] == "long" ||
								tokens[i] == "int" ||
								tokens[i] == "char")
							{
								if (tokens[i] != "unsigned")
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
						is_struct = true;
						type = "struct " + tokens[1];
					}
					//else
					//{
					//	cout << "UNKNOWN TYPE " << "\"" << type << "\"" << endl;
					//}


					string declarations;


					size_t first_index = 1;

					if (is_struct || is_const)
						first_index = 2;

					for (size_t j = first_index; j < tokens.size(); j++)
						declarations += tokens[j];

					vector<string> declaration_tokens = std_strtok(declarations, "[,;]\\s*");

					for (size_t j = 0; j < declaration_tokens.size(); j++)
					{
						if(finished_with_semi_colon)
							output << type << " " << declaration_tokens[j] << ';';
						else 
						{
							declaration_tokens[j].pop_back();
							output << type << " " << declaration_tokens[j];
						}
						output << endl;
					}

				}


			}

			string str = output.str();

			if (false == finished_with_semi_colon && str[str.size() - 2] == ';')
			{
				//output.seekp(-1, std::ios_base::end);
				//output.seekp(-1, std::ios_base::end);

				//str.pop_back();
				//str.pop_back();
				//str += '\n';
				//////str.pop_ba
				//output.str(str);

				//	cout << str << endl;

				//output.clear();
				//output << "";
				//// 
				//output << str << endl;
			}
		}

		infile.close();

		ofstream outfile(filenames[i]);

		outfile << output.str() << endl;

		outfile.close();
	}

	return 0;
}