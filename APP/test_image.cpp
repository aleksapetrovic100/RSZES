#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <regex>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define x_max 640
#define y_max 480

#define BLUE 0x001F
#define GREEN 0x07E0
#define RED 0xF800
#define BLACK 0x0000
#define YELLOW (RED + GREEN)
#define MAX_PKT_SIZE (640*480*4)

using namespace std;

unsigned int get_colour(string colour)
{
	if (colour == "BLACK")
		return BLACK;
	else if (colour == "RED")
		return RED;
	else if (colour == "GREEN")
		return GREEN;
	else if (colour == "BLUE")
		return BLUE;
	else if (colour == "YELLOW")
		return YELLOW;

	cout<<"colour invalid. "<<endl;
	exit(3);
}
void bckg(unsigned int color, unsigned int *picture)
{
	for(int i = 0; i <= x_max* y_max; i++)
		picture[i] = color;
}
void line_h(int x1, int x2, int y, unsigned int color, unsigned int *picture)
{
	for(int i = x1; i <= x2; i++)
		picture[y*x_max + i] = color;
}
void line_v(int x, int y1, int y2, unsigned int color, unsigned int *picture)
{
	for(int i = y1; i <= y2; i++)
		picture[i*x_max + x] = color;
}
void rectangle(int x1, int x2, int y1, int y2, unsigned int color, unsigned int *picture)
{
	for(int i = x1; i <= x2; i++)
		for(int j = y1; j <= y2; j++)
			picture[j*x_max + i] = color;
}
void command_execution(string command_line, unsigned int *picture)
{
	regex cmd("(BCKG|LINE_V|LINE_H|RECT):");
	smatch match;

	if(regex_search(command_line, match, cmd))
	{
		if(match[1] == "BCKG")
		{
			regex color("(BLACK|RED|GREEN|BLUE|YELLOW)");
			if(regex_search(command_line, match, color))
			{
				cout << "BCKG is: " << match[1] << endl;
				bckg(get_colour(match[1]), picture);
			}
		}
		if(match[1] == "LINE_H")
		{
			regex h_line("(\\d+),(\\d+),(\\d+);(BLACK|RED|GREEN|BLUE|YELLOW)");
			if(regex_search(command_line, match, h_line))
			{
				cout << "LINE_H is" << match[1] << match[2] << match[3] << match[4] << endl; 
				line_h(stoi(match[1]), stoi(match[2]), stoi(match[3]), get_colour(match[4]), picture);
			}
		}
		if(match[1] == "LINE_V")
		{
			regex v_line("(\\d+),(\\d+),(\\d+);(BLACK|RED|GREEN|BLUE|YELLOW)");
			if(regex_search(command_line, match, v_line))
			{
				cout << "LINE_V is" << match[1] << match[2] << match[3] << match[4] << endl; 
				line_v(stoi(match[1]), stoi(match[2]), stoi(match[3]), get_colour(match[4]), picture);
			}
		}
		if(match[1] == "RECT")
		{
			regex rect("(\\d+),(\\d+),(\\d+),(\\d+);(BLACK|RED|GREEN|BLUE|YELLOW)");
			if(regex_search(command_line, match, rect))
			{
				cout << "RECT is" << match[1] << match[2] << match[3] << match[4] << match[5] << endl; 
				rectangle(stoi(match[1]), stoi(match[2]), stoi(match[3]), stoi(match[4]), get_colour(match[5]), picture);
			}
		}
	}
	else
		cout << "No command found" << endl;
}

int main(int argc, char *argv[])
{
	FILE *command_file;
	char *command_line;
	unsigned int *picture;
	size_t nbytes = 50;

	if(argc < 2)
	{
		cout << "Put command file in .txt format" << endl;
		return -1;	
	}
	else
    	command_file = fopen(argv[1], "r");

    	if(command_file == NULL)
	{
        puts("File could not be opened.");
   		return -1;
	}
    	puts("File opened!");
	command_line = (char*) malloc(nbytes + 1);

	

	int fd;
	fd = open("/dev/vga_dma", O_RDWR|O_NDELAY);
	if (fd < 0)
	{
		printf("Cannot open /dev/vga for write\n");
		return -1;
	}

	picture = (unsigned int*)mmap(0,640*480*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	while(getline(&command_line, &nbytes, command_file) >= 0)
	{
		puts("Command is:");
		puts(command_line);
		command_execution(command_line, picture);
	}
    	fclose(command_file);

	munmap(picture, MAX_PKT_SIZE);
	close(fd);
	if (fd < 0)
	{
		printf("Cannot close /dev/vga for write\n");
		return -1;
	}

    return 0;
}


