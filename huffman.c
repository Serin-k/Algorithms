//#define BINARY_MODE	

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct Node
{
	unsigned char	data;	// 문자	(알파벳)
	int		freq; 			// 빈도
	struct	Node *left;		// 왼쪽 서브트리 포인터
	struct	Node *right; 	// 오른쪽 서브트리 포인터
} tNode;

////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	int	last;		// 힙에 저장된 마지막 element의 index
	int	capacity;	// heapArr의 크기
	tNode **heapArr;
} HEAP;

HEAP *heapCreate(int capacity);
static void _reheapUp(HEAP *heap, int index);
int heapInsert(HEAP *heap, tNode *data);
static void _reheapDown(HEAP *heap, int index);
tNode *heapDelete(HEAP *heap);
void heapDestroy(HEAP *heap);
int read_chars(FILE *fp, int *ch_freq);
void free_huffman_code(char *codes[]);
tNode *newNode(char data, int freq);
tNode *make_huffman_tree(int *ch_freq);
void traverse_tree(tNode *root, char *code, int depth, char *codes[]);
void make_huffman_code(tNode *root, char *codes[]);
void destroyTree(tNode *root);
int encoding(char *codes[], FILE *infp, FILE *outfp);
//int encoding_binary(char *codes[], FILE *infp, FILE *outfp);
void decoding(tNode *root, FILE *infp, FILE *outfp);
//void decoding_binary(tNode *root, FILE *infp, FILE *outfp);
//////////////////////////////////////////////////////////////////////////////////////
int read_chars(FILE *fp, int *ch_freq) {
	int count = 0;
	int tmp;
	//int temp = 0;

	while ((tmp = fgetc(fp)) != EOF) {		//문자 하나씩 읽어와서
		ch_freq[tmp]++;						//빈도수 증가
		count++;							//비트수연산
	}
	return count;
}

void free_huffman_code(char *codes[]) {
	for (int i = 0; i < 256; i++)
		free(codes[i]);
}

tNode *newNode(char data, int freq) {
	tNode *newPtr;
	newPtr = (tNode*)malloc(sizeof(tNode));	//할당
	newPtr->data = data;	//데이터 입력
	newPtr->freq = freq;	//빈도수 입력
	newPtr->left = NULL;	//자식노드 포인터는 NULL로 초기화
	newPtr->right = NULL;

	return newPtr;
}

tNode *make_huffman_tree(int *ch_freq) {
	int capacity = 256;
	int frequency, success;
	char data;
	HEAP *Heap;
	tNode *Node, *firstNode, *secondNode;
	tNode *sumNode;
	Heap = heapCreate(capacity);				//1.빈 노드 생성
	for (int i = 0; i < capacity; i++) {
		frequency = ch_freq[i];
		data = i;
		Node = newNode(data, frequency);		//2.데이터와 빈도가 채워진 개별노드
		success = heapInsert(Heap, Node);		//3.힙에 삽입
		if (success);
		else {
			printf("Heap Insertion Fail!\n");
			break;
		}
	}

	while (1) {
		firstNode = heapDelete(Heap);				//4.최소값 트리 2개 추출
		secondNode = heapDelete(Heap);
		if (secondNode == NULL) break;

		sumNode = (tNode*)malloc(sizeof(tNode));
		frequency = (firstNode->freq) + (secondNode->freq);	//5.두 트리 결합 후 새 노드에 추가
		data = 0;
		sumNode->freq = frequency;
		sumNode->data = data;
		sumNode->left = firstNode;
		sumNode->right = secondNode;

		success = heapInsert(Heap, sumNode);			//6.새 트리를 힙에 삽입
		if (success);
		else {
			printf("Heap Insertion Fail!\n");
			break;
		}
	}
	heapDestroy(Heap);
	
	return firstNode;								//트리 루트노드의 포인터 리턴

}

void traverse_tree(tNode *root, char *code, int depth, char *codes[]) {
	if (!root->left && !root->right) {		//리프노드의 경우, 코드 저장
		char *ptr;
		ptr = strdup(code);
		codes[root->data] = ptr;
		code[depth-1] = '\0';				//출력한 자리(비트)는 NULL값으로 초기화
		return;
	}
	code[depth] = '0';
	traverse_tree(root->left, code, depth + 1, codes);		//왼쪽으로 내려가며 0저장
	code[depth] = '1';
	traverse_tree(root->right, code, depth + 1, codes);		//오른쪽으로 내려가며 0저장
	code[depth] = '\0';

	return;
}

void make_huffman_code(tNode *root, char *codes[]) {
	int depth = 0;
	char code[300] = { 0, };
	char **init_codes = codes;
	tNode *init_root = root;
	char *init_code = code;
	
	traverse_tree(root, code, depth, codes);		//codes에 문자별 코드를 저장
	return;
}

void destroyTree(tNode *root) {
	if (!(root->left) && !(root->right)) {		//리프노드부터 free
		free(root->left);
		free(root->right);
		free(root);
		return;
	}
	destroyTree(root->left);		//왼쪽으로 내려가며
	destroyTree(root->right);		//오른쪽으로 내려가며
	free(root);
	
	return;
}

int encoding(char *codes[], FILE *infp, FILE *outfp) {
	int tmp;
	int count = 0;
	while ((tmp = fgetc(infp)) != EOF) {		//한글자 읽어와서
		fputs(codes[tmp], outfp);				//그 글자에 해당하는 코드 적어넣음
		count += strlen(codes[tmp]);			//비트수연산
	}
	count /= 8;
	
	return count;
}
//int encoding_binary(char *codes[], FILE *infp, FILE *outfp);

void decoding(tNode *root, FILE *infp, FILE *outfp) {
	int tmp;
	tNode *init_root = root;
	while ((tmp = fgetc(infp)) != EOF) {
		if (tmp == '0')
			root = root->left;
		else if (tmp == '1')
			root = root->right;
			
		if (!(root->left) && !(root->right)) {		//리프노드를 만났을 경우 출력
			fputc(root->data, outfp);
			root = init_root;
		}			
	}
}
//void decoding_binary(tNode *root, FILE *infp, FILE *outfp);

////////////////////////////////////////////////////////////////////////////////
// 문자별 빈도 출력 (for debugging)
void print_char_freq(int *ch_freq)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		printf("%d\t%d\n", i, ch_freq[i]); // 문자인덱스, 빈도
	}
}

////////////////////////////////////////////////////////////////////////////////
// 문자별 빈도를 이용하여 허프만 트리와 허프만 코드를 생성
// return value : 허프만 트리의 root node
tNode *run_huffman(int *ch_freq, char *codes[])
{
	tNode *root;
	root = make_huffman_tree(ch_freq);

	make_huffman_code(root, codes);

	return root;
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드를 화면에 출력
void print_huffman_code(char *codes[])
{
	int i;

	for (i = 0; i < 256; i++)
	{
		printf("%d\t%s\n", i, codes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// argv[1] : 입력 텍스트 파일
// argv[2] : 바이너리 코드 (encoding 결과) 
// argv[3] : 출력 텍스트 파일 (decoding 결과)
int main( int argc, char **argv)
{
	FILE *fp;
	FILE *infp, *outfp;
	int ch_freq[256] = {0,}; // 문자별 빈도
	char *codes[256]; // 문자별 허프만 코드 (ragged 배열)
	tNode *huffman_tree; // 허프만 트리
	
	if (argc != 4)
	{
		fprintf( stderr, "%s input-file encoded-file decoded-file\n", argv[0]);
		return 1;
	}

	////////////////////////////////////////
	// 입력 텍스트 파일
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 1;
	}

	// 텍스트 파일로부터 문자별 빈도 저장
	int num_bytes = read_chars( fp, ch_freq);	//전체적으로 몇바이트를 읽었는지도 저장

	fclose( fp);

	// 문자별 빈도 출력 (for debugging)	코멘트처리해서제출
	//print_char_freq( ch_freq);
	
	// 허프만 코드/트리 생성
	huffman_tree = run_huffman( ch_freq, codes);	//루트를 가리키는 노드포인터
	
	// 허프만 코드 출력 (stdout)
	print_huffman_code( codes);

	////////////////////////////////////////
	// 입력: 텍스트 파일
	infp = fopen( argv[1], "rt");
	
#ifdef BINARY_MODE
	// 출력: 바이너리 코드
	outfp = fopen( argv[2], "wb");
#else
	// 출력: 바이너리 코드
	outfp = fopen( argv[2], "wt");
#endif

	// 허프만코드를 이용하여 인코딩(압축)
#ifdef BINARY_MODE
	int encoded_bytes = encoding_binary( codes, infp, outfp);
#else
	int encoded_bytes = encoding( codes, infp, outfp);
#endif

	// 허프만 코드 메모리 해제
	free_huffman_code( codes);
	
	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	// 입력: 바이너리 코드
#ifdef BINARY_MODE
	infp = fopen( argv[2], "rb");
#else
	infp = fopen( argv[2], "rt");
#endif

	// 출력: 텍스트 파일
	outfp = fopen( argv[3], "wt");

	// 허프만 트리를 이용하여 디코딩
#ifdef BINARY_MODE
	decoding_binary( huffman_tree, infp, outfp);
#else
	decoding( huffman_tree, infp, outfp);	//입력파일과 동일한 파일내용이 저장되어야함
#endif

	// 허프만 트리 메모리 해제
	destroyTree( huffman_tree);

	fclose( infp);
	fclose( outfp);

	////////////////////////////////////////
	printf( "# of bytes of the original text = %d\n", num_bytes);	//압축률 저장
	printf( "# of bytes of the compressed text = %d\n", encoded_bytes);
	printf( "compression ratio = %.2f\n", ((float)num_bytes - encoded_bytes) / num_bytes * 100);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 힙의 내용 출력 (for debugging)
void heapPrint(HEAP *heap)
{
	int i;
	tNode **p = heap->heapArr;
	int last = heap->last;

	for (i = 0; i <= last; i++)
	{
		printf("[%d]%c(%6d)\n", i, p[i]->data, p[i]->freq);
	}
	printf("\n");
}

////////////////////////////////////////////////////////////////////////////////
// 힙 생성
// 배열을 위한 메모리 할당 (capacity)
// last = -1
HEAP *heapCreate(int capacity)
{
	HEAP *heap;

	heap = (HEAP *)malloc(sizeof(HEAP));
	if (!heap) return NULL;

	heap->last = -1;
	heap->capacity = capacity;
	heap->heapArr = (tNode **)malloc(sizeof(tNode *) * capacity);
	if (heap->heapArr == NULL)
	{
		fprintf(stderr, "Error : not enough memory!\n");
		free(heap);
		return NULL;
	}
	return heap;
}

////////////////////////////////////////////////////////////////////////////////
// 최소힙 유지
static void _reheapUp(HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	int parent;

	while (1)
	{
		if (index == 0) return; // root node

		parent = (index - 1) / 2;

		if (arr[index]->freq < arr[parent]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[parent];
			arr[parent] = temp;

			index = parent;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 힙에 원소 삽입
// _reheapUp 함수 호출
int heapInsert(HEAP *heap, tNode *data)
{
	if (heap->last == heap->capacity - 1)
		return 0;

	(heap->last)++;
	(heap->heapArr)[heap->last] = data;

	_reheapUp(heap, heap->last);

	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// 최소힙 유지
static void _reheapDown(HEAP *heap, int index)
{
	tNode **arr = heap->heapArr;
	tNode *leftData;
	tNode *rightData;
	int noright = 0;
	int largeindex; // index of left or right child with large key

	while (1)
	{
		if ((index * 2 + 1) > heap->last) return; // leaf node (there is no left subtree)

		leftData = arr[index * 2 + 1];
		if (index * 2 + 2 <= heap->last) rightData = arr[index * 2 + 2];
		else noright = 1;

		if (noright || leftData->freq < rightData->freq) largeindex = index * 2 + 1; // left child
		else largeindex = index * 2 + 2; // right child

		if (arr[index]->freq > arr[largeindex]->freq) // exchange (for minheap)
		{
			tNode *temp = arr[index];
			arr[index] = arr[largeindex];
			arr[largeindex] = temp;

			index = largeindex;

			noright = 0;
		}
		else return;
	}
}

////////////////////////////////////////////////////////////////////////////////
// 최소값 제거
// _reheapDown 함수 호출
tNode *heapDelete(HEAP *heap)
{
	if (heap->last == -1) return NULL; // empty heap

	tNode *data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];

	(heap->last)--;

	_reheapDown(heap, 0);

	return data;
}

////////////////////////////////////////////////////////////////////////////////
// 힙 메모리 해제
void heapDestroy(HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
}
