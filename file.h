#include <fstream>
using namespace std;




void writeFile_R(vector<vector<double>> map)
{
	cout<<"柱坐标文件保存中…\n";
	ofstream out("r.txt");
	for (int i = 0;i < map.size();i++)
	{
		for (int j = 0;j < map[i].size(); j++)
		{
			out<<map[i][j]<<"  ";
		}
		out<<endl;
	}
	cout<<"保存成功！\n";
}

void writeFile_X(vector<vector<double>> map)
{
	cout<<"直角坐标文件保存中…\n";
	ofstream out("x.txt");
	for (int i = 0;i < map.size();i++)
	{
		for (int j = 0;j < map[i].size(); j += 3)
		{
			out<<map[i][j]<<","<<map[i][j+1]<<","<<map[i][j+2]<<"  ";
		}
		out<<endl;
	}
	cout<<"保存成功！\n";
}

double getpoint(string s)
{
	int i = 0;
	for (i;s[i] != ' ';i++);
	string tem = s.substr(0,i);
	s = s.substr(i+1,s.length()-i-1);
	return atof(tem.c_str());

}

void loadfile(string rs,vector<vector<double>> rmap)
{
	ifstream in(rs);

	string tem;
	
	for (int i = 0;i < rmap.size();i++)
	{
		getline(in,tem);
		for (int j = 0;j < rmap[i].size(); j++)
		{
			rmap[i][j] = getpoint(tem);
		}
		
	}
}
