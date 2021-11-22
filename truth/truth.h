#ifndef TRUTH_H
#define TRUTH_H

#define ERR(msg)                              \
	fprintf(stderr, "Error: %s\n", msg);  \
	exit(1)

#define LOG(msg) \
	if (verbose) { fprintf(stderr, "%d:\t%s\n", __LINE__, msg); }

typedef enum { AND = 1, OR, NOT } Operation;
typedef enum {
	NullOperand = 0,
	StatementOperand = 1,
	NumericOperand
} OperandType;
typedef struct {
	OperandType t;
	union {
		struct Statement *s;
		char oper;
	} data;
} Operand;
typedef struct {
	Operand operands[2];
	Operation op;
} Statement;

/* Parsing */
Statement *parse(char **program);

#endif