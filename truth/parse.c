#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "truth.h"

extern bool verbose;

static const int opermax = 3;
static int opers = 0;
static char operands[128] = {0};

void parseparam(char **program, Statement *stat, int pos)
{
	char oper = -1;

	LOG("\tSTART NAMED PARAMETER");
	while (isalnum(*(*program+1))) {
		operands[opers++] = **program;

		if (oper == -1) {
			oper = **program;
			stat->operands[pos].data.oper = oper;
		}

		(*program)++;
	}

	LOG("\tEND NAMED PARAMETER");
}

Statement *parse(char **program)
{
	LOG("------START RECURSION------");
	Statement *stat = malloc(sizeof(Statement));

	char *c;
	int params = 0;
	int expects = -1;
	bool operfound = false;
	char slice[opermax + 1];
	for (c = *program; *c != '\00' && *c != ')'; c++) {
		strncpy(slice, c, opermax);
		slice[opermax] = '\00';

		/* Full size operations */
		/* TODO: Add support for shorthands (& / | / !) */
		if (strcmp("AND", slice) == 0) {
			expects = 2;

			if (params != 1) {
				ERR("Expected one operand preceeding AND");
			} else if (operfound) {
				ERR("Expected only a single operation");
			}

			LOG("====[AND]====");
			stat->op = AND;

			c += 3;
			operfound = true;
			continue;
		} else if (strcmp("NOT", slice) == 0) {
			expects = 1;

			if (params > 0) {
				ERR("NOT expects single operand to follow, not preceed");
			} else if (operfound) {
				ERR("Expected only a single operation");
			}

			LOG("====[NOT]====");
			stat->op = NOT;

			c += 3;
			operfound = true;
			continue;
		} else if (strncmp("OR", slice, 2) == 0) {
			expects = 2;

			if (params != 1) {
				ERR("Expected one operand preceeding OR");
			} else if (operfound) {
				ERR("Expected only a single operation");
			}

			LOG("====[OR]====");
			stat->op = OR;

			c += 2;
			operfound = true;
			continue;
		}

		if (isspace(*c)) {
			continue;
		} else if (*c == '(') {
			if (expects != -1 && params > expects) {
				ERR("Too many parameters provided");
			}

			LOG("\tSTART STATEMENT PARAMETER");
			stat->operands[params].t = StatementOperand;
			c++;
			stat->operands[params].data.s = (struct Statement *)parse(&c);
			params++;
			LOG("\tEND STATEMENT PARAMETER");
		} else {
			if (expects != -1 && params > expects) {
				ERR("Too many parameters provided");
			}

			stat->operands[params].t = NumericOperand;
			parseparam(&c, stat, params);
			params++;
		}
	}

	if (expects != -1 && params != expects) {
		ERR("Wrong number of operands provided");
	} else if (!operfound) {
		ERR("Statement must contain an operand");
	}

	LOG("------END RECURSION------");

	*program = c;
	return stat;
}