// ============================================================================
// list.c -- YOUR WORK for Lab 1.
//
// Implement every function below so it matches its contract in list.h. Each has
// a TODO describing what to do; the header has the ownership rule and Big-O.
//
// The placeholder bodies let the project compile and stay leak-free BEFORE you
// start, so the whole workflow (make, make valgrind, make format) works from
// minute one. As you implement each function, real behaviour appears.
//
// The `(void)x;` lines just tell the compiler "I know x is unused for now" so
// -Werror does not stop you. Delete each one once you use that variable.
// ============================================================================

#include "list.h"
#include <stdbool.h>
#include <stdlib.h> // you will need malloc and free

TokenList list_create(void) {
    // GIVEN: an empty list is all-null with length 0. (This one is done for you
    // as an example of a designated initializer -- study it, then build the
    // rest below.)
    return (TokenList){.head = NULL, .tail = NULL, .length = 0};
}

void list_destroy(TokenList *list) {
    // TODO: walk every node from head. For EACH node, in this order:
    //   1) TokenNode *next = current->next;   // save next FIRST
    //   2) token_free(&current->token);       // free the owned lexeme
    //   3) free(current);                     // free the node box
    //   4) current = next;
    // Then reset the header (e.g. *list = list_create();).
    // Freeing the node before saving ->next is a use-after-free -- ASan catches
    // it, so try it once on purpose to see the report.
    if (list == NULL) {
        return;
    }
    TokenNode *current = list->head;
    while (current != NULL) {
        TokenNode *next = current->next;
        token_free(&current->token);
        free(current);
        current = next;
    }
    *list = list_create();
}

bool list_push_front(TokenList *list, Token token) {
    // TODO: allocate a node (malloc), store `token` in it, and link it at the
    // FRONT (new node's next = old head; head = new node). If the list was empty
    // (tail == nullptr), the new node is also the tail. Then length++.
    // On malloc failure: token_free(&token); return false;  (do not leak!)
    if (list == NULL) {
        token_free(&token);
        return false;
    }

    TokenNode *new_node = malloc(sizeof(TokenNode));
    if (new_node == NULL) {
        token_free(&token);
        return false;
    }

    new_node->token = token;
    new_node->next  = list->head;
    list->head      = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }

    list->length++;
    return true;
}

bool list_push_back(TokenList *list, Token token) {
    // TODO: allocate a node holding `token` and append it at the BACK. If the
    // list is empty, it becomes both head and tail; otherwise link tail->next to
    // it and move tail. Then length++. Same malloc-failure rule as above.
    if (list == NULL) {
        token_free(&token);
        return false;
    }

    TokenNode *new_node = malloc(sizeof(TokenNode));
    if (new_node == NULL) {
        token_free(&token);
        return false;
    }

    new_node->token = token;
    new_node->next  = NULL;

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail       = new_node;
    }

    list->length++;
    return true;
}

bool list_insert_after(TokenList *list, TokenNode *node, Token token) {
    // TODO: insert a new node holding `token` immediately after `node`. Careful
    // with the two links: new->next = node->next; node->next = new. If `node`
    // was the tail, update tail. Then length++. If `node` is nullptr, or malloc
    // fails, token_free(&token) and return false.
    if (list == NULL || node == NULL) {
        token_free(&token);
        return false;
    }

    TokenNode *new_node = malloc(sizeof(TokenNode));
    if (new_node == NULL) {
        token_free(&token);
        return false;
    }

    new_node->token = token;
    new_node->next  = node->next;
    node->next      = new_node;

    if (list->tail == node) {
        list->tail = new_node;
    }

    list->length++;
    return true;
}

TokenNode *list_find_first(const TokenList *list, TokenKind kind) {
    // TODO: walk from head; return the first node whose token.kind == kind, or
    // nullptr if none is found.
    if (list == NULL) {
        return NULL;
    }

    TokenNode *current = list->head;
    while (current != NULL) {
        if (current->token.kind == kind) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

bool list_remove_first(TokenList *list, TokenKind kind) {
    // TODO: find the first node with this kind while remembering the node BEFORE
    // it (`prev`). Unlink it (mind the head case where prev is nullptr, and the
    // tail case where you must move tail back to prev). Free its token then the
    // node. length--. Return true if removed, false if not found.
    if (list == NULL || list->head == NULL) {
        return false;
    }

    TokenNode *prev    = NULL;
    TokenNode *current = list->head;

    while (current != NULL) {
        if (current->token.kind == kind) {
            if (prev == NULL) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }

            if (list->tail == current) {
                list->tail = prev;
            }

            token_free(&current->token);
            free(current);
            list->length--;
            return true;
        }

        prev    = current;
        current = current->next;
    }

    return false;
}

size_t list_length(const TokenList *list) {
    // TODO: return the length you maintain in the header.
    if (list == NULL) {
        return 0;
    }
    return list->length;
}

void list_print(const TokenList *list, FILE *out) {
    // TODO: for each node, print one line to `out`. Suggested format (matches
    // the reference so grading scripts line up):
    //   fprintf(out, "  %-9s '%s' (%d:%d)\n",
    //           token_kind_name(cur->token.kind), cur->token.lexeme,
    //           cur->token.line, cur->token.col);
    if (list == NULL || out == NULL) {
        return;
    }

    const TokenNode *current = list->head;
    while (current != NULL) {
        fprintf(out, "  %-9s '%s' (%d:%d)\n", token_kind_name(current->token.kind),
                current->token.lexeme, current->token.line, current->token.col);
        current = current->next;
    }
}

void list_to_dot(const TokenList *list, FILE *out) {
    // TODO (do this LAST, after the list works): print Graphviz DOT to `out`.
    // A minimal version is fine to start:
    //   digraph tokens { rankdir=LR; node [shape=record];
    //     n0 [label="{KIND|lexeme}"]; n0 -> n1; ... last -> nnull; }
    // See list.h and the README for what the picture should look like.
    if (list == NULL || out == NULL) {
        return;
    }

    fprintf(out, "digraph tokens {\n");
    fprintf(out, "  rankdir=LR;\n");
    fprintf(out, "  node [shape=record];\n");

    const TokenNode *current = list->head;
    size_t index = 0;

    while (current != NULL) {
        if (current->token.lexeme[0] == '-' && current->token.lexeme[1] == '>') {
            fprintf(out, "  n%zu [label=\"{%s|-\\>}\"];\n", index,
                    token_kind_name(current->token.kind));
        } else {
            fprintf(out, "  n%zu [label=\"{%s|%s}\"];\n", index,
                    token_kind_name(current->token.kind), current->token.lexeme);
        }

        if (current->next != NULL) {
            fprintf(out, "  n%zu -> n%zu;\n", index, index + 1);
        } else {
            fprintf(out, "  n%zu -> nnull;\n", index);
        }

        current = current->next;
        index++;
    }

    fprintf(out, "  nnull [label=\"nullptr\", shape=plaintext];\n");
    fprintf(out, "}\n");
}

bool list_check_invariant(const TokenList *list) {
    // TODO: return true only if the list is well formed:
    //   - length == number of nodes reachable from head
    //   - tail is the last node, and is nullptr exactly when the list is empty
    // Returning true here is a placeholder; implement it so it can actually
    // catch a bookkeeping mistake for you.
    if (list == NULL) {
        return false;
    }

    size_t count             = 0;
    const TokenNode *current = list->head;
    const TokenNode *last    = NULL;

    while (current != NULL) {
        count++;
        last    = current;
        current = current->next;
    }

    if (count != list->length) {
        return false;
    }

    if (list->tail != last) {
        return false;
    }

    if ((list->head == NULL && list->tail != NULL) || (list->head != NULL && list->tail == NULL)) {
        return false;
    }

    return true;
}