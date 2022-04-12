#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/*
Something like Python
>> y = 2
>> z = 2
>> x = 3*y + 4/(2*z)

*/

/*
the only type: integer
everything is an expression

	statement 	:= END | expr END
	expr      	:= term expr_tail
	expr_tail 	:= ADD_SUB_AND_OR_XOR term expr_tail | NiL
	term      	:= factor term_tail
	term_tail 	:= MUL_DIV factor term_tail|NiL
	factor    	:= INT | ADD_SUB INT | ADD_SUB ID | ID ASSIGN expr| ID | LPAREN expr RPAREN

*/

#define MAXLEN 256
#define TBLSIZE 64
#define MAX_R 8
int used_r[MAX_R];
int is_assed;

typedef enum { MISPAREN, NOTNUMID, NOTFOUND, RUNOUT,MULASS,TBD } ErrorType;
typedef enum {
	UNKNOWN, END, INT, ID, ORANDXOR, ADDSUB, MULDIV, ASSIGN,
	LPAREN, RPAREN, ENDFILE
} TokenSet;

typedef struct {
	char name[MAXLEN];
	int val;
	int assed;
} Symbol;

Symbol table[TBLSIZE];

int x_assed;
int y_assed;
int z_assed;

int sbcount = 3;

typedef struct _Node {
	char lexeme[MAXLEN];
	TokenSet data;
	int val;
	struct _Node *left, *right;
} BTNode;

TokenSet lookahead = UNKNOWN;
char lexeme[MAXLEN];

BTNode* factor(void);
BTNode* term(void);
BTNode* term_tail(BTNode *);
BTNode* expr(void);
BTNode* expr_tail(BTNode *);
void statement(void);
char* getLexeme(void);
TokenSet getToken(void);
void advance(void);
void error(ErrorType);
int match(TokenSet);
int evaluateTree(BTNode *);
void printPrefix(BTNode *);
void freeTree(BTNode *);
BTNode* makeNode(TokenSet, const char *);
int getval(void);
int setval(char *, int);


void simpily(BTNode *root) {
	if (root == NULL || root->data == INT || root->data == ID)
		return;
	simpily(root->left);
	simpily(root->right);
	if (root->left->data == ID && root->right->data == ID) {
		if (strcmp(root->left->lexeme,root->right->lexeme)==0) {
			root->val = 0;
			root->left = NULL;
			root->right = NULL;
		}
	}
	if (root->left->lexeme[0] == 'x' || root->left->lexeme[0] == 'y' || root->left->lexeme[0] == 'z')
		return;
	if (root->right->lexeme[0] == 'x' || root->right->lexeme[0] == 'y' || root->right->lexeme[0] == 'z')
		return;

	root->left = NULL;
	root->right = NULL;





	

}


int is_x(BTNode* root) {
	if (strcmp(root->lexeme, "x") == 0)return 1;
	else return 0;
}
int is_y(BTNode* root) {
	if (strcmp(root->lexeme, "y") == 0)return 1;
	else return 0;
}
int is_z(BTNode* root) {
	if (strcmp(root->lexeme, "z") == 0)return 1;
	else return 0;
}

int main(void)
{
	/*
	freopen( "input.in" , "r" , stdin ) ;
	freopen( "output.out" , "w" , stdout ) ;
	*/


	table[0].name[0] = 'x';
	table[0].assed = 1;
	//printf("MOV [0] r0\n");

	table[1].name[0] = 'y';
	table[1].assed = 1;
//	printf("MOV [4] r1\n");

	table[2].name[0] = 'z';
	table[2].assed = 1;
	//printf("MOV [8] r2\n");


	printf(">> ");
	while (1) {
		statement();
	}
	return 0;
}


int getval(void)
{
	int i, retval, found;

	if (match(INT)) {
		retval = atoi(getLexeme());
	}
	else if (match(ID)) {
		i = 0; found = 0; retval = 0;
		while (i < sbcount && !found) {
			if (strcmp(getLexeme(), table[i].name) == 0) {
				retval = table[i].val;
				found = 1;
				break;
			}
			else {
				i++;
			}
		}
		if (!found) {
			if (sbcount < TBLSIZE) {
				strcpy(table[sbcount].name, getLexeme());
				table[sbcount].val = 0;
				table[sbcount].assed = 0;
				sbcount++;
			//	printf("sbcount = %d\n", sbcount);
			}
			else {
				error(RUNOUT);
			}
		}
	}
	return retval;
}
int setval(char *str, int val)
{
	int i, retval;
	i = 0;
	while (i < sbcount) {
		if (strcmp(str, table[i].name) == 0) {
			table[i].val = val;
			table[i].assed = 1;
			retval = val;
			break;
		}
		else {
			i++;
		}
	}
	return retval;
}

int evaluateTree(BTNode *root)
{
	int retval = 0, lv, rv;
	int i;
	if (root != NULL)
	{
		switch (root->data)
		{
		case ID:
		case INT:
			retval = root->val;
			break;
		case ASSIGN:
		case ADDSUB:
		case MULDIV:
			rv = evaluateTree(root->right);
			lv = evaluateTree(root->left);

			if (strcmp(root->lexeme, "+") == 0)
				retval = lv + rv;
			else if (strcmp(root->lexeme, "-") == 0)
				retval = lv - rv;
			else if (strcmp(root->lexeme, "*") == 0)
				retval = lv * rv;
			else if (strcmp(root->lexeme, "/") == 0)
				retval = lv / rv;
	
			else if (strcmp(root->lexeme, "|") == 0)
				retval = lv | rv;	
			else if (strcmp(root->lexeme, "&") == 0)
				retval = lv & rv;
			else if (strcmp(root->lexeme, "^") == 0)
				retval = lv ^ rv;

			else if (strcmp(root->lexeme, "=") == 0)
				retval = setval(root->left->lexeme, rv);


			break;
		default:
			retval = 0;
		}
	}
	return retval;
}
void pre(BTNode *root) {
	if (root != NULL) {
		pre(root->left);
		pre(root->right);

		//printf("%s\n", root->lexeme);
	}
}

/* print a tree by pre-order. */
void printPrefix(BTNode *root)
{
	int i,j;
	if (root != NULL)
	{
		printPrefix(root->left);

		if (root->data == ASSIGN) {
			if (is_assed)error(MULASS);

			if (root->left->data != ID) {
			//	printf("can't assign %s\n", root->left->lexeme);
				error(TBD);
			}
			is_assed = 1;
		}

		printPrefix(root->right);

		switch (root->data)
		{
		case ASSIGN:
		{
			
			for (i = MAX_R - 1; i >= 0; i--) {
				if (used_r[i]) {
					used_r[i] = 1;
					break;
				}
			}

		//	if (i < 0)printf("bug!\n");
			for (j = 0; j < 64; j++) {
				if (strcmp(root->left->lexeme, table[j].name) == 0)
					break;
			}
			if (j >= 64)error;
			printf("MOV [%d] r%d\n", j * 4, i);

			used_r[i] = 0;

			return;
			break;
		}

		case INT:
		{
			for (i = 0; i < MAX_R; i++)
				if (!used_r[i]) {
					used_r[i] = 1;
					break;
				}
			printf("MOV r%d %d\n", i, root->val);
			break;
		}
		case ID:
		{
			if (!is_assed) {
				//printf("ID %s passed\n", root->lexeme);
				break;
			}//count after the assign
		//	printf("now, ID is %s\n", root->lexeme);
			for (i = 0; i < TBLSIZE; i++) {//check if varible is defined
				if (strcmp(table[i].name, root->lexeme) == 0) {
					if (!(table[i].assed)) {
					//	printf("%s not defined\n", table[i].name);
						error(TBD);
					}
				}
			}

			for (i = 0; i < MAX_R; i++) {
				if (!used_r[i]) {
					used_r[i] = 1;
					break;
				}
			}
			for (j = 0; j < 64; j++) {
				if (strcmp(root->lexeme, table[j].name) == 0)
					break;
			}
			if (j >= 64)printf("bug!\n");

			printf("MOV r%d [%d]\n", i, j * 4);

			break;
		}

		case ADDSUB:
		case ORANDXOR:
		case MULDIV:
			
			for (i = MAX_R-1; i >= 0; i--)
				if (used_r[i])break;
			switch (root->lexeme[0])
			{
			case '+':
				printf("ADD r%d r%d\n", i - 1, i);
				break;
			case'-':
				printf("SUB r%d r%d\n", i - 1, i);
				break;
			case'*':
				printf("MUL r%d r%d\n", i - 1, i);
				break;
			case'/':
				printf("DIV r%d r%d\n", i - 1, i);
				break;
			case'|':
				printf("OR r%d r%d\n", i - 1, i);
				break;		
			case'&':
				printf("AND r%d r%d\n", i - 1, i);
				break;
			case'^':
				printf("XOR r%d r%d\n", i - 1, i);
				break;

			default:
				break;
			}
			used_r[i] = 0;
		default:
			break;
		}

	
		


	}
}



/* create a node without any child.*/
BTNode* makeNode(TokenSet tok, const char *lexe) {
	BTNode *node = (BTNode*)malloc(sizeof(BTNode));
	strcpy(node->lexeme, lexe);
	node->data = tok;
	node->val = 0;
	node->left = NULL;
	node->right = NULL;
	return node;
}

TokenSet getToken(void)
{
	int i;
	char c;

	while ((c = fgetc(stdin)) == ' ' || c == '\t');  // ©¿²¤ªÅ¥Õ¦r¤¸

	if (isdigit(c)) {
		lexeme[0] = c;
		c = fgetc(stdin);
		i = 1;
		while (isdigit(c) && i < MAXLEN) {
			lexeme[i] = c;
			++i;
			c = fgetc(stdin);
		}
		ungetc(c, stdin);
		lexeme[i] = '\0';
		return INT;
	}
	else if (c == '+' || c == '-') {
		lexeme[0] = c;
		lexeme[1] = '\0';
		return ADDSUB;
	}
	else if (c == '|' || c == '&' || c == '^') {
		lexeme[0] = c;
		lexeme[1] = '\0';
		return ORANDXOR;
	}
	else if (c == '*' || c == '/') {
		lexeme[0] = c;
		lexeme[1] = '\0';
		return MULDIV;
	}
	else if (c == '\n') {
		lexeme[0] = '\0';
		return END;
	}
	else if (c == '=') {
		strcpy(lexeme, "=");
		return ASSIGN;
	}
	else if (c == '(') {
		strcpy(lexeme, "(");
		return LPAREN;
	}
	else if (c == ')') {
		strcpy(lexeme, ")");
		return RPAREN;
	}
	else if (isalpha(c) || c == '_') {
		lexeme[0] = c;
		c = fgetc(stdin);
		i = 1;
		while (isalpha(c) || isdigit(c) || c == '_') {
			lexeme[i] = c;
			++i;
			c = fgetc(stdin);
		}
		ungetc(c, stdin);
		lexeme[i] = '\0';
		return ID;
	}
	else if (c == EOF) {
		return ENDFILE;
	}
	else {
		return UNKNOWN;
	}
}

/* factor := INT | ADD_SUB INT | ADD_SUB ID | ID ASSIGN expr| ID | LPAREN expr RPAREN */

BTNode* factor(void)
{
	BTNode* retp = NULL;
	char tmpstr[MAXLEN];

	if (match(INT)) {
		retp = makeNode(INT, getLexeme());
		retp->val = getval();
		advance();

	}
	else if (match(ID)) {
		BTNode* left = makeNode(ID, getLexeme());
		left->val = getval();
		strcpy(tmpstr, getLexeme());
		advance();
		if (match(ASSIGN)) {
			retp = makeNode(ASSIGN, getLexeme());
			advance();
			retp->right = expr();
			retp->left = left;
		}
		else retp = left;
	

	}
	else if (match(ADDSUB)) {
		strcpy(tmpstr, getLexeme());
		advance();
		if (match(ID) || match(INT)) {
			retp = makeNode(ADDSUB, tmpstr);
			if (match(ID))
				retp->right = makeNode(ID, getLexeme());
			else
				retp->right = makeNode(INT, getLexeme());
			retp->right->val = getval();
			retp->left = makeNode(INT, "0");
			retp->left->val = 0;
			advance();
		}
		else {
			error(NOTNUMID);
		}
	}
	else if (match(ORANDXOR)) {
		strcpy(tmpstr, getLexeme());
		advance();
		if (match(ID) || match(INT)) {
			retp = makeNode(ADDSUB, tmpstr);
			if (match(ID))
				retp->right = makeNode(ID, getLexeme());
			else
				retp->right = makeNode(INT, getLexeme());
			retp->right->val = getval();
			retp->left = makeNode(INT, "0");
			retp->left->val = 0;
			advance();
		}
		else {
			error(NOTNUMID);
		}

	}
	else if (match(LPAREN)) {
		advance();
		retp = expr();
		if (match(RPAREN)) {
			advance();
		}
		else {
			error(MISPAREN);
		}
	}
	else {
		error(NOTNUMID);
	}
	return retp;
}

/* term := factor term_tail */
BTNode* term(void)
{
	BTNode *node;

	node = factor();

	return term_tail(node);
}

/* term_tail := MUL_DIV factor term_tail|NiL */
BTNode* term_tail(BTNode *left)
{
	BTNode *node;

	if (match(MULDIV)) {
		node = makeNode(MULDIV, getLexeme());
		advance();

		node->left = left;
		node->right = factor();

		return term_tail(node);
	}
	else
		return left;
}

/* expr := term expr_tail */
BTNode* expr(void)
{
	BTNode *node;

	node = term();
	
	return expr_tail(node);
}

/* expr_tail := ADD_SUB_AND_OR_XOR term expr_tail | NiL */
BTNode* expr_tail(BTNode *left)
{
	BTNode *node;

	if (match(ADDSUB)) {
		node = makeNode(ADDSUB, getLexeme());
		advance();

		node->left = left;
		node->right = term();

		return expr_tail(node);
	}
	else if (match(ORANDXOR)) {
		node = makeNode(ORANDXOR, getLexeme());
		advance();

		node->left = left;
		node->right = term();

		return expr_tail(node);
		
	}
	else
		return left;
}

void advance(void)
{
	lookahead = getToken();
}

int match(TokenSet token)
{
	if (lookahead == UNKNOWN) advance();
	return token == lookahead;
}

char* getLexeme(void)
{
	return lexeme;
}

/* statement := END | expr END */

void statement(void)
{
	BTNode* retp;

	if (match(ENDFILE)) {
		printf("MOV r0 [0]\nMOV r1 [4]\nMOV r2 [8]\n");
		printf("EXIT 0\n");
		exit(0);

	}
	else if (match(END)) {

		printf(">> ");
		advance();

	}
	else {

		retp = expr();
		if (match(END)) {
			evaluateTree(retp);

			for (int i = 0; i < MAX_R; i++)
				used_r[i] = 0;
			is_assed = 0;

		//	pre(retp);
			printPrefix(retp); 

			freeTree(retp);

			printf(">> ");
			advance();
		}
		else if (!(match(END))) {
			//printf("input illegal\n");
			error(TBD);
		}

	}
}
void error(ErrorType errorNum)
{
	/*switch (errorNum) {
	case MISPAREN:
		fprintf(stderr, "Mismatched parenthesis\n");
		break;
	case NOTNUMID:
		fprintf(stderr, "Number or identifier expected\n");
		break;
	case NOTFOUND:
		fprintf(stderr, "%s not defined\n", getLexeme());
		break;
	case RUNOUT:
		fprintf(stderr, "Out of memory\n");
		break;
	case MULASS:
		fprintf(stderr, "Multiple assign\n");
		break;
	}*/
	printf("EXIT 1\n");
	exit(0);
}
/* clean a tree.*/
void freeTree(BTNode *root) {
	if (root != NULL) {
		freeTree(root->left);
		freeTree(root->right);
		free(root);
	}
}
