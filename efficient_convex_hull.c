#include <stdlib.h> // atoi, rand, qsort, malloc
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> //time
#include <math.h>	//fabs

#define RANGE 10000

typedef struct		//점구조체
{
	int x;
	int y;
} t_point;

typedef struct		//선구조체
{
	t_point from;
	t_point to;
} t_line;

float distance(float a, float b, float c, t_point p);
void separate_points(t_point *points, int num_point, t_point from, t_point to, t_point *s1, t_point *s2, int *n1, int *n2);
t_line *upper_hull(t_point *points, int num_point, t_point p1, t_point pn, t_line *lines, int *num_line, int *capacity/*, int count*/);

////////////////////////////////////////////////////////////////////////////////
// function declaration
// 점들의 집합(points; 점의 수 num_point)에서 점 p1과 점 pn을 잇는 직선의 upper hull을 구하는 함수 (재귀호출)
// [output] lines: convex hull을 이루는 선들의 집합
// [output] num_line: 선의 수
// [output] capacity: lines에 할당된 메모리의 용량 (할당 가능한 선의 수)
// return value: 선들의 집합(lines)에 대한 포인터
t_line *upper_hull(t_point *points, int num_point, t_point p1, t_point pn, t_line *lines, int *num_line, int *capacity/*, int count*/) {
	//printf("\nrecursion!!\n");
	//(s1)에 남은 점이 없을 때(기준되는 직선 안쪽으로 모든 점이 들어올 때) 해당 직선을 lines에 저장
	//&& count > 0
	if (num_point == 0) {
		lines[*num_line].from = p1;
		lines[*num_line].to = pn;
		(*num_line)++;
		if (*num_line == *capacity) {				//선 개수가 10개가 넘어가면 realloc
			(*capacity) *= 2;
			lines = (t_line *)realloc(lines, *capacity * sizeof(t_line));
		}
		//count++;
		return lines;
	}
	else if (num_point > 0) {
		//직선에서 가장 멀리 떨어진 pmax구하기
		float dis;
		float a, b, c;
		float max = -1;
		int num = -1;
		t_point pmax;
		a = pn.y - p1.y;
		b = p1.x - pn.x;
		c = (p1.x * pn.y) - (p1.y * pn.x);
		for (int i = 0; i < num_point; i++) {
			dis = distance(a, b, c, points[i]);
			if (dis > max && dis > 0) {	//지금 점이 p1, pn이 아니고, 여태 최대 거리값보다 크면
				max = dis;		//지금 값을 max로,
				num = i;		//지금 인덱스를 num에 저장
			}
		}
		if (num >= 0) {				//pmax구해졌다면
			pmax = points[num];		//pmax설정
			//p1 -> pmax 직선을 기준으로하는 직선식에 다른 점들을 대입해서
			t_point *s11 = (t_point *)malloc(sizeof(t_point) * num_point);	//s11을 위한 공간. 전체 점의 개수만큼 메모리를 할당해준다. 
			assert(s11 != NULL);
			int num11 = 0;
			t_point *s12 = (t_point *)malloc(sizeof(t_point) * num_point);	//s12을 위한 공간. 전체 점의 개수만큼 메모리를 할당해준다. 
			assert(s12 != NULL);
			int num12 = 0;
			t_point *s_trash = (t_point *)malloc(sizeof(t_point) * num_point);	//s12을 위한 공간. 전체 점의 개수만큼 메모리를 할당해준다. 
			assert(s_trash != NULL);
			int st = 0;
			t_point *s_trash2 = (t_point *)malloc(sizeof(t_point) * num_point);	//s12을 위한 공간. 전체 점의 개수만큼 메모리를 할당해준다. 
			assert(s_trash != NULL);
			int st2 = 0;
			//구한pmax를 기준으로 좌우 분리
			separate_points(points, num_point, p1, pmax, s11, s_trash, &num11, &st);

			//pmax기준으로, 직선 p1->pmax기준으로 다시 호출 
			lines = upper_hull(s11, num11, p1, pmax, lines, num_line, capacity/*, count*/);
			//printf("leftout\n");

			separate_points(points, num_point, pmax, pn, s12, s_trash2, &num12, &st2);
			//좌변 구하는거 빠져나왔다면 우변 구하도록
			lines = upper_hull(s12/*points*/, num12/*num_point*/, pmax, pn, lines, num_line, capacity/*, count*/);	//오른쪽직선기준
			//printf("rightout\n");

			free(s11);
			free(s12);
			free(s_trash);
			free(s_trash2);
		}
		else
			return lines;
	}
	return lines;
}
//인자로 점의 집합 points, 점의 수 num_point받아서 점 두개에 대한 upper hull구하기
//upper hull안에 upper hull을 구하는 재귀함수
//결과물은 lines와 num_line에 저장. upper hull이 구해지면 lines에 저장되는 것 num_line은 기존에 3개가 있었는데 2개가 생겼으면 5개로 바꿔줘야함. int pointer로 선언됨
//lines를 위해 할당한 용량 사이즈 capacity. t_line 사이즈에 할당된 사이즈. 초기값은 10개정도 잡는데, 하다보면 넘어갈때가 있으니 넘어갈때 capacity 수정하며 메모리 할당하고 새로운 라인추가.
//리턴값으로 t_line포인터를 주는데 t_line 포인터로 lines를 왜 또 받냐? (함수 리턴값이랑 인자랑 왜 똑같이 받냐?)
//내부에서 lines에 대한 메모리를 재할당하면 주소가 바뀔 수 있음. lines가 가리키는 주소가. 그래서 내부적으로 재할당한 주소를 다시 받아서 리턴하게
//시작하자마자 주어진 점(p1->pn)을 연결하는 직선과 가장 거리가 먼 점 하나 pmax찾을 때 쓰는 distance 함수. (인자 전달 잘 해줘야함)
//재귀함수라서 recursion을 빠져나오기 위한 base case: 주어진 점num_point가 0이 될 때, (s1에 있는 점의 개수==0인 상태)p1과 pn을 잇는 선 자체가 upper hull에 포함됨. 
//두 점을 잇는 직선과 points에 있는 점들 중 거리가 최대인 점을 찾을 때, 교재에서는 값이 같으면 각에 따라서 선택하라고 했지만, 안해도 잘 돌아가더라. 




// distance = |ax1+by1-c| / sqrt(a^2 + b^2)
// 실제로는 sqrt는 계산하지 않음
// return value: 직선과 점 사이의 거리 (분모 제외)
float distance(float a, float b, float c, t_point p) {
	float dis;
	dis = (a * p.x) + (b * p.y) - c;
	dis = fabs(dis);
	dis = dis / ((a*a) + (b*b));

	return dis;
}
//직선과 점 사이의 길이 구하는 함수
//직선은 직선 방정식에 대한 파라미터 a,b,c로 줌. a,b,c 값을 알면 직선을 알 수 있다.
//그 직선과 점p 사이의 거리값
//p가 x1, y1이라고 한다면, ax1+by1-c의 절대값 나누기 (a의 제곱 + b의 제곱)의 루트값인데,
//루트는 사용하지 않아도 상관없음. 어차피 가장 큰 값을 알면 되는 거니까. 상대적으로만 알면 되는 것



// 두 점(from, to)을 연결하는 직선(ax + by - c = 0)으로 n개의 점들의 집합 s(점의 수 num_point)를 s1(점의 수 n1)과 s2(점의 수 n2)로 분리하는 함수
// [output] s1 : 직선의 upper(left)에 속한 점들의 집합 (ax+by-c < 0)
// [output] s2 : lower(right)에 속한 점들의 집합 (ax+by-c > 0)
// [output] n1 : s1 집합에 속한 점의 수
// [output] n2 : s2 집합에 속한 점의 수
void separate_points(t_point *points, int num_point, t_point from, t_point to, t_point *s1, t_point *s2, int *n1, int *n2) {
	float a, b, c;
	a = to.y - from.y;
	b = from.x - to.x;
	c = (from.x * to.y) - (from.y * to.x);
	for (int i = 0; i < num_point; i++) {
		if ((a * points[i].x) + (b * points[i].y) - c < 0 && ((points[i].x != from.x || points[i].y != from.y) && (points[i].x != to.x || points[i].y != to.y))) {	//직선의 마이너스(왼쪽) 영역->s1
			s1[*n1] = points[i];
			(*n1)++;
		}
		else if ((a * points[i].x) + (b * points[i].y) - c > 0 && ((points[i].x != from.x || points[i].y != from.y) && (points[i].x != to.x || points[i].y != to.y))) {	//직선의 플러스(오른쪽) 영역->s2
			s2[*n2] = points[i];
			(*n2)++;
		}
		//else	//직선 위에 있는 점
	}
}
//점의 집합이 주어지고, 점의 갯수 주어질 때 이 점들을 두 점 from-to연결하는 직선에 대한 upper에 속하는 점들과 lower에 속하는 점들을 찾는.
//upper점들을 s1에, lower를 s2에
//upper/lower어케아냐? 직선에 넣어서 (ax + by - c)에 넣어서 나오는 값이 마이너스냐 플러스냐. 마이너스면 upper s1, 플러스면 lower s2에 넣으면 됨
//결과는 s1, s2, s1에 속하는 점들의 갯수 n1, s2에 속하는 점들의 갯수 n2저장

//구현할 때 자기 자신의 점. from->to에서 왼/오 보는데 from자신과 to자신은 s1과 s2에 속하지 않도록. (아마 s1,s2에 집어넣을 때 if ==from || ==to 하면 안넣게 해야할듯)



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
// qsort를 위한 비교 함수
//S를 x좌표 기준으로 정렬. 맨 왼쪽과 맨 오른쪽 포인트 찾기 위해 정렬하는 과정. 
//qsort를 사용하기 위해서는 비교함수를 써야한다. 점들의 구조체배열을 정렬하는 것
int cmp_x(const void *p1, const void *p2)
{
	t_point *p = (t_point *)p1;
	t_point *q = (t_point *)p2;

	float diff = p->x - q->x;

	return ((diff >= 0.0) ? ((diff > 0.0) ? +1 : 0) : -1);
}

////////////////////////////////////////////////////////////////////////////////
void print_points(t_point *points, int num_point)
{
	int i;
	printf("\n#points\n");

	for (i = 0; i < num_point; i++)
		printf("points(%d,%d)\n", points[i].x, points[i].y);
}

////////////////////////////////////////////////////////////////////////////////
void print_line_segments(t_line *lines, int num_line)
{
	int i;

	printf("\n#line segments\n");

	for (i = 0; i < num_line; i++)
		printf("segments(%d,%d,%d,%d)\n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
}

////////////////////////////////////////////////////////////////////////////////
// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of lines
// return value : pointer of set of line segments that forms the convex hull
t_line *convex_hull(t_point *points, int num_point, int *num_line)
{
	int capacity = 10;	//초기화. 우선 10개 공간

	t_line *lines = (t_line *)malloc(capacity * sizeof(t_line));
	*num_line = 0;	//저장된거 없으니까

	// s1: set of points
	t_point *s1 = (t_point *)malloc(sizeof(t_point) * num_point);	//s1을 위한 공간. 전체 점의 개수만큼 메모리를 할당해준다. 
	assert(s1 != NULL);

	// s2: set of points
	t_point *s2 = (t_point *)malloc(sizeof(t_point) * num_point);	//s2을 위한 공간. 전체 점의 개수만큼 메모리를 할당해준다. 
	assert(s2 != NULL);

	int n1, n2; // number of points in s1, s2, respectively
	n1 = 0;
	n2 = 0;

	// x 좌표에 따라 정렬된 점들의 집합이 입력된 경우
	// points[0] : leftmost point (p1)
	// points[num_point-1] : rightmost point (pn)

	// 점들을 분리 (양 극단의 extreme포인트로 나누는데, 가장 왼쪽은 points[0]에, 가장 오른쪽은 points[num_point-1]에 저장되어있음)
	separate_points(points, num_point, points[0], points[num_point - 1], s1, s2, &n1, &n2);

	// upper hull을 구한다.
	lines = upper_hull(s1, n1, points[0], points[num_point - 1], lines, num_line, &capacity/*, 0*/);		//s1집합안에서의 upperhull구하고
	lines = upper_hull(s2, n2, points[num_point - 1], points[0], lines, num_line, &capacity/*, 0*/);		//s2의 lower hull은 연결하는 직선의 점 순서를 반대로 해서 upper hull을 구하면 된다. 그렇게해서 계속 lines에 추가해주면 num_line도 바뀌는데? 얘는 포인터라 계속 똑같이 써도 되는거ㅇㅇ capacity는 int라서 &로 쓴 것

	free(s1);
	free(s2);

	return lines;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
	float x, y;
	int num_point; // number of points

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

	t_point *points;		//구조체 선언	
	points = (t_point *)malloc(sizeof(t_point) * num_point);
	assert(points != NULL);

	// making points
	srand(time(NULL));	//난수 초기화
	for (int i = 0; i < num_point; i++)	//난수발생
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;

		points[i].x = x;
		points[i].y = y;
	}

	fprintf(stderr, "%d points created!\n", num_point);

	// sort the points by their x coordinate
	qsort(points, num_point, sizeof(t_point), cmp_x);//qsort써서 정렬 (정렬하고자하는 데이터, 개수, 사이즈, 정렬하는 기준)


	print_header("convex.png");

	print_points(points, num_point);

	// convex hull algorithm (과제1과 똑같음!)
	int num_line;
	t_line *lines = convex_hull(points, num_point, &num_line);

	fprintf(stderr, "%d lines created!\n", num_line);

	print_line_segments(lines, num_line);

	print_footer();

	free(points);
	free(lines);

	return 0;
}
