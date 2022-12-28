#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>

// shared variables
const int COUNT = 1e2;
clock_t time_slice[COUNT];
bool runner_stop = false;

DWORD WINAPI calc()
{
	
	int cnt = 0;
	clock_t start;
	while (cnt <= COUNT)
	{
		start = clock();
		Sleep(0);
		time_slice[cnt ++] = clock() - start;
	}
	runner_stop = true; // signal runner to stop

	ExitThread(0);
}

DWORD WINAPI runner()
{
	while (!runner_stop) ;

	ExitThread(0);
}

int main()
{
	HANDLE cur_h = GetCurrentThread();
	SetThreadAffinityMask(cur_h, 1 << 14);

	HANDLE ca, ru;

	ru = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runner, NULL, 0, NULL);
	SetThreadAffinityMask(ru, 1 << 15);

	Sleep(1); // ensure that runner go into the loop

	ca = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)calc, NULL, 0, NULL);
	SetThreadAffinityMask(ca, 1 << 15);

	WaitForSingleObject(ru, INFINITE);
	WaitForSingleObject(ca, INFINITE);

	// print result
	std::ofstream dst_file;
	dst_file.open("result.txt", std::ios::out);
	if (dst_file)
	{
		int avg = 0;
		for (int i = 0; i < COUNT; i++)
		{
			avg += time_slice[i];
			dst_file << time_slice[i] << '\n';
		}
		avg /= COUNT;
		dst_file << avg << '\n';
		dst_file.close();
	}
	else
	{
		std::cout << "error!" << std::endl;
	}

	ExitThread(0);
}