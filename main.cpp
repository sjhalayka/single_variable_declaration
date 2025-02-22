#include <iostream>
#include <string>
#include <filesystem>
#include <regex>
#include <cctype>
#include <fstream>
using namespace std;


inline void trim_left_whitespace(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
inline void trim_right_whitespace(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

//string trim_left_whitespace(const string& s)
//{
//	string ret;
//	bool found_non_whitespace = false;
//
//	for (size_t i = 0; i < s.size(); i++)
//	{
//		if(false == found_non_whitespace && (s[i] == ' ' || s[i] == '\t'))
//		{
//			continue;
//		}
//		else
//		{
//			found_non_whitespace = true;
//			ret += s[i];
//		}
//	}
//
//	return ret;
//}
//
//string trim_right_whitespace(const string& s)
//{
//	string ret;
//	bool found_non_whitespace = false;
//
//	for (size_t i = s.size() - 1; i >= 0;)
//	{
//		if (false == found_non_whitespace && (s[i] == ' ' || s[i] == '\t'))
//		{
//			continue;
//		}
//		else
//		{
//			found_non_whitespace = true;
//			ret += s[i];
//		}
//
//		if (i > 0)
//			i--;
//		else
//			break;
//
//	}
//
//	return ret;
//}

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

	vector<string> types;
	types.push_back("static ");
	types.push_back("const ");
	types.push_back("short ");
	types.push_back("long ");
	types.push_back("int ");
	types.push_back("char ");
	types.push_back("unsigned ");
	types.push_back("signed ");
	types.push_back("float ");
	types.push_back("double ");
	types.push_back("size_t ");
	types.push_back("FILE ");
	types.push_back("DIR ");
	types.push_back("gzFile ");
	types.push_back("struct ");

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




	for (size_t i = 0; i < filenames.size(); i++)
	{
		ifstream infile(filenames[i]);

		if (infile.fail())
			continue;

		string line;
		string prev_lines;

		cout << endl << endl << endl;
		cout << filenames[i] << endl << endl;
		string type = "";

		ostringstream output;

		while (getline(infile, line))
		{
			if (line == "")
			{
				output << endl;
				continue;
			}

			if (line[line.size() - 1] == '\\')
			{
				prev_lines = prev_lines + line;
				continue;
			}
			else
			{
				prev_lines = line;
			}


				
			trim_right_whitespace(prev_lines);

			bool finished_with_semi_colon = false;

			if (prev_lines[prev_lines.size() - 1] == ';')
				finished_with_semi_colon = true;

			bool finished_with_comma = false;

			if (prev_lines[prev_lines.size() - 1] == ',' || prev_lines[prev_lines.size() - 1] == ')')
			{
				finished_with_comma = true;
			}

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
				vector<string> tokens = std_strtok(statements[s], "[ \t]\\s*");

				if (tokens.size() == 0)
					continue;

				for (size_t j = 0; j < tokens.size(); j++)
				{
					trim_left_whitespace(tokens[j]);

					if(j < tokens.size() - 1)
						tokens[j] += ' ';
				}

				// Found a # or / as the first character
				if (tokens[0][0] == '#' || tokens[0][0] == '/')
				{
					//cout << tokens[0] << endl;

					//for(size_t x = 0; x < statements.size(); x++)
					//for (size_t j = 0; j < statements[x].size(); j++)
					//	if (statements[x][j] == '\\')
					//		statements[x][j] = ' ';

					for (size_t x = 0; x < tokens.size(); x++)
					{
						if(tokens[x].size() > 0)
							tokens[x].pop_back();

						output << tokens[x] << ' ';
					}



					output << endl;

					break;
				}


				if (tokens[0].size() >= 3 &&
					tokens[0][0] == 'f' &&
					tokens[0][1] == 'o' &&
					tokens[0][2] == 'r')
				{
					//cout << tokens[0] << endl;

					for (size_t x = 0; x < statements.size(); x++)
					{
						for (size_t j = 0; j < statements[x].size() - 1; j++)
							output << statements[x][j];

						if (x < statements.size() - 1)
							output << ';';
					}

					if (finished_with_semi_colon)
						output << ';';

					output << endl;

					break;
				}



				bool found_type = false;
				bool is_struct = false;
				bool is_const = false;
				//bool is_static = false;

			//	cout << "TOKEN0 " << "\"" << tokens[0] << "\"" << endl;

				// Is known type?
				if (types.end() != find(
						types.begin(),
						types.end(),
						tokens[0]))
				{
					found_type = true;
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

				if (finished_with_comma)
				{
					statements[s].pop_back();
					output << statements[s] << endl;

					continue;
				}
				
			
				

				if (statements[s].length() > 0 &&
					(';' == statements[s][statements[s].length() - 1] ||
					',' == statements[s][statements[s].length() - 1]))
				{


					if (type == "" || s == 0)
					{
						type = tokens[0];

						if (tokens[0] == "static ")
						{
							//is_static = true;

							type = "static ";

							for (size_t i = 1; i < tokens.size(); i++)
							{
								if (//tokens[i] == "static " ||
									tokens[i] == "size_t " ||
									tokens[i] == "FILE " ||
									tokens[i] == "DIR " ||
									tokens[i] == "gzFile " ||
									tokens[i] == "double " ||
									tokens[i] == "float " ||
									tokens[i] == "unsigned " ||
									tokens[i] == "signed " ||
									tokens[i] == "short " ||
									tokens[i] == "long " ||
									tokens[i] == "int " ||
									tokens[i] == "char ")
								{
									/*if (tokens[i] != "static ")*/
									type += tokens[i] + " ";

									tokens.erase(tokens.begin() + i);
									i = 0;
								}
							}
						}
						else if (tokens.size() > 1 && tokens[0] == "const")
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
						else if (tokens.size() > 1 && tokens[0] == "long ")
						{

							type = "long ";

							for (size_t i = 0; i < tokens.size(); i++)
							{
								if (
									tokens[i] == "unsigned" ||
									tokens[i] == "signed" ||
									tokens[i] == "int")
								{
									if (tokens[i] != "long ")
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
					}

					string declarations;

					size_t first_index = 1;

					//if (is_struct || is_const)
					//	first_index = 2;

					for (size_t j = first_index; j < tokens.size(); j++)
						declarations += tokens[j];

					//cout << "DECLARATIONS " << endl;

					vector<string> declaration_tokens = std_strtok(declarations, "[,;]\\s*");

					for (size_t j = 0; j < declaration_tokens.size(); j++)
					{
						cout << type << " " << declaration_tokens[j] << endl;

						if(finished_with_semi_colon)
							output << type << " " << declaration_tokens[j] << ';';
						else 
						{
							//declaration_tokens[j].pop_back();
							output << type << " " << declaration_tokens[j] << ';';
						}

						output << endl;
					}

				}


			}
		}

		infile.close();

		ofstream outfile(filenames[i]);

		outfile << output.str() << endl;

		outfile.close();
	}

	return 0;
}