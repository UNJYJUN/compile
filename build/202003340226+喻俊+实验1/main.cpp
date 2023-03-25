#include <stack>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

struct edge // NFAͼ�еı�
{
	char weight;
	char next;
};

string symbolSet;		 // ����ķ��ż�
string startNormalForm;	 // ���������ʽ
string fullNormalForm;	 // �������ӷ�������ʽ
string suffixForm;		 // ת���ɵĺ�׺���ʽ
string stateSet;		 // ״̬��
map<char, int> priority; // ����������ȼ�
char start = '@';
map<char, vector<edge>> nfa;
vector<string> transferFunction; // ת�ƺ���

// ������������ȼ�
void setPrecedence()
{
	priority['*'] = 3;
	priority['.'] = 2;
	priority['|'] = 1;
}

// ��input.txt�ж�ȡ���ż�,����ʽ
void input()
{
	ifstream in("input.txt");
	if (!in)
	{
		cout << "�ļ�����" << endl;
		exit(1);
	}
	in >> symbolSet >> startNormalForm;
}

// �������ӷ�.
void toFullNormalForm()
{
	for (int i = 0; i < startNormalForm.length(); i++)
	{
		char tmp = startNormalForm[i];
		char next;
		if (i == startNormalForm.length() - 1)
		{
			next = '\0';
		}
		else
		{
			next = startNormalForm[i + 1];
		}
		if (((tmp != '(' && tmp != '.' && tmp != '|') || tmp == ')' || tmp == '*') && (next != ')' && next != '*' && next != '|' && next != '.' && next != '\0'))
		{
			suffixForm = suffixForm + tmp + '.';
		}
		else
		{
			suffixForm = suffixForm + tmp;
		}
	}
}

// ���ɺ�׺
void toSuffixForm()
{
	stack<char> cstack;
	for (int i = 0; i < suffixForm.length(); i++)
	{
		char tmp = suffixForm[i];
		if (tmp == '(')
			cstack.push(tmp);
		else if (tmp == ')')
		{
			while (cstack.top() != '(')
			{
				char t = cstack.top();
				cstack.pop();
				fullNormalForm = fullNormalForm + t;
			}
			cstack.pop();
		}
		else if (tmp == '|' || tmp == '*' || tmp == '.')
		{
			while (!cstack.empty())
			{
				char t = cstack.top();
				if (priority[t] >= priority[tmp])
				{
					fullNormalForm = fullNormalForm + t;
					cstack.pop();
				}

				else if (priority[t] < priority[tmp])
				{
					cstack.push(tmp);
					break;
				}
			}
			if (cstack.empty())
				cstack.push(tmp);
		}
		else
		{
			fullNormalForm = fullNormalForm + tmp;
		}
	}
	while (!cstack.empty())
	{
		char t = cstack.top();
		cstack.pop();
		fullNormalForm = fullNormalForm + t;
	}
}

// �ҵ���̬
char findFinalState(char a, set<char> x = {})
{
	if (nfa[a].empty())
		return a;
	else
	{
		x.insert(a);
		int i = 0;
		while (x.find(nfa[a][i].next) != x.end())
		{
			i++;
		}
		char t = findFinalState(nfa[a][i].next, x);
		return t;
	}
}

// ����׺���ʽת����nfa,���س�̬
char toNfa()
{
	stack<char> h;
	set<char> x;
	for (int i = 0; i < fullNormalForm.length(); i++)
	{
		char tmp = fullNormalForm[i];
		if (tmp == '.')
		{
			edge e;
			e.weight = '~';
			char t = h.top();
			h.pop();
			x.clear();
			char b = findFinalState(h.top());
			e.next = t;
			nfa[b].push_back(e);
		}
		else if (tmp == '|')
		{
			edge e;
			e.weight = '~';
			start++;
			char t = h.top();
			h.pop();
			x.clear();
			char b = findFinalState(t);
			e.next = t;
			nfa[start].push_back(e);
			e.next = start + 1;
			nfa[b].push_back(e);
			t = h.top();
			h.pop();
			x.clear();
			b = findFinalState(t);
			e.next = t;
			nfa[start].push_back(e);
			e.next = start + 1;
			nfa[b].push_back(e);
			h.push(start);
			stateSet = stateSet + start + char(start + 1);
			start++;
		}
		else if (tmp == '*')
		{
			edge e;
			e.weight = '~';
			start++;
			char t = h.top();
			h.pop();
			x.clear();
			char b = findFinalState(t);
			e.next = t;
			nfa[start].push_back(e);
			nfa[b].push_back(e);
			e.next = start + 1;
			nfa[b].push_back(e);
			nfa[start].push_back(e);
			h.push(start);
			stateSet = stateSet + start + char(start + 1);
			start++;
		}

		else
		{
			start++;
			edge e;
			e.weight = tmp;
			e.next = start + 1;
			nfa[start].push_back(e);
			h.push(start);
			stateSet = stateSet + start + char(start + 1);
			start++;
		}
	}
	return h.top();
}

// ���ת�ƺ�������
void getTransferFunction()
{
	for (auto inter = nfa.begin(); inter != nfa.end(); inter++)
	{
		for (int i = 0; i < inter->second.size(); i++)
		{
			string tmp;
			tmp = tmp + '(' + inter->first + ',' + inter->second[i].weight + ',' + inter->second[i].next + ')';
			transferFunction.push_back(tmp);
		}
	}
}
int main()
{
	setPrecedence();
	input();
	toFullNormalForm();
	toSuffixForm();
	char startState = toNfa();
	ofstream out("output.txt");
	out << stateSet << endl;
	out << symbolSet << endl;
	out << startState << endl;
	out << findFinalState(startState) << endl;
	getTransferFunction();
	for (int i = 0; i < transferFunction.size(); i++)
	{
		out << transferFunction[i] << endl;
	}
}
