#include <iostream>
#include <algorithm>
#include <stack>

int N;
int main()
{
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);

	std::cin >> N;
	std::stack<std::pair<int, int> > st;
	for (int i = 1; i <= N; i++)
	{
		int tmp;
		std::cin >> tmp;

		while (!st.empty() && st.top().second <= tmp)
			st.pop();
		
		if (!st.empty() && st.top().second > tmp)
			std::cout << st.top().first << " ";
		else if (st.empty())
			std::cout << "0 ";
		st.push({i, tmp});
	}
}