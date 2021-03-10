#include <stdlib.h> 
#include <stdio.h>
#include <time.h> 

#define RANGE 10000

typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

////////////////////////////////////////////////////////////////////////////////
void print_header(char *filename)
{
	printf("#! /usr/bin/env Rscript\n");
	printf("png(\"%s\", width=700, height=700)\n", filename);

	printf("plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer(void)
{
	printf("dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points(t_point *points, int num_point) {
	int i;
	printf("\n#points\n");
	for (i = 0 ; i<num_point ; i++)
		printf("points(%d, %d)\n", points[i].x, points[i].y);
}

/*
#line segments
segments(7107,2909,7107,2909)
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments(t_line *lines, int num_line){
	int i;
	printf("\n#line segments\n");
	for (i=0; i<num_line; i++)
		printf("segments(%d, %d, %d, %d)\n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
}

t_line *convex_hull(t_point *points, int num_point, int *num_line){
	*num_line = 0;
	int capacity = 10;
	t_line * f_lines;
	f_lines = (t_line *)malloc(capacity * sizeof(t_line));
	
	for(int i=0; i< num_point-1; i++){					//기준점이 되는 점
		for(int j=i+1; j<num_point; j++){				//기준점과 직선을 이룰 점
			int a = points[j].y - points[i].y;			//직선식의 계수,상수 구하기
			int b = points[i].x - points[j].x;
			int c = (points[i].x * points[j].y) - (points[i].y * points[j].x);
			
			int checker = 0;							//각 점들의 위치(양/음수) 확인을 위한 변수
			int value = 0;
			int k; 
			for(k=0; k<num_point; k++) {				//직선을 기준으로 양/음수 여부를 확인할 점들
				value = (a * points[k].x) + (b * points[k].y) - c;
				if (k == 0) {							//처음 한 점을 대입했을 때의 양수/음수 여부 확인
					if (value < 0)
						checker = -1;
					else if (value > 0)
						checker = 1;
					else
						checker = 0;
				}
				else {									//앞서 확인한 양/음수 여부와 나머지 점 비교
					if (checker == -1){					//앞선 점을 대입한 값이 음수였을 때
							if (value <= 0);
							else 						//한 점이라도 일치하지 않을 시 직선식 다시 설정					
								break;		
					}
					else if (checker == 1){				//앞선 점을 대입한 값이 양수였을 때
							if (value >= 0);
							else 									
								break;							
					}
					else { 								//checker ==0 앞서 확인한 점이 직선 위에 있는 점이었을 때(직선식에 대입한 값이 0일 때), 현재 점이 양수인지 음수인지 확인
						if (value < 0)
							checker = -1;
						else if (value > 0)
							checker = 1;
						else
							checker = 0;
					}
				}
				
			}
			if (k >= num_point) {						//해당 직선을 기준으로 나머지 점들이 다 한쪽에 몰려있는 것을 확인했다면, 해당 직선을 배열에 추가
				f_lines[*num_line].from.x = points[i].x;
				f_lines[*num_line].from.y = points[i].y;
				f_lines[*num_line].to.x = points[j].x;
				f_lines[*num_line].to.y = points[j].y;
				(*num_line)++;
				if (*num_line == capacity){				//선 개수가 10개가 넘어가면 realloc
					capacity *= 2;
					f_lines = (t_line *)realloc(f_lines, capacity * sizeof(t_line));
				}
			}	
		}
	}
	return f_lines;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int x, y;
	int num_point; 
	int num_line; 

	if (argc != 2) 
	{
		printf("%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi(argv[1]); 
	if (num_point <= 0) 
	{
		printf("The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points = (t_point *)malloc(num_point * sizeof(t_point)); 

	t_line *lines; 

	// making n points
	srand(time(NULL));
	for (int i = 0; i < num_point; i++) 
	{
		x = rand() % RANGE + 1; 
		y = rand() % RANGE + 1; 

		points[i].x = x; 
		points[i].y = y;
	}

	fprintf(stderr, "%d points created!\n", num_point); 

	print_header("convex.png");

	print_points(points, num_point); 

	lines = convex_hull( points, num_point, &num_line);

	fprintf( stderr, "%d lines created!\n", num_line); 

	print_line_segments( lines, num_line); 

	print_footer();

	free(points);
	free(lines);

	return 0;
}
